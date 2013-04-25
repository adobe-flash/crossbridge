/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.sampler.*;

var global_e2:Error;

flash.sampler.startSampling();

try {
    throw new Error();
} catch (e:Error) {
    global_e2 = e; // save a reference to the error, so that the sampler will
                    // get a live reference when it is triggered
}

var s = flash.sampler.getSamples();
for each (var ss in s)
{
    // nothing -- just iterate to force the samples to be generated
    // print(ss);
}

flash.sampler.stopSampling();

// if we get here without crashing, we pass
print("test PASSED!");
