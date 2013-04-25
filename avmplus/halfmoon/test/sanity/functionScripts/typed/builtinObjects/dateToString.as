// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
	Date.prototype.formatDate = function ():String {
        var result:String = this.getMinutes();
        print(result);
        return result;
	}


	var date:Date = new Date("1/1/2007 1:11:11");
	var longFormat:String = date.formatDate();
}
