// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component nanojit
%%category  codealloc

%%prefix

// This conditionalization is clumsy, but we don't seem to have any other
// mechanism to enable a selftest conditionally upon the configuration.

#ifdef VMCFG_NANOJIT

using nanojit::NIns;
using nanojit::CodeList;
using nanojit::CodeAlloc;

static bool verbose = false;

class Randomizer {
private:

    TRandomFast seed;

public:

    Randomizer()
    {
        MathUtils::initRandom(&seed);
    }
    
    // Return integer in range [min,max).

    uint32_t range(uint32_t min, uint32_t max)
    {
        return min + MathUtils::Random(max - min, &seed);
    }
};

class CodeAllocDriver {
public:

    struct CodeListProfile {
        uint32_t largefreq;     // 1 in 'largefreq' chance codelist will be "large"
        uint32_t smallmin;      // Minimum size of "small" codelist
        uint32_t smallmax;      // Maximum size of "small" codelist
        uint32_t largemin;      // Minimum size of "large" codelist
        uint32_t largemax;      // Maximum size of "large" coelist
    };

    struct AllocationProfile {
        uint32_t limit;         // Largest block allocation permitted, zero for no limit
        uint32_t leftfreq;      // 1 in 'leftfreq' chance returned space will start at beginning of block
    };

    CodeAllocDriver(uint32_t max_codelists, CodeListProfile& codelist_profile, AllocationProfile& alloc_profile);
    
    void run(uint32_t iterations);
        
private:

    Randomizer rand;
    uint32_t n_active;          // Maximum number of simultaneously-allocated codelists
    CodeListProfile cp;         // Parameters controlling distribution of codelist sizes
    AllocationProfile ap;       // Parameters controlling allocation of each codelist

    void makeCodeList(uint32_t index, CodeAlloc& alloc, CodeList*& code);
};

CodeAllocDriver::CodeAllocDriver(uint32_t max_codelists, CodeListProfile& codelist_profile, AllocationProfile& alloc_profile)
    : n_active(max_codelists), cp(codelist_profile), ap(alloc_profile)
{}

// Create a codelist at the specified index in the codelist pool.
// The length of the codelist is generated pseudo-randomly.

void CodeAllocDriver::makeCodeList(uint32_t index, CodeAlloc& alloc, CodeList*& code)
{
     NIns* start;
     NIns* end;
  
     int32_t size = ((rand.range(0, cp.largefreq) > 0)
                     ? rand.range(cp.smallmin, cp.smallmax)
                     : rand.range(cp.largemin, cp.largemax));
     if (verbose)
         AvmLog("code-heap-test: codelist %d size %d\n", index, size);
     while (size > 0) {
         alloc.alloc(start, end, ap.limit);
         uint32_t blksize = uint32_t(end - start);
         if (verbose)
             AvmLog("code-heap-test: alloc  %x-%x %d\n", start, start, blksize);
         size -= blksize;
         if (size < 0) {
             // Last allocated block was not completely used, so we will return some of it.
             // We choose randomly whether to return a prefix of the block, or a segment out
             // of the middle.  This doesn't correspond precisely to normal usage in nanojit.
             // Note that if we are not retaining at least two bytes, we cannot split in the middle.
             int32_t excess = -size;
             if (rand.range(0, ap.leftfreq) > 0 || blksize - excess < 2) {
                 // Hole starts at left
                 // Note that hole at right is deliberately not handled in CodegenAlloc.cpp, results in assert.
                 if (verbose)
                     AvmLog("code-heap-test: hole l %x-%x %d\n", start, start + excess, excess);
                 alloc.addRemainder(code, start, end, start, start + excess);
             } else {
                 // Hole will go in middle
                 uint32_t offset = rand.range(1, blksize - excess);
                 if (verbose)
                     AvmLog("code-heap-test: hole m %x-%x %d\n", start + offset, start + offset + excess, excess);
                 alloc.addRemainder(code, start, end, start + offset, start + offset + excess);
             }
         } else {
             // Add entire block to codelist.
             CodeAlloc::add(code, start, end);
         }
     }
}

// Repeatedly construct and destroy codelists.
// We maintain a pool of codelists.  On each iteration, we replace one
// of the existing codelists with a new one, freeing the old.

void CodeAllocDriver::run(uint32_t iterations)
{
    nanojit::Config config;
    // Enable page protection flag checking during test code
    config.check_page_flags = true;

    CodeAlloc alloc(&config);

    CodeList** codelists = mmfx_new_array(CodeList*, n_active);

    VMPI_memset(codelists, 0, n_active * sizeof(CodeList*));

    for (uint32_t i = 0; i < iterations; i++) {
        uint32_t victim = rand.range(0, n_active);
        alloc.freeAll(codelists[victim]);
        makeCodeList(victim, alloc, codelists[victim]);
        //alloc.logStats();
        debug_only( alloc.sanity_check(); )
    }

    mmfx_delete_array(codelists);
}

#endif /* VMCFG_NANOJIT */

%%methods

#ifdef VMCFG_NANOJIT

typedef CodeAllocDriver::CodeListProfile CodeListProfile;
typedef CodeAllocDriver::AllocationProfile AllocationProfile;

static CodeListProfile cp = { 5, 1, 1*1024, 1, 16*1024 };

static AllocationProfile ap[] = { { 0,     2 },
                                  #ifndef NANOJIT_64BIT
                                  // 128-byte minimum is too small on 64-bit platorms.
                                  { 128,   2 },
                                  #endif
                                  { 512,   2 },
                                  { 1024,  2 },
                                  { 2048,  2 },
                                  { 4096,  2 } };

static uint32_t n_ap = sizeof(ap) / sizeof(AllocationProfile);

#endif /* VMCFG_NANOJIT */

%%test allocfree

#ifdef VMCFG_NANOJIT

for (uint32_t i = 0; i < n_ap; i++) {
    CodeAllocDriver* driver = mmfx_new(CodeAllocDriver(20, cp, ap[i]));
#if defined(DEBUG) && !(defined(VMCFG_IA32) || defined(VMCFG_AMD64))
    // This test is very slow in debug builds, due to the calls to sanity_check().
    // Run an abbreviated version of the test except on desktop platforms.
    driver->run(200);
#else
    driver->run(20000);
#endif
    mmfx_delete(driver);
 }

#endif /* VMCFG_NANOJIT */

// We pass if we don't crash or assert.
%%verify true

