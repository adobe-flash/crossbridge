/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Parse,
    namespace Parse;

/* Key issue to be tackled: how do we represent compile-time environments,
 * and do we share them between the front end and the back end?  And what
 * do we use them for?
 *
 *   - let flattening: if a binding is not captured (by a function or by
 *     eval or by with) then it can be hoisted and renamed and all uses
 *     can use the renamed name.  This is aided by mapping a name to an
 *     identifier structure that records the new name, probably.  So
 *     Ast::IdentExpr structured would be shared (note that "public::x"
 *     means the same as "x" if x is local, and the former must be renamed
 *     if the latter is).
 *
 *     Actually renamed 'let' bindings just turn into localprops, no?
 *     (The new name is a register number.)  Except at global level.
 *     But at global level we want no renaming ever.
 *
 *   - Early binding.  Right now the back end maintains its own hash table
 *     of name -> register mappings (where appropriate); we should share
 *     the environment created here.
 *
 *   - The back end's context structure will mirror the one we create
 *     here, tracking exception handlers and scopes.  So we should share
 *     that too, to avoid creating it twice.  (Maybe.)
 *
 *   - The problem is in part that the Head is conveniently open, it
 *     has a list of fixtures and a list of exprs.  The list of fixtures
 *     must presumably become a hash table.  But is there an ordering
 *     requirement among the fixtures?  (We could use a parallel hash
 *     table for fast lookup.)
 */

internal final class Rib 
{
    static const SYSTEM = -1;
    static const GLOBAL = 0;
    static const CLASS_STATIC = 1;
    static const CLASS_INSTANCE = 2;
    static const FUNCTION = 3;
    static const BLOCK = 4;

    var defaultNamespace: (Ast::Expr | Ast::Namespace);
    var strict:           Boolean;
    var open_namespaces:  Ast::NamespaceSetList;
    const head:           Ast::Head;
    const link:           Rib;
    const tag;

    // Now we can use the trick where if the top namespace has no
    // entries then we reuse it, since it is a completely functional
    // data structure.

    function Rib(tag, link) 
        : tag = tag
        , link = link
        , defaultNamespace = Ast::publicNS
        , head = new Ast::Head([], [])
        , strict = link ? link.strict : false
    {
        if (link == null)
            open_namespaces = new Ast::NamespaceSetList(null, null);
        else if (link.open_namespaces.nsset == null)
            open_namespaces = link.open_namespaces;
        else
            open_namespaces = link.open_namespaces.pushScope();
    }

    function openNamespace(ns) {
        let nss;
        for ( nss = open_namespaces.nsset ; nss != null && !Ast::nsEquals(nss.ns, ns) ; nss = nss.link )
            ;
        if (nss == null)
            open_namespaces = open_namespaces.pushNamespace(ns);
    }

    static function makeSystemRib(topFixtures) {
        let r = new Rib(Rib.SYSTEM, null);
        Util::pushOnto(r.head.fixtures, topFixtures);
        return r;
    }
}

final class Definer
{
    const parser;

    private var env: Rib;
    private var top_function: Ast::FuncAttr;

    function Definer (parser, topFixtures)
        : parser = parser
        , env = Rib.makeSystemRib(topFixtures)
        , top_function = new Ast::FuncAttr(null)
    {
    }

    function enterGlobal(): void
        enterRib(Rib.GLOBAL);

    function exitGlobal() 
        exitRib();

    function enterClassStatic(): void 
        enterRib(Rib.CLASS_STATIC);

    function exitClassStatic() 
        exitRib();

    function enterClassInstance(): void 
        enterRib(Rib.CLASS_INSTANCE);

    function exitClassInstance() 
        exitRib();

    function enterFunction(attrs): void {
        let node = new Ast::FuncAttr(top_function);
        node.is_native = attrs.native;
        top_function.children.push(node);
        top_function = node;
        enterRib(Rib.FUNCTION);
    }

    function exitFunction() {
        let vars = exitRib();
        let attr = top_function;
        top_function = top_function.parent;

        return [vars, attr];
    }

    function topFunction() {
        return top_function;
    }

    function enterBlock(): void 
        enterRib(Rib.BLOCK);

    function exitBlock() 
        exitRib();

    private function enterRib(tag): void
        env = new Rib(tag, env);

    private function exitRib(): Ast::Head {
        let head = env.head;
        env = env.link;
        return head;
    }

    function addFixtures(initType, fixtures, isStatic=false): void
        Util::pushOnto(findHead(initType, isStatic).fixtures, fixtures);

    function addInits(initType, inits, isStatic=false): void
        Util::pushOnto(findHead(initType, isStatic).exprs, inits);

    function addFixture(initType, name, fixture, isStatic=false): void
        findHead(initType, isStatic).fixtures.push(new Ast::Fixture(name,fixture));

    private function findHead(initType, isStatic) {
        if (initType == Ast::letInit)
            return env.head;
        for ( let e=env ; e != null ; e = e.link )
            if (e.tag != Rib.BLOCK && (!isStatic || e.tag == Rib.CLASS_STATIC))
                return e.head;
        Parse::internalError(this, "Fell off the end in Context::findHead(): " + initType + "," + isStatic);
    }

    function getOpenNamespaces()
        env.open_namespaces;

    function getDefaultNamespace()
        env.defaultNamespace;

    function setDefaultNamespace (ident: NamespaceExpr) {
        env.defaultNamespace = resolveNamespaceExpr(ident);
    }

    function openNamespace (ident: NamespaceExpr): void
        env.openNamespace( resolveNamespaceExpr(ident) );

    function pushNamespace(ns): void
        env.openNamespace(ns);

    function getStrict()
        env.strict;

    function setStrict(mode): void
        env.strict = mode;

    // Compile-time name resolution.
    //
    // FIXME.  This algorithm is probably not correct yet, and it is
    // certainly incomplete (does not handle qualified names).

    // SYNTACTIC CONDITION.  Name resolved at compile time must be
    // unambiguously resolvable to a fixture binding.
    //
    // SYNTACTIC CONDITION.  Looking up a value that ought to be a
    // namespace must in fact resolve to a namespace binding.

    // FIXME -- implement.
    //
    // SYNTACTIC CONDITION.  A "with" scope found during resolution
    // should result in an error being thrown.  It's easier to throw
    // the error here than during parsing since we may find the
    // binding nested inside a "with" scope, in which case the "with"
    // is not really a problem.  (But it's possible that /any/ "with"
    // should disqualify /all/ compile-time resolutions inside its
    // body, regardless of shadowing.)
    //
    // SYNTACTIC CONDITION.  What about the non-strict eval operator?
    // It can introduce shadowing bindings too.

    /* resolveNamespaceExpr is responsible for something like 20% of
     * the front-end time (9 May 2008).  It's not possible to cache
     * results here without some sort of reservation mechanism, since
     * a "found" result can turn into a "not found" result later
     * (ambiguous definition).  So don't optimize this now.
     *
     * Compiling parse.es, about 1e6 iterations of the inner loop of
     * hasName() is executed, all doing a string comparison.  The
     * strings probably compare === so it may not be a big problem,
     * but interned symbols might help.  For those that are ===, a
     * call is made to Ast::nsEquals(), which performs another string
     * comparison but never reaches the case where the hashes have to
     * be compared.
     *
     * calls to hasName: 32151
     * loops in hasName: 916000
     * of which calls to nsEquals: 5783
     */
    function resolveNamespaceExpr(ident: NamespaceExpr) : Ast::Namespace {
        switch type (ident) {
        case (id: Ast::Identifier) {
            let fxtr = findFixtureWithIdentifier (id.ident, null);
            if (fxtr && fxtr.data is Ast::NamespaceFixture)
                return fxtr.data.ns;
            if (fxtr == null)
                Parse::syntaxError(this, "Fixture not found: " + ident);
            Parse::internalError(this, "Fixture with unknown value " + fxtr.data);
        }
        case (qi: Ast::QualifiedIdentifier) {
            Parse::internalError(this, "resolveNamespaceExpr: no qualified identifier references");
        }
        case (id: Ast::LiteralString) {
            return new Ast::ForgeableNamespace(id.strValue);
        }
        case (objref: Ast::ObjectRef) {
            Parse::internalError(this, "resolveNamespaceExpr: no object references");
        }
        }
    }

    function resolveTypeNameExpr(ident: Ast::IdentExpr) : Ast::TypeExpr {

        // This will look pretty much exactly like resolveNamespaceExpr, above, but
        // will look for types instead.

        return ident;  // FIXME -- this is not actually a TypeExpr
    }

    function resolveIdentExpr (ident: Ast::IdentExpr, it: Ast::INIT_TARGET) : Ast::FixtureName {
        if (ident is Ast::Identifier) {
            let fx = findFixtureWithIdentifier(ident.ident, it);
            if (fx == null)
                Parse::syntaxError(this, "Unable to resolve identifier " + ident);
            return fx.name;
        }

        Parse::internalError(this, "resolveIdentExpr: case not implemented " + ident);
    }

    private function findFixtureWithIdentifier (id: Ast::IDENT, it: ? Ast::INIT_TARGET) {
        for ( let nssl = getOpenNamespaces() ; nssl != null ; nssl = nssl.link ) {
            let fx = findFixtureWithNames(id, nssl.nsset, it);
            if (fx != null) 
                return fx;
        }
        return null;
    }

    private function findFixtureWithNames (id, nss, it: ? Ast::INIT_TARGET) {
        let env = this.env;

        if (it == Ast::instanceInit) {
            while (env != null && env.tag != Rib.CLASS_INSTANCE)
                env = env.link;
            return findFixtureWithNamesInHead(env.head, id, nss);
        }

        while (env != null) {
            let result = findFixtureWithNamesInHead(env.head, id, nss);
            if (result != null)
                return result;
            env = env.link;
        }
        
        return null;
    }

    private function findFixtureWithNamesInHead(head, id, nss: Ast::NamespaceSet) {
        let ns = null;
        while (nss != null) {
            if (hasName (head, id, nss.ns)) {
                if (ns !== null)
                    Parse::syntaxError(this, "Ambiguous reference to '" + id + "': defined in " + ns + " and " + nss.ns);
                ns = nss.ns;
            }
            nss = nss.link;
        }

        if (ns != null)
            return getFixture (head, id, ns);

        return null;
    }

    // The use of a cache here speeds up hasName significantly, but
    // should be considered experimental.  The hash table in the cache
    // wants to merge with the table used in the back end, at some
    // point.  See notes near the top of the file.

    private function getCache(head) {
        if (head.cache != null) {
            Util::assert(head.cache.length <= head.fixtures.length);
            if (head.cache.length < head.fixtures.length)
                extendTable(head.cache, head.fixtures);
            return head.cache;
        }

        // "5" is empirically determined, and "10" would be about the
        // same; "0" is too low and "20" is too high, for sure.

        if (head.fixtures.length > 5) {
            head.cache = { length: 0, table: newTable() };
            extendTable(head.cache, head.fixtures);
        }

        return head.cache;
    }

    private function newTable()
        new Util::Hashtable((function (x) x.id.hash), 
                            (function (a,b) a.id === b.id && Ast::nsEquals(a.ns, b.ns)),
                            null);

    private function extendTable(cache, fxtrs) {
        for ( let i=cache.length, limit=fxtrs.length ; i < limit ; i++ ) {
            let pn = fxtrs[i].name;
            cache.table.write({id: pn.name.id, ns: pn.name.ns}, fxtrs[i]);
        }
        cache.length = fxtrs.length;
    }

    // Avoids allocation.
    private var name_helper = { id: null, ns: null };

    private function hasName (head, id, ns) {
        let cache = getCache(head);

        // For small rib sets we just search sequentially
        if (cache == null) {
            let fxtrs = head.fixtures;
            for ( let i=0, limit=fxtrs.length ; i < limit ; i++ ) {
                let pn = fxtrs[i].name;
                if (pn.name.id == id && Ast::nsEquals(pn.name.ns, ns))
                    return true;
            }
            return false;
        }

        name_helper.id = id;
        name_helper.ns = ns;
        return cache.table.read(name_helper) != null;
    }

    // This should use the cache too but (a) it's correct if it
    // doesn't and (b) this method accounts for almost none of the
    // running time at present.

    private function getFixture (head, id, ns) {
        let fxtrs = head.fixtures;
        for ( let i=0, limit=fxtrs.length ; i < limit ; i++ ) {
            let pn = fxtrs[i].name;
            if (pn.name.id == id && Ast::nsEquals(pn.name.ns, ns))
                return fxtrs[i];
        }
        Parse::internalError(this, "Name not found " + ns + "::" + id );
    }
}

