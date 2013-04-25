#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

import getopt,sys,socket,re,os,platform,string,datetime,string

def parseName(toplevel,names):
    fullname=names[0]
    testname=fullname[fullname.rfind('/')+1:]
    classname=toplevel+'.'+fullname[0:fullname.rfind('/')]
    classname=classname.replace('/','.')
    time=out=''
    for tok in names[1:]:
        if tok.startswith('threadid') or tok.startswith('--threadid') or tok.startswith('--androidid'):
           out+=tok+' '
        elif tok=='time':
           continue
        else:
           try:
               float(tok)
               time=tok
           except: 
               testname+=' '+tok
    if out!='':
        out=out[0:-1]+'\n'
    return classname,testname,time,out            

def fixForXmlEscape(str):
    str=str.replace('&','&amp;')
    str=str.replace("'",'&apos;')
    str=str.replace('<','&lt;')
    str=str.replace('>','&gt;')
    str=str.replace('"','&quot;')
    return str

def fixForXmlCdata(str):
    str=str.strip()
    if len(str)>0:
        # change ]]> in source string
        str=str.replace(']]>','] ]>') 
        # remove invalid unicode chars
        str=re.sub('[\x00-\x08\x0B-\x0C\x0E-\x1F\x7F]','',str)
        str="<![CDATA[%s]]>" % str
    return str

def checkForMatchingVersion(file,version):
    lines=open(file).read().split('\n')
    for line in lines:
        if re.search('property name="version"',line):
            if re.search('value="%s"' % version,line):
                return True
            else:
                return False

def convertPerformanceToJunit(infile,outfile,toplevel):
    lines=open(infile).read().split('\n')
    properties={}
    metric='time'
    dir='.'
    testcontents=''
    skips=errors=failures=total=time=0
    hostname=socket.gethostname()
    if toplevel==None or toplevel=='':
        toplevel=platform.platform()
    systemout=systemerr=''
    properties['platform']=toplevel
    properties['version']='unknown'
    multMetrics=False
    measurements=[]
    for line in lines:
        tokens=line.split()
        if line.startswith('Tamarin tests') and len(tokens)>3:
            tm=tokens[4]
            if tm.find('.'):
                tm=tm[0:tm.find('.')]
            properties['timestamp']='%sT%s' % (tokens[3],tm)
        elif line.startswith('avm') and len(tokens)>3:
            properties['avm']=tokens[1]
            properties['version']=tokens[-1]
        elif line.startswith('iterations') and len(tokens)>1:
            properties['iterations']=tokens[1]
        elif line.startswith('Dir') and len(tokens)>1:
            dir=tokens[1]
            dir=dir.replace('.','_')
        elif line.startswith('Metric') and len(tokens)>1:
            metric=tokens[1]
        elif line.startswith('Executing') or line.startswith("test"):
            pass
        elif len(tokens)==1:
            name=tokens[0]
            multMetrics=True
        elif len(tokens)==2 or len(tokens)==4:
            if multMetrics:
                metric=tokens[0]
            else:
                name=tokens[0]
            value=tokens[1]
            if value.endswith("M"):
                value=value[0:-1]
            try:
                float(value)
            except:
                value=0
            measurements.append([dir,name,metric,value])
    insertMeasurements=False
    if os.path.exists(outfile):
        if checkForMatchingVersion(outfile,properties['version']):
            insertMeasurements=True
        else:
            ctr=0
            newoutfile=outfile
            if newoutfile.endswith('.xml'):
                newoutfile=newoutfile[0:-4]
            while True:
                if os.path.exists('%s%d.xml' % (newoutfile,ctr))==False:
                    outfile='%s%d.xml' % (newoutfile,ctr)
                    break
                if checkForMatchingVersion('%s%d.xml' % (newoutfile,ctr),properties['version']):
                    insertMeasurements=True
                    outfile="%s%d.xml" % (newoutfile,ctr)
                    break
                ctr+=1
    if insertMeasurements:
        lines=open(outfile).read().split('\n')
        for measurement in measurements:
            found=False
            currentclass=currentmetric='unknown'
            for i in range(len(lines)):
                if re.search('classname="%s.%s".* name="%s"' % (toplevel,measurement[0],measurement[1]),lines[i]):
                    currentclass=measurement[0]
                    currentmetric=measurement[1]
                if re.search('<measurement>',lines[i]) and measurement[0]==currentclass and measurement[1]==currentmetric:
                    lines.insert(i+1,'<measurement><name>%s</name><value>%s</value></measurement>' % (measurement[2],measurement[3]))
                    found=True
                    break
            if found==False:
                lasttest=len(lines)-1
                for i in range(len(lines)):
                    if lines[i].lstrip().startswith('</testcase>'):
                        lasttest=i
                        
                lines.insert(lasttest+1,'<testcase classname="%s.%s" name="%s" time="0">' % (toplevel,measurement[0],measurement[1]))
                lines.insert(lasttest+2,'<system-out>')
                lines.insert(lasttest+3,'<![CDATA[')
                lines.insert(lasttest+4,'<measurement><name>%s</name><value>%s</value></measurement>' % (measurement[2],measurement[3]))
                lines.insert(lasttest+5,']]>')
                lines.insert(lasttest+6,'</system-out>')
                lines.insert(lasttest+7,'</testcase>')
        open(outfile,'w').write(string.join(lines,'\n'))
    else:
        contents='<?xml version="1.0" encoding="UTF-8" ?>\n'
        contents+='<testsuite skip="%s" errors="%s" failures="%d" hostname="%s" name="performance-%s" tests="%d" time="%s" timestamp="%s">\n' % (skips,errors,failures,hostname,toplevel,len(measurements),time,properties['timestamp'])
        contents+='<properties>\n'
        for key in properties.keys(): 
            contents+='<property name="%s" value="%s" />\n' % (key,properties[key])
        contents+='</properties>\n'
        for measurement in measurements:
            lines=contents.split('\n')
            found=False
            newcontents=''
            for i in range(len(lines)):
                newcontents+=lines[i]+'\n'
                if i>2 and re.search('classname="%s.%s".* name="%s"' % (toplevel,measurement[0],measurement[1]),lines[i-2]):
                   found=True
                   newcontents+='<measurement><name>%s</name><value>%s</value></measurement>\n' % (measurement[2],measurement[3])
            if found:
                contents=newcontents
            else:   
                contents+='<testcase classname="%s.%s" name="%s" time="0">\n<system-out>\n<![CDATA[\n<measurement><name>%s</name><value>%s</value></measurement>\n]]>\n</system-out>\n</testcase>\n' % (toplevel,measurement[0],measurement[1],measurement[2],measurement[3])
        contents+='<system-out>%s</system-out>\n<system-err>%s</system-err>\n</testsuite>' % (fixForXmlCdata(systemout),fixForXmlCdata(systemerr))
        open(outfile,'w').write(contents)
    return outfile

def convertPlayerToJunit(infile,outfile,toplevel):
    properties={}
    lines=open(infile).read().split('\n')
    hostname=socket.gethostname()
    name=toplevel
    currentTest=None
    passes=0
    allpasses=0
    fails=0
    allfails=0
    errors=0
    skips=0
    time="0"
    timestamp=datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
    results={}
    tests=[]
    for line in lines:
        if currentTest==None:
            tokens=line.split(':')
            if len(tokens)==2 and tokens[1]!='' and tokens[0]!='test':
                properties[tokens[0].strip()]=tokens[1].strip()
        if line.startswith('test:'):
            if currentTest!=None:
                if len(results[currentTest]['failures'])>0:
                    fails+=1
                else:
                    passes+=1
            out=''
            if lastline.find('PASSED!')==-1 and lastline.find('FAILED!')==-1 and currentTest!=None:
                results[currentTest]['output']=string.join(results[currentTest]['output'].split('\n')[0:-2],'\n')
                out=lastline
            currentTest=line[5:].strip()
            tests.append(currentTest)
            results[currentTest]={'output':out,'passes':[],'failures':[]}
        elif line.find('PASSED!')>0 and currentTest!=None:
            results[currentTest]['passes'].append(line)
            allpasses+=1
        elif line.find('FAILED!')>0 and currentTest!=None:
            results[currentTest]['failures'].append(line)
            allfails+=1
        if currentTest!=None:
            results[currentTest]['output']+=line+'\n'
        lastline=line
    contents='<?xml version="1.0" encoding="UTF-8" ?>\n'
    contents+='<testsuite skip="%s" errors="%s" failures="%d" hostname="%s" name="%s" tests="%d" time="%s" timestamp="%s">\n' % (skips,errors,fails,hostname,name,passes+fails,time,timestamp)
    contents+='<properties>\n'
    contents+='<property name="passes" value="%s"/>\n' % (passes)
    contents+='<property name="failures" value="%s"/>\n' % (fails)
    contents+='<property name="testpasses" value="%s"/>\n' % (allpasses)
    contents+='<property name="testfailures" value="%s"/>\n' % (allfails)
    for prop in properties.keys():
        contents+='<property name="%s" value="%s"/>\n' % (prop,properties[prop])
    contents+="</properties>\n"
    for test in tests:
        contents+='<testcase classname="%s.%s" name="%s" time="%s">\n' % (toplevel,'ats',test,'0')
        if len(results[test]['failures'])>0:
            contents+='<failure message="FAILED!" type="failure">'
            failout=''
            for fail in results[test]['failures']:
                failout+=fail+'\n'
            contents+=fixForXmlCdata(failout)
            contents+='</failure>\n'
        if results[test]['output']!='':
            contents+="<system-out>%s</system-out>\n" % fixForXmlCdata(results[test]['output'])
        contents+='</testcase>\n'
    contents+='<system-out></system-out><system-err></system-err>\n</testsuite>\n'
    open(outfile,'w').write(contents)
            
def convertAotToJunit(infile,outfile,toplevel):
    fail=''
    error=''
    skip=''
    skiplist={}
    out=''
    hostname=socket.gethostname()
    name=toplevel
    test=None
    time=''
    timestamp=''
    properties={}
    properties['failures']='0'
    properties['unexpected passes']='0'
    properties['expected failures']='0'
    properties['config']=''
    testresults=[]
    systemout=''
    systemerr=''
    finishedTests=False
    lastout=''
    skips=0
    tests=0
    errors=0
    fails=0
    totaltime=0
    
    lines=open(infile).read().split('\n')
    for line in lines:
        line=line.strip()
        tokens=line.split()
        if line.startswith('java version'):
            properties['java.version']=tokens[2].replace('"','')
        if line=='':
            continue
        if line.startswith('thread count:'):
            properties['threads']=tokens[2]
        if line.startswith('+ ./runtests.py'):
            properties['runtests']=line
        if line.startswith('Tamarin tests started:'):
            tm=properties['started']=tokens[3]
            if tm.find('.'):
                tm=tm[0:tm.find('.')]
            timestamp='%sT%s' % (tokens[3],tm)
        if line.startswith('current configuration:'):
            properties['config']=tokens[2]
        if line.startswith('asc version:'):
            properties['asc.version']=tokens[2]
        if line.startswith('Tests complete at'):
            properties['endtime']=tokens[3]+' '+tokens[4]
        if line.startswith('Start Date'):
            properties['startime']=tokens[2]+' '+tokens[3]
        if line.startswith('Test Time'):
            properties['testtime']=tokens[3]
        if line.startswith('Skipping'):
            skipname=tokens[1]
            skiplist[skipname]=line
            continue
        if line.startswith('ERROR:'):
            error=line
            errors+=1
        if line.startswith('java.io.FileNotFoundException'):
            error=''
            errors-=1
        if line.startswith('compiling') or line.startswith('Excluding'):
            continue
        if line.startswith('AOT compiling'):
            if test!=None:
                if test in skiplist:
                    skip=skiplist[test]
                    skips+=1
                else:
                    skip=''
                testresults.append({'test':test,'time':time,'out':out,'skip':skip,'fail':fail,'class':classname,'name':testname,'failure':error})
            error=''
            test=tokens[2]
            classname,testname,time,out=parseName(toplevel,tokens[2:])
            if time=='':
                time='0.0'
            totaltime+=float(time)
            continue
        elif line.startswith('test run'):
            properties['status']=line
            break
        else:
            if test!=None:
                out+=line+'\n'
    properties['failures']=errors
    properties['time']=totaltime
    contents='<?xml version="1.0" encoding="UTF-8" ?>\n'
    contents+='<testsuite skip="%s" errors="%s" failures="%d" hostname="%s" name="%s-%s" tests="%d" time="%s" timestamp="%s">\n' % (skips,errors,fails,hostname,properties['config'],toplevel,tests,totaltime,timestamp)
    contents+='  <properties>\n'
    for key in properties.keys(): 
        contents+='    <property name="%s" value="%s" />\n' % (key,properties[key])
    contents+='  </properties>\n'
    for testresult in testresults:
        contents+='  <testcase classname="%s.%s-%s" name="%s" time="%s">' % (properties['config'],properties['config'],testresult['class'],testresult['name'],testresult['time'])
        if 'out' in testresult and testresult['out']!='':
            out+=testresult['name']+'\n'+testresult['out']+'\n'
        if 'skip' in testresult and testresult['skip']!='':
            contents+='<skipped message="%s" type="%s">%s</skipped>\n  ' % (fixForXmlEscape(testresult['skip']),'skipped',fixForXmlCdata(testresult['skip']))
            out+=testresult['skip']+'\n'
            testresult['out']+=testresult['skip']
        if 'failure' in testresult and testresult['failure']!='':
            contents+='<failure message="%s" type="%s">%s</failure>\n  ' % (testresult['failure'],'failure',fixForXmlCdata(testresult['failure']))
        if testresult['out']!='':
            contents+='<system-out>%s</system-out>' % fixForXmlCdata(testresult['out'])
        contents+='</testcase>\n'
    contents+='<system-out>%s</system-out>\n<system-err>%s</system-err>\n</testsuite>' % (fixForXmlCdata(systemout),fixForXmlCdata(systemerr))
    open(outfile,'w').write(contents)

def convertSelftestsToJunit(infile,outfile,toplevel):
    lines=open(infile).read().split('\n')
    properties={}
    properties['timestamp']=datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
    dir='.'
    testcontents=''
    passes=fails=0
    hostname=socket.gethostname()
    if toplevel==None or toplevel=='':
        toplevel=platform.platform()
    systemout=systemerr=''
    properties['platform']=toplevel
    testcaselist=[]
    testcases={}
    failures={}
    test=''
    for line in lines:
        if line.startswith('['):
            line=line.strip()
            line=line[1:]
            if line.endswith(']'):
                line=line[:-1]
            tokens=line.split(',')
            for i in range(len(tokens)):
                tokens[i]=tokens[i].strip()
                if tokens[i].startswith("'") and tokens[i].endswith("'"):
                    tokens[i]=tokens[i][1:-1]
            if tokens[0]=='pass':
                test="%s.%s.%s.%s" % (toplevel, tokens[1],tokens[2],tokens[3])
                testcaselist.append(test)
                testcases[test]=''
                passes+=1
            elif tokens[0]=='failure':
                test="%s.%s.%s.%s" % (toplevel, tokens[1],tokens[2],tokens[3])
                testcaselist.append(test)
                testcases[test]=''
                failures[test]=''
                for i in range(len(tokens)-3):
                    failures[test]+=tokens[i]+' '
                failures[test]+='\n'
                fails+=1
        else:
            testcases[test]+=line+'\n'
    properties['passes']=passes
    properties['failures']=fails

    contents='<?xml version="1.0" encoding="UTF-8" ?>\n'
    contents+='<testsuite skip="%s" errors="%s" failures="%d" hostname="%s" name="selftests-%s" tests="%d" time="%s" timestamp="%s">\n' % (0,0,fails,hostname,toplevel,len(testcaselist),0,properties['timestamp'])
    contents+='<properties>\n'
    for key in properties.keys():
        contents+='<property name="%s" value="%s" />\n' % (key,properties[key])
    contents+='</properties>\n'
    for testcase in testcaselist:
        names=testcase.split('.')
        classname="%s.%s" % (names[0],names[1])
        testname=names[2]
        if testcase in failures:
            contents+='<testcase classname="%s" name="%s" time="0">\n<failure message="FAILED" type="failure">\n<![CDATA[%s]]>\n</failure>\n<system-out>\n<![CDATA[\n%s]]>\n</system-out>\n</testcase>\n' % (classname,testname,failures[testcase],testcases[testcase])
        else:
            if testcases[testcase].strip()=='':
                contents+='<testcase classname="%s" name="%s" time="0"/>\n' % (classname,testname)
            else:
                contents+='<testcase classname="%s" name="%s" time="0">\n<system-out>\n<![CDATA[\n%s]]>\n</system-out>\n</testcase>\n' % (classname,testname,testcases[testcase])
    contents+='<system-out>%s</system-out>\n<system-err>%s</system-err>\n</testsuite>' % (fixForXmlCdata(systemout),fixForXmlCdata(systemerr))
    open(outfile,'w').write(contents)
    return outfile


def convertAcceptanceToJunit(infile,outfile,toplevel):
    skips=0
    errors='0'
    failures=''
    hostname=socket.gethostname()
    name=toplevel
    tests=''
    time=''
    timestamp=''
    properties={}
    properties['failures']='0'
    properties['unexpected passes']='0'
    properties['expected failures']='0'
    testresults=[]
    systemout=''
    systemerr=''
    finishedTests=False
    lastout=''
    failedfiles=0
    totalfiles=0
    
    lines=open(infile).read().split('\n')
    for line in lines:
        tokens=line.split()
        if line=='':
            continue
        if line.startswith('Tamarin tests'):
            tm=tokens[4]
            if tm.find('.'):
                tm=tm[0:tm.find('.')]
            timestamp='%sT%s' % (tokens[3],tm)
        if line.startswith('current configuration:'):
            properties['configuration']=tokens[2]
        if line.startswith('avm version:'):
            properties['avmversion']=tokens[2]
        if line.startswith('thread count:'):
            properties['threadcount']=tokens[2]
        if line.startswith('Executing'):
            tests=tokens[1]
            if len(tokens)>5:
               properties['shell']=tokens[5]
        if finishedTests==False and len(tokens)>2 and re.search('^[0-9.]+',line) and tokens[1]!="error":
            totalfiles+=1
            if lastout!='' and len(testresults)>0:
                testresults[len(testresults)-1]['out']+=lastout
            lastout=''
            testobj={}
            fullname=tokens[2:]
            testobj['class'],testobj['name'],testobj['time'],testobj['out']=parseName(toplevel,fullname)
            testresults.append(testobj)
        elif finishedTests==False:
            if re.search('^\s+skipping...',line) or re.search('^\s+expected failure',line):
                skips+=1
                lastobj=testresults[len(testresults)-1]
                lastobj['skip']=True
                lastobj['skipout']=line.strip()
                lastobj['skiptype']='skipped'
                if re.search('^\s+skipping...',line):
                    lastobj['skipmessage']='skipped'
                else:
                    lastobj['skipmessage']='expected_Failure'
                lastout+=line+'\n'
            elif re.search('^\s+FAILED',line):
                failedfiles+=1
                lastobj=testresults[len(testresults)-1]
                lastobj['failure']=True
                lastobj['failuremessage']=line
                lastobj['failuretype']='failure'
                lastobj['failureout']=lastout+'\n'+line
                lastout+=line+'\n'
            else:
                lastout+=line+'\n'
        if line.startswith('EXPECTED FAILURES:') or line.startswith('FAILURES'):
            finishedTests=True
        if line.startswith('Start Date'):
            properties['startdate']=tokens[2]+' '+tokens[3]
        if line.startswith('End Date'):
            properties['enddate']=tokens[3]+' '+tokens[4]
        if line.startswith('Test Time'):
            time=tokens[3]
            properties['time']=tokens[3]
        if line.startswith('passes'):
            tests=tokens[2]
            properties['passes']=tokens[2]
        if line.startswith('failures'):
            failures=tokens[2]
            properties['failures']=tokens[2]
        if line.startswith('expected failures'):
            properties['expected failures']=tokens[3]
        if line.startswith('tests skipped'):
            properties['tests skipped']=tokens[3]
        if line.startswith('unexpected passes'):
            properties['unexpected passes']=tokens[3]

    if ('configuration' in properties)==False:
        properties['configuration']=toplevel
    contents='<?xml version="1.0" encoding="UTF-8" ?>\n'
    contents+='<testsuite skip="%s" errors="%s" failures="%d" hostname="%s" name="%s" tests="%d" time="%s" timestamp="%s">\n' % (skips,errors,failedfiles,hostname,name,totalfiles,time,timestamp)
    contents+='  <properties>\n'
    for key in properties.keys(): 
        contents+='    <property name="%s" value="%s" />\n' % (key,properties[key])
    contents+='  </properties>\n'
    for testresult in testresults:
        out=''
        contents+='  <testcase classname="%s.%s" name="%s" time="%s">' % (properties['configuration'],testresult['class'],testresult['name'],testresult['time'])
        if 'out' in testresult:
            out+=testresult['out']+'\n'
        if 'skip' in testresult:
            if not 'failure' in testresult:
                contents+='<skipped message="%s" type="%s">%s</skipped>\n  ' % (fixForXmlEscape(testresult['skipmessage']),testresult['skiptype'],fixForXmlCdata(testresult['skipout']))
            out+=testresult['skipout']+'\n'
        if 'failure' in testresult:
            contents+='<failure message="%s" type="%s">%s</failure>\n  ' % (testresult['failuremessage'],testresult['failuretype'],fixForXmlCdata(testresult['failureout']))
            out+=testresult['failureout']+'\n'
        if out!='':
            contents+='<system-out>%s</system-out>' % fixForXmlCdata(out)
        contents+='</testcase>\n'
    contents+='<system-out>%s</system-out>\n<system-err>%s</system-err>\n</testsuite>' % (fixForXmlCdata(systemout),fixForXmlCdata(systemerr))
    open(outfile,'w').write(contents)


if __name__ == '__main__':

    infile='runtests.txt'
    outfile='runtests.xml'
    toplevel='acceptance'
    type='player'
    usage='--type=runtests|player|performance|aot|selftest --ifile=input file --ofile=junit xml file'
        
    try:
        opts, args = getopt.getopt(sys.argv[1:], '', ['ofile=','ifile=','toplevel=','type='])
    except:
        print(sys.argv)
        print('error parsing options\n%s' % usage)
        sys.exit(1)
    
    for o,v in opts:
        if o=='--ifile':
            infile=v
        elif o=='--ofile':
            outfile=v
        elif o=='--toplevel':
            toplevel=v
        elif o=='--type':
            type=v
    
    if os.path.exists(infile)==False:
        print('error: file %s does not exist' % infile)
        sys.exit(1)
    
    print("converting file '%s' to '%s' toplevel is '%s'" % (infile,outfile,toplevel))
    if type=='acceptance':
        convertAcceptanceToJunit(infile,outfile,toplevel)
    elif type=='player':
        convertPlayerToJunit(infile,outfile,toplevel)
    elif type=='performance':
        convertPerformanceToJunit(infile,outfile,toplevel)
    elif type=='aot':
        convertAotToJunit(infile,outfile,toplevel)
    elif type=='selftests':
        if toplevel=='acceptance':
            toplevel='selftests'
        convertSelftestsToJunit(infile,outfile,toplevel)
    else:
        print("error: unknown type '%s'. known types are acceptance,player,performance,runtests" % type)

    print("created file '%s'" % outfile)

