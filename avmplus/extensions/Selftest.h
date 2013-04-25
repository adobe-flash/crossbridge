/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_SELFTEST

namespace avmplus
{
    #include "SelftestClasses.h"
    class Selftest;

    // Call 'selftests()' to create a selftest runner, instantiate all the
    // test classes, run all the tests, and delete all selftest objects.
    //
    // The component_glob, category_glob, or name_glob may be NULL (matching
    // anything) or must be globs that are matched against component name,
    // category names, and test names, respectively; names that match are
    // selected for running.
    //
    // For example, to run only the avmplus tests (but all of them) one would
    // call this as
    //
    //    selftests("avmplus", NULL, NULL);
    //
    // Typically the values come from command line arguments.

    void selftests(AvmCore* core, const char* component_glob, const char* category_glob, const char* name_glob);


    // The SelftestRunner maintains a set of test objects and manages the
    // running of tests; it should not be instantiated except from the 'selftests()'
    // function.

    class SelftestRunner : public MMgc::GCRoot
    {
        friend class Selftest;

    public:
        SelftestRunner(AvmCore* core);
        ~SelftestRunner();

        // 'run()' is called by the 'selftests()' function to run the tests and
        // record the results.  See above for a description of the arguments.

        void run(const char* component_glob, const char* category_glob, const char* name_glob);

    private:
        // 'join()' is called by the Selftest constructor to register a
        // Selftest instance with this runner.

        void join(Selftest* test);
        void createSelftestClasses();
        void createGeneratedSelftestClasses();

        void logStart();
        void logEnd();
        void logTest();
        void logPass();
        void logFailure(const char* text, const char* file, int line);
        void logException();
        void logCrash();

        AvmCore*  core;
        Selftest* tests;             // linked through the 'next' field of 'Selftest'
        Selftest* last;              // last element in the list
        Atom      token;             // used by logging, typically a heap-allocated double

        const char *test_component;  // maintained
        const char *test_category;   //   by the
        const char *test_name;       //     'run()' method

        static bool match(const char* glob, const char* s);
    };


    // Specific test categories subclass Selftest and pass information
    // about the tests to the constructor.  The selftest runner uses
    // the information to decide whether to run a particular test.
    //
    // The pair component/category must be globally unique, and the
    // test names must be unique for a particular subclass of Selftest.

    class Selftest
    {
        friend class SelftestRunner;

    public:
        Selftest(AvmCore* core, const char* component, const char* category, const char** names, const bool* explicits);
        virtual ~Selftest();

        virtual void prologue();
        virtual void epilogue();
        virtual void run(int n) = 0;

    protected:
        void verifyPass(int expr, const char* text_expr, const char* file, int line);

        AvmCore*     core;
        const char*  component;   // component, eg 'avmplus' or 'player'
        const char*  category;    // category, eg 'peephole' or 'jit'
        const char** names;       // test names
        const bool*  explicits;   // explicitness flags

    private:
        Selftest*    next;        // next test in a list owned by the SelftestRunner
    };
}

#endif // VMCFG_SELFTEST
