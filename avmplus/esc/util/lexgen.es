/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Generate a mega-switch for keyword recognition.
 * Based on the 2008-05-05 grammar draft.
 *
 * Run this script, capture the output, remove the last two lines of
 * the captured output (which should be "default:\nbreak bigswitch;").
 * Then replace the code in lex-scan.es between the lines "Begin
 * generated code" and "End generated code" by the output.  Then
 * reindent (Emacs is your friend).
 */

var keywords = [[ "break", "Token::Break"],
                [ "case", "Token::Case"],
                [ "cast", "Token::Cast", "!ESC::flags.es3_keywords"],
                [ "catch", "Token::Catch"],
                [ "class", "Token::Class", "!ESC::flags.es3_keywords"],
                [ "const", "Token::Const", "!ESC::flags.es3_keywords"],
                [ "continue", "Token::Continue"],
                [ "debugger", "Token::Debugger", "ESC::flags.es4_kwd_debugger"],
                [ "default", "Token::Default"],
                [ "delete", "Token::Delete"],
                [ "do", "Token::Do"],
                [ "dynamic", "Token::Dynamic", "!ESC::flags.es3_keywords"],
                [ "else", "Token::Else"],
                [ "false", "Token::False"],
                [ "final", "Token::Final", "!ESC::flags.es3_keywords"],
                [ "finally", "Token::Finally"],
                [ "for", "Token::For"],
                [ "function", "Token::Function"],
                [ "if", "Token::If"],
                [ "in", "Token::In"],
                [ "instanceof", "Token::InstanceOf"],
                [ "interface", "Token::Interface", "!ESC::flags.es3_keywords"],
                [ "is", "Token::Is", "!ESC::flags.es3_keywords"],
                [ "let", "Token::Let", "!ESC::flags.es3_keywords"],
                [ "namespace", "Token::Namespace", "!ESC::flags.es3_keywords"],
                [ "native", "Token::Native", "!ESC::flags.es3_keywords"],
                [ "new", "Token::New"],
                [ "null", "Token::Null"],
                [ "override", "Token::Override", "!ESC::flags.es3_keywords"],
                [ "return", "Token::Return"],
                [ "static", "Token::Static", "!ESC::flags.es3_keywords"],
                [ "super", "Token::Super", "!ESC::flags.es3_keywords"],
                [ "switch", "Token::Switch"],
                [ "this", "Token::This"],
                [ "throw", "Token::Throw"],
                [ "true", "Token::True"],
                [ "try", "Token::Try"],
                [ "type", "Token::Type", "!ESC::flags.es3_keywords"],
                [ "typeof", "Token::TypeOf"],
                [ "use", "Token::Use"],
                [ "var", "Token::Var"],
                [ "void", "Token::Void"],
                [ "while", "Token::While"],
                [ "with", "Token::With"],
                [ "yield", "Token::Yield", "!ESC::flags.es3_keywords"],
                [ "__proto__", "Token::Proto", "!ESC::flags.es3_keywords"]];

keywords.sort(function (a, b) {  return a[0] < b[0] ? -1 : a[0] > b[0] ? 1 : 0 });
print(generate( keywords, 0 ));

// notPartOfIdent is an Array that maps ASCII codes and Unicode space
// values that definitely terminate any identifier to "true", and
// every other character code to "undefined".  If the code for a
// character following a keyword maps to true in this table then we
// can safely decide that we've seen a keyword, and if it doesn't map
// to true then we're not going to have to look for a keyword in
// identifier().  The latter is the case only because the program will
// cause a syntax error to be signalled if there is a
// non-identifier-constituent Unicode character terminating a keyword.

function generate(tbl, pos) {
    var a = "_".charCodeAt(0);
    var n = "z".charCodeAt(0) - "_".charCodeAt(0) + 1;
    var buckets = new Array(n);
    var newDefault = false;
    var count = 0;

    for ( var i=0 ; i < n ; i++ )
        buckets[i] = new Array();

    var tblItem; // used below if there's only one
    for ( var i=0 ; i < tbl.length ; i++ ) {
        tblItem = tbl[i];
        var item = tblItem[0];
        if (item.length > pos) {
            var x = item.charCodeAt(pos) - a;
            buckets[x].push(tblItem);
            count++;
        }
        if (item.length == pos)
            newDefault = tblItem;
    }

    var s = "";
    if (count == 1) {
        var name = tblItem[0];
        s += "if (";
        for ( var i=pos ; i < name.length ; i++ )
            s += "src.charCodeAt(curIndex+" + (i-1) + ") == " + name.charCodeAt(i) + " /* Char::" + name.charAt(i) + " */ &&\n";
        if (tblItem[2])
            s += tblItem[2] + " &&\n";
        s += "notPartOfIdent[src.charCodeAt(curIndex+" + (name.length-1) + ")]) {\n"
        s += "curIndex += " + (name.length-1) + ";\n";
        s += "return " + tblItem[1] + ";\n";
        s += "}\n";
    }
    if (count > 1) {
        if (pos > 0)
            s += "switch(src.charCodeAt(curIndex+" + (pos - 1) + ")) {\n";
    
        for ( var i=0 ; i < n ; i++ ) {
            if (buckets[i].length > 0) {
                s += ("case " + (a + i) + ": /* Char::" + String.fromCharCode(a + i) + " */\n" + 
                      generate(buckets[i], pos+1));
            }
        }
        s += "default:\n";
    }

    if (newDefault) {
        let precond = "";
        if (newDefault[2])
            precond = newDefault[2] + " && ";
        s += "if (" + precond + "!(notPartOfIdent[src.charCodeAt(curIndex+" + (newDefault[0].length - 1)+ ")])) \n";
        s += "break bigswitch;\n";
        s += "curIndex += " + (newDefault[0].length - 1) + ";\n";
        s += "return " + newDefault[1] + ";\n";
    }
    else
        s += "break bigswitch;\n";

    if (count > 1 && pos > 0)
        s += "}\n";
        
    return s;
}
