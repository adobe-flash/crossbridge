/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use namespace Abc,
    namespace Asm,
    namespace Ast,
    namespace Release,
    namespace "avmplus",
    namespace "flash.utils";

class AbcEncoderCpp 
{
    var verbose : Boolean;
    var fname: String; // full filename as specified.
    var basename: String; // base-name portion of the filename
    var bytes:ByteArray;
    var abc : ABCFile;
    var constants: ABCConstantPoolParser;

    function AbcEncoderCpp(filename: string, v:Boolean = false): fname=filename, verbose=v {
        bytes = ByteArray.readFile(fname+".abc");

        abc = Abc::parseAbcFile(new ABCByteStream(bytes));
        constants = new ABCConstantPoolParser(abc.constants);

        let path_bits = fname.split(/[\/\\]/);
        basename = path_bits[path_bits.length-1];
    }

    function encode() {
        let items = process_abc(abc);
        // items is an array of [namebits, value]
        // namebits itself is an array of strings, which are joined 
        // by '_' to make the name
        let lines = []
            for each (var item in items) {
                let [bits,v] = item;
                // better way to get non-empty values?
                bits = bits.filter( function(elt, ndx) {return elt.length!=0;})
                // convert dots or colons to underscores.
                bits.every( function(elt, ndx) {bits[ndx] = elt.replace(/[\.:]/g, "_");return true;} );
                lines.push("const int " + bits.join('_') + " = " + v + ";");
            }
        lines.push("extern const unsigned char " + basename + "_abc_data[];");
        lines.push('');
        let h_data = lines.join("\n");

        // The .cpp
        lines = ["const unsigned char " + basename + "_abc_data[" + bytes.length + "] = {"]
            let enc_bytes = []
            for (let i=0;i<bytes.length;i++) {
                if (i && i % 16 == 0) {
                    lines.push(enc_bytes.join(", ") + ",")
                    enc_bytes = [];
                }
                let this_byte = bytes[i].toString(16);
                if (this_byte.length == 1)
                this_byte = "0" + this_byte;
                enc_bytes.push("0x" + this_byte);
            }
        lines.push(enc_bytes.join(", ") + " };");
        lines.push('');
        let cpp_data = lines.join("\n");
        return [h_data, cpp_data];
    }

    function process_traits(prefixes: Array, traits) {
        let ret = new Array();
        for each (let t in traits) {
            let [trait_ns, trait_name] = constants.getConstantName(t.name);
            let kind = (t.kind&0x0F);
            if (verbose)
                print("checking", trait_ns + "::" + trait_name, "(kind=", kind, ")")

                    switch (kind) {
                    case TRAIT_Method: // its a method_info
                    case TRAIT_Getter:
                    case TRAIT_Setter:
                        let suffix = kind==TRAIT_Getter ? 'get' : (kind==TRAIT_Setter ? 'set' : '');
                        let mi = abc.methods[t.val];
                        if (mi.flags & METHOD_Native) {
                            // 'private' names get special treatment apparently!
                            let is_private = constants.getConstantNameType(t.name) == CONSTANT_PrivateNamespace;
                            if (is_private) {
                                ret.push([[trait_ns, 'private', trait_name, suffix], t.val]);
                            } else {
                                ret.push([prefixes.concat([trait_ns, trait_name, suffix]), t.val]);
                            }
                        }
                        break;
                    case TRAIT_Class:
                        ret.push([["abcclass", trait_ns, trait_name], t.val]);
                        // now get all the members from the class.
                        let klass: ABCClassInfo = abc.classes[t.val];
                        let new_prefixes = prefixes.concat([trait_ns, trait_name])
                            let sub:Array = process_traits(new_prefixes, klass.traits)
                            ret = ret.concat(sub);
                        break;
                        // Ones we know we ignore.
                    case TRAIT_Slot:
                    case TRAIT_Const:
                        break;
                        // Any others we log a message incase its important!
                    default:
                        print("Skipping trait", trait_ns, trait_name, ": kind=", kind);
                        break;
                    }
        }
        return ret;
    }

    function process_abc(abc: ABCFile): Array {
        let ret = new Array();
        for each (var s in abc.scripts) {
            ret = ret.concat(process_traits([], s.traits));
        }

        for each (let i in abc.instances) {

            let iprefixes = constants.getConstantName(i.name);

            // do the "instance init" function if its native.
            let iinit = abc.methods[i.iinit]
                if (iinit.flags & METHOD_Native)
                    // we duplicate the class name as the method name.
                    ret.push([iprefixes.concat([iprefixes[iprefixes.length-1]]), i.iinit]);
            // and the instance methods
            ret = ret.concat(process_traits(iprefixes, i.traits))
                }

        // Now do the packages
        // XXX - this is bogus - while it works on ASC generated .abc
        // files, ESC doesn't support '-import' yet, so external package
        // references do not exist...
        for each (let s in constants.utf8_pool) {
            let bits = s.split('$')
                if (bits.length==2) {
                    ret.push([['abcpackage', bits[0]], int(bits[1])-1])
                }
        }
        // MarkH can't explain the -2!! (especially as the other 2 counts
        // are not adjusted!)
        // XXXXX - must we use the filename?
        ret.push([[basename, 'abc', 'method_count'], abc.methods.length-2])
            ret.push([[basename, 'abc', 'class_count'], abc.classes.length])
            ret.push([[basename, 'abc', 'script_count'], abc.scripts.length])
            ret.push([[basename, 'abc', 'length'], bytes.length])
            return ret;
    }

} // class AbcEncoderCpp

{
    let fname = System.argv[0];
    let verbose = false;

    if( fname == "verbose" ) {
        fname = System.argv[1];
        verbose = true;
    }
    if (fname==undefined) 
        throw "no file name given";

    let encoder = new AbcEncoderCpp(fname, verbose);

    let [h_data, cpp_data] = encoder.encode();
    let prefix = "// This is a GENERATED file - do not edit\n";

    Util::writeStringToFile(prefix + h_data, fname+".h");
    print (fname+".h, "+h_data.length+" chars written");

    Util::writeStringToFile(prefix + cpp_data, fname+".cpp");
    print (fname+".cpp, "+cpp_data.length+" chars written");
}
