/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace ESC,
    namespace ESC;

internal var profile_data = {};

function startProfile() {
    "flash.sampler"::startSampling();
}

internal function printProf()
    this.name + "\n   {self:" + this.self + ", children: " + this.children + ", new:" + this.new + "}";

internal function printCallers() {
    let s = "{";
    for each ( let v in this )
        s += v + ", ";
    s += "}";
    return s;
}

internal function printCaller()
    this.name + "= " + this.count;

internal function frame2name(s) {
    let name = s + " ";
    if (!profile_data.hasOwnProperty(name))
        profile_data[name] = {name: name, callers: { toString: printCallers }, self:0, children: 0, new:0, toString: printProf};
    return name;
}

function snapshotProfile() {
    "flash.sampler"::pauseSampling();
    let samples = "flash.sampler"::getSamples();
    for each (let sample in samples) {
        if (sample.stack) {
            let name = frame2name(sample.stack[0]);
            switch type (sample) {
            case (s: "flash.sampler"::DeleteObjectSample) { }
            case (s: "flash.sampler"::NewObjectSample) { 
                profile_data[name].new += 1; 
            }
            case (s: *) { 
                profile_data[name].self += 1;
                if (sample.stack.length > 1) {
                    let n2 = frame2name(sample.stack[1]);
                    if (!profile_data[name].callers.hasOwnProperty(n2))
                        profile_data[name].callers[n2] = { name: n2, count: 0, toString: printCaller };
                    profile_data[name].callers[n2].count += 1;
                }
                for ( let i=1 ; i < sample.stack.length ; i++ ) {
                    let parent_name = frame2name(sample.stack[i]);
                    profile_data[parent_name].children += 1;
                }
            }
            }
        }
    }
    "flash.sampler"::clearSamples();
    "flash.sampler"::startSampling();
}

function dumpProfile(filename) {
    let tmp = [];
    let s = "";

    for (let name in profile_data)
        tmp.push(profile_data[name]);

    timeProfile((function (x) x.self), "self");
    timeProfile((function (x) x.self + x.children), "subtree");

    s += "\n";
    s += "ALLOC PROFILE\n";
    s += "\n";
    s += "Allocs   Pct    Name\n";
    s += "-------- ------ -------------------------------\n";

    tmp.sort(function(a,b) b.new-a.new);

    let total = 0;

    for ( let i=0 ; i < tmp.length && tmp[i].new > 0 ; i++ )
        total += tmp[i].new;
    for ( let i=0 ; i < tmp.length && tmp[i].new > 0 ; i++ )
        s += "" + right(tmp[i].new, 8) + " " + right((100*tmp[i].new/total).toFixed(1), 5) + "% " + tmp[i].name + "\n";

    Util::writeStringToFile(s, filename);
    print("Profile written to " + filename);

    function timeProfile(val, bywhat) {
        s += "\n";
        s += "TIME PROFILE (sorted by " + bywhat + ")\n";
        s += "\n";
        s += "Self     Subtree  Pct    Name\n";
        s += "-------- -------- ------- -------------------------------\n";

        tmp.sort(function (a,b) val(b) - val(a));

        let total = 0;
        let v;

        for ( let i=0 ; i < tmp.length && val(tmp[i]) > 0 ; i++ )
            total += tmp[i].self;  // [sic]
        for ( let i=0 ; i < tmp.length && (v = val(tmp[i])) > 0 ; i++ ) {
            s += "" + right(tmp[i].self, 8) + " " + right(tmp[i].self+tmp[i].children, 8) + right((100*v/total).toFixed(1), 6) + "% " + tmp[i].name + "\n";
            /* Print the known callers of the method in arbitrary order
            if (tmp[i].name == "Array/get length() ")
                s += "      " + tmp[i].callers.toString().split(",").join("\n      ") + "\n";
            */
        }
    }

    function right(x, n) {
        x = String(x);
        return "                    ".substring(0,n).substring(x.length) + x;
    }
}

function stopProfile() {
    "flash.sampler"::stopSampling();
}
