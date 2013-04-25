// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package flascc;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.PrintWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;
import java.util.Date;

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

import flascc.FlasccNamespaceTranslator;

import static com.adobe.flash.abc.ABCConstants.*;

public class SwcGen extends NilABCVisitor {
    private String curprefix = "";
    public ArrayList<String> defs = new ArrayList<String>();
    private Boolean inClass = false;

    final class _MethodBodyVisitor extends NilMethodBodyVisitor {
        @Override
        public int visitException(Label arg0, Label arg1, Label arg2,
                Name arg3, Name arg4) {
            return 0;
        }
    }

    final class _MethodVisitor extends NilMethodVisitor {
        @Override
        public IMethodBodyVisitor visitBody(MethodBodyInfo arg0) {
            return new _MethodBodyVisitor();
        }
    }

    boolean ignoreName(Name n) {
        Namespace ns = n.getQualifiers().getSingleQualifier();

        if (ns.getName() != null && ns.getName().startsWith("__force_ordering_ns"))
            return false;

        return ns.getKind() != CONSTANT_PackageNs;
    }

    final class _TraitsVisitor extends NilTraitsVisitor {
        @Override
        public ITraitVisitor visitClassTrait(int arg0, Name name, int arg2,
                ClassInfo arg3) {
            if (name.getKind() == ABCConstants.CONSTANT_Qname
                    || name.getKind() == ABCConstants.CONSTANT_QnameA) {
                if (ignoreName(name))
                    return null;
                String nm = name.getQualifiers().getSingleQualifier().getName();
                if (nm.length() > 0)
                    nm += ":";
                defs.add(nm + name.getBaseName());
            }
            return null;
        }

        @Override
        public ITraitVisitor visitMethodTrait(int arg0, Name name, int arg2,
                MethodInfo arg3) {
            if(inClass)
                return null;

            if (name.getKind() == ABCConstants.CONSTANT_Qname
                    || name.getKind() == ABCConstants.CONSTANT_QnameA) {
                if (ignoreName(name))
                    return null;
                String ns = name.getQualifiers().getSingleQualifier().getName();
                if (ns.length() > 0) {
                    ns = ns.replace(':', '.');
                    ns += ":";
                } else {
                    ns = curprefix;
                }

                defs.add(ns + name.getBaseName());
            }
            return null;
        }

        @Override
        public ITraitVisitor visitSlotTrait(int arg0, Name name, int arg2,
                Name arg3, Object arg4) {
            if(inClass)
                return null;
            
            if (name.getKind() == ABCConstants.CONSTANT_Qname
                    || name.getKind() == ABCConstants.CONSTANT_QnameA) {
                String ns = name.getQualifiers().getSingleQualifier().getName();

                if (ignoreName(name))
                    return null;

                if (ns.length() > 0) {
                    ns = ns.replace(':', '.');
                    ns += ":";
                } else {
                    ns = curprefix;
                }

                defs.add(ns + name.getBaseName());
                // System.err.println(ns + name.getBaseName());
            }

            return null;
        }
    }

    final class _ClassVisitor extends NilClassVisitor {

        @Override
        public void visitEnd() {
            inClass = false;
        }

        @Override
        public ITraitsVisitor visitClassTraits() {
            inClass = true;
            return new _TraitsVisitor();
        }
    }

    final class _ScriptVisitor extends NilScriptVisitor {
        @Override
        public ITraitsVisitor visitTraits() {
            return new _TraitsVisitor();
        }
    }

    @Override
    public IClassVisitor visitClass(InstanceInfo iinfo, ClassInfo cinfo) {
        curprefix = iinfo.name.getBaseName();

        if (curprefix.length() > 0)
            curprefix += ":";

        return new _ClassVisitor();
    }

    @Override
    public IScriptVisitor visitScript() {
        curprefix = "";
        return new _ScriptVisitor();
    }

    @Override
    public IMethodVisitor visitMethod(MethodInfo arg0) {
        return new _MethodVisitor();
    }

    public static SWF genCatalog(OutputStream _os, SWF swf, String namespace) throws Exception {
        PrintWriter os = new PrintWriter(_os);
        os.println("<?xml version=\"1.0\" encoding =\"utf-8\"?>"
                + "<swc xmlns=\"http://www.adobe.com/flash/swccatalog/9\">"
                + "<versions>"
                + "<swc version=\"1.2\" />"
                + "<flex version=\"4.1.0\" build=\"16076\" minimumSupportedVersion=\"3.0.0\" />"
                + "</versions>" + "<features>" + "<feature-script-deps />"
                + "<feature-files />" + "</features>" + "<libraries>"
                + "<library path=\"library.swf\">\n");

        ArrayList<String> deps = new ArrayList<String>();
        deps.add("Object");
        deps.add("XML");
        deps.add("flash.utils:ByteArray");
        deps.add("flash.utils:Dictionary");
        deps.add("flash.utils:IDataInput");
        deps.add("flash.utils:IDataOutput");
        deps.add("flash.events:Event");
        deps.add("flash.display:DisplayObjectContainer");
        deps.add("flash.text:TextField");

        ArrayList<String> extradeps = new ArrayList<String>();

        int synthdep = 0;

        int abccount = 0;
        for (SWFRecord record : swf.records) {
            if (record.tag == 82) // abc
            {
                abccount++;
            }
        }

        SWFRecord records[] = swf.records;
        SWFRecord newRecords[] = (namespace == null) ? null : new SWFRecord[records.length]; // will be same length

        for(int i = 0; i < records.length; i++)
        {
            SWFRecord record = records[i];

            if (record.tag == 82) // abc
            {
                int dataOffset = 4;
                StringBuffer abcname = new StringBuffer();
                while (record.payload[dataOffset] != 0)
                    // skip past flags+name
                    abcname.append((char) record.payload[dataOffset++]);
                dataOffset++;
                byte[] abcdata = new byte[record.payload.length - dataOffset];
                System.arraycopy(record.payload, dataOffset, abcdata, 0,
                        record.payload.length - dataOffset);

                ABCParser parser;

		if(namespace != null) // rewrite it!
		{
			ABCEmitter emitter = new ABCEmitter();
			parser = new ABCParser(abcdata);
			parser.setConstantPoolTranslator(new FlasccNamespaceTranslator(namespace));
       	        	parser.parseABC(emitter);
			abcdata = emitter.emit();
                        byte[] payload = new byte[abcdata.length + dataOffset];
                        System.arraycopy(record.payload, 0, payload, 0, dataOffset);
                        System.arraycopy(abcdata, 0, payload, dataOffset, abcdata.length);
			newRecords[i] = new SWFRecord((short)82, payload);
		}

                SwcGen stv = new SwcGen();

		parser = new ABCParser(abcdata);
                parser.parseABC(stv);

                os.println("<script name=\"" + abcname.toString() + "\" mod=\""
                        + (new Date()).getTime() + "\">");
                for (String def : stv.defs) {
                    os.println("<def id=\"" + def + "\"/>");
                }

                for (String dep : deps) {
                    os.println("<dep id=\"" + dep + "\" type=\"i\"/>");
                }

                for (String dep : extradeps) {
                    os.println("<dep id=\"" + dep + "\" type=\"i\"/>");
                }

                deps.clear();
                deps.add("Object");
                deps.add("XML");
                deps.add("flash.utils:ByteArray");
                deps.add("flash.utils:Dictionary");
                deps.add("flash.utils:IDataInput");
                deps.add("flash.utils:IDataOutput");
                deps.add("flash.events:Event");
                deps.add("flash.display:DisplayObjectContainer");
                deps.add("flash.text:TextField");
                deps.addAll(stv.defs);

                if (stv.defs.size() > 0)
                    extradeps.add(stv.defs.get(0));

                os.println("</script>");
            }
            else if(namespace != null)
                newRecords[i] = record;

        }
        os.println("</library></libraries></swc>");
	os.close();
	return (namespace == null) ? swf : new SWF(swf.header, newRecords);
    }
}
