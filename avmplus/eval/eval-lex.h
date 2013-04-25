/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

enum Token {
    // Operators
    //
    // The values assigned for operators are fixed; they are used
    // to construct the table Compiler::opcodeMapping in eval-parse.cpp.
    // If you add entries to the operators list you *must* extend that
    // table.
    //
    // Keep them alphabetical.
    
    T_As = 0,
    T_Assign,
    T_BitwiseAnd,
    T_BitwiseAndAssign,
    T_BitwiseNot,
    T_BitwiseOr,
    T_BitwiseOrAssign,
    T_BitwiseXor,
    T_BitwiseXorAssign,
    T_Delete,
    T_Divide,
    T_DivideAssign,
    T_Equal,
    T_GreaterThan,
    T_GreaterThanOrEqual,
    T_In,
    T_InstanceOf,
    T_Is,
    T_LeftShift,
    T_LeftShiftAssign,
    T_LessThan,
    T_LessThanOrEqual,
    T_LogicalAnd,
    T_LogicalAndAssign,
    T_LogicalOr,
    T_LogicalOrAssign,
    T_Minus,
    T_MinusAssign,
    T_MinusMinus,
    T_Multiply,
    T_MultiplyAssign,
    T_Not,
    T_NotEqual,
    T_Plus,
    T_PlusAssign,
    T_PlusPlus,
    T_Remainder,
    T_RemainderAssign,
    T_RightShift,
    T_RightShiftAssign,
    T_StrictEqual,
    T_StrictNotEqual,
    T_TypeOf,
    T_UnsignedRightShift,
    T_UnsignedRightShiftAssign,
    T_Void,
    
    T_OPERATOR_SENTINEL,
    
    // Sundry punctuation
    
    T_LeftParen = 100,
    T_RightParen,
    T_Comma,
    T_Dot,
    T_DoubleDot,
    T_TripleDot,
    T_LeftDotAngle,
    T_Colon,
    T_DoubleColon,
    T_Semicolon,
    T_Question,
    T_LeftBracket,
    T_RightBracket,
    T_LeftBrace,
    T_RightBrace,
    T_AtSign,
    T_XmlLeftBrace,
    T_XmlRightBrace,
    T_XmlEquals,
    T_XmlLeftAngle,
    T_XmlRightAngle,
    T_XmlLeftAngleSlash,
    T_XmlSlashRightAngle,
    
    // Keywords.  Commented-out entries are operators, above.
    
    /*T_As,*/
    T_Break = 200,
    T_Case,
    T_Catch,
    T_Class,
    T_Const,
    T_Continue,
    T_Default,
    /*T_Delete,*/
    T_Do,
    T_Dynamic,
    T_Else,
    T_False,
    T_Final,
    T_Finally,
    T_For,
    T_Function,
    T_Goto,
    T_If,
    T_Import,
    /*T_In,*/
    T_Include,
    /*T_InstanceOf,*/
    T_Interface,
    T_Internal,
    /*T_Is,*/
    T_Namespace,
    T_Native,
    T_New,
    T_Null,
    T_Override,
    T_Package,
    T_Private,
    T_Protected,
    T_Public,
    T_Return,
    T_Static,
    T_Super,
    T_Switch,
    T_This,
    T_Throw,
    T_True,
    T_Try,
    /*T_TypeOf,*/
    T_Use,
    T_Var,
    /*T_Void,*/
    T_While,
    T_With,
    
    // Sundry other tokens.
    
    T_Identifier = 300,
    T_IntLiteral,
    T_UIntLiteral,
    T_DoubleLiteral,
    T_FloatLiteral,
    T_RegexpLiteral,
    T_StringLiteral,
    T_XmlCDATA,                 //  "<![CDATA[...]]>"  (including the punctuation, ditto for the three following tokens)
    T_XmlComment,               //  "<!-- ... -->"
    T_XmlProcessingInstruction, //  "<? ... ?>
    T_XmlString,                //  '...' or "..."
    T_XmlName,                  //  string of XMLName characters
    T_XmlWhitespace,            //  string of XMLWhitespace characters
    T_XmlText,                  //  string of characters that are not XMLName or XMLWhitespace

    // Meta-tokens.
    
    T_EOS = 400,
    T_BreakSlash,               // "/" seen and consumed
    T_BreakRightAngle,          // ">" seen and consumed
    T_BreakLeftAngle,           // "<" seen and consumed

    // LAST also serves double duty as NONE
    
    T_LAST = 500
};

// Value carrier for tokens that carry values.

union TokenValue {
    double    d;                // T_DoubleLiteral
    float     f;                // T_FloatLiteral
    int32_t   i;                // T_IntLiteral
    uint32_t  u;                // T_UintLiteral
    Str      *s;                // T_StringLiteral, T_RegexpLiteral, T_Identifier
};
        

/**
 * Lexical analysis.
 *
 * A client retrieves a stream of tokens from the lexer by calling
 * lex() repeatedly.  When the special tokens T_BreakSlash and
 * T_BreakRightAngle are returned the client must disambiguate
 * the context by calling divideOperator() or regexp() in the forme
 * case and rightAngle() or shiftOrRelationalOperator() in the latter.
 *
 * A few tokens carry values.  These values are available through
 * accessor functions on the lexer when the most recent call to
 * the lexer returned the particular token in question.  In debug
 * builds there are checks to catch incorrect uses of these APIs.
 *
 * A line number is maintained by the lexer and made available
 * through an accessor function.  Following the return of a token,
 * the line number corresponds to the line number of the last
 * consumed character of the most recently consumed token.  The only
 * multi-line tokens are strings, regular expression literals, and
 * identifiers containing \<newline> sequences.
 */

class Lexer {
public:
    /**
     * @param compiler  The compiler structure, from which we take flags and allocator
     * @param src  The source text as a string with a trailing NUL; it may contain
     *             embedded NULs but the last is considered a terminator, not part
     *             of the input
     * @param keyword_or_ident  True iff this scanner is simply being used to check
     *             whether an identifier that contains a backslash sequence looks
     *             like a keyword.
     */
    Lexer(Compiler* compiler, const wchar* src, uint32_t srclen, bool keyword_or_ident=false);

    Token lex(uint32_t* linep, TokenValue* valuep);         // Lex a token
    Token regexp(uint32_t* linep, TokenValue* valuep);      // Following T_BreakSlash, to lex a regex literal
    Token divideOperator(uint32_t* linep);                  // Following T_BreakSlash, to lex a division operator
    Token rightAngle(uint32_t* linep);                      // Following T_BreakRightAngle, to lex '>' at the end of a type instantiator
    Token leftAngle(uint32_t* linep);                       // Following T_BreakLeftAngle, to lex '<' in a vector initializer
    Token rightShiftOrRelationalOperator(uint32_t* linep);  // Following T_BreakRightAngle, to lex a shift or relational operator
    Token leftShiftOrRelationalOperator(uint32_t* linep);   // Following T_BreakLeftAngle, to lex a shift or relational operator
    
    /**
     *  Last consumed character must have been c; back up once
     */
    void xmlPushback(wchar c);
    
    /**
     * Lex one XML atom.
     * xmlAtom returns one of:
     *
     *   XmlComment
     *   XmlCDATA
     *   XmlProcessingInstruction
     *   XmlName
     *   XmlWhitespace
     *   XmlText
     *   XmlString
     *   XmlLeftBrace
     *   XmlRightBrace
     *   XmlEquals
     *   XmlLeftAngle
     *   XmlRightAngle
     *   XmlLeftAngleSlash
     *   XmlSlashRightAngle
     *
     * For XmlComment, XmlCDATA, XmlProcessingInstruction, XmlName, XmlWhitespace, XmlText,
     * and XmlString, valuep->s is set to the actual text.
     */
    Token xmlAtom(uint32_t* linep, TokenValue* valuep);

#ifdef DEBUG
    void trace();                                       // enable tracing
    bool getTrace() const;                              // retrieve the current tracing flag
#endif
    
private:
    enum {
        // Special spaces
        UNICHAR_LS = 0x2028,
        UNICHAR_PS = 0x2029,
        
        // Various Zs characters
        UNICHAR_Zs1 = 0x1680,
        UNICHAR_Zs2 = 0x180E,
        UNICHAR_Zs3 = 0x2000,
        UNICHAR_Zs4 = 0x2001,
        UNICHAR_Zs5 = 0x2002,
        UNICHAR_Zs6 = 0x2003,
        UNICHAR_Zs7 = 0x2004,
        UNICHAR_Zs8 = 0x2005,
        UNICHAR_Zs9 = 0x2006,
        UNICHAR_Zs10 = 0x2007,
        UNICHAR_Zs11 = 0x2008,
        UNICHAR_Zs12 = 0x2009,
        UNICHAR_Zs13 = 0x200A,
        UNICHAR_Zs14 = 0x202F,
        UNICHAR_Zs15 = 0x205F,
        UNICHAR_Zs16 = 0x3000,
        
        // Byte-order mark - we treat it like a space
        UNICHAR_BOM = 0xFEFF,
    };
    
    enum {
        // The character among the LS/PS, BOM, and Zs* with the lowest value
        UNICHAR_LOWEST_ODDSPACE = 0x1680
    };
    
    // 8 bits available in the char_attrs table
    enum {
        CHAR_ATTR_OCTAL = 1,
        CHAR_ATTR_DECIMAL = 2,
        CHAR_ATTR_HEX = 4,
        CHAR_ATTR_LETTER = 8,
        CHAR_ATTR_UNDERBAR = 16,
        CHAR_ATTR_DOLLAR = 32,
        
        CHAR_ATTR_INITIAL = CHAR_ATTR_LETTER | CHAR_ATTR_UNDERBAR | CHAR_ATTR_DOLLAR,
        CHAR_ATTR_SUBSEQUENT = CHAR_ATTR_INITIAL | CHAR_ATTR_DECIMAL
    };
    
    Token lexImpl();
    Token regexpImpl();
    Token divideOperatorImpl();
    Token rightAngleImpl();
    Token leftAngleImpl();
    Token rightShiftOrRelationalOperatorImpl();
    Token leftShiftOrRelationalOperatorImpl();
    
    Token xmlAtomImpl();
    Token xmlMarkup(Token t);
    Token xmlWhitespace();
    Token xmlName();
    Token xmlString();
    Token xmlText();
    bool isXmlNameStart(wchar c);
    bool isXmlNameSubsequent(wchar c);
    
    void lineComment();
    void blockComment();
    
    Token identifier();
    
    Token stringLiteral(int delimiter);
    
    int escapeSequence();
    int octalOrNulEscape();
    int octalEscape(int n);
    int hexEscape(int n);
    int unicodeEscape();
    
    Token numberLiteral();
    Token integerLiteral(int base);
    Token floatingLiteral();
    bool checkNextCharForNumber(bool allowFloat);
    bool numberLiteralPrime();
    void numberFraction(bool has_leading_digits);
    void numberExponent();
    bool octalDigits(int k);
    bool decimalDigits(int k);
    bool hexDigits(int k);
    bool digits(int k, int mask);
    double parseDouble();
    float parseFloat();
    double parseInt(int base);
    
    bool notPartOfIdent(int c);
    bool isUnicodeIdentifierStart(int c);
    bool isUnicodeIdentifierPart(int c);
#ifdef DEBUG
    void print(Token t, uint32_t l, TokenValue v);
#endif
    
    Compiler * const    compiler;
    const wchar*        src;        // input
    const wchar*        limit;      // one past end of input
    const wchar*        idx;        // next char in input
    const wchar*        mark;       // a remembered position, typically the start of a lexeme (not always valid)
    uint32_t            lineno;     // line number of last char of last token returned
    const bool          keyword_or_ident;
                                    // true if this lexer instance is just used for checking whether an
                                    // identifier that contains a backslash sequence looks like a keyword
#ifdef DEBUG
    Token               last_token; // last token returned
    bool                traceflag;  // true iff we're tracing
#endif
    TokenValue          val;        // temporary slot
    
    // Character attributes for the ASCII range, bit vectors of the CHAR_ATTR_ values above.
    static const uint8_t char_attrs[128];
};
}}
