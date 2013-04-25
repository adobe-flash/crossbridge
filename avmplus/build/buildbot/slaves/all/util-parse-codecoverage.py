#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

'''
usage:
  The script parses a bullseye code coverage binary file and generates a list of missing functions, and several csv files
  for use with confluence tables and charts.

  requirement for the csv filename output to look correct must run this script with cwd set to the tamarin-redux directory

  parsecodecoverage.py --covfile avm.cov --build=5110
  env variables: $coverage_exclude_regions

  input:
     --covfile <bullseye merged binary .cov>
     --build   <hg revision number>
     
  output:
     for output files the $basename is the covfile with the .cov removed e.g. avmshell_s_cov_64.cov produces avmshell_s_cov_64-info.csv
     these files are created:
         $basename-info.csv              # build number and timestamp
         $basename-summaryfn.csv         # function data table for current build for table
         $basename-summarybc.csv         # branches/conditions data table for current build for table
         $basename-missingfn.csv         # list of uncovered functions

     these files are appended:
         $basename-recentfn.csv          # uncovered function historical chart for this iteration (month)
         $basename-recentbc.csv          # uncovered branch/conditions historical chart for this iteration (month)
         $basename-milestonefn.csv       # uncovered function historical chart for recent iterations/milestones
         $basename-milestonebc.csv       # uncovered branches/conditions historical chart for recent iterations/milestones


  bullseye tool must be installed and in the path, covfn is executed from bullseye tested with version 7.13.32

  CSV output formats:
  $basename-missingfn.csv format:  build,module name,source file name, function name
  $basename-info.csv format (single line): build: ###, updated: YYYY-MM-DD HH:MM
  $basename-summaryfn.csv format: module,uncovered fns,total fns,% fn cov
  $basename-summarygc.csv format: module,uncovered branches,% branch cov,total branches,% branch cov

'''
import getopt,os,re,subprocess,sys,datetime

class ParseCodeCoverage:
    covfile=None
    incsvfile=None
    outcsvfile=None

    datadir='./data'
    missingfnfile=None
    info=None
    summary=None
    fnsummary=None
    fnpercentsummary=None
    bcsummary=None
    bcpercentsummary=None
    basefile=None
    modulelist=None
    skips=[]
    bullseyedir=None
    build='unknown'

    

    options=''
    longOptions=['covfile=','build=','incsvfile=']
    region=''

    def __init__(self):
        self.parseOptions()

        if os.environ.has_key('coverage_exclude_regions'):
            self.region=os.environ['coverage_exclude_regions']
        
        if os.environ.has_key('bullseyedir')==False:
            print("error: must set bullseyedir environment variable to the bullseye/bin directory")
            sys.exit(1)
        self.bullseyedir=os.environ['bullseyedir']

        # if incsvfile not set run covfn to generate the csv file
        if self.incsvfile==None:
            self.runcovfn()
        else:
            self.outcsvfile=self.incsvfile

        # parse the csv file into missingfn csv string, and dictionaries: module:fncovered, module:fntotal, 
        #       module:bccovered, module:bctotal
        csvdata,fnucovered,fntotal,bcucovered,bctotal = self.processCSV()
        self.modulelist=fntotal.keys()
        self.modulelist.sort(key=str.lower)

        # write the missing fn csv file
        self.missingfnfile=self.basefile+'-missingfn.csv'
        self.saveCSV(csvdata)

        # calculate missing fn diff against milestone
        self.prepareFileForDiff(self.basefile+'-milestone-missingfn.csv')
        self.prepareFileForDiff(self.basefile+'-missingfn.csv')
        cmd="diff -U 0 %s %s" % (self.basefile+'-milestone-missingfn.csv.1',self.basefile+'-missingfn.csv.1')
        process=subprocess.Popen(cmd,shell=True,stdout=open(self.basefile+'-missingfn-diffs.csv','w'),stderr=subprocess.STDOUT)
        (stdout,stderr)=process.communicate()

        # write the csv table for current build and csv table with build number and timestamp
        self.info=self.basefile+'-info.csv'
        self.summaryfn=self.basefile+'-summaryfn.csv'
        self.generateSummaryTableFn(fnucovered,fntotal)
        self.summarybc=self.basefile+'-summarybc.csv'
        self.generateSummaryTableBc(bcucovered,bctotal)
   
        self.appendData(self.basefile+'-recentfn.csv',fnucovered,fntotal)
        self.appendData(self.basefile+'-recentbc.csv',bcucovered,bctotal)
        self.appendData(self.basefile+'-milestonefn.csv',fnucovered,fntotal)
        self.appendData(self.basefile+'-milestonebc.csv',bcucovered,bctotal)

        print('finished')        

    def prepareFileForDiff(self,file):
        f=open(file)
        contents=f.read()
        f.close()
        newcontents=''
        lines=contents.split('\n')
        for line in lines:
            tokens=line.split(',')
            if len(tokens)==0:
                continue
            tokens=tokens[1:]
            newcontents+=",".join(tokens)+"\n"
        f=open(file+".1","w")
        f.write(newcontents)
        f.close()

    def usage(self,c):
        print('usage: %s [options]' % sys.argv[0])
        print('    --covfile     set the bullseye .cov file to be processing, assumes bullseye is installed')
        print('    --incsvfile   set the csv file to load, the csv is returned by covfn --csv > file.csv,')
        print('                     if not set covfn --csv is run')
        print('    --build       set the build number')
        sys.exit(c)

    def parseOptions(self):
        try:
            opts,args = getopt.getopt(sys.argv[1:], self.options,self.longOptions)
        except:
            print(sys.exc_info()[1])
            self.usage(2)

        for o,v in opts:
            if o in ('--covfile'):
                self.covfile=v
                if v.find('.')>-1:
                    v=v[0:v.find('.')]
                self.basefile=v
            if o in ('--incsvfile'):
                self.incsvfile=v
                if v.find('.')>-1:
                    v=v[0:v.find('.')-1]
                self.basefile=v
            if o in ('--build'):
                self.build=v

        if self.covfile==None and self.incsvfile==None:
            print('--covfile or --incsvfile must be set')
            sys.exit(1)

    def runcovfn(self):
        outfile=self.basefile+'.csv'
        print('processing .cov binary to %s...' % outfile)
 
        covarg=''
        if self.covfile!=None:
            covarg='--file %s' % self.covfile

        cmd='%s/covfn %s --no-banner --csv %s' % (self.bullseyedir,covarg,self.region)
        if os.path.exists(outfile):
            os.unlink(outfile)

        process=subprocess.Popen(cmd,shell=True,stdout=open(outfile,'w'),stderr=subprocess.PIPE)
        (stdout,stderr)=process.communicate()
        self.outcsvfile=outfile

    def processCSV(self):
        print('parsing %s...' % self.outcsvfile)
        lines=open(self.outcsvfile).read()
        output=""
        # throw away header(1st) and summary(last) lines
        lines=lines.split('\n')[0:-1]
        uncoveredfncount={}
        totalfncount={}
        uncoveredbccount={}
        totalbccount={}
        output+='build,module,source,function\n'
        modulefiles={}
        for line in lines:
            tokens=self.mysplit(line)
            if len(tokens)<6:
                continue
            # check if file name contains extra directories
            if tokens[1].find('repo/')>-1:
                tokens[1]=tokens[1][tokens[1].find('repo/')+5:]
            if tokens[1].find('tamarin-redux/')>-1:
                tokens[1]=tokens[1][tokens[1].find('tamarin-redux/')+14:]
            # set module to name before '/'
            module=''
            dirs=re.findall('[A-Za-z0-9_.-]+',tokens[1])
            if len(dirs)>1:
                module=dirs[0]
            else:
                continue
            # skip modules for third-party apis
            if module in self.skips:
                continue
            # check if function is not covered
            if tokens[3]=='0':
                output+='%s,%s,%s,"%s"\n'%(self.build,module,tokens[1],tokens[0])
                if uncoveredfncount.has_key(module)==False:
                    uncoveredfncount[module]=0
                uncoveredfncount[module]+=1
            # add to function total
            if totalfncount.has_key(module)==False:
                totalfncount[module]=0
            totalfncount[module]+=1
            # add to b/c covered
            if uncoveredbccount.has_key(module)==False:
                uncoveredbccount[module]=0
            uncoveredbccount[module]+=int(tokens[4])
            # add to b/c total
            if totalbccount.has_key(module)==False:
                totalbccount[module]=0
            totalbccount[module]+=int(tokens[5])
        # flip b/c totals to make uncovered
        for module in uncoveredbccount.keys():
            uncoveredbccount[module]=totalbccount[module]-uncoveredbccount[module]
        return output, uncoveredfncount, totalfncount, uncoveredbccount,totalbccount
        

    def saveCSV(self,csvdata):
        print('saving csv data to %s' % self.missingfnfile)
        open(self.missingfnfile,'w').write(csvdata)

    def appendData(self,filename,uncovered,total):
        print('appending results to %s' % filename)
        uncoveredsum=0
        totalsum=0
        for module in self.modulelist:
            if uncovered.has_key(module)==False:
                if total.has_key(module)==False:
                    print('WARNING: module %s is not in module list' % module)
                continue
            uncoveredsum+=uncovered[module]
            totalsum+=total[module]
        percent=self.calcpercent(uncoveredsum,totalsum)
        if os.path.exists(filename)==False:
            print("ERROR: file %s does not exist, generating new file")
            contents="build\ncore\nnanojit\nMMgc\ngenerated\nplatform\nextensions\nVMPI\nvmbase"
        else:
            contents=open(filename).read()
        newcontents=''
        lines=contents.split('\n')
        for line in lines:
            if line=='':
                continue
            tokens=line.split(',')
            if tokens[0]=='build':
                value=self.build
            elif tokens[0] in self.modulelist==False:
                value="0"
            elif tokens[0]=='total':
                value=percent
            else:
                if uncovered.has_key(tokens[0])==False:
                    uncov=0
                else:
                    uncov=uncovered[tokens[0]]
                value=self.calcpercent(uncov,total[tokens[0]])
            newcontents+="%s,%s\n" % (line,value)
        f=open(filename,'w')
        f.write(newcontents)
        f.close()

    def generateSummaryTableFn(self,fnuncovered,fntotal):
        print('generating fn summary data...')
        if os.path.exists(self.summaryfn):
            os.unlink(self.summaryfn)
        fnuncoveredsum=0
        fntotalsum=0
        contents='module,uncovered functions,total functions,% function coverage\n'
        for module in self.modulelist:
            if fnuncovered.has_key(module)==False:
                continue
            fnuncoveredsum+=fnuncovered[module]
            fntotalsum+=fntotal[module]
            contents+='%s,%s,%s,%s%s\n' % (module,fnuncovered[module],fntotal[module],self.calcpercent(fnuncovered[module],fntotal[module]),'%')
        contents+='total,%s,%s,%s%s\n' % (fnuncoveredsum,fntotalsum,self.calcpercent(fnuncoveredsum,fntotalsum),'%')
        open(self.summaryfn,'w').write(contents)
        open(self.info,'w').write('current build: %s,function coverage: %s%s' % 
                                 (self.build,
                                  self.calcpercent(fnuncoveredsum,fntotalsum),'%')
                                 )

    def generateSummaryTableBc(self,bcuncovered,bctotal):
        print('generating bc summary data...')
        if os.path.exists(self.summarybc):
            os.unlink(self.summarybc)
        bcuncoveredsum=0
        bctotalsum=0
        contents='module,uncovered branches,total branches,% branch coverage\n'
        for module in self.modulelist:
            if bcuncovered.has_key(module)==False:
                continue
            bcuncoveredsum+=bcuncovered[module]
            bctotalsum+=bctotal[module]
            contents+='%s,%s,%s,%s%s\n' % (module,bcuncovered[module],bctotal[module],self.calcpercent(bcuncovered[module],bctotal[module]),'%')
        contents+='total,%s,%s,%s%s\n' % (bcuncoveredsum,bctotalsum,self.calcpercent(bcuncoveredsum,bctotalsum),'%')
        open(self.summarybc,'w').write(contents)
        open(self.info,'a').write(',branch coverage: %s%s,updated: %s' % 
                                 (self.calcpercent(bcuncoveredsum,bctotalsum),'%',
                                  datetime.datetime.today().strftime('%Y-%m-%d %H:%M'))
                                 )

    def generateSummary(self, file, modulescount):
        print('generating summary %s...' % file)

        if os.path.exists(file)==False:
            summaryFile=open(file,'w')
            summaryFile.write('build\n')
            for module in self.modulelist:
                summaryFile.write('%s\n' % module)
            summaryFile.close()

        modules=modulescount.keys()
        modulefile=open(file).read()
        modulefilelistupdated=''
        fieldslen=0
        for line in modulefile.split('\n'):
            if line=='':
                continue
            # check if historical builds exceed max
            tokens=line.split(',')
            if len(tokens)>self.maxBuilds:
                first=tokens[0]
                tokens=tokens[(len(tokens)-self.maxBuilds+1):]
                tokens.insert(0,first)
                line=",".join(tokens)
            if line.startswith('build'):
                modulefileupdated=line+','+self.build+'\n'
            else:
                fields=line.split(',')
                fieldslen=len(fields)
                if modulescount.has_key(fields[0]):
                    modulefileupdated+="%s,%s\n" % (line,modulescount[fields[0]])
                    modules.remove(fields[0])

        for newmodule in modules:
            line=newmodule
            for i in range(fieldslen-1):
                line+=',0'
            line='%s,%s\n' % (line,modulescount[newmodule])
            modulefileupdated+=line
        open(file,'w').write(modulefileupdated)

    def generatePercentSummary(self, file, modulescount, totalmodulescount):
        print('generating percent function summary data...')

        if os.path.exists(file)==False:
            summaryFile=open(file,'w')
            summaryFile.write('build\n')
            for module in self.modulelist:
                summaryFile.write('%s\n' % module)
            summaryFile.close()
        modulefile=open(file).read()
        modulefilelistupdated=''
        fieldslen=0
        modules=modulescount.keys()
        for line in modulefile.split('\n'):
            if line=='':
                continue
            # check if historical builds exceed max
            tokens=line.split(',')
            if len(tokens)>self.maxBuilds:
                first=tokens[0]
                tokens=tokens[(len(tokens)-self.maxBuilds+1):]
                tokens.insert(0,first)
                line=",".join(tokens)
            if line.startswith('build'):
                modulefileupdated=line+','+self.build+'\n'
            else:
                fields=line.split(',')
                fieldslen=len(fields)
                if modulescount.has_key(fields[0]):
                    modulefileupdated+="%s,%s\n" % (line,self.calcpercent(modulescount[fields[0]],totalmodulescount[fields[0]]))
                    modules.remove(fields[0])
        for newmodule in modules:
            line=newmodule
            for i in range(fieldslen-1):
                line+=',0'
            line='%s,%s\n' % (line,self.calcpercent(modulescount[newmodule],totalmodulescount[newmodule]))
            modulefileupdated+=line
        open(file,'w').write(modulefileupdated)
        
    def calcpercent(self,value,total):
        value=float(value)
        total=float(total)
        pct=(total-value)*100.0/total
        return '%.1f' % pct

    # workaround since python split does not allow " to span multiple tokens
    # "func1(param1,param)","foo","foo" would not split correctly
    def mysplit(self,line):
        tokens=[]
        while True:
            if len(line)==0:
                break
            if line[0]=='"':
                line=line[1:]
                if line.find('"')==-1:
                    tokens.append(line)
                    break
                tokens.append(line[0:line.find('"')])
                line=line[line.find('"')+2:]
            else:
                if line.find(',')==-1:
                    tokens.append(line)
                    break
                else:
                    tokens.append(line[0:line.find(',')])
                    line=line[line.find(',')+1:]
        return tokens 

if __name__ == '__main__':
    p = ParseCodeCoverage()
