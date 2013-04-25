/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.net
{

    /**
     * Preserves the class (type) of an object when the object is encoded in Action Message Format (AMF).
     * When you encode an object into AMF, this function saves the alias for its class, so that you can
     * recover the class when decoding the object.
     * If the encoding context did not register an alias for an object's class, the object
     * is encoded as an anonymous object. Similarly, if the decoding context does not have the same
     * alias registered, an anonymous object is created for the decoded data.
     *
     * <p>LocalConnection, ByteArray, SharedObject, NetConnection and NetStream are all examples
     * of classes that encode objects in AMF.</p>
     *
     * <p>The encoding and decoding contexts do not need to use the same class for an alias;
     * they can intentionally change classes, provided that the destination class contains all of the members
     * that the source class serializes.</p>
     *
     * @param aliasName The alias to use.
     * @param classObject The class associated with the given alias.
     *
     * @throws TypeError If either parameter is <code>null</code>.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @includeExample examples\RegisterClassAliasExample.as -noswf
     * @see flash.net.ObjectEncoding ObjectEncoding class
     *
     *  @playerversion Lite 4
     */
    [native("Toplevel::registerClassAlias")]
    public native function registerClassAlias(aliasName:String, classObject:Class):void;


    /**
     * Looks up a class that previously had an alias registered through a call to the <code>registerClassAlias()</code>
     * method.
     * <p>This method does not interact with the <code>flash.utils.getDefinitionByName()</code>
     * method.</p>
     *
     * @param aliasName The alias to find.
     *
     * @return The class associated with the given alias. If not found, an exception will be thrown.
     *
     * @throws ReferenceError The alias was not registered.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @see flash.net#registerClassAlias() registerClassAlias()
     * @internal includeExample examples\GetClassByAliasExample.as -noswf
     *
     *  @playerversion Lite 4
     */
    [native("Toplevel::getClassByAlias")]
    public native function getClassByAlias(aliasName:String):Class;
}

