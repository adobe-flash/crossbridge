/* -*- mode: java -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Usage: 

   Run this from the core/ directory, the program needs to know where
   to find wopcodes.cpp and there's no parameter to specify it.
   Normally you want the input to be "peephole.tbl" and the output to 
   be "peephole.cpp":
   
     avmshell peephole.abc -- peephole.tbl peephole.cpp
 */

/* Generate peephole optimization state tables from a description.

     peepspec ::= peep*
     peep     ::= pattern guard? action
     pattern  ::= "pattern" ident (";" ident)* eol
     guard    ::= expr
     action   ::= expr (";" expr)
     expr     ::= token+
     token    ::= any C++ token valid in an expression
                | $integer.integer
     ident    ::= C++ identifier
     eol      ::= newline or end-of-file

   Identifiers in the pattern must be opcode names; these are resolved at 
   compile time and the peephole optimizer state machine uses them to
   transition from one state to another.  The guard is evaluated in case 
   there are side conditions apart from the opcode values; the guard must 
   be a C++ expression.  The action represents individual instruction
   words that replace the instructions matched by the pattern; each word
   should be a C++ expression.

   The syntax $n.m refers to the m'th word of the n'th matched instruction.
   It can be used in the guard and action.

   The guard will be evaluated zero or one times per match.  (The current
   implementation only invokes a guard after longer matches have failed,
   so zero times is typical - you can't track state machine progress by
   having side effects in the guard.)

   For example (this one has only one instruction in the action):

     pattern  OP_getlocal ; OP_getlocal ; OP_getlocal
     guard    $0.1 < 1024 && $1.1 < 1024 && $2.1 < 1024
     action   OP_get3locals ; ($2.1 << 20) | ($1.1 << 10) | $0.1

   Continuation lines are allowed, they start with '*' in the first column.
   The continuation line, less its '*', is pasted onto the end of the
   preceding line.

   Comment lines start with // (possibly preceded by blanks).  Comment lines
   and blank lines are ignored everywhere before continuation lines are 
   resolved.  Comments can't follow patterns, guards, or actions on the 
   same line.

   There can be multiple patterns that match the same instruction
   sequence.  The guards are evaluated in the order they appear in the
   input file, and the first action whose guard is true is selected;
   the other actions are ignored.

   Restrictions (not checked by this program):

   - The action cannot introduce more words than the instruction originally
     occupied  (it might not be hard to lift this restriction). 

   - Lookupswitch must not appear in these patterns.

   - Any PC-relative branch instruction must be the last in the pattern or
     in the replacement code.

 */
   
package peephole 
{
    import avmplus.*;

    class Pattern
    {
	const P, G, A;

	function Pattern(P, G, A) {
	    this.P = P;
	    this.G = G;
	    this.A = A;
	}

	public function toString() {
	    return G + " " + A;
	}
    }

    class Node 
    {
	const instr;
	const patterns = new Vector.<Pattern>;
	const children = new Vector.<Node>;

	function Node(instr) {
	    this.instr = instr;
	}

	function getChild(instr) {
	    for ( var i=0, limit=children.length ; i < limit ; i++ )
		if (children[i].instr == instr)
		    return children[i];
	    var c = new Node(instr);
	    children.push(c);
	    return c;
	}

	public function toString() {
	    var s = "";
	    if (patterns.length > 0)
		s += " (" + instr + " -> {" + patterns.join(",") + "})";
	    if (children.length > 0) 
		s += " (" + instr + " -> [" + children.join(",") + "])";
	    return s;
	}
    }

    class State
    {
	var stateno = 0;
	var numTransitions = 0;
	var transitionPtr = 0;
	var guardIndex = 0;
	var fail = 0;
	var failShift = 0;
	var next = null;   // next node at same depth as this node, see computeFailures
	var depth = 0;     // depth of this node
	var ancestor = null;  // the node we came from, or null for the start state
	var documentation = "";

	public function toString() {
	    return "[" + [stateno,numTransitions,transitionPtr,guardIndex,fail,failShift,depth].join(",") + "]";
	}
    }

    // readOpcodes() fleshes out these tables.

    const opcode = {};
    const jump_opcodes = {};
    const MAXINSTR = 350; // last opcode in table above plus one would be adequate

    const opname = new Vector.<String>(MAXINSTR, true);
    for ( var i=0 ; i < opname.length ; i++ )
	opname[i] = "-";

    function assert(cond) {
        if (!cond)
            throw new Error("Assertion failed");
    }

    var longest = 0;

    function readOpcodes() {
	var state = 0;
	var i = 0;
	File.read("wopcodes.cpp").
	    split("\n").
	    filter(function (l) { 
		    var r = state == 1;
		    if (state == 0 && l.match(/^\s*\/\/\s*BEGIN\s*$/))
			state = 1;
		    else if (state == 1 && l.match(/^\s*\/\/\s*END\s*$/))
			state = 2;
		    if (r && l.match(/^\s*\/\//))
			r = false;
		    if (r && l.match(/^\s*$/))
			r = false;
		    return r;
		}).
	    forEach(function (l) {
		     // The format of a line is { num, ... N("name") }
		     // We only care about the second number (the "jumps" attribute) and the
		     // name for the time being.
		     var r = (/^\s*\{\s*([0-9]+)\s*,\s*([0-9]+)[^N]+N\(\"([^\"]+)\"\)/.exec(l));
		     assert(r != null);
		     if (!(r[3].match(/^0x/))) {
			 opcode[r[3]] = i;
			 opname[i] = r[3];
			 longest = Math.max(longest, r[3].length);
			 if (parseInt(r[2]) != 0)
			     jump_opcodes[r[3]] = true;
		     }
		     i++;
		 });
    }

    function preprocess(lines) {
	var j = 0;
	var first = true;
	for ( var i=0, limit=lines.length ; i < limit ; i++ ) {
	    if (/^\s*$/.test(lines[i]))
		continue;
	    if (/^\s*\/\//.test(lines[i]))
		continue;
	    assert(/^(\*|pattern\s+|guard\s+|action\s+)/.test(lines[i]));
	    if (lines[i].charAt(0) == '*') {
		assert(!first);
		lines[j] += lines[i].substring(1);
		continue;
	    }
	    lines[j++] = lines[i];
	    first = false;
	}
	lines.length = j;
	return lines;
    }

    function parse(lines) {
	var output = [];
	var P = null;
	var G = null;
	var A = null;
	var res;
	for ( var i=0, limit=lines.length ; i < limit ; i++ ) {
	    var L = lines[i].replace(/\$([0-9]+)\.([0-9]+)/g, "I[$1][$2]");
	    if (P == null) {
		res = /^pattern\s+(.*)$/.exec(L);
		assert(res != null);
		P = res[1].split(/\s*;\s*/);
		continue;
	    }

	    if (G == null) {
		res = /^guard\s+(.*)$/.exec(L);
		if (res != null) {
		    G = res[1];
		    continue;
		}
	    }

	    if (A == null) {
		res = /^action\s+(.*)$/.exec(L);
		assert(res != null);
		A = res[1].split(/\s*;\s*/);
	    }

	    assert( P != null && A != null );
	    output.push(new Pattern(P, G, A));
	    P = G = A = null;
	} 
	return output;
    }

    function buildTrie(patterns) {
	var T = new Node("");

	for ( var i=0, ilimit=patterns.length ; i < ilimit ; i++ ) {
	    var p = patterns[i];
	    var container = T;

	    for ( var j=0, jlimit=p.P.length ; j < jlimit-1 ; j++ )
		container = container.getChild(p.P[j]);
	    container.getChild(p.P[jlimit-1]).patterns.push(p);
	}
	return T;
    }

    const states = new Vector.<State>;
    const transitions = new Vector.<Array>;
    const actions = new Vector.<* /* (Pattern | Vector.<Pattern>) */>;
    const toplevel = new Vector.<int>(MAXINSTR, true);
    const depths = new Vector.<State>(20, true);

    function generate(trie) {
	var c = trie.children;
	for ( var i=0 ; i < c.length ; i++ ) {
	    if (!(c[i].instr in opcode)) {
		print(c[i].instr);
		for ( var y in opcode ) 
		    print(y);
	    }
	    assert(c[i].instr in opcode);
	    toplevel[opcode[c[i].instr]] = expand(c[i], 1, null, c[i].instr);
	}
	var failures = computeFailures();
	for ( var i=1 ; i < failures.length ; i++ ) {
	    assert(failures[i] >= 0);
	    if (failures[i] > 0) {
		var s = states[i-1];
		var t = states[failures[i]-1];
		// Only record failure transition if target state is not on the path to
		// this state (because if it is then the backtracking logic takes care of it)
		var q;
		for ( q=s.ancestor ; q != t && q != null ; q=q.ancestor )
		    ;
		if (q == null) {
		    // record a shift that is the difference in depth of the current node and the target node.
		    s.fail = failures[i];
		    s.failShift = s.depth-t.depth;
		}
	    }
	}
    }

    // Dragon Book 2nd Ed exercises 3.31 and 3.32.

    function computeFailures() {
	var f = new Vector.<int>(states.length+1, true);
	for ( var i=0 ; i < f.length ; i++ )
	    f[i] = -1;

	for ( var s=depths[1] ; s != null ; s=s.next )
	    f[s.stateno] = 0;

	for ( var d=1 ; d < depths.length ; d++ ) {
	    for ( var sd=depths[d] ; sd != null ; sd=sd.next ) {
		for ( var a=0 ; a < MAXINSTR ; a++ ) {
		    var s2 = g(sd.stateno, a);
		    if (s2 != -1) {
			var s = f[sd.stateno];
			while (g(s, a) == -1)
			    s = f[s];
			f[s2] = g(s, a);
		    }
		}
	    }
	}
	return f;

	// g(m,x) = n >= 0 if there is a transition from m to n on x
	// g(0,x) = 0      if there is no transition from 0 on x
	// g(m,x) = -1     if m > 0 and there is no transition from m on x 

	function g(stateno, input) {
	    if (stateno == 0)
		return toplevel[input];
	    var s = states[stateno-1];
	    var t = s.transitionPtr;
	    var n = s.numTransitions;
	    for ( var i=0 ; i < n ; i++ )
		if (transitions[t+i][0] == input)
		    return transitions[t+i][1];
	    return -1;
	}
    }

    function expand(node, depth, ancestor, documentation) {
	var state = new State;
	state.next = depths[depth];
	depths[depth] = state;
	state.depth = depth;
	state.ancestor = ancestor;
	state.documentation = documentation;
	if (node.patterns.length > 0) {
	    state.guardIndex = actions.length + 1;
	    if (node.patterns.length > 1)
		actions.push(node.patterns);
	    else
		actions.push(node.patterns[0]);
	}
	var trans = new Vector.<Array>;
	for ( var i=0 ; i < node.children.length ; i++ ) {
	    var ci = node.children[i];
	    if (!(ci.instr in opcode))
		print(ci.instr);
	    assert(ci.instr in opcode);
	    trans.push([opcode[ci.instr], expand(ci, depth+1, state, documentation + " " + ci.instr)]);
	}
	trans.sort(function (a,b) { return a[0] - b[0] });
	state.numTransitions = trans.length;
	state.transitionPtr = trans.length > 0 ? transitions.length : 0;
	for ( var i=0 ; i < trans.length ; i++ )
	    transitions.push(trans[i]);
	var stateno = states.push(state);
	state.stateno = stateno;
	return stateno;
    }

    function formatStates() {
	var s = [];
	s.push("const WordcodeEmitter::peep_state_t WordcodeEmitter::states[] = {");
	s.push("//n  s  t  g  f");
	s.push("{ 0, 0, 0, 0, 0 }, // Invalid");
	for ( var i=0 ; i < states.length ; i++ ) {
	    var S = states[i];
	    assert(S.numTransitions < 256);
	    assert(S.failShift < 256);
	    assert(S.transitionPtr < 65536);
	    assert(S.guardIndex < 65536);
	    assert(S.fail < 65536);
	    s.push( "{ " + 
		    S.numTransitions + ", " +
		    S.failShift + ", " +
		    S.transitionPtr + ", " +
		    S.guardIndex + ", " +
		    S.fail +" }, " + 
		    "// " + ((i+1)%10 == 0 ? pad(i+1,4) : "   ") + S.documentation);
	}
	s.push("};");
	return s.join("\n");
    }

    function formatTransitions() {
	var s = [];
	s.push("const WordcodeEmitter::peep_transition_t WordcodeEmitter::transitions[] = {");
	for ( var i=0 ; i < transitions.length ; i++ ) {
	    assert(transitions[i][1] < 65536);
	    s.push("{ WOP_" + opname[transitions[i][0]] + ", " + transitions[i][1] + " }," + (i > 0 && i % 10 == 0 ? " // " + i : ""));
	}
	s.push("};");
	return s.join("\n");
    }

    function pad(s,n) {
	s = String(s);
	while (s.length < n)
	    s += " ";
	return s;
    }

    function formatToplevel() {
	var s = [];
	s.push("const uint16 WordcodeEmitter::toplevel[] = {");
	var i=0;
	while (i < MAXINSTR) {
	    var t = "";
	    var n = "";
	    for ( var j=0 ; j < 8 && i < MAXINSTR ; j++, i++ ) {
		assert(toplevel[i] < 65536);
		t += toplevel[i] + ", ";
		n += " " + pad(opname[i], longest);
	    }
	    t += "//" + n;
	    s.push(t);
	}
	s.push("};");
	return s.join("\n");
    }

    function formatActions() {

	function makeAssert(P) {
	    var s = "";
	    for ( var i=0 ; i < P.length ; i++ ) {
		if (i > 0)
		    s += " && ";
		s += "I[" + i + "][0] == NEW_OPCODE(WOP_" + P[i] + ")";
	    }
	    return "        AvmAssert(" + s + ");";
	}

	function emit(A) {
	    var s = [];
	    var undo = false;
	    s.push("        if (" + (A.G ? A.G : "true") + ") {");
	    if (A.P[A.P.length-1] in jump_opcodes) {
		undo = true;
		s.push("            undoRelativeOffsetInJump();");
	    }
	    s.push("            S[0] = WOP_" + A.A[0] + ";");
	    s.push("            R[0] = NEW_OPCODE(WOP_" + A.A[0] + ");");
	    for ( var j=1 ; j < A.A.length ; j++ )
		s.push("            R[" + j + "] = " + A.A[j] + ";");
	    s.push("            return replace(" + A.P.length + "," + A.A.length + (undo ? ",true" : "") + ");");
	    s.push("        }");
	    return s.join("\n");
	}

	var s = [];
	s.push("bool WordcodeEmitter::commit(uint32 action)");
	s.push("{");
	s.push("    switch (action) {");
	for ( var i=0 ; i < actions.length ; i++ ) {
	    var A = actions[i];
	    s.push("    case " + (i+1) + ":");
	    if (A is Vector.<Pattern>) {
		s.push(makeAssert(A[0].P));
		for ( var j=0 ; j < A.length ; j++ )
		    s.push(emit(A[j]));
	    }
	    else {
		s.push(makeAssert(A.P));
		s.push(emit(A));
	    }
	    s.push("        return false;");
	}
	s.push("    default:");
	s.push("        AvmAssert(!\"Should not happen\");");
	s.push("        return false;");
	s.push("    }");
	s.push("}");
	return s.join("\n");
    }

    if (System.argv.length != 2) {
        print("Usage: peephole.abc -- inputfile outputfile");
        System.exit(1);
    }

    readOpcodes();
    generate(buildTrie(parse(preprocess(File.read(System.argv[0]).split("\n")))));
    File.write(System.argv[1],
               "/* This Source Code Form is subject to the terms of the Mozilla Public\n" +
               " * License, v. 2.0. If a copy of the MPL was not distributed with this\n" +
               " * file, You can obtain one at http://mozilla.org/MPL/2.0/. */\n\n" 
	       "// Generated by utils/peephole.as\n\n" +
	       "#include \"avmplus.h\"\n\n" +
	       "namespace avmplus\n" +
	       "{\n" +
	       "#ifdef AVMPLUS_PEEPHOLE_OPTIMIZER\n\n" +

	       formatStates() + "\n\n" +
	       formatTransitions() + "\n\n" +
	       formatToplevel() + "\n\n" +
	       formatActions() + "\n\n" +

	       "#endif // AVMPLUS_PEEPHOLE_OPTIMIZER\n" +
	       "}\n");
}
