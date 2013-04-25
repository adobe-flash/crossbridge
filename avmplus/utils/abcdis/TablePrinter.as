/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package
{
    import IPrinter;
    import flash.utils.ByteArray;
    
    public class TablePrinter
    {
        public function TablePrinter(nCols : uint, minPadding : uint)
        {
            m_nCols = nCols;
            m_minPadding = minPadding;
            m_rows = new Vector.<IRow>();
        }
        
        public function addRow(r : Array) : void
        {
            if (r.length != m_nCols)
                throw new Error("Invalid row");
            m_rows.push(new Row(r));
        }
        
        public function addSpannedRow() : IPrinter
        {
            var r : SpannedRow = new SpannedRow();
            m_rows.push(r);
            return r;
        }
                
        public function toString() : String
        {
            var bytes : ByteArray = new ByteArray();
            var printer : IPrinter = new ByteArrayPrinter(bytes);
            print(printer)
            bytes.position = 0
            return bytes.readUTFBytes(bytes.length)
        }
        
        public function print(p : IPrinter) : void
        {
            var colWidths : Array = new Array(m_nCols);
            var i : uint;
            var r : IRow;
            for ( i = 0; i < m_nCols; ++i)
                colWidths[i] = 0;
            
            for each ( r in m_rows )
                r.measure(colWidths, m_minPadding)
            
            for each ( r in m_rows )
                r.print(p, colWidths)
        }
        
        private var m_nCols : uint;
        private var m_minPadding : uint;
        private var m_rows : Vector.<IRow>;
    }
}

import IPrinter

interface IRow
{
    function measure(colWidths : Array, minPadding : uint) : void;
    function print(p:IPrinter, colWidths : Array) : void;
}

class Row implements IRow
{
    public function Row(cells : Array)
    {
        this.cells = cells;
    }
    
    private var cells : Array;
    
    public function measure(colWidths : Array, minPadding : uint) : void
    {
        for (var i : uint = 0; i < cells.length; ++i )
            colWidths[i] = Math.max(colWidths[i], getRowItemStr(i).length + minPadding);
    }
    
    public function print(p : IPrinter, colWidths : Array) : void
    {
        var rowStr : String = "";
        for (var i : uint = 0; i < cells.length; ++i )
            rowStr += padString(getRowItemStr(i), colWidths[i]);
        p.println(rowStr)
    }
    
    private function getRowItemStr(i : uint) : String
    {
        if (cells[i] === null)
            return "null"
        try
        {
            return cells[i].toString();
        }
        catch (e : *)
        {
            trace(cells[0].toString());
            trace(i);
            trace(cells[i]);
            throw e;
        }
        return null;
    }
    
    private static function padString(s : String, minLength : uint) : String
    {
        while (s.length < minLength)
            s += " ";
        return s;
    }
}

class SpannedRow implements IRow, IPrinter
{
    public function SpannedRow()
    {
        lines = new Vector.<String>();
    }
    
    private var lines : Vector.<String>;
    
    public function measure(colWidths : Array, minPadding : uint) : void
    {
    }
    
    public function print(p : IPrinter, colWidths : Array) : void
    {
        for each ( var s : String in lines)
        {
            p.println(s)
        }
    }
    
    public function println(s : String) : void
    {
        lines.push(s)
    }
}