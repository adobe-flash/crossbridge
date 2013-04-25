// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


dynamic class DynamicArray extends Array {}
class SealedDynamicArray extends DynamicArray {}

function run_tests(b:Array, mode:String)
{
	b[0] = 44;
}
print(run_tests(new SealedDynamicArray, "semisealed")); 

