/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

/**
 * The compiler acts as a coordinating class, managing memory, interning strings,
 * knowing about settings, etc.  Pretty much everyone points to a compiler instance,
 * which in turn provides the host context and allocator instances.
 */
class Compiler {
public:
    Compiler(HostContext* context, const wchar* filename, const wchar* src, uint32_t srclen, bool public_by_default);
    ~Compiler() { destroy(); }

    void compile();
    void destroy();                                     // clean up all resources

    Str* intern(const wchar* w, uint32_t nchars);       // intern a string
    Str* intern(const char* w);                         // intern a NUL-terminated string
    Str* intern(uint32_t u);                            // convert to base-10 string, then intern it

    // A lineno of 0 is ignored here
    void syntaxError(uint32_t lineno, SyntaxError fmt, ...);
    void internalError(uint32_t lineno, const char* fmt, ...);
    void internalWarning(uint32_t lineno, const char* fmt, ...);

    HostContext * const context;    // For access to the host environment
    Allocator* const allocator;     // Compiler-private bump-a-pointer never-free heap
    const wchar * const filename;   // Some human-readable name of the originator of the script, NUL-terminated
    const uint32_t tableSize;       // Number of elements in strTable; the abc layer uses this for the multiname table size too
    
    // Flags, currently somewhat ad-hoc, that are used by the compiler to control the
    // language accepted.

    const bool es3_keywords;    // True iff we should only recognize ECMAScript 3.0 keywords
    const bool standard_regex;  // True iff we should only recognize standard regex syntax, not \<newline> nor the "x" flag
    const bool liberal_idents;  // True iff we allow the use of \u to create identifers that look like keywords, or non-E262-sanctioned characters in identifiers
    const bool local_functions; // True iff we allow block-local function definitions (hoist name, init when reached)
    const bool octal_literals;  // True iff we should recognize 0[0-7]+ as octal (not in AS3)
    const bool origin_is_file;  // True iff input came from file, so "include" should be allowed
    const bool debugging;       // True iff we should generate debug information
    
    uint32_t namespace_counter; // Counter for anonymous namespaces

private:
    Str** const     strTable;   // fixed-length hash table using chaining on collision, chained through 'next' (in private heap)

    void internalWarningOrError(bool error, uint32_t lineno, const char* fmt, va_list args);

public:
    // Compiler components
    Lexer          lexer;
    Parser         parser;
    ABCFile        abc;         // must be initialized before the ID_ members
    
    // Intern all strings after strTable has been initialized

    // Pre-interned strings signifying themselves, keep alphabetical
    Str * const SYM_;           // ""
    Str * const SYM_AS3;
    Str * const SYM_AS3_vec;    // __AS3__.vec
    Str * const SYM_Array;
    Str * const SYM_CONFIG;
    Str * const SYM_Namespace;
    Str * const SYM_Number;
    Str * const SYM_Object;
    Str * const SYM_RegExp;
    Str * const SYM_Vector;
    Str * const SYM_XML;
    Str * const SYM_XMLList;
    Str * const SYM_anonymous;
    Str * const SYM_arguments;
    Str * const SYM_children;
    Str * const SYM_config;
    Str * const SYM_each;
    Str * const SYM_extends;
    Str * const SYM_get;
    Str * const SYM_implements;
    Str * const SYM_int;
    Str * const SYM_length;
    Str * const SYM_namespace;
#ifdef DEBUG
    Str * const SYM_print;
#endif
    Str * const SYM_set;
    Str * const SYM_use;
    Str * const SYM_xml;

    // Pre-interned strings for other uses
    Str * const str_filename;   // Str representation of the 'filename' member above

    // Namespaces and names that are commonly used
    const uint32_t NS_public;       // "public public"
    const uint32_t NS_private;      // private
    const uint32_t NS_internal;     // "file internal" == private
    const uint32_t NS_AS3_vec;      // __AS3__.vec
    const uint32_t ID_Array;        // public::Array
    const uint32_t ID_Namespace;    // public::Namespace
    const uint32_t ID_Number;       // public::Number
    const uint32_t ID_Object;       // public::Object
    const uint32_t ID_RegExp;       // public::RegExp
    const uint32_t ID_Vector;       // __AS3__.vec::Vector
    const uint32_t ID_XML;          // public::XML
    const uint32_t ID_XMLList;      // public::XMLList
    const uint32_t ID_children;     // public::children
    const uint32_t ID_int;          // public::int
    const uint32_t ID_length;       // public::length
#ifdef DEBUG
    const uint32_t ID_print;        // public::print
#endif
    const uint32_t NSS_public;      // [public,internal]
    const uint32_t MNL_public;      // [public]::<>
    const uint32_t MNL_public_attr; // [public]::<> for @attr names
};
}}
