/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package
{
    import IPrinter;
    
    public class IndentingPrinter implements IPrinter
    {
        public function IndentingPrinter(delegate : IPrinter, initialIndent : uint, indentIncrement : uint)
        {
            m_delegate = delegate;
            m_currentIndent = makeIndentStr(initialIndent);
            m_indentIncrement = indentIncrement;
        }
        
        private static function makeIndentStr(indent : uint) : String
        {
            var result : String = "";
            for (var i : uint = 0 ; i < indent; ++i)
                result += " ";
            return result;
        }
        
        private var m_delegate : IPrinter;
        private var m_currentIndent : String;
        private var m_indentIncrement : uint;
        
        public function println(s : String) : void
        {
            if (s.length > 0)
                s = m_currentIndent + s
            m_delegate.println(s);
        }
        
        public function indent() : void
        {
            var newIndent : uint = m_currentIndent.length + m_indentIncrement;
            m_currentIndent = makeIndentStr(newIndent);
        }
        
        public function unindent() : void
        {
            var newIndent : uint = m_currentIndent.length - m_indentIncrement;
            m_currentIndent = makeIndentStr(newIndent);
        }
    }
}