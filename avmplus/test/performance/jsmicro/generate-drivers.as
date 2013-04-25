// Usage: avmshell generate-drivers.abc -- *-[0-9]*.js

package
{
    import avmplus.*;

    var copyright =
        [   "/*",
            " *  This Source Code Form is subject to the terms of the Mozilla Public",
            " *  License, v. 2.0. If a copy of the MPL was not distributed with this",
            " *  file, You can obtain one at http://mozilla.org/MPL/2.0/.",
            " */"].join("\n") + "\n";

    function fn(s, ex1, ex2) {
        var loc = s.lastIndexOf(ex1);
        if (loc == -1)
            throw new Error("Bogus file name: " + ex1);
        return s.substring(0,loc) + ex2;
    }

    for each ( var f in System.argv ) {
        File.write(fn(f, ".js", ".as"),
                   copyright +
                   "\n" +
                   "include \"driver.js\"\n" +
                   "include \"" + f + "\"\n");
        File.write(fn(f, ".js", ".html"),
                   "<!--\n" +
                   copyright +
                   "-->\n" +
                   "\n" +
                   "<script src=\"html-prefix.js\"></script>\n" +
                   "<script src=\"driver.js\"></script>\n" +
                   "<script src=\"" + f + "\"></script>\n");
    }
}
