/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Namespaces used by the compiler, in the form of a FIXTURES datum.

ESC var bootstrap_namespaces =
    ["Abc",
     "AbcEncode",
     "AbcDecode",
     "Asm",
     "Ast",
     "Char",
     "Debug",
     "Emit",
     "ESC", 
     "Gen",
     "Lex",
     "Parse", 
     "Release", 
     "Token", 
     "Util"].map(function (id) {
                     return new Ast::Fixture(new Ast::PropName(new Ast::Name(Ast::publicNS, Token::intern(id))),
                                             new Ast::NamespaceFixture(new Ast::UnforgeableNamespace(Token::intern(id)))); });
