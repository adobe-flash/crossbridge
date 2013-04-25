/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
#ifdef DEBUGGER

	uintptr OSDep::startIntWriteTimer(uint32 /*millis*/, int* /*addr*/)
	{
		return 0;
	}

	void OSDep::stopTimer(uintptr /*handle*/)
	{
	}

#endif // DEBUGGER
}
