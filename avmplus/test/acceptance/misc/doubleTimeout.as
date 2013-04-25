/* -*- Mode: js; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

try {
    while (true) {}
} catch(err) {
    try {
        while (true) {}
    } catch(err2) {
        print('Caught error: ' + err2.toString());
        print('FAILED: Error #1503 (kScriptTerminatedError) should not be catchable')
    }
}
