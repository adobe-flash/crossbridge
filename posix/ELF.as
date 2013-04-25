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

package com.adobe.flascc
{
import flash.utils.ByteArray;

/**
* simple ELF file creation helper
* basic section/symbol support
* @private
*/
[ExcludeClass] public class ELF
{

// EI_VERSIONs
public static const EV_NONE:int = 0;
public static const EV_CURRENT:int = 1;

// EI_CLASSes
public static const ELFCLASSNONE:int = 0;
public static const ELFCLASS32:int = 1;
//public static const ELFCLASS64:int = 2; // won't write out 64 bit correctly

// EI_DATAs
public static const ELFDATANONE:int = 0;
public static const ELFDATA2LSB:int = 1;
public static const ELFDATA2MSB:int = 2;

// e_types
public static const ET_NONE:int = 0;
public static const ET_REL:int = 1;
public static const ET_EXEC:int = 2;
public static const ET_DYN:int = 3;
public static const ET_CORE:int = 4;

// section types
public static const SHT_NULL:int = 0;
public static const SHT_PROGBITS:int = 1;
public static const SHT_SYMTAB:int = 2;
public static const SHT_STRTAB:int = 3;
public static const SHT_NOTE:int = 7;
public static const SHT_NOBITS:int = 8;
public static const SHT_REL:int = 9;

// section flags
public static const SHF_WRITE:int = 0x1;
public static const SHF_ALLOC:int = 0x2;
public static const SHF_EXECINSTR:int = 0x4;
public static const SHF_TLS:int = 0x400;

// symbol binding (or w/ type => st_info)
public static const STB_LOCAL:int = 0 << 4;
public static const STB_GLOBAL:int = 1 << 4;
public static const STB_WEAK:int = 2 << 4;

// symbol type
public static const STT_NOTYPE:int = 0;
public static const STT_OBJECT:int = 1;
public static const STT_FUNC:int = 2;
public static const STT_SECTION:int = 3;
public static const STT_FILE:int = 4;
public static const STT_TLS:int = 6;

// structure sizes
private static const _EHSIZE:int = 52;
private static const _PHENTSIZE:int = 32;
private static const _SHENTSIZE:int = 40;
private static const _SYMENTSIZE:int = 16;

// returns a ByteArray populated w/ ELF described by desc
// which is comprised of lists of sections and symbols:
// var desc:Object = {
//   sections: [
//     { sh_name: ".text", sh_type: ELF.SHT_PROGBITS, sh_flags: ELF.SHF_EXECINSTR, sh_size: 1024 },
//     { sh_name: ".data", sh_type: ELF.SHT_PROGBITS, sh_flags: ELF.SHF_WRITE, data: [someByteArray, someOffset, someLength] } // offset and length are optional
//   ],
//   symbols: [
//     { st_name: "foo", st_shndx: 1/*text*/, st_value: 123, st_size: 10, st_info: ELF.STB_GLOBAL|ELF.STT_FUNC },
//     { st_name: "bar", st_shndx: 2/*data*/, st_value: 1, st_size: 23, st_info: ELF.STB_LOCAL },
//   ]
// };
// var ba:ByteArray = ELF.create(desc);

public static function createFromSyms(syms:Array, sects:Object, mem:ByteArray = null, fixups:Vector.<int> = null):ByteArray
{
  var sections:Array = [];
  var symbols:Array = [];
  var relocSects:Object = {};

  for(var name:String in sects)
  {
    var sect:Object = {
      sh_name: name,
      sh_type: SHT_PROGBITS,
      sh_flags:
        (name.substr(0, 5) == ".text") ? (SHF_ALLOC|SHF_EXECINSTR) : 
        (name.substr(0, 6) == ".debug") ? 0:
        (SHF_ALLOC|SHF_WRITE),
      sh_addr: uint(sects[name][0]),
      sh_size: uint(sects[name][1])
    };
    if(mem && sect.sh_addr < mem.length && sect.sh_size)
    {
      var data:ByteArray = new ByteArray;
      data.endian = "littleEndian";
      data.writeBytes(mem, sect.sh_addr, sect.sh_size);
      sect.data = [data];
    }
    sections.push(sect);
  }

  // sort by addr to make the elf pretty (and easy to search);
  sections = sections.sortOn("sh_addr", Array.NUMERIC);

  if(fixups) // "un" fixup memory associated w/ a fixup
  {
    for(var snum:int = 0; snum < sections.length; snum++)
    {
      // add a symbol for the section
      symbols.push({
        st_name: sections[snum].sh_name,
        st_shndx: snum+1,
        st_value: sections[snum].sh_addr,
        st_info: STB_LOCAL | STT_SECTION 
      });
    }

    function sectForAddr(addr:uint):Object
    {
      var j:int = 0;

      while(j < sections.length)
      {
        var sect:Object = sections[j];
        if(addr >= sect.sh_addr && addr < (sect.sh_addr + sect.sh_size))
        {
          sect.__num = j;
          return sect;
        }
        j++;
      }
      return null;
    }
    for(var i:int = 0; i < fixups.length; i++)
    {
      var fixup:int = fixups[i];
      var fixupSect:Object = sectForAddr(fixup);

      if(!fixupSect.data || fixupSect.data.length < 1)
        continue;

      // only de-fix up debug sections
      if(fixupSect.sh_name.substr(0,6) != ".debug")
        continue;

      mem.position = fixup;

      var val:uint = mem.readUnsignedInt();
      var valSect:Object = sectForAddr(val);

      if(!valSect)
        continue;

      // only de-fix up debug<=>debug refs
      if(valSect.sh_name.substr(0,6) != ".debug")
        continue;

      var sdata:ByteArray = fixupSect.data[0];

      sdata.position = fixup - fixupSect.sh_addr;
      sdata.writeInt(val - valSect.sh_addr); // subtract out section

      var rsect:Object = relocSects[fixupSect.sh_name];
      
      if(!rsect)
      {
        rsect = relocSects[fixupSect.sh_name] = {
          sh_name: ".rel" + fixupSect.sh_name,
          sh_info: fixupSect.__num+1,
          sh_type: SHT_REL,
          sh_entsize: 8,
          data: [new ByteArray]
        };
        rsect.data[0].endian = "littleEndian";
        sections.push(rsect);
      }

      rsect.data[0].writeInt(fixup);
      rsect.data[0].writeInt(((valSect.__num+1)<<8)|1);
    }

    for each(var tsect:Object in relocSects)
      tsect.sh_link = sections.length+2; // will be the symtab
  }

  for each(var sym:Array in syms)
    if(sym.length >= 4) // [type, name(, sectName, value)] -- want a value!
    {
      var type:String = sym[0];
      var _name:String = sym[1];
      var sectName:String = sym[2];
      var value:uint = sym[3];
      var shndx:int = 0;

      // find section num
      while(sectName != sections[shndx].sh_name)
        shndx++;
    
      var global:Boolean = (type.toUpperCase() == type);

      symbols.push({
        // some hackery to make the symbols what gdb wants...
        st_name: (global && _name.substr(0,1) == "_") ? _name.substr(1) :
            (type == 't' && _name.substr(0,1) == "L") ? "." + _name :
            _name,
        st_shndx: shndx+1,
        st_value: value,
        st_info: global ? STB_GLOBAL : STB_LOCAL
      });
    }
  // pretend to be a freebsd/386 dyn object... TODO set gnutarget?
  return create({e_type: ET_DYN, EI_OSABI: 9, e_machine: 3, sections: sections, symbols: symbols});
}

public static function create(desc:Object):ByteArray
{  
  // get desc[prop] or default val
  function descDef(prop:String, val:*):*
  {
    if(typeof(desc[prop]) == "undefined")
      return val;
    return desc[prop];
  }

  var ba:ByteArray = new ByteArray;

  ba.endian = "littleEndian";
  
  // ehdr 
  ba.writeByte(0x7f);
  ba.writeUTFBytes("ELF");
  ba.writeByte(descDef("EI_CLASS", ELFCLASS32)); // 32-bit
  ba.writeByte(descDef("EI_DATA", ELFDATA2LSB)); // little-endian
  ba.writeByte(descDef("EI_VERSION", EV_CURRENT)); // e_version
  ba.writeByte(desc.EI_OSABI);
  ba.position = ba.length = 16; // zero out rest of e_ident
  ba.writeShort(desc.e_type); // none e_type by default
  ba.writeShort(desc.e_machine); // none e_machine by default
  ba.writeInt(descDef("e_version", EV_CURRENT)); // e_version
  ba.writeInt(desc.e_entry); // no e_entry by default
  var phdroffsetloc:uint = ba.position
  ba.writeInt(0); // fixup the offset later
  ba.writeInt(_EHSIZE); // shdr offset (follows ehdr for us)
  ba.writeInt(desc.e_flags); // e_flags
  ba.writeShort(_EHSIZE); // e_ehsize
  ba.writeShort(_PHENTSIZE); // e_phentsize
  ba.writeShort(2); // e_phnum
  ba.writeShort(_SHENTSIZE); // e_shentsize
  ba.writeShort(0); // e_shnum
  ba.writeShort(0); // e_strndx
  
  var sections:Array = desc.sections;

  if(!sections)
    sections = [];

  // null section
  sections.unshift({});

  // need a strtab
  sections.push({sh_name: ".strtab", sh_type: SHT_STRTAB});

  var symbols:Array = desc.symbols;

  if(symbols)
  {
    symbols.unshift({}); // null symbol
    sections.push( // need a symtab if symbols
      {sh_name: ".symtab", sh_type: SHT_SYMTAB, sh_link: sections.length-1, sh_entsize: _SYMENTSIZE});
  }

  // set shnum
  ba.position = _EHSIZE-4;
  ba.writeShort(sections.length);
  ba.position = ba.length;

  // section name string fixups
  var strs:Object = {};

  // record a use of a particular string to fixup later
  function addStrUse(str:String):void
  {
    var users:Array = strs[str];

    if(!users)
      users = strs[str] = [];
    users.push(ba.position);
  }

  var i:int;
  var section:Object;
  var name:String;

  // write out sections
  for(i = 0; i < sections.length; i++)
  {
    section = sections[i];
    name = section.sh_name;
    if(name !== null)
      addStrUse(name);
    ba.writeInt(0); // sh_name
    ba.writeInt(section.sh_type);
    ba.writeInt(section.sh_flags);
    ba.writeInt(section.sh_addr);
    ba.writeInt(0); // sh_offset
    ba.writeInt(section.sh_size);
    ba.writeInt(section.sh_link);
    ba.writeInt(section.sh_info);
    ba.writeInt(section.sh_addralign);
    ba.writeInt(section.sh_entsize);
  }

  var symtabDataOffs:int = ba.position;

  // write out symbols
  if(symbols)
  {
    for(i = 0; i < symbols.length; i++)
    {
      var symbol:Object = symbols[i];

      name = symbol.st_name;
      if(name !== null)
        addStrUse(name);
      ba.writeInt(0); // st_name
      ba.writeInt(symbol.st_value);
      ba.writeInt(symbol.st_size);
      ba.writeByte(symbol.st_info);
      ba.writeByte(symbol.st_other);
      ba.writeShort(symbol.st_shndx);
    }
  }

  var strtabDataOffs:int = ba.position;
  var strList:Array = [""]; // ensure empty is first

  for(name in strs)
    if(name != "")
      strList.push(name);

  var j:int;

  // write out strings, fix up users
  for(i = 0; i < strList.length; i++)
  {
    name = strList[i];

    var strDataOffs:int = ba.position;

    ba.writeUTFBytes(name);
    ba.writeByte(0);
    var users:* = strs[name];
    for(j = 0; users && j < users.length; j++)
    {
      ba.position = users[j];
      ba.writeInt(strDataOffs-strtabDataOffs);
    }
    ba.position = ba.length;
  }

  // fix up e_strndx
  var strndx:int = sections.length - (symbols ? 2 : 1);

  ba.position = _EHSIZE-2;
  ba.writeShort(strndx);
  ba.position = ba.length;

  // fix up strtab's offset/size
  ba.position = _EHSIZE + _SHENTSIZE * strndx + 16;
  ba.writeInt(strtabDataOffs);
  ba.writeInt(ba.length - strtabDataOffs);

  if(symbols)
  {
    // fix up symtab's offset/size/entsize
    ba.position += _SHENTSIZE - 8;
    ba.writeInt(symtabDataOffs);
    ba.writeInt(strtabDataOffs - symtabDataOffs);
  }

  ba.position = ba.length;

  // add any section data
  for(i = 0; i < sections.length; i++)
  {
    section = sections[i];

    var data:Array = section.data;

    if(data)
    {
      var dataOffs:int = ba.position;

      ba.writeBytes(data[0], data[1], data[2]);
      ba.position = _EHSIZE + _SHENTSIZE * i + 16;
      ba.writeInt(dataOffs);
      ba.writeInt(ba.length-dataOffs);
      ba.position = ba.length;
    }
  }

  // write out two dummy program headers
  var phdroffset:uint = ba.position;
  ba.position = phdroffsetloc;
  ba.writeInt(phdroffset);
  ba.position = ba.length;

  for(i = 0; i < 2; i++)
  {
    ba.writeInt(1); // p_type = PT_LOAD
    ba.writeInt(0); // p_offset
    ba.writeInt(0); // p_vaddr
    ba.writeInt(0); // p_paddr
    ba.writeInt(0); // p_filesz
    ba.writeInt(0xFFFFFFFF); // p_memsz
    ba.writeInt(0); // p_flags
    ba.writeInt(0); // p_align
  }

  return ba;
}

}

}

if(false) // TODO -- test code, remove
{

import avmplus.File;
import flash.utils.ByteArray;
import com.adobe.flascc.ELF;

var someData:ByteArray = new ByteArray;
someData.writeUTFBytes("Chello!");

var desc:Object = {
  sections: [
    { sh_name: ".bss", sh_type: ELF.SHT_NOBITS, sh_size: 1024 },
    { sh_name: ".blah", sh_type: ELF.SHT_PROGBITS, data: [someData] },
    { sh_name: ".text", sh_flags: ELF.SHF_EXECINSTR, sh_type: ELF.SHT_PROGBITS, data: [someData, 1] },
    { sh_name: ".data", sh_flags: ELF.SHF_WRITE, sh_type: ELF.SHT_PROGBITS, data: [someData, 2, 3] }
  ],
  symbols: [
    { st_name: "foo", st_shndx: 3/*text*/, st_value: 123, st_size: 10, st_info: ELF.STB_GLOBAL|ELF.STT_FUNC },
    { st_name: "bar", st_shndx: 4/*data*/, st_value: 1, st_size: 23 },
    { st_name: "fooz", st_shndx: 4/*data*/, st_value: -1, st_size: -23, st_info:ELF.STB_GLOBAL }
  ]
};

File.writeByteArray("./test.o", ELF.create(desc));
  
}
