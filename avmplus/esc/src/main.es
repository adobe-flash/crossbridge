/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Interactive/non-interactive shell.  If there are command line
 * arguments, treat them as filenames to load and eval, and then exit
 * after processing.  Otherwise, read input from the console.
 */

use default namespace internal;

use namespace "avmplus";
use namespace "flash.utils";

{
    // Users can use -no-debug to turn it off; enabling debugging is
    // the right choice for the REPL.

    ESC::flags.debugging = true;

    let files = ESC::filterCommandLine(Util::commandLineArguments());

    if (files.length == 0) {
        //let v = ESC::version;
        //print("ESC v" + v.major + "." + v.minor + " (\"" + v.nick + "\")");
        repl();
    }
    else {
        for ( let i=0 ; i < files.length ; i++ )
            ESC::compileAndLoadFile(files[i]);
    }
    System.exit(0);
}

// "eval" really belongs in the builtins, but OK here for the moment.
public function eval(...args)
    ESC::evaluateInScopeArray(args, [], "");

function repl() {
    while( true ) {
        let s = "";
        System.write("es> ");
        while( true ) {
            try {
                s += System.readLine();
                let [_,_,res] = ESC::compileAndLoadString(s, "(repl)");
                if (res !== undefined)
                    print(res);
                break; // worked - this command is complete.
            } catch (x) {
                // If it is a premature-EOF error, read another line
                if (x.message.indexOf("found EOS") == -1) {
                    let msg = x.getStackTrace();
                    if (!msg) { // probably a non *_Debugger build
                        msg = x;
                    }
                    print(msg);
                    break;
                }
                // else fall through and read another line
            }
        }
    }
}


