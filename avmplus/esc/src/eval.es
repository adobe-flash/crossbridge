/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package es4
{
    import avmplus.File

    // shell

    public function evalFile(src_name)
    {
        var base_name = src_name.split('.')[0];
        var abc_name = base_name+".abc";
        var src = File.read(src_name);
        return evalString(src);
    }

    public function evalString(src)
    {
        var evaluator = new Evaluator;
        try {
            var result = evaluator.eval(src);
        }
        catch(x)
        {
            print("\n**" + x + "**");
            var result = <LiteralString value=""/>;
        }
        return result;
    }
}

include "debugger.as"
include "parser.as"
include "definer.as"
include "emitter.as"
include "interpreter.as"
include "evaluator.as"

{
    import avmplus.System;
    import avmplus.File;
    import es4.*;
    
    if( System.argv.length < 1 ) {
        print(File.read("es4_start.txt"));
        while( true ) {
            System.write("es4>");
            var s = System.readLine();
            if( s == ".quit" ) 
                break;
            else if( s.indexOf(".help") == 0 ) {
                print(File.read("es4_start.txt"));
                print(File.read("es4_help.txt"));
            }
            else if( s.indexOf(".trace") == 0 ) {
                var arg_name = s.substr(7);
                if( arg_name == "on" ) {
                    es4.log_mode = debug;
                    print("tracing on");
                }
                else {
                    es4.log_mode = release;
                    print("tracing off");
                }
            }
            else if( s.indexOf(".load") == 0 ) {
                var arg_name = s.substr(6);
                var result = evalFile(arg_name);
                if( result != void 0 ) {
                    print(result);
                }
            }
            else if( s.indexOf(".test") == 0 ) {
                var result = testABCEmitter();
                if( result != void 0 ) {
                    print(result);
                }
            }
            else {
                var result = evalString(s);
                if( result != void 0 ) {
                    print(result);
                }
            }
        }
    }
    else {
        print(System.argv[0]);
        var result = evalFile(System.argv[0]);
        print(result);
    }
}

