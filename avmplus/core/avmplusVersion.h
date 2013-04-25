/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* -*- tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define avmplus_stringify2(x)     # x
#define avmplus_stringify(x)      avmplus_stringify2(x)

// The visible build code: d(evelopment) or r(elease candidate), followed by an identifying string.
//
#define AVMPLUS_BUILD_CODE          "cyclone"

#ifdef AVMPLUS_DESC
#define AVMPLUS_DESC_STRING         avmplus_stringify(AVMPLUS_DESC)
#endif


#ifdef DEBUGGER
#define AVMPLUS_BIN_DEBUGGER_TAG "-debugger"
#else
#define AVMPLUS_BIN_DEBUGGER_TAG ""
#endif

#ifdef DEBUG
#define AVMPLUS_BIN_TYPE_TAG  "debug"
#else
#define AVMPLUS_BIN_TYPE_TAG  "release"
#endif

#define AVMPLUS_BIN_TYPE  AVMPLUS_BIN_TYPE_TAG AVMPLUS_BIN_DEBUGGER_TAG

// A possibly-obsolete build number, not used by the VM itself but
// possibly by embedders.  It is supposed to count 1-n for development
// builds, and restart at 1-n for release builds.
#define AVMPLUS_BUILD_NUMBER        0

// User-facing version numbering.
//
// AVMPLUS_MAJOR_VERSION and AVMPLUS_MINOR_VERSION track the engine generation
// and release number within a generation, respectively.
//
// AVMPLUS_MAJOR_VERSION is updated when we think it is appropriate to do so
// (major technology change, incompatible API changes maybe, new bytecodes
// maybe).  The rules for this are not set in stone.  It signifies that
// "something important just happened".  We have to just reach a consensus on
// whether something should trigger a change.
//
// AVMPLUS_MINOR_VERSION is updated on /every/ branch from tamarin-redux
// into an official release and is therefore a serial number for the release.

#define AVMPLUS_MAJOR_VERSION       2
#define AVMPLUS_MINOR_VERSION       1

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=660030

#define AVMPLUS_SECURITY_SUFFIX " sec"

// The simple user-facing version number
#define AVMPLUS_VERSION_USER        avmplus_stringify(AVMPLUS_MAJOR_VERSION) "." avmplus_stringify(AVMPLUS_MINOR_VERSION) AVMPLUS_SECURITY_SUFFIX

// A more complex user-facing version number
#define AVMPLUS_VERSION_STRING      avmplus_stringify(AVMPLUS_MAJOR_VERSION) "," avmplus_stringify(AVMPLUS_MINOR_VERSION) "," avmplus_stringify(AVMPLUS_BUILD_NUMBER) "," AVMPLUS_BUILD_CODE AVMPLUS_SECURITY_SUFFIX

#else

// The simple user-facing version number
#define AVMPLUS_VERSION_USER        avmplus_stringify(AVMPLUS_MAJOR_VERSION) "." avmplus_stringify(AVMPLUS_MINOR_VERSION)

// A more complex user-facing version number
#define AVMPLUS_VERSION_STRING      avmplus_stringify(AVMPLUS_MAJOR_VERSION) "," avmplus_stringify(AVMPLUS_MINOR_VERSION) "," avmplus_stringify(AVMPLUS_BUILD_NUMBER) "," AVMPLUS_BUILD_CODE

#endif

// Define Mac only resource change to 'vers' resource so that player installer will
// replace previous player versions
#define AVMPLUS_MAC_RESOURCE_MINOR_VERSION  0x00

