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
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

public class SwfStrip
{
	static class AbcStripper extends DelegatingABCVisitor
	{
		Set<String> _symClassEntries;
		
		public AbcStripper(IABCVisitor inner, Set<String> symClassEntries)
		{
			super(inner);
			_symClassEntries = symClassEntries;
		}

		static class _MetadataVisitor extends DelegatingMetadataVisitor
		{
			boolean _correspondsToSymClassEntry;
			
			public _MetadataVisitor(IMetadataVisitor inner, boolean correspondsToSymClassEntry)
			{
				super(inner);
				_correspondsToSymClassEntry = correspondsToSymClassEntry;
			}

			@Override
			public void visit(Metadata md) {
				String name = md.getName();
				
				// only strip our stuff
				if(name.equals("Csym") ||
						name.equals("Weak") ||
						name.equals("GlobalMethod") ||
						name.equals("LinkABC") ||
						(name.equals("HexData") && _correspondsToSymClassEntry)) // only strip HexData if it's on a class we saw in a SymbolClass
					return;
				super.visit(md);
			}
		}
		
		static class _TraitVisitor extends DelegatingTraitVisitor
		{
			boolean _correspondsToSymClassEntry;
			
			public _TraitVisitor(ITraitVisitor inner, boolean correspondsToSymClassEntry)
			{
				super(inner);
				_correspondsToSymClassEntry = correspondsToSymClassEntry;
			}

			@Override
			public IMetadataVisitor visitMetadata(int arg0) {
				return new _MetadataVisitor(super.visitMetadata(arg0), _correspondsToSymClassEntry);
			}
		}

		static class _TraitsVisitor extends DelegatingTraitsVisitor
		{
			Set<String> _symClassEntries;
			
			public _TraitsVisitor(ITraitsVisitor inner, Set<String> symClassEntries)
			{
				super(inner);
				_symClassEntries = symClassEntries;
			}

			@Override
			public ITraitVisitor visitClassTrait(int arg0, Name name,
					int arg2, ClassInfo arg3) {
				return new _TraitVisitor(super.visitClassTrait(arg0, name, arg2, arg3),
						_symClassEntries != null && _symClassEntries.contains(name.getSingleQualifier().getName() + "." + name.getBaseName()));
			}

			@Override
			public ITraitVisitor visitMethodTrait(int arg0, Name arg1,
					int arg2, MethodInfo arg3) {
				return new _TraitVisitor(super.visitMethodTrait(arg0, arg1, arg2, arg3), false);
			}

			@Override
			public ITraitVisitor visitSlotTrait(int arg0, Name arg1,
					int arg2, Name arg3, Object arg4) {
				return new _TraitVisitor(super.visitSlotTrait(arg0, arg1, arg2, arg3, arg4), false);
			}
		}

		static class _ClassVisitor extends DelegatingClassVisitor
		{
			public _ClassVisitor(IClassVisitor delegate)
			{
				super(delegate);
			}

			@Override
			public ITraitsVisitor visitClassTraits() {
				return new _TraitsVisitor(super.visitClassTraits(), null);
			}

			@Override
			public ITraitsVisitor visitInstanceTraits() {
				return new _TraitsVisitor(super.visitInstanceTraits(), null);
			}
		}
		
		@Override
		public IClassVisitor visitClass(InstanceInfo arg0, ClassInfo arg1) {
			return new _ClassVisitor(super.visitClass(arg0, arg1));
		}

		static class _MethodBodyVisitor extends DelegatingMethodBodyVisitor
		{
			public _MethodBodyVisitor(IMethodBodyVisitor delegate)
			{
				super(delegate);
			}

			@Override
			public ITraitsVisitor visitTraits() {
				return new _TraitsVisitor(super.visitTraits(), null);
			}
		}
		
		static class _MethodVisitor extends DelegatingMethodVisitor
		{
			public _MethodVisitor(IMethodVisitor delegate)
			{
				super(delegate);
			}

			@Override
			public IMethodBodyVisitor visitBody(MethodBodyInfo arg0) {
				return new _MethodBodyVisitor(super.visitBody(arg0));
			}
		}
		
		@Override
		public IMethodVisitor visitMethod(MethodInfo arg0) {
			return new _MethodVisitor(super.visitMethod(arg0));
		}

		static class _ScriptVisitor extends DelegatingScriptVisitor
		{
			Set<String> _symClassEntries;
			
			public _ScriptVisitor(IScriptVisitor delegate, Set<String> symClassEntries)
			{
				super(delegate);
				_symClassEntries = symClassEntries;
			}

			@Override
			public ITraitsVisitor visitTraits() {
				return new _TraitsVisitor(super.visitTraits(), _symClassEntries);
			}
		}
		
		@Override
		public IScriptVisitor visitScript() {
			return new _ScriptVisitor(super.visitScript(), _symClassEntries);
		}
	}
	
	public static SWF process(SWF inSWF) throws Exception
	{
		SWFHeader header = inSWF.header;
		SWFRecord[] records = inSWF.records;
		Set<String> symClassEntries = new HashSet<String>();

		// find SymbolClass blocks and record registered classes
		for(int i = 0; i < records.length; i++)
		{
			SWFRecord record = records[i];

			if(record.tag == 76) // SymbolClass
			{
				ByteArrayInputStream bis = new ByteArrayInputStream(record.payload);
				int numEntries = bis.read() | bis.read() << 8;
				
				while(numEntries-- != 0)
				{
					bis.read(); // ignore char id
					bis.read();
					ByteArrayOutputStream bos = new ByteArrayOutputStream();
					int ch;
					
					while((ch = bis.read()) > 0)
						bos.write(ch);
					String name = new String(bos.toByteArray(), Charset.forName("UTF-8"));
					symClassEntries.add(name);
				}
			}
		}
		
		ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();
		
		// transcode			
		for(int i = 0; i < records.length; i++)
		{
			SWFRecord record = records[i];

			if(record.tag == 82) // abc
			{
				int dataOffset = 4;
				
				while(record.payload[dataOffset] != 0) // skip past flags+name
					dataOffset++;
				dataOffset++;
				byte[] inABC = new byte[record.payload.length - dataOffset];
				System.arraycopy(record.payload, dataOffset, inABC, 0, record.payload.length - dataOffset);
				ABCEmitter emitter = new ABCEmitter();
				emitter.setAllowBadJumps(true);
				AbcStripper stripper = new AbcStripper(emitter, symClassEntries);
				ABCParser parser = new ABCParser(inABC);
				parser.parseABC(stripper);
				byte[] outABC = emitter.emit();
				byte[] payload = new byte[outABC.length + dataOffset];
				System.arraycopy(record.payload, 0, payload, 0, dataOffset);
				System.arraycopy(outABC, 0, payload, dataOffset, outABC.length);
				newRecords.add(new SWFRecord((short)82, payload));
			}
			else
				newRecords.add(record);
		}
		
		// build output swf
		return new SWF(header, newRecords.toArray(new SWFRecord[0]));			
	}
}
