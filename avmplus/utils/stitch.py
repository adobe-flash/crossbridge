# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# lhansen@adobe.com / 2007-08-06
#
# Usage:
#  stitch <inputfile> <outputfile>
#
# This program transforms input to output by peforming the following actions.
#
#  - check that there are no illegal characters in the input: ASCII only, no tabs
#  - remove HTML comments
#  - replace wiki formatting with HTML formatting
#  - renumber headers to account for inclusion levels (all files start with H1)
#  - stitch together files by processing INCLUDEs that reference HTML and CSS files
#  - include source code by processing INCLUDEs that reference ES4 or SML fragments
#  - resolve cross references
#
# Wiki formatting summary:
#
#  - = ... = for H1, == ... == for H2, etc
#  - ''...'' for <code>...</code>
#  - [[...]] for <code>[[...]]</code>
#  - **...** for <b>...</b>
#  - ^^...^^ for <code><b>...</b></code>
#  - //...// for <i>...</i>
#  - %%...%% for ..., unprocessed
#  - {{{ ... }}} for <pre>...</pre>, blank lines removed at the beginning and end,
#    contents unprocessed.  The triple braces must be at the start of a line.
#  - ""..."" for bold, normal text (handy inside the scope of //..//)
#  - <entity> for some simple replacements, see full list in the code below
#  - foo_{bar} for foo<sub>bar</sub>

# Rules for processing in general:
#  - Read the entire file
#  - Pass 1:
#    - scan for illegal characters
#  - Pass 2:
#    - remove /<!-- ... -->/
#    - replace /(\=)+\s+([...]+)\s+\1/ with the appropriate header tag 
#    - replace /<h([1-5]) (...)>(...)</h\1>/ with appropriate tags and the
#      content from the tag and text
#    - replace <XREF ...>
#    - replace tags like <INFINITY> with correct character codes
#  - Pass 3:
#    - process INCLUDE files:
#      - for HTML files, process them separately using the same process, 
#        incrementing the header level counter on entry and CDing to the
#        correct directory.  Then insert the finished text.
#      - for source files, extract the contents as requested and insert
#        it, undented, in a <pre> tag
#  - Finally, if we're at the top level file:
#    - Spit out the file
#  - Otherwise, our caller consumes the text, so just return it
#
# Directories for processing INCLUDE:
#  - the startup directory is resolve(".")
#  - the directory for .sml files is resolve("..")
#  - the directory for .es files is resolve("../builtins")
#  - the directory for .html and .css files is always the directory of 
#    the including file


# FIXME: get the header computation logic right and document it.
# FIXME: how to do cross references properly?
# FIXME: include SML source code

import re, sys, os, os.path

DEBUG = False
LINELIM = 1000

htmlcomment = re.compile(r"<!--(?:.|\s)*?-->")
wikiheader = re.compile(r"^(\=+)\s+(.*?)\s+\1", re.M)
htmlheader = re.compile(r"<h([1-6])((?:\".*?\"|[^\"])*?)>(.*?)</h\1>")
xreftag = re.compile(r"<XREF\s+target=\"(.*?)\"\s*>")
includetag = re.compile(r"<h[1-6]|<(?:INCLUDE|INCLUDECTX|SIGNATURE)(?:\".*?\"|[^\"])*?>")
signaturetag = re.compile(r"<h[1-6]|<SIGNATURE(?:\".*?\"|[^\"])*?>")
htmlInclude = re.compile(r"<(?:INCLUDE|SIGNATURE)\s*file=\"(.*?\.(?:html|css))\">")
smlInclude = re.compile(r"<(?:INCLUDE|SIGNATURE)\s+file=\"(.*?\.sml)\"\s+name=\"(.*?)\">")
esInclude = re.compile(r"<(?:INCLUDE|SIGNATURE)\s+file=\"(.*?\.es)\"\s+name=\"(.*?)\">")
esIncludeCtx = re.compile(r"<INCLUDECTX\s+file=\"(.*?\.es)\"\s+name=\"(.*?)\"\s+ctx=\"(.*?)\">")
hdrprefix = re.compile(r"<h([1-6])")
wikiformatCode = re.compile(r"''((?:.|\s)*?)''")
wikiformatLiteralBold = re.compile("\"\"((?:.|\\s)*?)\"\"")
wikiformatSpecial = re.compile(r"(\[\[(.*?)\]\])")
wikiformatBold = re.compile(r"\*\*((?:.|\s)*?)\*\*")
wikiformatES4 = re.compile(r"\^\^((?:.|\s)*?)\^\^")
wikiformatItalic = re.compile(r"//((?:.|\s)*?)//")
wikiformatLiteral = re.compile(r"(?!%%--[0-9]+--%%)%%(.*?)%%")
wikiformatLiteralRecover = re.compile(r"%%--([0-9]+)--%%")
wikiformatCodeblock = re.compile(r"^\{\{\{((?:.|[\n\r])*?)^\}\}\}", re.M)
entitytag = re.compile(r"<(INFINITY|NOTE|EXAMPLE|SPECNOTE|FIXME|COMP|IMPLNOTE|LDOTS|LEQ|GEQ|LT|GT|TIMES|PI|P|p|DESC|RETN|IMPL|SEM|SHORTIMPL|---)>")
subscript = re.compile(r"([a-zA-Z]+)_\{([^}]+)\}")
informative = re.compile(r"informative::([A-Za-z0-9_]+)");
helper = re.compile(r"helper::([A-Za-z0-9_]+)");

entities = { "INFINITY": "&#x221E;",
            "NOTE": "<p class=\"note\"><b>NOTE</b>&nbsp;&nbsp; ",
            "EXAMPLE": "<p class=\"note\"><b>EXAMPLE</b>&nbsp;&nbsp; ",
            "SPECNOTE": "<p class=\"note\"><b>SPEC NOTE</b>&nbsp;&nbsp; ",
            "COMP": "<p class=\"note\"><b>COMPATIBILITY NOTE</b>&nbsp;&nbsp; ",
            "IMPLNOTE": "<p class=\"note\"><b>IMPLEMENTATION NOTE</b>&nbsp;&nbsp; ",
            "FIXME": "<p class=\"fixme\"><b>FIXME</b>&nbsp;&nbsp; ",
            "LDOTS": "&#x0085;",
            "LEQ": "&#x2264;",
            "GEQ": "&#x2265;",
            "LT": "<",
            "GT": ">",
            "TIMES": "&#x00D7;",
            "PI": "&#x03C0;",
            "P": "<P><span class=\"pcounter\"></span>",
            "p": "<P><span class=\"pcounter\"></span>",
            "DESC": "<P class=\"section\"><b>Description</b> <P><span class=\"pcounter\"></span>",
            "RETN": "<P class=\"section\"><b>Returns</b> <P><span class=\"pcounter\"></span>",
            "IMPL": "<P class=\"implsection\"><b>Implementation</b> <P><span class=\"pcounter\"></span>",
            "SEM": "<P class=\"implsection\"><b>Semantics</b> <P><span class=\"pcounter\"></span>",
            "SHORTIMPL": "<P class=\"implsection\"><b>Implementation</b>",
            "---": "&#0151;"
         }

currentlevel = 0

es_dir = os.path.abspath("../builtins")
sml_dir = os.path.abspath("..")

# fn is the filename from the INCLUDE tag: it has no directory component.
# Returns a sequence of strings.
#
# Works by looking for the string 'name' at the beginning of a line
# preceded only by n whitespace chars, grabbing that line and all
# subsequent lines up to a line with <= n whitespace chars preceding
# non-whitespace chars or up to and including the line with a } in
# position n.  Trailing empty lines, if any, are removed.
#
# If it's too slow to process a file every time we want to extract a
# function then this interface can be backed by a simple program that
# processes each file once, extracting all functions and storing them
# in some easier format (maybe).

def isIdent(c):
    return c >= "A" and c <= "Z" or c >= "a" and c <= "z" or c >= "0" and c <= "9" or c == "_"

def unComment(s):
    i = len(s)-1
    while i > 0 and s[i] != "\"":
        if s[i] == "/" and s[i-1] == "/":
            while i >= 0 and s[i] == "/":
                i = i - 1
            s = s[0:i+1].rstrip()
            i = len(s)-1
        else:
            i = i - 1
    return s

def adHocFixFunctionType(s):
    s = re.sub(r"/\*: Callable\*/", ": Callable", s)
    s = re.sub(r"/\*this: Callable,\*/", "this: Callable,", s)
    return s

def extractES(fn, name, isSignature, isContextual):
    f = open(os.path.normpath(es_dir + "/" + fn), 'r')
    outside = True
    # Avoid matching prefixes of names
    lastIsIdent = isIdent(name[len(name)-1])
    name = reEscape(name)
    if lastIsIdent:
        name = name + r"(?![a-zA-Z0-9_])"
    starting = re.compile("^( *)" + name)
    starting2 = re.compile("^( *)" + str(isContextual))
    blanks = 0
    prev = ""
    inContext = False
    for line in f:
        line = adHocFixFunctionType(line)
        if outside:
            if isContextual and not inContext:
                m = starting2.search(line)
                if m:
                    inContext = True
                continue
            m = starting.search(line)
            if m:
                ending = re.compile("^" + m.group(1) + r"[^\s]")
                openbrace = re.compile(m.group(1) + r"\{")
                closebrace = re.compile(m.group(1) + r"\}")
                res = [line.rstrip()]
                outside = False
                continue
        else:
            line = unComment(line.rstrip())
            if ending.search(line):
                # Special case for common pattern: open brace indented like the name
                if openbrace.search(line):
                    res = res + [line]
                    continue
                if closebrace.search(line):
                    res = res + [line]
                    break
                break
            if line == "":
                # Count blank lines, but not if the previous line ended with {
                if not(len(prev) > 0 and prev[len(prev)-1] == "{"):
                    blanks = blanks+1
            elif res == []:
                # Skip blanks at the beginning
                res = res + [line]
            else:
                # Add one blank line for a sequence of blanks
                if blanks > 0:
                    res = res + [""]
                blanks = 0
                res = res + [line]
                prev = line
    f.close()
    if outside:
        print(fn + ": Could not find definition for " + name)
        sys.exit(1)
    undent = len(re.search(r"^(\s*)", res[0]).group(1))
    if isSignature:
        s = res[0][undent:]
        s = re.sub(r" native", "", s)
        if s[len(s)-1] == "{" or s[len(s)-1] == ";":
            s = s[:len(s)-1].rstrip()
        if len(s) >= LINELIM:
            i = len(s)-1;
            while i >= 0 and s[i] != ')':
                if s[i] == ':' and (i == 0 or s[i-1] == ' ' or s[i-1] == ')'):
                    s = s[:i] + "\n        " + s[i:]
                    break
                i = i - 1
        return s + " &#x0085";
    else:
        ss = "\n"
        for s in res:
            ss = ss + s[undent:] + "\n"
        return ss

def extractSML(fn, name):
    f = open(os.path.normpath(sml_dir + "/" + fn), 'r')
    outside = True
    # Avoid matching prefixes of names
    lastIsIdent = isIdent(name[len(name)-1])
    name = reEscape(name)
    if lastIsIdent:
        name = name + r"(?![a-zA-Z0-9_])"
    starting = re.compile("^( *)" + name)
    blanks = 0
    prev = ""
    ldots = re.compile(r"\(\* *LDOTS *\*\)")
    lparen = re.compile(r"\(\* LPAREN \*\)")
    ldots_rparen = re.compile(r"\(\* LDOTS_RPAREN \*\)")
    spec = re.compile(r"\(\* *SPEC:([^\*]*)\*\)")
    skipping = False
    for line in f:

        line = spec.sub(r"\1", line)
        line = lparen.sub(r"(", line)
        line = ldots_rparen.sub(r"&#x0085;)", line)

        # Skip informative lines
        if re.search("BEGIN_INFORMATIVE", line):
            skipping = True
            continue

        if re.search("END_INFORMATIVE", line):
            skipping = False
            continue

        if re.search("INFORMATIVE", line):
            continue

        if skipping:
            continue


        if outside:
            m = starting.search(line)
            if m:

                # format comments indicating elision and stop there
                if ldots.search(line):
                    line = line.rstrip()
                    line = ldots.sub("&#x0085;", line)
                    res = [line]
                    outside = False
                    break

                if re.search("^datatype", name):
                    ending = re.compile("^" + m.group(1) + r"([^\s]|     and)")
                elif re.search("^withtype", name):
                    ending = re.compile("^" + m.group(1) + r"([^\s]|     and)")
                elif re.search("^type", name):
                    ending = re.compile("^" + m.group(1) + r"([^\s]| and)")
                else:
                    ending = re.compile("^" + m.group(1) + r"[^\s]")

            res = [line.rstrip()]
            outside = False
            continue
        else:
            line = line.rstrip()

            if ending.search(line):
                break
            else:
                # format comments indicating elision and stop there
                if ldots.search(line):
                    line = ldots.sub("&#x0085;", line)
                    res = res + [line]
                    break

            if line == "":
                # Count blank lines, but not if the previous line ended with {
                if not(len(prev) > 0 and prev[len(prev)-1] == "{"):
                    blanks = blanks+1
            elif res == []:
                # Skip blanks at the beginning
                res = res + [line]
            else:
                # Add one blank line for a sequence of blanks
                if blanks > 0:
                    res = res + [""]
                blanks = 0
                res = res + [line]
                prev = line
    f.close()
    if outside:
        print(fn + ": Could not find definition for " + name)
        sys.exit(1)
    undent = len(re.search(r"^(\s*)", res[0]).group(1))
    ss = "\n"
    for s in res:
        ss = ss + s[undent:] + "\n"
    return ss

def replaceWiki(m):
    return "<h" + str(len(m.group(1))) + ">" + m.group(2) + "</h" + str(len(m.group(1))) + ">"

# Ad-hoc: if a header contains a leftparen, then it's probably a call,
# so we want to preserve spaces.  So insert nbsp for space sequences.

def headerFormat(s):
    if re.search(r"\(", s):
        return re.sub(r"\s+", "&nbsp;", s.strip())
    else:
        return s

def replaceHTML(m, hdrlvl):
    k = str(int(m.group(1)) + hdrlvl - 1)
    return "<h" + k + m.group(2) + ">" + headerFormat(m.group(3)) + "</h" + k + ">"

def replaceXREF(m):
    return "<span class=\"xref\">" + m.group(1) + "</span>"

def removeInitialNewine(s):
    if s[0] == "\n":
        return s[1:]
    else:
        return s

def nsCollapse(s):
    s = re.sub(informative, r"<em>\1</em>", s)
    s = re.sub(helper, r"<em>\1</em>", s)
    return s

def replaceInclude(m, hdrlvl, fn):
    global currentlevel
    isSignature = re.search(r"^<SIGNATURE",m.group(0))
    isContextual = re.search(r"^<INCLUDECTX",m.group(0))
    ms = hdrprefix.match(m.group(0))
    if ms:
        currentlevel = int(ms.group(1))
        return m.group(0)
    ms = htmlInclude.match(m.group(0))
    if ms:
        olddir = os.getcwd()
        oldlevel = currentlevel
        if os.path.dirname(ms.group(1)) != "":
            os.chdir(os.path.dirname(ms.group(1)))
        r = process(os.path.basename(ms.group(1)), currentlevel+1)
        currentlevel = oldlevel
        os.chdir(olddir)
        return r
    ms = smlInclude.match(m.group(0))
    if ms:
        return "<PRE>" + reSML(extractSML(ms.group(1), ms.group(2))) + "</PRE>"
    if isContextual:
        ms = esIncludeCtx.match(m.group(0))
    else:
        ms = esInclude.match(m.group(0))
    if ms:
        if isSignature:
            return htmlEscape(extractES(ms.group(1), ms.group(2), True, False))
        else:
            if isContextual:
                isContextual = ms.group(3)
            return "<PRE>" + nsCollapse(htmlEscape(extractES(ms.group(1), ms.group(2), False, isContextual))) + "</PRE>"
    print(fn + ": Invalid INCLUDE directive: " + m.group(0))
    sys.exit(1)

def replaceEntity(m):
    global entities
    return entities[m.group(1)]

literals = []

def htmlEscape(s):
    return re.sub("<", "&#60;", s)

def reSML(s):
    return re.sub(r"([^a-zA-Z0-9])(\||=>|abstype|and|andalso|as|case|datatype|do|else|end|exception|fn|fun|handle|if|in|infix|infixr|let|local|nonfix|of|op|open|orelse|raise|rec|then|type|val|with|withtype|while)([^a-zA-Z0-9])", r"\1<b>\2</b>\3", s);

def reEscape(s):
    return re.sub(r"([\(\)\[\]\.\*\+\?\{\}\^\$])", r"\\\1", s)

def hideLiteral(m):
    global literals
    k = len(literals)
    literals = literals + [htmlEscape(m.group(1))]
    return "%%--" + str(k) + "--%%"

def hideCodeblock(m):
    global literals
    k = len(literals)
    literals = literals + ["<PRE>" + htmlEscape(m.group(1)) + "</PRE>"]
    return "%%--" + str(k) + "--%%"

def revealLiteral(m):
    global literals
    return literals[int(m.group(1))]

def numberPreformatted(m):
    lines = re.split(r"\n", m.group(1))
    ss = ""
    i = 0
    while lines[i] == "":
        i = i+1
    j = len(lines)
    while lines[j-1] == "":
        j = j-1
    if i == j:
        return ""
    k = 1
    while i < j:
        n = str(k)
        indent = "    "[0:5-len(n)]
        ss = ss + n + indent + lines[i] + "\n"
        k = k + 1
        i = i + 1
    return "<PRE>" + ss + "</PRE>"

def trace(s):
    if DEBUG:
        print(s)

def process(fn, hdrlvl):
    global literals
    trace("FILE: " + fn)
    input = open(fn, 'r')
    text = input.read()
    text = re.sub(htmlcomment, "", text)
    for i in range(len(text)):
        cc = ord(text[i])
        if cc > 127 or cc < 32 and cc != 10 and cc != 13:
            print(fn + ": Non-ASCII character in at location " + str(i) + ": " + str(cc))
            sys.exit(1)
    literals = []
    # entities and signatures are replaced even in <PRE> blocks, so do that first.
    trace("entity")
    text = re.sub(entitytag, replaceEntity, text)
    trace("signature")
    text = re.sub(signaturetag, lambda m: replaceInclude(m, hdrlvl, fn), text)
    trace("codeblock")
    text = re.sub(wikiformatCodeblock, hideCodeblock, text) 
    trace("wikiliteral")
    text = re.sub(wikiformatLiteral, hideLiteral, text)
    trace("wikiheader")
    text = re.sub(wikiheader, replaceWiki, text)
    trace("htmlheader")
    text = re.sub(htmlheader, lambda m: replaceHTML(m, hdrlvl), text)
    trace("xreftag")
    text = re.sub(xreftag, replaceXREF, text)
    trace("wikicode")
    text = re.sub(wikiformatCode, r"<code>\1</code>", text)
    trace("wikiliteralbold")
    text = re.sub(wikiformatLiteralBold, "<span class=\"literal\">\\1</span>", text)
    trace("wikispecial")
    text = re.sub(wikiformatSpecial, r"<code>\1</code>", text)
    trace("wikibold")
    text = re.sub(wikiformatBold, r"<b>\1</b>", text)
    trace("wikiES4 code")
    text = re.sub(wikiformatES4, r"<code><b>\1</b></code>", text)
    trace("wikiitalic")
    text = re.sub(wikiformatItalic, r"<i>\1</i>", text)
    trace("wikiliteralrecover")
    text = re.sub(wikiformatLiteralRecover, revealLiteral, text)
    trace("include")
    text = re.sub(includetag, lambda m: replaceInclude(m, hdrlvl, fn), text)
    trace("subscripts")
    text = re.sub(subscript, r"\1<sub>\2</sub>", text)
    trace("numberpredefined")
    
    # FIXME (comment by graydon): this last substitution doubly-counterizes blocks that were
    # added by <INCLUDE>, so it winds up looking like junk with overlapping numbers. Don't
    # know if this is crucial for some other aspect of formatting. Disabling for now.
    #
    # text = re.sub(r"<PRE>\n","<PRE><span class=\"pcounter\"></span>", text)

    trace("done!")
    return text

if len(sys.argv) != 3:
    print("Usage: stitch inputfile outputfile")
    sys.exit(2)

#premerge = re.compile(r"^</PRE>\s*^<PRE>", re.M)
# Python quality software:  Feed enough text to this RE and it whines about "recursion limit exceeded".
# prenumber = re.compile(r"^<PRE>((?:.|\s)*?)^</PRE>", re.M)

text = process(sys.argv[1], 1)
#text = re.sub(premerge, "", text);
# See above
# text = re.sub(prenumber, numberPreformatted, text);
output = open(sys.argv[2], 'w')
output.write(text)
output.close()

