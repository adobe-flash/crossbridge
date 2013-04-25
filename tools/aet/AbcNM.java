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

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

// simple command line utility that produces nm-like output when run against
// flascc-generated abc files
public class AbcNM extends NilABCVisitor {

    static class NMMetadataVisitor extends NilMetadataVisitor {
        @Override
        public void visit(Metadata md) {
            String name = md.getName();
            if (name.equals("Csym")) {
                String[] values = md.getValues();
                String symType = values[0];
                String symName = values[1];

                System.out.print("         " + symType + " " + symName + "\n");
            }
        }
    }

    static class NMTraitVisitor extends NilTraitVisitor {
        @Override
        public IMetadataVisitor visitMetadata(int arg0) {
            return new NMMetadataVisitor();
        }
    }

    static class NMTraitsVisitor extends NilTraitsVisitor {
        @Override
        public ITraitVisitor visitClassTrait(int arg0, Name arg1, int arg2,
                ClassInfo arg3) {
            return new NMTraitVisitor();
        }

        @Override
        public ITraitVisitor visitMethodTrait(int arg0, Name arg1, int arg2,
                MethodInfo arg3) {
            return new NMTraitVisitor();
        }

        @Override
        public ITraitVisitor visitSlotTrait(int arg0, Name arg1, int arg2,
                Name arg3, Object arg4) {
            return new NMTraitVisitor();
        }
    }

    static class NMScriptVisitor extends NilScriptVisitor {
        @Override
        public ITraitsVisitor visitTraits() {
            return new NMTraitsVisitor();
        }
    }

    @Override
    public IScriptVisitor visitScript() {
        return new NMScriptVisitor();
    }

    @Override
    public IMethodVisitor visitMethod(MethodInfo minfo) {
        return null; // FIXME needed to avoid exceptions when seeing MOPs
        // opcodes
    }
}
