/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.utils
{
//
// Dictionary
//

/**
 * The Dictionary class lets you create a dynamic collection of properties, which uses strict equality
 * (<code>===</code>) for key comparison. When an object is used as a key, the object's
 * identity is used to look up the object, and not the value returned from calling <code>toString()</code> on it.
 * <p>The following statements show the relationship between a Dictionary object and a key object:</p>
 * <pre>
 * var dict = new Dictionary();
 * var obj = new Object();
 * var key:Object = new Object();
 * key.toString = function() { return "key" }
 *
 * dict[key] = "Letters";
 * obj["key"] = "Letters";
 *
 * dict[key] == "Letters"; // true
 * obj["key"] == "Letters"; // true
 * obj[key] == "Letters"; // true because key == "key" is true b/c key.toString == "key"
 * dict["key"] == "Letters"; // false because "key" === key is false
 * delete dict[key]; //removes the key
 * </pre>
 *
 * @playerversion Flash 9
 * @langversion 3.0
 * @see ../../operators.html#strict_equality === (strict equality)
 *
 *
 *  @playerversion Lite 4
 */
[native(cls="DictionaryClass", gc="exact", instance="DictionaryObject", methods="auto")]
dynamic public class Dictionary
{
    private native function init(weakKeys:Boolean):void;

    /**
     * Creates a new Dictionary object. To remove a key from a Dictionary object, use the <code>delete</code> operator.
     *
     * @param weakKeys Instructs the Dictionary object to use "weak" references on object keys.
     * If the only reference to an object is in the specified Dictionary object, the key is eligible for
     * garbage collection and is removed from the table when the object is collected.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    public function Dictionary(weakKeys:Boolean=false)
    {
        init(weakKeys);
    }

    // Note: clients are free to replace with method returning non-string
    prototype.toJSON = function (k:String):* { return "Dictionary"; }

    // Bug 651641: we do not want toJSON enumerated.
    _dontEnumPrototype(prototype);
};
}
