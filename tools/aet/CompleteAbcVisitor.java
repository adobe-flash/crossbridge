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

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

import static com.adobe.flash.abc.ABCConstants.*;

public class CompleteAbcVisitor extends NilABCVisitor {
    // client API

    public void visitMethodStart(MethodInfo minfo) {
    }

    public void visitMethodEnd(MethodInfo minfo) {
    }

    public void visitMethodBodyStart(MethodBodyInfo mbinfo) {
    }

    public void visitMethodBodyEnd(MethodBodyInfo mbinfo) {
    }

    public int visitException(Label arg0, Label arg1, Label arg2, Name arg3,
            Name arg4) {
        return 0;
    }

    public void visitInstruction(int arg0) {
    }

    public void visitInstruction(int arg0, int arg1) {
    }

    public void visitInstruction(int op, Object arg) {
    }

    public void visitScriptTraitsStart() {
    }

    public void visitScriptTraitsEnd() {
    }

    public void visitScriptTraitMetadata(Metadata md) {
    }

    public void visitScriptTraitEnd() {
    }

    public void visitScriptInit(MethodInfo arg0) {
    }

    public void visitScriptStart() {
    }

    public void visitScriptEnd() {
    }

    public void visitClassTrait(int arg0, Name arg1, int arg2, ClassInfo arg3) {
    }

    public void visitMethodTrait(int arg0, Name arg1, int arg2, MethodInfo arg3) {
    }

    public void visitSlotTrait(int arg0, Name arg1, int arg2, Name arg3,
            Object arg4) {
    }

    // Visitor classes

    class _MethodBodyVisitor extends NilMethodBodyVisitor {
        MethodBodyInfo mbinfo;

        public _MethodBodyVisitor(MethodBodyInfo _mbinfo) {
            mbinfo = _mbinfo;
        }

        @Override
        public void visit() {
            CompleteAbcVisitor.this.visitMethodBodyStart(mbinfo);
        }

        @Override
        public void visitEnd() {
            CompleteAbcVisitor.this.visitMethodBodyEnd(mbinfo);
        }

        @Override
        public void visitInstruction(int arg0) {
            CompleteAbcVisitor.this.visitInstruction(arg0);
        }

        @Override
        public void visitInstruction(int arg0, int arg1) {
            CompleteAbcVisitor.this.visitInstruction(arg0, arg1);
        }

        @Override
        public void visitInstruction(int op, Object arg) {

        }

        @Override
        public int visitException(Label arg0, Label arg1, Label arg2,
                Name arg3, Name arg4) {
            return CompleteAbcVisitor.this.visitException(arg0, arg1, arg2,
                    arg3, arg4);
        }
    }

    class _MethodVisitor extends NilMethodVisitor {
        MethodInfo minfo;

        public _MethodVisitor(MethodInfo _minfo) {
            minfo = _minfo;
        }

        @Override
        public void visit() {
            CompleteAbcVisitor.this.visitMethodStart(minfo);
        }

        @Override
        public void visitEnd() {
            CompleteAbcVisitor.this.visitMethodEnd(minfo);
        }

        @Override
        public IMethodBodyVisitor visitBody(MethodBodyInfo mbinfo) {
            return new _MethodBodyVisitor(mbinfo);
        }
    }

    class _TraitVisitor extends NilTraitVisitor {
    }

    class _ScriptTraitMetadataVisitor extends NilMetadataVisitor {
        @Override
        public void visit(Metadata md) {
            CompleteAbcVisitor.this.visitScriptTraitMetadata(md);
        }
    }

    class _ScriptTraitVisitor extends NilTraitVisitor {
        @Override
        public IMetadataVisitor visitMetadata(int count) {
            return new _ScriptTraitMetadataVisitor();
        }

        @Override
        public void visitEnd() {
            CompleteAbcVisitor.this.visitScriptTraitEnd();
        }
    }

    class _ScriptTraitsVisitor extends NilTraitsVisitor {
        @Override
        public void visit() {
            CompleteAbcVisitor.this.visitScriptTraitsStart();
        }

        @Override
        public void visitEnd() {
            CompleteAbcVisitor.this.visitScriptTraitsEnd();
        }

        @Override
        public ITraitVisitor visitClassTrait(int arg0, Name arg1, int arg2,
                ClassInfo arg3) {
            CompleteAbcVisitor.this.visitClassTrait(arg0, arg1, arg2, arg3);
            return new _ScriptTraitVisitor();
        }

        @Override
        public ITraitVisitor visitMethodTrait(int arg0, Name arg1, int arg2,
                MethodInfo arg3) {
            CompleteAbcVisitor.this.visitMethodTrait(arg0, arg1, arg2, arg3);
            return new _ScriptTraitVisitor();
        }

        @Override
        public ITraitVisitor visitSlotTrait(int arg0, Name arg1, int arg2,
                Name arg3, Object arg4) {
            CompleteAbcVisitor.this
                    .visitSlotTrait(arg0, arg1, arg2, arg3, arg4);
            return new _ScriptTraitVisitor();
        }
    }

    class _ScriptVisitor extends NilScriptVisitor {
        @Override
        public void visit() {
            CompleteAbcVisitor.this.visitScriptStart();
        }

        @Override
        public void visitEnd() {
            CompleteAbcVisitor.this.visitScriptEnd();
        }

        @Override
        public void visitInit(MethodInfo arg0) {
            CompleteAbcVisitor.this.visitScriptInit(arg0);
        }

        @Override
        public ITraitsVisitor visitTraits() {
            return new _ScriptTraitsVisitor();
        }
    }

    // AbcVisitor method impls

    @Override
    public IScriptVisitor visitScript() {
        return new _ScriptVisitor();
    }

    @Override
    public IMethodVisitor visitMethod(MethodInfo minfo) {
        return new _MethodVisitor(minfo);
    }
}