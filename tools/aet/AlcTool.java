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

import java.io.*;
import java.util.*;
import java.util.zip.*;
import java.util.regex.Pattern;

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

import flascc.*;

public class AlcTool {
    public static void usage() {
        System.err.println("This tool is an internal part of the flascc SDK");
        System.err.println("You shouldn't be invoking it directly");
        System.exit(-1);
    }

    public static void invalidSWF() {
        System.err.println("Invalid SWF");
        System.exit(-1);
    }

    public static void main(String[] args) {
        if (args.length == 0 || args[0].equals("-h") || args[0].equals("-help")) {
            usage();
        }

        try {
            if (args[0].equals("-abcnm")) {
                abcnm(args);
            } else if (args[0].equals("-swfstrip")) {
                swfstrip(args);
            } else if (args[0].equals("-swcgen")) {
                swcgen(args);
            } else if (args[0].equals("-swfresolve")) {
                swfresolve(args);
            } else if (args[0].equals("-nopabc")) {
                nopabc(args);
            } else if (args[0].equals("-nopswf")) {
                nopswf(args);
            } else if (args[0].equals("-slimit")) {
                slimit(args);
            } else if (args[0].equals("-stackify")) {
                stackify(args);
            } else {
                usage();
            }
        } catch (Exception e) {
            e.printStackTrace(System.err);
            System.exit(-1);
        }
    }

    public static byte[] getBytesFromFile(File file) throws IOException {
        InputStream is = new FileInputStream(file);
        long length = file.length();
        byte[] bytes = new byte[(int) length];
        int offset = 0;
        int numRead = 0;
        while (offset < bytes.length
                && (numRead = is.read(bytes, offset, bytes.length - offset)) >= 0) {
            offset += numRead;
        }
        is.close();
        return bytes;
    }

    // usage: alctool.jar -abcnm [-s | abcfile]
    public static void abcnm(String[] args) throws IOException {
        if (args.length > 2) {
            usage();
        }

        if (args.length == 1) // read abc from stdin
        {
            ByteArrayOutputStream bas = new ByteArrayOutputStream();
            byte[] chunk = new byte[0x100000];
            int read;

            while ((read = System.in.read(chunk)) > 0)
                bas.write(chunk, 0, read);

            ABCParser parser = new ABCParser(bas.toByteArray());
            bas.reset();
            parser.parseABC(new AbcNM());
        } else if (args[1].equals("-s")) {
            // act as a "server" for handling multiple abcs

            // client writes 0 or more abc data "chunks" by writing a
            // network-ordered 32-bit chunk size
            // followed by that number of bytes of abc data

            // if the chunk size is 0, that indicates that previous chunks
            // comprise a whole abc
            // and the server should process it and write output for it,
            // followed by an empty line
            // to signal the end of output for that abc

            // rinse, repeat!

            DataInputStream dis = new DataInputStream(System.in);
            ByteArrayOutputStream bas = new ByteArrayOutputStream();

            for (;;) {
                int chunkSize;

                try {
                    chunkSize = dis.readInt();
                } catch (EOFException e) {
                    break; // stdin close here == done, clean exit
                }

                if (chunkSize == 0) // end of an abc
                {
                    ABCParser parser = new ABCParser(bas.toByteArray());
                    bas.reset();
                    parser.parseABC(new AbcNM());
                    // empty line to signal end of response for this abc
                    System.out.print("\n");
                    continue;
                }

                // accumulate bytes!
                byte[] chunk = new byte[chunkSize];
                dis.readFully(chunk);
                bas.write(chunk);
            }
        } else // read from file
        {
            byte[] data;
            File file = new File(args[1]);
            FileInputStream finput = new FileInputStream(file);
            data = new byte[(int) file.length()];

            if (data.length != finput.read(data)) {
                System.err.println("failed to read file " + args[1]);
                System.exit(-1);
            }

            ABCParser parser = new ABCParser(data);
            parser.parseABC(new AbcNM());
        }
    }

    // usage: alctool.jar -swcgen -ns namespace -i library.swf -o catalog.xml
    public static void swcgen(String[] args) throws Exception {
        OutputStream os = null;
        InputStream is = System.in;
        boolean compressed = true;
	   File inFile = null;
	   String namespace = null;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-ns"))
                namespace = args[++i];
            else if (args[i].equals("-i"))
                is = new FileInputStream(inFile = new File(args[++i]));
            else if (args[i].equals("-o"))
                os = new FileOutputStream(new File(args[++i]));
            else
                usage();
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();
        if (swf == null)
            invalidSWF();

        swf = SwcGen.genCatalog(os, swf, namespace);

	   FileOutputStream sos = new FileOutputStream(inFile);
	   swf.write(sos, true).close();
    }

    // usage: alctool.jar -stackify [-o out.swf] [in.swf]
    public static void stackify(String[] args) throws IOException, Exception {
        InputStream is = System.in;
        OutputStream os = System.out;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-o"))
                os = new FileOutputStream(new File(args[++i]));
            else
                is = new FileInputStream(new File(args[i]));
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();
        if (swf == null)
            invalidSWF();

        // destructive to input swf!
        swf = Stackify.process(swf);
        swf.write(os, true).close();
    }

    // usage: alctool.jar -swfstrip [-o out.swf] [in.swf] [--uncompressed]
    public static void swfstrip(String[] args) throws IOException, Exception {
        InputStream is = System.in;
        OutputStream os = System.out;
        boolean compressed = true;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-o"))
                os = new FileOutputStream(new File(args[++i]));
            else if (args[i].equals("--uncompressed"))
                compressed = false;
            else
                is = new FileInputStream(new File(args[i]));
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();
        if (swf == null)
            invalidSWF();

        // destructive to input swf!
        swf = SwfStrip.process(swf);
        swf.write(os, compressed).close();
    }

    static class LinkableAbcAnalysis extends CompleteAbcVisitor {
        public boolean linkable;

        @Override
        public void visitScriptTraitMetadata(Metadata md) {
            if (md.getName().equals("LinkABC"))
                linkable = true;
        }

        public static boolean shouldLinkAbc(byte[] bytes) {
            LinkableAbcAnalysis laa = new LinkableAbcAnalysis();
            new ABCParser(bytes).parseABC(laa);
            return laa.linkable;
        }
    }

    private static byte[] getAbcBytes(SWFRecord record) {
        if (record.tag == 82 || record.tag == 72) // DoABC
        {
            int dataOffset = 0;

            if (record.tag == 82) {
                dataOffset = 4;
                while (record.payload[dataOffset] != 0) {
                    // skip past flags+name
                    dataOffset++;
                }
                dataOffset++;
            }
            int len = record.payload.length - dataOffset;
            byte[] bytes = new byte[len];
            System.arraycopy(record.payload, dataOffset, bytes, 0, len);
            return bytes;
        }

        return null;
    }

    // FB 4.6 doesn't code-hint SWCs where private names have the empty string for a namespace
    static class EmptyNSFixer extends com.adobe.flash.abc.ABCParser.ConstantPoolTranslator
    {
        public EmptyNSFixer()
        {
        }

        @Override
        public Namespace translateNamespace(Namespace ns)
        {
            if(ns != null && ns.getKind() == ABCConstants.CONSTANT_PrivateNs && (ns.getName() == null || ns.getName().equals("")))
                return new Namespace(ABCConstants.CONSTANT_PrivateNs, "private");
            return ns;
        }

        @Override
        public Name translateName(Name name)
        {
            return name;
        }
    }

    public static SWF linkAbcs(SWF swf, Boolean optimize) throws IOException,
    Exception {
        // iterate over all records
        SWFHeader header = swf.header;
        SWFRecord[] records = swf.records;
        ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();
        Boolean abcslinked = false;

        for (int i = 0; i < records.length; i++) {
            SWFRecord record = records[i];
            if (record.tag == 82 || record.tag == 72) // DoABC
            {
                if (abcslinked)
                    continue;

                ArrayList<byte[]> abcs = new ArrayList<byte[]>();

                int firstdataOffset = 0;

                if (record.tag == 82) {
                    firstdataOffset = 4;
                    while (record.payload[firstdataOffset] != 0) {
                        // skip past flags+name
                        firstdataOffset++;
                    }
                    firstdataOffset++;
                }

                for (int j = i; j < records.length; j++) {
                    SWFRecord nextrecord = records[j];
                    if (nextrecord.tag == 82 || nextrecord.tag == 72) // DoABC
                    {
                        byte[] bytes = getAbcBytes(nextrecord);
                        abcs.add(bytes);
                    }
                }

                ABCLinker.ABCLinkerSettings linkerSettings = new ABCLinker.ABCLinkerSettings();
                linkerSettings.setOptimize(optimize);
                linkerSettings.setStripDebugOpcodes(optimize);
                linkerSettings.setKeepMetadata(null);
                byte[] linkedAbc = ABCLinker.linkABC(abcs,
                        ABCConstants.VERSION_ABC_MAJOR_FP10,
                        ABCConstants.VERSION_ABC_MINOR_FP10, linkerSettings);

                ABCEmitter emitter = new ABCEmitter();
                emitter.setAllowBadJumps(true);
                IABCVisitor stackify = new Stackify(emitter);
                ABCParser parser = new ABCParser(linkedAbc);
                parser.setConstantPoolTranslator(new EmptyNSFixer());
                parser.parseABC(stackify);
                linkedAbc = emitter.emit();
                abcs.clear();
                abcs.add(linkedAbc);
                linkedAbc = ABCLinker.linkABC(abcs,
                        ABCConstants.VERSION_ABC_MAJOR_FP10,
                        ABCConstants.VERSION_ABC_MINOR_FP10, linkerSettings);

                byte[] payload = new byte[firstdataOffset + linkedAbc.length];
                System.arraycopy(record.payload, 0, payload, 0, firstdataOffset);
                System.arraycopy(linkedAbc, 0, payload, firstdataOffset,
                        linkedAbc.length);
                newRecords.add(new SWFRecord((short) record.tag, payload));
                abcslinked = true;
            } else {
                newRecords.add(record);
            }
        }

        return swf = new SWF(header, newRecords.toArray(new SWFRecord[0]));
    }

    // usage: alctool.jar -swfresolve [-swc namespace] [-forceordering] [-swfstrip] [-o
    // out.swf/.swc] [in.swf] [-id charIDstart]
    public static void swfresolve(String[] args) throws IOException, Exception {
        InputStream is = System.in;
        String of = null;
        String swfpreloader = null;
        short startCharID = (short) 0xfff0; // TODO ensure unused?
        boolean forceordering = false;
        boolean makeswc = false;
        boolean swfstrip = false;
        boolean optimize = false;
	String replacementNS = null;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-o"))
                of = args[++i];
            else if (args[i].equals("-forceordering"))
                forceordering = true;
            else if (args[i].equals("-optimize"))
                optimize = true;
            else if (args[i].equals("-swc"))
	    {
                makeswc = true;
		replacementNS = args[++i];
	    }
            else if (args[i].equals("-swfns"))
              replacementNS = args[++i];
            else if (args[i].equals("-swfstrip"))
                swfstrip = true;
            else if (args[i].equals("-swfpreloader"))
                swfpreloader = args[++i];
            else if (args[i].equals("-id"))
                startCharID = (short) Integer.parseInt(args[++i]);
            else
                is = new FileInputStream(new File(args[i]));
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();
        if (swf == null)
            invalidSWF();

        // destructive to input swf!
        swf = SwfResolve.process(swf, startCharID, forceordering, replacementNS);

        if (swfstrip)
            swf = SwfStrip.process(swf);

        swf = linkAbcs(swf, optimize);

        if(swfpreloader != null) {
            SWF preloader = SWF.read(new FileInputStream(new File(swfpreloader)));
            ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();

            for(int i = 0; i < preloader.records.length; i++) {
                SWFRecord record = preloader.records[i];
                if(record.tag != 0)
                    newRecords.add(record);
            }
            int symcount = 0;
            for(int i = 0; i < swf.records.length; i++) {
                SWFRecord record = swf.records[i];
                if(record.tag == 69)
                    continue;
                if(record.tag == 76 && (++symcount) == 2)
                    continue;

                newRecords.add(record);
            }

            swf = new SWF(swf.header, newRecords.toArray(new SWFRecord[0]));
        }

        OutputStream os = of == null ? System.out : new FileOutputStream(new File(of));
        if (makeswc) {
            ByteArrayOutputStream bos;
            ZipOutputStream zos = new ZipOutputStream(os);

            zos.putNextEntry(new ZipEntry("catalog.xml"));
            bos = new ByteArrayOutputStream();
            swf = SwcGen.genCatalog(bos, swf, null);
            zos.write(bos.toByteArray());

            zos.putNextEntry(new ZipEntry("library.swf"));
            bos = new ByteArrayOutputStream();
            swf.write(bos, true).close();
            zos.write(bos.toByteArray());

            File asdocsdir = new File(new File(new File(AS3Wig.class.getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParentFile(), "share"), "asdocs");

            zos.putNextEntry(new ZipEntry("docs/"));
            for(File f : asdocsdir.listFiles()) {
                 if (!f.isDirectory()) {
                    String adobens = Pattern.quote(FlasccNamespaceTranslator.flasccNS);

                    zos.putNextEntry(new ZipEntry("docs/" + f.getName().replaceAll(adobens, replacementNS)));
                    RandomAccessFile docfile = new RandomAccessFile(f, "r");
                    byte[] b = new byte[(int)docfile.length()];
                    docfile.read(b);

                    String s = new String(b);
                    s = s.replaceAll(adobens, replacementNS);
                    b = s.getBytes("UTF-8");

                    zos.write(b);
                 }
            }

            zos.close();
        } else {
            swf.write(os, true).close();
        }
    }

    // usage: alctool.jar -slimit [-o out.swf] [in.swf]
    public static void slimit(String[] args) throws IOException, Exception {
        InputStream is = System.in;
        String of = null;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-o"))
                of = args[++i];
            else
                is = new FileInputStream(new File(args[i]));
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();
        if (swf == null)
            invalidSWF();

        // iterate over all records
        SWFHeader header = swf.header;
        SWFRecord[] records = swf.records;
        ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();

        for (int i = 0; i < records.length; i++) {
            SWFRecord record = records[i];
            records[i] = null;
            newRecords.add(record);

            if (i == 0) {
                newRecords.add(new SWFRecord((short) 65, new byte[] {
                        (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF }));
            }
        }

        // build output swf
        swf = new SWF(header, newRecords.toArray(new SWFRecord[0]));
        OutputStream os = of == null ? System.out : new FileOutputStream(
                new File(of));
        swf.write(os, true).close();
    }

    // usage: alctool.jar -nopabc [-o out.abc] [in.abc]
    public static void nopabc(String[] args) throws IOException, Exception {
        File is = null;
        String of = null;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-o"))
                of = args[++i];
            else
                is = new File(args[i]);
        }

        byte[] bytes = getBytesFromFile(is);
        ABCEmitter emitter = new ABCEmitter();
        emitter.setAllowBadJumps(true);
        ABCParser parser = new ABCParser(bytes);
        parser.parseABC(emitter);
        byte[] newbytes = emitter.emit();

        OutputStream os = of == null ? System.out : new FileOutputStream(
                new File(of));
        os.write(newbytes);
        os.close();
    }

    // usage: alctool.jar -nopswf [-o out.swf] [in.swf]
    public static void nopswf(String[] args) throws IOException, Exception {
        InputStream is = System.in;
        String of = null;

        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("-o"))
                of = args[++i];
            else
                is = new FileInputStream(new File(args[i]));
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();
        if (swf == null)
            invalidSWF();

        // iterate over all records
        SWFHeader header = swf.header;
        SWFRecord[] records = swf.records;
        ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();

        for (int i = 0; i < records.length; i++) {
            SWFRecord record = records[i];
            records[i] = null;

            if (record.tag == 82) // DoABC
            {
                int dataOffset = 4;

                while (record.payload[dataOffset] != 0) {
                    // skip past flags+name
                    dataOffset++;
                }
                dataOffset++;
                int len = record.payload.length - dataOffset;

                byte[] bytes = new byte[len];
                System.arraycopy(record.payload, dataOffset, bytes, 0, len);
                ABCEmitter emitter = new ABCEmitter();
                emitter.setAllowBadJumps(true);
                ABCParser parser = new ABCParser(bytes);
                parser.parseABC(emitter);
                byte[] abcBytes = emitter.emit();

                byte[] payload = new byte[dataOffset + abcBytes.length];
                System.arraycopy(record.payload, 0, payload, 0, dataOffset);
                System.arraycopy(abcBytes, 0, payload, dataOffset,
                        abcBytes.length);
                newRecords.add(new SWFRecord((short) 82, payload));
            } else {
                newRecords.add(record);
            }
        }

        // build output swf
        swf = new SWF(header, newRecords.toArray(new SWFRecord[0]));
        OutputStream os = of == null ? System.out : new FileOutputStream(
                new File(of));
        swf.write(os, true).close();
    }
}
