/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

{
    let total_frontend = 0;
    let total_backend = 0;

    let before = new Date();

    let files = ESC::filterCommandLine(Util::commandLineArguments());

    let prof = ESC::flags.profile_compiler;

    if (prof)
        ESC::startProfile();

    for ( let i=0, limit=files.length ; i < limit ; i++ ) {
        let fname = files[i];
        let [parse,cogen] = ESC::compileFile(fname);
        total_frontend += parse;
        total_backend += cogen;
        print (fname);
        print ("  Scan+parse:  " + parse + " ms");
        print ("  Cogen:       " + cogen + " ms");

        // This is a hack.  The profiler needs to be callback-based; the
        // snapshot is captured so that the profiler won't lose data.
        if (prof)
            ESC::snapshotProfile();
    }

    let after = new Date();

    if (files.length > 1) {
        print("");
        print("Total time: " + (after - before));
        print("Front end:  " + total_frontend);
        print("Back end:   " + total_backend);
    }

    if (prof) {
        ESC::stopProfile();
        ESC::dumpProfile("esc.profile");
    }
}

