/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{
    [native(cls="ErrorClass", gc="exact", instance="ErrorObject", methods="auto")]
    public dynamic class Error
    {
        prototype.name = "Error"
        prototype.message = "Error"

        // Error.length = 1 per ES3
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        // TODO mark message as String once compiler super() bug is fixed
        // E262 {}
        /* enumerable */ public var message;
        public var name;

        // JS Error has these props:
        //    message:String
        //    fileName:String
        //    lineNumber:String
        //    stack:String
        //    name:String

        function Error(message = "", id = 0)
        {
            this.message = message;
            this._errorID = id;
            this.name = prototype.name;
        }

        prototype.toString = function():String
        {
            var e:Error = this
            return e.message !== "" ? e.name + ": " + e.message : e.name;
        }
        _setPropertyIsEnumerable(prototype, "toString", false);

        // avm+ specific, works in debugger builds only
        public native function getStackTrace():String;
        public native static function getErrorMessage(index:int):String;

        // avm+ specific utility method
        public static function throwError(type:Class, index:uint, ... rest)
        {
            // This implements the same error string formatting as the native
            // method PrintWriter::formatP(...) any changes to this method should
            // also be made there to keep the two in sync.
            var i=0;
            var f=function(match, pos, string)
            {
                var arg_num = -1;
                switch(match.charAt(1))
                {
                    case '1':
                        arg_num = 0;
                        break;
                    case '2':
                        arg_num = 1;
                        break;
                    case '3':
                        arg_num = 2;
                        break;
                    case '4':
                        arg_num = 3;
                        break;
                    case '5':
                        arg_num = 4;
                        break;
                    case '6':
                        arg_num = 5;
                        break;
                }
                if( arg_num > -1 && rest.length > arg_num )
                    return rest[arg_num];
                else
                    return "";
            }
            throw new type(Error.getErrorMessage(index).replace(/%[0-9]/g, f), index);
        }

        private var _errorID : int;

        public function get errorID() : int
        {
            return this._errorID;
        }
    }

    [native(cls="DefinitionErrorClass", gc="exact", instance="DefinitionErrorObject", methods="auto")]
    public dynamic class DefinitionError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "DefinitionError"
        function DefinitionError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="EvalErrorClass", gc="exact", instance="EvalErrorObject", methods="auto")]
    public dynamic class EvalError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "EvalError"
        function EvalError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="RangeErrorClass", gc="exact", instance="RangeErrorObject", methods="auto")]
    public dynamic class RangeError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "RangeError"
        function RangeError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="ReferenceErrorClass", gc="exact", instance="ReferenceErrorObject", methods="auto")]
    public dynamic class ReferenceError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "ReferenceError"
        function ReferenceError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="SecurityErrorClass", gc="exact", instance="SecurityErrorObject", methods="auto")]
    public dynamic class SecurityError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "SecurityError"
        function SecurityError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="SyntaxErrorClass", gc="exact", instance="SyntaxErrorObject", methods="auto")]
    public dynamic class SyntaxError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "SyntaxError"
        function SyntaxError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="TypeErrorClass", gc="exact", instance="TypeErrorObject", methods="auto")]
    public dynamic class TypeError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "TypeError"
        function TypeError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="URIErrorClass", gc="exact", instance="URIErrorObject", methods="auto")]
    public dynamic class URIError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "URIError"
        function URIError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="VerifyErrorClass", gc="exact", instance="VerifyErrorObject", methods="auto")]
    public dynamic class VerifyError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "VerifyError"
        function VerifyError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="UninitializedErrorClass", gc="exact", instance="UninitializedErrorObject", methods="auto")]
    public dynamic class UninitializedError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "UninitializedError"
        function UninitializedError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }

    [native(cls="ArgumentErrorClass", gc="exact", instance="ArgumentErrorObject", methods="auto")]
    public dynamic class ArgumentError extends Error
    {
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1

        prototype.name = "ArgumentError"
        function ArgumentError(message = "", id = 0)
        {
            super(message, id);
            this.name = prototype.name;
        }
    }
}

package flash.errors
{
    public dynamic class IOError extends Error
    {
        public function IOError(message:String = "", id:int = 0)
        {
            super(message, id);
        }   
    }

    public dynamic class EOFError extends IOError
    {
        public function EOFError(message:String = "", id:int = 0)
        {
            super(message, id);
        }   
    }

    public dynamic class MemoryError extends Error
    {
        public function MemoryError(message:String = "", id:int = 0)
        {
            super(message, id);
        }   
    }

    public dynamic class IllegalOperationError extends Error
    {
        public function IllegalOperationError(message:String = "", id:int = 0)
        {
            super(message, id);
        }   
    }

    IOError.prototype.name                  = "IOError"
    MemoryError.prototype.name              = "MemoryError"
    EOFError.prototype.name                 = "EOFError"
    IllegalOperationError.prototype.name    = "IllegalOperationError"
}