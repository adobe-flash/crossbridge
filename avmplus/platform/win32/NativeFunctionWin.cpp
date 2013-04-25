/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */



#include "avmplus.h"

namespace avmplus
{
    /**
     * call a native method using windows thiscall calling conventions.
     * we use the traits of the function to figure out what native types
     * to convert to, as follows:
     *
     *  AS type         C++ type
     *  -------         --------
     *  Void            void
     *  Object          Atom
     *  Boolean         bool      (unsigned 8bit)
     *  int             int       (signed 32bit)
     *  Number          double    (float 64bit)
     *  String          Stringp   (const String *)
     *  Class           ClassClosure*
     *  MovieClip       MovieClipObject*   (similar for any other class)
     */
}
