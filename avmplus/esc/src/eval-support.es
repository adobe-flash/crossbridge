/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* ECMAScript support for ESC */

ESC var eval_counter = 0;
ESC var eval_hook;         // will be assigned the newly compiled function

/* The wrapper around ESC that compiles and runs code to be eval'd.
 *
 * "scopes" must contain all scopes except the global one, which
 * is implicit.
 */
ESC function evaluateInScopeArray(args, scopes, scopedesc, strict) {
    if (args.length > 0) {
        if (args[0] is String) {
            let id = ESC::eval_counter++;
            let file = "(EVAL CODE)";
            let name = Token::intern("$eval$" + id + "$");
            let parser = new Parse::Parser( args[0], ESC::getTopFixtures(), file );
            let prog = parser.program(false);

            // These checks could be moved into the parser, and might be, by and by.

            for ( let fs=prog.head.fixtures, limit=fs.length, i=0 ; i < limit ; i++ ) {
                let f = fs[i];
                if (strict || !(f.data is Ast::MethodFixture || f.data is Ast::ValFixture))
                    Util::syntaxError(file, 0, "Binding form not legal: " + f.data);
            }

            //Asm::listify = true;
            Util::loadBytes(Gen::cgEval(prog, name, scopedesc).getBytes());
            //Asm::listify = false;

            return ESC::eval_hook(scopes);
        }
        else
            return args[0];
    }
    else
        return undefined;
}

// This is the true eval function.  Install this as "eval" in the global object.

// ESC function eval(...rest)
//     ESC::evalCompiler(rest, [], "");
