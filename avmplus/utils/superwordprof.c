/* -*- c-indentation-style: stroustrup -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Superword profiling for Tamarin.
 *
 * Usage summary:
 *    superwordprof [options] ( code-file sample-file ) ...
 *
 * Options:
 *    -c n    Cutoff: do not show sequences whose dynamic execution count
 *            is less than n (default 100)
 *
 *    -l n    Cutoff: show only sequences whose length is at most n
 *            (default unbounded)
 *
 *    -f      Flat: print results in flat form as well as hierarchical
 *
 *    -h      Disable hierarchical report
 *
 *    -o      Track operand values symbolically
 *
 *    -v      Verbose: print useful diagnostics on stderr
 *
 *
 * Description:
 *
 * First enable SUPERWORD_PROFILING in core/avmbuild.h and rebuild
 * Tamarin.  Then run Tamarin on a program; the output will be two
 * files, superwordprof_code.txt and superwordprof_sample.txt.
 *
 * Then run the present program, providing those two files in that
 * order as arguments.  Output is presented on stdout describing
 * common dynamic code sequences.  More than one pair of files can be
 * provided; the output represents aggregate statistics across all the
 * runs.
 *
 *
 * Missing features:
 *
 * - Ability to track argument values symbolically, eg, to discover
 *   whether SETLOCAL; GETLOCAL is really SETLOCAL a; GETLOCAL a
 *   (which we might optimize) or SETLOCAL a; GETLOCAL b (which we
 *   shouldn't)
 *
 * - Ability to abstract instruction sequences that leave the stack
 *   invariant, so that GETGLOBALSCOPE; <comp>; SETSLOT can be
 *   recognized as a cliche, for example.  This also aids the handling
 *   of commutative operations where the right operand is often a
 *   constant and an "operate with immediate" instruction would be
 *   effective.  Consider "C * x" => "x * C" for constant "C" some number 
 *   (at least) and operator "*" at least in the set &, |, ^, *, ==, ===.
 *   The key is that that there are no observable changes to the 
 *   order of evaluation as long as one of the operands is a constant.  
 *   What we're looking for here is BITS; <comp>; OP.  (The compiler 
 *   should really be able to help us out here by performing the
 *   transformation, but doesn't at present.)  A plausible way to handle
 *   this is to track stack effects (+ / -) separately, so that a
 *   single instruction that has a +1/-0 effect is considered a trivial
 *   value generator (like GETGLOBALSCOPE and BITS above); then a
 *   sequence of instructions that have a +1/-0 effect and never goes
 *   negative is an arbitrary value producer; then an OP that is (+1/-2)
 *   or (+0/-2) is the operator (like SETSLOT or AND).  We only need
 *   to track within the basic block (or even within the peephole
 *   window), so it's very simple.  But in practice this is like the dog
 *   chasing a car: what do we do when we catch it?  Tracking this in
 *   the peephole optimizer in the VM might be a bit too elaborate.
 *
 * Bugs:
 *
 * - Some of the tables are fixed size and the program halts when they
 *   overflow.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

typedef struct codestruct codestruct_t;
struct codestruct {
    unsigned *code;
    unsigned start;
    unsigned limit;
    codestruct_t *next;
};

typedef struct trie trie_t;
struct trie {
    unsigned opcode;
    unsigned opd1;
    unsigned opd2;
    unsigned count;
    trie_t *right_sibling;
    trie_t *left_child;
};

typedef struct seq seq_t;
struct seq {
    unsigned* opcode;
    unsigned* operand1;
    unsigned* operand2;
    unsigned length;
    unsigned count;
    int suffix;
};

enum {
    OP_bkpt=0x01,
    OP_throw=0x03,
    OP_getsuper=0x04,
    OP_setsuper=0x05,
    OP_dxns=0x06,
    OP_dxnslate=0x07,
    OP_kill=0x08,
    OP_ifnlt=0x0C,
    OP_ifnle=0x0D,
    OP_ifngt=0x0E,
    OP_ifnge=0x0F,
    OP_jump=0x10,
    OP_iftrue=0x11,
    OP_iffalse=0x12,
    OP_ifeq=0x13,
    OP_ifne=0x14,
    OP_iflt=0x15,
    OP_ifle=0x16,
    OP_ifgt=0x17,
    OP_ifge=0x18,
    OP_ifstricteq=0x19,
    OP_ifstrictne=0x1A,
    OP_lookupswitch=0x1B,
    OP_pushwith=0x1C,
    OP_popscope=0x1D,
    OP_nextname=0x1E,
    OP_hasnext=0x1F,
    OP_pushnull=0x20,
    OP_pushundefined=0x21,
    OP_nextvalue=0x23,
    OP_pushtrue=0x26,
    OP_pushfalse=0x27,
    OP_pushnan=0x28,
    OP_pop=0x29,
    OP_dup=0x2A,
    OP_swap=0x2B,
    OP_pushstring=0x2C,
    OP_pushdouble=0x2F,
    OP_pushscope=0x30,
    OP_pushnamespace=0x31,
    OP_hasnext2=0x32,
    OP_newfunction=0x40,
    OP_call=0x41,
    OP_construct=0x42,
    OP_callmethod=0x43,
    OP_callstatic=0x44,
    OP_callsuper=0x45,
    OP_callproperty=0x46,
    OP_returnvoid=0x47,
    OP_returnvalue=0x48,
    OP_constructsuper=0x49,
    OP_constructprop=0x4A,
    OP_callproplex=0x4C,
    OP_callsupervoid=0x4E,
    OP_callpropvoid=0x4F,
    OP_applytype=0x53,
    OP_newobject=0x55,
    OP_newarray=0x56,
    OP_newactivation=0x57,
    OP_newclass=0x58,
    OP_getdescendants=0x59,
    OP_newcatch=0x5A,
    OP_findpropstrict=0x5D,
    OP_findproperty=0x5E,
    OP_finddef=0x5F,
    OP_getlex=0x60,
    OP_setproperty=0x61,
    OP_getlocal=0x62,
    OP_setlocal=0x63,
    OP_getglobalscope=0x64,
    OP_getscopeobject=0x65,
    OP_getproperty=0x66,
    OP_getouterscope=0x67,
    OP_initproperty=0x68,
    OP_deleteproperty=0x6A,
    OP_getslot=0x6C,
    OP_setslot=0x6D,
    OP_getglobalslot=0x6E,
    OP_setglobalslot=0x6F,
    OP_convert_s=0x70,
    OP_esc_xelem=0x71,
    OP_esc_xattr=0x72,
    OP_convert_i=0x73,
    OP_convert_u=0x74,
    OP_convert_d=0x75,
    OP_convert_b=0x76,
    OP_convert_o=0x77,
    OP_checkfilter=0x78,
    OP_coerce=0x80,
    OP_coerce_b=0x81,
    OP_coerce_a=0x82,
    OP_coerce_i=0x83,
    OP_coerce_d=0x84,
    OP_coerce_s=0x85,
    OP_astype=0x86,
    OP_astypelate=0x87,
    OP_coerce_u=0x88,
    OP_coerce_o=0x89,
    OP_negate=0x90,
    OP_increment=0x91,
    OP_inclocal=0x92,
    OP_decrement=0x93,
    OP_declocal=0x94,
    OP_typeof=0x95,
    OP_not=0x96,
    OP_bitnot=0x97,
    OP_add=0xA0,
    OP_subtract=0xA1,
    OP_multiply=0xA2,
    OP_divide=0xA3,
    OP_modulo=0xA4,
    OP_lshift=0xA5,
    OP_rshift=0xA6,
    OP_urshift=0xA7,
    OP_bitand=0xA8,
    OP_bitor=0xA9,
    OP_bitxor=0xAA,
    OP_equals=0xAB,
    OP_strictequals=0xAC,
    OP_lessthan=0xAD,
    OP_lessequals=0xAE,
    OP_greaterthan=0xAF,
    OP_greaterequals=0xB0,
    OP_instanceof=0xB1,
    OP_istype=0xB2,
    OP_istypelate=0xB3,
    OP_in=0xB4,
    OP_increment_i=0xC0,
    OP_decrement_i=0xC1,
    OP_inclocal_i=0xC2,
    OP_declocal_i=0xC3,
    OP_negate_i=0xC4,
    OP_add_i=0xC5,
    OP_subtract_i=0xC6,
    OP_multiply_i=0xC7,
    OP_getlocal0=0xD0,
    OP_getlocal1=0xD1,
    OP_getlocal2=0xD2,
    OP_getlocal3=0xD3,
    OP_setlocal0=0xD4,
    OP_setlocal1=0xD5,
    OP_setlocal2=0xD6,
    OP_setlocal3=0xD7,
    OP_debugline=0xF0,
    OP_debugfile=0xF1,
    OP_bkptline=0xF2,
    OP_ext_pushbits=0x101,
    OP_ext_push_doublebits=0x102
};

#define LAST_INSTR    0x102
#define INSTRCOUNT    LAST_INSTR+1
#define DEF(name, opds)     assert(name < INSTRCOUNT); iname[name] = #name + 3; operands[name] = opds
#define MIN(a,b)      ((a) < (b) ? (a) : (b))

const char *progname = "superwordprof";
int verbose = 0;
int flat = 0;
int hierarchical = 1;
int operand_tracking = 0;
int cutoff_count = 100;
int cutoff_length = INT_MAX;
int scale = 0;
codestruct_t codestructs[1000];
int wordsize = 4;
int nextcodestruct = 0;
trie_t toplevel[INSTRCOUNT];
int jumps[INSTRCOUNT];
int operands[INSTRCOUNT];
const char *iname[INSTRCOUNT];
unsigned ibuf[1000];
unsigned operand1[1000];
unsigned operand2[1000];
seq_t seqs[10000];
int nextseq = 0;

void read_code(const char* filename);
void read_samples(const char* filename);
void extract_superwords();
int fail(const char *s, ...);
int codestruct_cmp(const void *a, const void *b);
int pc_cmp(const void *a, const void *b);
int seq_cmp(const void *a, const void *b);
trie_t* new_trie_node(unsigned opcode, unsigned opd1, unsigned opd2);
unsigned compute_operands(unsigned opcodes[], unsigned op, unsigned* opd1, unsigned* opd2, int trackeridx, unsigned opd[][10], unsigned next[]);
void descend(trie_t *t, int level);
void emit(int level, unsigned count);
void printseq(int i, int indent);
void printhseq(int i, int level) ;
void *safemalloc(size_t nbytes);

int main(int argc, char** argv) 
{
    int i;

    for ( i=0 ; i < INSTRCOUNT ; i++ ) {
	toplevel[i].opcode = i;
	toplevel[i].count = 0;
	toplevel[i].left_child = NULL;
	toplevel[i].right_sibling = NULL;
	jumps[i] = 0;
	operands[i] = 0;
	iname[i] = "***";
    }
    jumps[OP_throw] = 1;
    jumps[OP_ifnlt] = 1;
    jumps[OP_ifnle] = 1;
    jumps[OP_ifngt] = 1;
    jumps[OP_ifnge] = 1;
    jumps[OP_jump] = 1;
    jumps[OP_iftrue] = 1;
    jumps[OP_iffalse] = 1;
    jumps[OP_ifeq] = 1;
    jumps[OP_ifne] = 1;
    jumps[OP_iflt] = 1;
    jumps[OP_ifle] = 1;
    jumps[OP_ifgt] = 1;
    jumps[OP_ifge] = 1;
    jumps[OP_ifstricteq] = 1;
    jumps[OP_ifstrictne] = 1;
    jumps[OP_lookupswitch] = 1;
    jumps[OP_returnvoid] = 1;
    jumps[OP_returnvalue] = 1;

    /* The operand count here is not important for correctness.  0 is
     * always a safe answer, and a too high number will result in an
     * out-of-bounds reference only occasionally :-P
     *
     * Getting the operand count right makes for better analysis,
     * though.
     */
    DEF(OP_bkpt, 0);
    DEF(OP_throw, 0);
    DEF(OP_getsuper, 1);
    DEF(OP_setsuper, 1);
    DEF(OP_dxns, 1);
    DEF(OP_dxnslate, 0);
    DEF(OP_kill, 1);
    DEF(OP_ifnlt, 1);
    DEF(OP_ifnle, 1);
    DEF(OP_ifngt, 1);
    DEF(OP_ifnge, 1);
    DEF(OP_jump, 1);
    DEF(OP_iftrue, 1);
    DEF(OP_iffalse, 1);
    DEF(OP_ifeq, 1);
    DEF(OP_ifne, 1);
    DEF(OP_iflt, 1);
    DEF(OP_ifle, 1);
    DEF(OP_ifgt, 1);
    DEF(OP_ifge, 1);
    DEF(OP_ifstricteq, 1);
    DEF(OP_ifstrictne, 1);
    DEF(OP_lookupswitch, 2);
    DEF(OP_pushwith, 0);
    DEF(OP_popscope, 0);
    DEF(OP_nextname, 0);
    DEF(OP_hasnext, 0);
    DEF(OP_pushnull, 0);
    DEF(OP_pushundefined, 0);
    DEF(OP_nextvalue, 0);
    DEF(OP_pushtrue, 0);
    DEF(OP_pushfalse, 0);
    DEF(OP_pushnan, 0);
    DEF(OP_pop, 0);
    DEF(OP_dup, 0);
    DEF(OP_swap, 0);
    DEF(OP_pushstring, 1);
    DEF(OP_pushdouble, 1);
    DEF(OP_pushscope, 0);
    DEF(OP_pushnamespace, 1);
    DEF(OP_hasnext2, 2);
    DEF(OP_newfunction, 1);
    DEF(OP_call, 1);
    DEF(OP_construct, 1);
    DEF(OP_callmethod, 2);
    DEF(OP_callstatic, 2);
    DEF(OP_callsuper, 2);
    DEF(OP_callproperty, 2);
    DEF(OP_returnvoid, 0);
    DEF(OP_returnvalue, 0);
    DEF(OP_constructsuper, 1);
    DEF(OP_constructprop, 1);
    DEF(OP_callproplex, 1);
    DEF(OP_callsupervoid, 2);
    DEF(OP_callpropvoid, 2);
    DEF(OP_applytype, 1);
    DEF(OP_newobject, 1);
    DEF(OP_newarray, 1);
    DEF(OP_newactivation, 0);
    DEF(OP_newclass, 1);
    DEF(OP_getdescendants, 1);
    DEF(OP_newcatch, 1);
    DEF(OP_findpropstrict, 1);
    DEF(OP_findproperty, 1);
    DEF(OP_finddef, 1);
    DEF(OP_getlex, 1);
    DEF(OP_setproperty, 1);
    DEF(OP_getlocal, 1);
    DEF(OP_setlocal, 1);
    DEF(OP_getglobalscope, 0);
    DEF(OP_getscopeobject, 1);
    DEF(OP_getproperty, 1);
    DEF(OP_getouterscope, 1);
    DEF(OP_initproperty, 1);
    DEF(OP_deleteproperty, 1);
    DEF(OP_getslot, 1);
    DEF(OP_setslot, 1);
    DEF(OP_getglobalslot, 1);
    DEF(OP_setglobalslot, 1);
    DEF(OP_convert_s, 0);
    DEF(OP_esc_xelem, 0);
    DEF(OP_esc_xattr, 0);
    DEF(OP_convert_i, 0);
    DEF(OP_convert_u, 0);
    DEF(OP_convert_d, 0);
    DEF(OP_convert_b, 0);
    DEF(OP_convert_o, 0);
    DEF(OP_checkfilter, 0);
    DEF(OP_coerce, 1);
    DEF(OP_coerce_b, 0);
    DEF(OP_coerce_a, 0);
    DEF(OP_coerce_i, 0);
    DEF(OP_coerce_d, 0);
    DEF(OP_coerce_s, 0);
    DEF(OP_astype, 1);
    DEF(OP_astypelate, 0);
    DEF(OP_coerce_u, 0);
    DEF(OP_coerce_o, 0);
    DEF(OP_negate, 0);
    DEF(OP_increment, 0);
    DEF(OP_inclocal, 1);
    DEF(OP_decrement, 0);
    DEF(OP_declocal, 1);
    DEF(OP_typeof, 0);
    DEF(OP_not, 0);
    DEF(OP_bitnot, 0);
    DEF(OP_add, 0);
    DEF(OP_subtract, 0);
    DEF(OP_multiply, 0);
    DEF(OP_divide, 0);
    DEF(OP_modulo, 0);
    DEF(OP_lshift, 0);
    DEF(OP_rshift, 0);
    DEF(OP_urshift, 0);
    DEF(OP_bitand, 0);
    DEF(OP_bitor, 0);
    DEF(OP_bitxor, 0);
    DEF(OP_equals, 0);
    DEF(OP_strictequals, 0);
    DEF(OP_lessthan, 0);
    DEF(OP_lessequals, 0);
    DEF(OP_greaterthan, 0);
    DEF(OP_greaterequals, 0);
    DEF(OP_instanceof, 0);
    DEF(OP_istype, 1);
    DEF(OP_istypelate, 0);
    DEF(OP_in, 0);
    DEF(OP_increment_i, 0);
    DEF(OP_decrement_i, 0);
    DEF(OP_inclocal_i, 1);
    DEF(OP_declocal_i, 1);
    DEF(OP_negate_i, 0);
    DEF(OP_add_i, 0);
    DEF(OP_subtract_i, 0);
    DEF(OP_multiply_i, 0);
    DEF(OP_getlocal0, 0);
    DEF(OP_getlocal1, 0);
    DEF(OP_getlocal2, 0);
    DEF(OP_getlocal3, 0);
    DEF(OP_setlocal0, 0);
    DEF(OP_setlocal1, 0);
    DEF(OP_setlocal2, 0);
    DEF(OP_setlocal3, 0);
    DEF(OP_debugline, 1);
    DEF(OP_debugfile, 1);
    DEF(OP_bkptline, 1);
    DEF(OP_ext_pushbits, 1);
    DEF(OP_ext_push_doublebits, 2);

    progname = argv[0];

    i=1;
    while (i < argc && argv[i][0] == '-') {
	if (strcmp(argv[i], "-c") == 0) {
	    if (i+1 == argc || sscanf(argv[i+1], "%d", &cutoff_count) != 1) {
		i=argc;
		break;
	    }
	    i += 2;
	}
	else if (strcmp(argv[i], "-l") == 0) {
	    if (i+1 == argc || sscanf(argv[i+1], "%d", &cutoff_length) != 1) {
		i=argc;
		break;
	    }
	    i += 2;
	}
	else if (strcmp(argv[i], "-f") == 0) {
	    flat = 1;
	    i++;
	}
	else if (strcmp(argv[i], "-h") == 0) {
	    hierarchical = 0;
	    i++;
	}
	else if (strcmp(argv[i], "-o") == 0) {
	    operand_tracking = 1;
	    i++;
	}
	else if (strcmp(argv[i], "-v") == 0) {
	    verbose = 1;
	    i++;
	}
	else {
	    i = argc;
	    break;
	}
    }
 
    if (i > argc-2 || (argc - i) % 2 != 0) {
	fprintf(stderr, "Usage: %s [options] ( code_file sample_file ) ... \n\n", progname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -c n     Set sample cutoff = n (default 100)\n");
	fprintf(stderr, "  -l n     Set length cutoff = n (default unbounded)\n");
	fprintf(stderr, "  -f       Enable flat report\n");
	fprintf(stderr, "  -h       Disable hierarchical report\n");
	fprintf(stderr, "  -o       Operand tracking\n");
	fprintf(stderr, "  -v       Verbose");
	return 1;
    }

    while (i < argc) {
	read_code(argv[i]);
	read_samples(argv[i+1]);
	i += 2;
	scale++;
    }
    extract_superwords();

    return 0;
}

/* First pass: read the code vectors */

void read_code(const char* filename)
{
    FILE *code_fp;
    unsigned total_wordcount, numcodes, signature;
    codestruct_t *codestruct;

    (code_fp = fopen(filename, "rb")) != NULL || fail("No such file: %s", filename);
    if (verbose)
	fprintf(stderr, "%s\n", filename);

    nextcodestruct = 0;    /* Don't bother deallocating data from previous code file */
    total_wordcount = 0;
    numcodes = 0;
    fread(&signature, sizeof(unsigned), 1, code_fp) == 1 || fail("No code signature");
    signature == 0xC0DEC0DE || fail("Bad code signature: %08x", signature);
    for (;;) {
	unsigned start, limit, wordcount;
	unsigned *codebuf;
	int res;

	if (fread(&start, sizeof(unsigned), 1, code_fp) != 1) 
	    break;
	fread(&limit, sizeof(unsigned), 1, code_fp) || fail("Could not read end");
	wordcount = (limit - start) / wordsize;
	total_wordcount += wordcount;
	codebuf = (unsigned*)safemalloc(wordcount * sizeof(unsigned));
	(res = fread(codebuf, sizeof(unsigned), wordcount, code_fp)) == wordcount || fail("Could not read code vector: %08x %08x %d (%d) %d", start, limit, wordcount, numcodes, res);
	nextcodestruct < sizeof(codestructs)/sizeof(codestruct_t) || fail("Out of codestruct memory: %d", nextcodestruct);
	codestruct = &codestructs[nextcodestruct++];
	codestruct->code = codebuf;
	codestruct->start = start;
	codestruct->limit = limit;
	numcodes++;
    }

    fclose(code_fp);

    qsort(codestructs, nextcodestruct, sizeof(codestruct_t), codestruct_cmp);

    if (verbose) {
	fprintf(stderr, "Number of code structs: %d\n", nextcodestruct);
	fprintf(stderr, "Words of code: %d\n", total_wordcount);
    }
}

/* Second pass: read the execution records.  Maintain a trie that is
 * indexed by the opcode on the top level and where each sub-level is
 * a linked list of instructions along with execution counts and their
 * subtrees.  Also maintain an array of current positions in the trie:
 * for each new instruction we get to, a new entry is added to this
 * array so that we track the sequence starting at that instruction
 * too.
 */
void read_samples(const char* filename)
{
    static unsigned bigbuf[1000000];
    FILE *sample_fp;
    unsigned total_samples, signature, prev_pc;
    codestruct_t *prev_codestruct;
    trie_t* tracker[1000];
    unsigned operand_value[1000][10];  /* Up to 10 distinct operand values per tracker.  We use offset 0..9 + 1; 0 means "no information" */
    unsigned next_symbol[1000];        /* Next symbol (= operand slot, if below 10) */
    int nexttracker;
    int trie_nodes = 0;
    int bigbuflim = 0;
    int bigbufptr = 0;
    int despecializations = 0;

    if (verbose)
	fprintf(stderr, "%s\n", filename);
    (sample_fp = fopen(filename, "rb")) != NULL || fail("No such file: %s", filename);

    fread(&signature, sizeof(unsigned), 1, sample_fp) == 1 || fail("No sample signature");
    signature == 0xDA1ADA1A || fail("Bad sample signature: %08x", signature);

    total_samples = 0;
    prev_codestruct = NULL;
    prev_pc = 0;
    nexttracker = 0;
    for (;;) {
	unsigned pc, opcode, ops, op, opcodes[10];
	codestruct_t *codestruct;

	if (bigbufptr == bigbuflim) {
	    int res;
	    res = fread(bigbuf, sizeof(unsigned), sizeof(bigbuf)/sizeof(bigbuf[0]), sample_fp);
	    if (res <= 0)
		break;
	    bigbufptr = 0;
	    bigbuflim = res;
	}
	pc = bigbuf[bigbufptr++];
	total_samples++;
	codestruct = (codestruct_t*)bsearch(&pc, codestructs, nextcodestruct, sizeof(codestruct_t), pc_cmp);
	codestruct != NULL || fail("Could not find pc=%08x anywhere in code memory.", pc);
	opcode = codestruct->code[(pc - codestruct->start)/wordsize];
	if (opcode > 255)
	    opcode = 256 + (opcode >> 8);

	/* Despecialize.  We can do it in the interpreter or here;
	 * doing it here is simpler for the moment, provided we don't
	 * have to adjust branch targets and so on.  Despecialization
	 * merely inserts opcodes into a buffer, across which we then
	 * iterate; in the degenerate case, the single opcode is not
	 * despecialized and is just inserted into the buffer alone.
	 *
	 * Right now there's no need to handle argument values.
	 *
	 * Open questions (likely answers are always "no"):
	 *  - Should we despecialize IFFALSE as NOT; IFTRUE?
	 *  - Should increment and decrement be despecialized as PUSHINT 1; ADD?
	 *  - Should inclocal and declocal be despecialized by breaking
	 *    it into GETLOCAL; INCREMENT; SETLOCAL?
	 */
	ops = 0;
	switch (opcode) {
	default: {
	    int n = operands[opcode];
	    opcodes[ops++] = opcode;
	    if (n > 0) { n--; opcodes[ops++] = codestruct->code[(pc - codestruct->start)/wordsize + 1]; }
	    if (n > 0) { n--; opcodes[ops++] = codestruct->code[(pc - codestruct->start)/wordsize + 2]; }
	    assert(n == 0);
	    break;
	}
	case OP_getlocal0:
	case OP_getlocal1:
	case OP_getlocal2:
	case OP_getlocal3:
	    opcodes[ops++] = OP_getlocal;
	    opcodes[ops++] = OP_getlocal - opcode;
	    break;
	case OP_setlocal0:
	case OP_setlocal1:
	case OP_setlocal2:
	case OP_setlocal3:
	    opcodes[ops++] = OP_setlocal;
	    opcodes[ops++] = OP_setlocal - opcode;
	    break;
	case OP_iflt:
	    opcodes[ops++] = OP_lessthan;
	    opcodes[ops++] = OP_iftrue;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifle:
	    opcodes[ops++] = OP_lessequals;
	    opcodes[ops++] = OP_iftrue;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifnlt:
	    opcodes[ops++] = OP_lessthan;
	    opcodes[ops++] = OP_iffalse;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifnle:
	    opcodes[ops++] = OP_lessequals;
	    opcodes[ops++] = OP_iffalse;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifgt:
	    opcodes[ops++] = OP_greaterthan;
	    opcodes[ops++] = OP_iftrue;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifge:
	    opcodes[ops++] = OP_greaterequals;
	    opcodes[ops++] = OP_iftrue;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifngt:
	    opcodes[ops++] = OP_greaterthan;
	    opcodes[ops++] = OP_iffalse;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifnge:
	    opcodes[ops++] = OP_greaterequals;
	    opcodes[ops++] = OP_iffalse;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifeq:
	    opcodes[ops++] = OP_equals;
	    opcodes[ops++] = OP_iftrue;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifstricteq:
	    opcodes[ops++] = OP_strictequals;
	    opcodes[ops++] = OP_iftrue;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifne:
	    opcodes[ops++] = OP_equals;
	    opcodes[ops++] = OP_iffalse;
	    opcodes[ops++] = 0;
	    break;
	case OP_ifstrictne:
	    opcodes[ops++] = OP_strictequals;
	    opcodes[ops++] = OP_iffalse;
	    opcodes[ops++] = 0;
	    break;
	}

	if (opcodes[0] != opcode || ops > 1)
	    despecializations++;

	if (prev_codestruct != codestruct)
	    nexttracker = 0;

	op = 0;
	while ( op < ops ) {
	    int i;
	    opcode = opcodes[op];
	    assert( opcode < INSTRCOUNT );
	    for ( i=0 ; i < nexttracker ; i++ ) {
		trie_t *t = tracker[i];
		unsigned opd1=0, opd2=0;
		if (operand_tracking)
		    compute_operands(opcodes, op, &opd1, &opd2, i, operand_value, next_symbol);
		if (t->left_child == NULL) {
		    t->left_child = new_trie_node(opcode, opd1, opd2);
		    ++trie_nodes;
		    tracker[i] = t->left_child;
		}
		else {
		    trie_t *t2 = t->left_child;
		    trie_t *t3 = NULL;
		    if (operand_tracking) {
			while (t2 != NULL && (t2->opcode != opcode || t2->opd1 != opd1 || t2->opd2 != opd2))
			    t3 = t2, t2 = t2->right_sibling;
		    }
		    else {
			while (t2 != NULL && t2->opcode != opcode)
			    t3 = t2, t2 = t2->right_sibling;
		    }
		    if (t2 != NULL) {
			/* Move the node to the head of the list in order to keep the
			   hottest nodes first */
			t2->count++;
			if (t3 != NULL) {  /* Otherwise it's already at the head of the list */
			    t3->right_sibling = t2->right_sibling;
			    t2->right_sibling = t->left_child;
			    t->left_child = t2;
			}
		    }
		    else {
			t2 = new_trie_node(opcode, opd1, opd2);
			++trie_nodes;
			t2->right_sibling = t->left_child;
			t->left_child = t2;
		    }
		    tracker[i] = t2;
		}
	    }
	    nexttracker < sizeof(tracker)/sizeof(tracker[0]) || fail("Out of tracker memory, probably a bug");
	    tracker[nexttracker++] = &toplevel[opcode];
	    toplevel[opcode].count++;
	    if (operand_tracking) {
		unsigned opd1, opd2;
		next_symbol[nexttracker-1] = 0;
		compute_operands(opcodes, op, &opd1, &opd2, nexttracker-1, operand_value, next_symbol);
		/* Wrong.  This breaks down for two-operand instructions at the top level,
		 * as there may be two variants: op[1,1] and op[1,2].  In almost all cases
		 * it will be latter, and it is very unlikely that this bug will cause 
		 * actual problems.  So ignore it, rather than reengineering it.
		 */
		toplevel[opcode].opd1 = opd1;
		toplevel[opcode].opd2 = opd2;
	    }

	    op += operands[opcode] + 1;

	    if (jumps[opcode]) {
		assert(op == ops);
		nexttracker = 0;
	    }
	}
	prev_codestruct = codestruct;
	prev_pc = pc;
    }

    fclose(sample_fp);

    if (verbose) {
	fprintf(stderr, "Samples processed: %d\n", total_samples);
	fprintf(stderr, "Despecializations: %d\n", despecializations);
	fprintf(stderr, "Trie nodes: %d\n", trie_nodes);
    }
}

/* In the node, the operand values are represented symbolically.  The
 * real values pertaining to this particular block are in the tracker
 * data.  So lookup the values there, grab symbols for matches,
 * generate new symbols for non-matches.  If symbols were generated
 * then we could still have a match, so must search.  Generated
 * symbols should be made available for the insertion code, below.
 */
unsigned compute_operands(unsigned code[], unsigned codeidx, unsigned* opd1, unsigned* opd2, int trackeridx, unsigned operand_value[][10], unsigned next_symbol[])
{
    int n;
    unsigned *p[2];
    p[0] = opd1;
    p[1] = opd2;
    unsigned **q = p;
    *opd1 = *opd2 = 0;

    for ( n=operands[code[codeidx++]] ; n > 0 ; n-- ) {
	unsigned x = code[codeidx++];
	unsigned symbol = 0;
	unsigned i;
	unsigned limit=MIN(next_symbol[trackeridx],10);
	for ( i=0 ; i < limit ; i++ ) {
	    if (operand_value[trackeridx][i] == x) {
		symbol = i+1;
		break;
	    }
	}
	if (symbol == 0) {
	    unsigned value_index = next_symbol[trackeridx]++;
	    if (value_index < 10)
		operand_value[trackeridx][value_index] = x;
	    symbol = value_index + 1;
	}
	assert(symbol != 0);
	**q++ = symbol;
    }

    return codeidx;
}

/* Third pass: extract all sequences and their execution counts.  We
 * walk the toplevel and then recur throughout each tree; if the count
 * at one level is less than the count at the previous level then one
 * sequence ended at the previous level but continues with the lower
 * count at this level.
 *
 * Discard sequences of length 1 (duh) and anything below the
 * user-settable cutoff.
 *
 * Every sequence that is a proper suffix of another sequence with the
 * same execution count is discarded too (subsumed by the longer
 * sequence).
 */
void extract_superwords()
{
    int i, j, ki, kj;

    for ( i=0 ; i < INSTRCOUNT ; i++ ) 
	descend(&toplevel[i], 0);

    qsort(seqs, nextseq, sizeof(seq_t), seq_cmp);

    for ( i=0 ; i < nextseq ; i++ ) {
	for ( j=i+1 ; j < nextseq && seqs[i].count == seqs[j].count ; j++ ) {
	    if (seqs[j].length < seqs[i].length) {
		for ( ki=seqs[i].length-1, kj=seqs[j].length-1 ; kj >= 0 && seqs[i].opcode[ki] == seqs[j].opcode[kj] ; ki--, kj-- )
		    ;
		if (kj < 0)
		    seqs[j].suffix = 1;
	    }
	}
    }

    /* Remove suffix sequences */
    j=0;
    for ( i=0 ; i < nextseq ; i++ )
	if (!seqs[i].suffix)
	    seqs[j++] = seqs[i];
    nextseq = j;

    /* Flat */

    if (flat)
	for ( i=0 ; i < nextseq ; i++ )
	    printseq(i, 0);
	

    /* Hierarchical.  Every sequence that is a proper prefix of
       another sequence will necessarily have a higher execution
       count, and will be printed with the longer sequence as a
       child. 

       Note this messes with the suffix flag.
    */

    if (hierarchical) {
	if (flat)
	    printf("\n\n----------\n\n");
	for ( i=0 ; i < nextseq ; i++ ) {
	    if (!seqs[i].suffix) {
		printseq(i, 0);
		printhseq(i, 1);
	    }
	}
    }
}

void printhseq(int i, int level) 
{
    int j, k, l;

    l=seqs[i].length;
    for ( j=i+1 ; j < nextseq ; j++ ) {
	if (seqs[j].suffix || seqs[j].length < l) 
	    goto next_j;
	for ( k=0 ; k < l ; k++ )
	    if (seqs[i].opcode[k] != seqs[j].opcode[k])
		goto next_j;
	printseq(j, level);
	seqs[j].suffix = 1;
	printhseq(j, level+1);
    next_j: 
	;
    }
}

void printseq(int i, int indent)
{
    int j;

    printf("%11d | ", seqs[i].count);
    for ( j=0 ; j < indent ; j++ )
	printf("  ");
    if (operand_tracking) {
	for ( j=0 ; j < seqs[i].length ; j++ ) {
	    printf("%s", iname[seqs[i].opcode[j]]);
	    if (seqs[i].operand1[j] != 0) {
		printf("[%d", seqs[i].operand1[j]);
		if (seqs[i].operand2[j] != 0)
		    printf(",%d", seqs[i].operand2[j]);
		printf("] ");
	    }
	    else
		printf(" ");
	}
    }
    else {
	for ( j=0 ; j < seqs[i].length ; j++ )
	    printf("%s ", iname[seqs[i].opcode[j]]);
    }
    printf("| ");
    for ( j=0 ; j < seqs[i].length ; j++ )
	printf("%x ", seqs[i].opcode[j]);
    printf("\n");
}

/* t is not NULL, and may or may not have children.  ibuf holds
 * instructions for parent nodes through level-1.  For each of t's
 * children, determine whether to emit an instruction sequence here
 * (if the count of child is lower than the count of t) or to continue
 * descending.  Only emit once.
 */
void descend(trie_t *t, int level)
{
    trie_t *t2, *t3;

    for ( t2=t ; t2 != NULL ; t2 = t2->right_sibling) {
	int emitted = 0;

	ibuf[level] = t2->opcode;
	operand1[level] = t2->opd1;
	operand2[level] = t2->opd2;

	if (t2->left_child == NULL) {
	    emit(level+1, t2->count);
	    continue;
	}

	for ( t3=t2->left_child ; t3 != NULL ; t3 = t3->right_sibling ) {
	    if (t3->count < t2->count && !emitted) {
		emitted = 1;
		emit(level+1, t2->count);
	    }
	}

	descend(t2->left_child, level+1);
    }
}

void emit(int level, unsigned count)
{
    if (level > 1 && level <= cutoff_length && count/scale >= cutoff_count) {
	nextseq < sizeof(seqs)/sizeof(seq_t) || fail("Out of seq memory.");
	seq_t *seq = &seqs[nextseq++];
	seq->opcode = (unsigned*) safemalloc(sizeof(unsigned) * level);
	memcpy(seq->opcode, ibuf, level*sizeof(unsigned));
	if (operand_tracking) {
	    seq->operand1 = (unsigned*) safemalloc(sizeof(unsigned) * level);
	    memcpy(seq->operand1, operand1, level*sizeof(unsigned));
	    seq->operand2 = (unsigned*) safemalloc(sizeof(unsigned) * level);
	    memcpy(seq->operand2, operand2, level*sizeof(unsigned));
	}
	else {
	    seq->operand1 = NULL;
	    seq->operand2 = NULL;
	}
	seq->count = count/scale;
	seq->length = level;
	seq->suffix = 0;
    }
}

trie_t* new_trie_node(unsigned opcode, unsigned opd1, unsigned opd2)
{
    trie_t* t2 = (trie_t*)malloc(sizeof(trie_t));
    t2 != NULL || fail("allocation failure: trie_t");
    t2->opcode = opcode;
    t2->opd1 = opd1;
    t2->opd2 = opd2;
    t2->count = 1;
    t2->left_child = NULL;
    t2->right_sibling = NULL;
    return t2;
}

int seq_cmp(const void *a, const void *b)
{
    // descending by count, longest first
    int res = ((seq_t*)b)->count - ((seq_t*)a)->count;
    if (res != 0)
	return res;
    return ((seq_t*)b)->length - ((seq_t*)a)->length;
}

int codestruct_cmp(const void *a, const void *b)
{
    if (a == b)
	return 0;
    if (((codestruct_t*)a)->start < ((codestruct_t*)b)->start)
	return -1;
    return 1;
}

int pc_cmp(const void *a, const void *b) {
    unsigned pc = *(unsigned*)a;
    codestruct_t* elt = (codestruct_t*)b;
    if (pc >= elt->start && pc < elt->limit)
	return 0;
    if (pc < elt->start)
	return -1;
    return 1;
}

void *safemalloc(size_t nbytes)
{
    void *m = malloc(nbytes);
    m != NULL || fail("Allocation failure: %d", nbytes);
    return m;
}

int fail(const char *s, ...)
{
    va_list args;
    
    fprintf(stderr, "%s: ", progname);
    va_start(args, s);
    vfprintf(stderr, s, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
    return 0;
}
