# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

abcdis_asfiles := $(abcdis_asfiles)\
  -in $(curdir)/abcdis/Utils.as\
  -in $(curdir)/abcdis/IPrinter.as\
  -in $(curdir)/abcdis/ByteArrayPrinter.as\
  -in $(curdir)/abcdis/TablePrinter.as\
  -in $(curdir)/abcdis/IndentingPrinter.as\
  -in $(curdir)/abcdis/abc/AbcAS3Traversal.as\
  -in $(curdir)/abcdis/abc/AbcAPIWriter.as\
  -in $(curdir)/abcdis/abc/AbcAsmWriter.as\
  -in $(curdir)/abcdis/abc/AbcDumpWriter.as\
  -in $(curdir)/abcdis/abc/AbcFormatWriter.as\
  -in $(curdir)/abcdis/abc/ABCReader.as\
  -in $(curdir)/abcdis/abc/AVMType.as\
  -in $(curdir)/abcdis/abc/BasicBlock.as\
  -in $(curdir)/abcdis/abc/Constants.as\
  -in $(curdir)/abcdis/abc/DOTWriter.as\
  -in $(curdir)/abcdis/abc/FrameState.as\
  -in $(curdir)/abcdis/abc/Instruction.as\
  -in $(curdir)/abcdis/abc/IReader.as\
  -in $(curdir)/abcdis/abc/MethodReference.as\
  -in $(curdir)/abcdis/abc/Reader.as\
  -in $(curdir)/abcdis/abc/Types/ABCFile.as\
  -in $(curdir)/abcdis/abc/Types/ABCObject.as\
  -in $(curdir)/abcdis/abc/Types/ClassInfo.as\
  -in $(curdir)/abcdis/abc/Types/ConstantMultiname.as\
  -in $(curdir)/abcdis/abc/Types/ConstantNamespace.as\
  -in $(curdir)/abcdis/abc/Types/ConstantNamespaceSet.as\
  -in $(curdir)/abcdis/abc/Types/Exception.as\
  -in $(curdir)/abcdis/abc/Types/InstanceInfo.as\
  -in $(curdir)/abcdis/abc/Types/MetadataInfo.as\
  -in $(curdir)/abcdis/abc/Types/MethodBody.as\
  -in $(curdir)/abcdis/abc/Types/MethodInfo.as\
  -in $(curdir)/abcdis/abc/Types/Pool.as\
  -in $(curdir)/abcdis/abc/Types/ScriptInfo.as\
  -in $(curdir)/abcdis/abc/Types/Trait.as\
  -in $(curdir)/abcdis/abc/Types/Traits.as\
  -in $(curdir)/abcdis/abc/Types/ValueKind.as\
  -in $(curdir)/abcdis/abc/Verifier.as\
  -in $(curdir)/abcdis/abc/XMLWriter.as\
  -in $(curdir)/abcdis/SWF/Constants.as\
  -in $(curdir)/abcdis/SWF/Rect.as\
  -in $(curdir)/abcdis/SWF/SWF.as\
  -in $(curdir)/abcdis/SWF/SWFReader.as\
  -in $(curdir)/abcdis/SWF/Tag.as\
  -in $(curdir)/abcdis/SWC/SWCReader.as\
  -in $(curdir)/abcdis/zip/ZipFile.as\
  -in $(curdir)/abcdis/ABCDump.as\
  $(NULL)

asc=java -jar $(ASC) -strict -d -md -AS3 -import ../generated/builtin.abc -import ../generated/shell_toplevel.abc 

utils:
	$(asc) $(abcdis_asfiles) $(curdir)/abcdis.as
	$(asc) $(abcdis_asfiles) $(curdir)/abcdisclassic.as

