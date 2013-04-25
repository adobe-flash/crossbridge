/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

#ifdef VMCFG_NANOJIT
#include "CodegenLIR.h"
#endif

//GCC only allows intrinsics if sse2 is enabled
#if (defined(_MSC_VER) || (defined(__GNUC__) && defined(__SSE2__))) && (defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64))
    #include <emmintrin.h>
#endif

#include "api-versions.cpp"

#include "avmplus-tracers.hh"

/*
 * TUNABLE PARAMETER: OSR_THRESHOLD_DEFAULT
 *
 * The number of method invocations or loop iterations to trigger compilation when OSR is enabled.
 *
 * A value of zero is interpreted specially, indicating that static initializers should always
 * be interpreted and other methods compiled eagerly, identical to the policy when AVMFEATURE_OSR
 * is not configured.  The default value may be overridden by runtime configuration, e.g.,
 * the "-osr" option to avmshell.  Additionally, the globally-configured threshold may be overridden
 * on a per-method basis via the ExecPolicy attribute, allowing content to opt-in or opt-out independently
 * of the default global OSR policy.  For diagnostic purposes, we may wish to definitively suppress OSR
 * regardless of content opt-in. By special dispensation, the switch "-osr=0" disables OSR globally and
 * cannot be overridden.  Programmatically, setting config.osr_threshold to zero does not disable opt-in,
 * rather, it is also necessary to set config.osr_enabled to false.
 *
 * The present threshold value has been set based on preliminary startup-time studies on AIR Android.
 */
#ifdef VMCFG_OSR
// All methods subject to OSR (where supported) using a default threshold of 17.
// Per-method ExecPolicy attributes may alter the threshold, or disable OSR by using threshold 0.
static const bool OSR_ENABLED_DEFAULT   = true;
static const int  OSR_THRESHOLD_DEFAULT = 17;    // tuned
#else
// All methods use threshold 0 by default, which selects the legacy non-OSR compilation policy.
// Per-method ExecPolicy attributes may set a non-zero threshold, however, allowing content to
// opt-in to OSR on a per-method basis as desired.
static const bool OSR_ENABLED_DEFAULT   = true;
static const int  OSR_THRESHOLD_DEFAULT = 0;     // legacy policy
#endif

#define AVMPLUS_STRING_DELETED ((Stringp)(1))

#ifndef MARK_SECURITY_CHANGE // http://frpbugapp.macromedia.com/bugapp/detail.asp?ID=999999
    /*
        This exists solely to ensure that tamarin-security differs from tamarin-redux/tamarin-central,
        and that the canonical form for the MARK_SECURITY_CHANGE macro remains in place.
        Please do not ever remove this ifdef or this comment. (srj)
    */
#endif

namespace avmplus
{
    using namespace MMgc;

    // Regex compilation cache.

    RegexCache::RegexCache()
        : m_timestamp(0)
        , m_wasted(0)
        , m_useful(0)
        , m_disabled(false)
    {
    }

    RegexCacheEntry& RegexCache::findCachedRegex(bool& found, String* source, String* options)
    {
        if (m_disabled) {
            found = false;
            return m_entries[0];
        }

        size_t smallest = 0;

        for ( size_t i=0 ; i < ARRAY_SIZE(m_entries) ; i++ ) {
            RegexCacheEntry& it = m_entries[i];
            if (it.match(source, options)) {
                it.timestamp = ++m_timestamp;
                it.hits++;
                found = true;
                return it;
            }
            if (it.timestamp < m_entries[smallest].timestamp)
                smallest = i;
        }

        // A miss.  Evict an entry and account for its utility.

        RegexCacheEntry& it = m_entries[smallest];

        // If a cache entry was used just once then it was wasted work;
        // if wasted work dominates then disable the cache.

        if (it.hits == 1)
            m_wasted++;
        else if (it.hits > 1)
            m_useful+=it.hits-1;
        if (m_timestamp > warmupTicks && m_wasted > m_useful*wastedWorkMultiplier) {
            //printf("DISABLED!\n");
            m_disabled = true;
        }

        // New entry

        it.clear();
        it.timestamp = ++m_timestamp;
        it.hits++;
        found = false;
        return it;
    }

    bool RegexCache::testCachedRegex(String* source, String* options)
    {
        if (m_disabled)
            return false;

        for ( size_t i=0 ; i < ARRAY_SIZE(m_entries) ; i++ )
            if (m_entries[i].match(source, options))
                return true;

        return false;
    }

    void RegexCache::clear()
    {
        for ( size_t i=0 ; i < ARRAY_SIZE(m_entries) ; i++ )
            m_entries[i].clear();
        m_timestamp = 0;
        m_wasted = 0;
        m_useful = 0;
        m_disabled = false;
    }

#ifdef DEBUG
    // This is called from the Atom code in vmbase
    bool testIsObject(avmplus::Atom atom)
    {
        return AvmCore::isObject(atom);
    }
#endif

    void AvmCore::setCacheSizes(const CacheSizes& cs)
    {
        #ifdef AVMPLUS_VERBOSE
        if (isVerbose(VB_traits))
        {
            console << "setCacheSize: bindings " << hexAddr(cs.bindings) << " metadata " << hexAddr(cs.metadata) << '\n';
        }
        #endif

        //AvmLog("setCacheSize: bindings %d metadata %d\n",cs.bindings,cs.metadata);

        m_tbCache->resize(cs.bindings);
        m_tmCache->resize(cs.metadata);
        m_msCache->resize(cs.methods);
    }

    void AvmCore::handleAbcUnloaded()
    {
        m_tbCache->flush();
        m_tmCache->flush();
        m_msCache->flush();
    }

    const uint32_t AvmCore::verbose_default = 0; // all off
    const bool AvmCore::methodNames_default = true;
    const bool AvmCore::oldVectorMethodNames_default = true;
    const bool AvmCore::verifyall_default = false;
    const bool AvmCore::verifyonly_default = false;
    const bool AvmCore::verifyquiet_default = false;
    const Runmode AvmCore::runmode_default = RM_mixed;
    const bool AvmCore::osr_enabled_default = OSR_ENABLED_DEFAULT;
    const uint32_t AvmCore::osr_threshold_default = OSR_THRESHOLD_DEFAULT;
    const uint32_t AvmCore::jitprof_level_default = 0; // no logging.
    const bool AvmCore::interrupts_default = false;
    const bool AvmCore::jitordie_default = false;

#ifdef AVMPLUS_VERBOSE
    #ifdef VMCFG_NANOJIT
        const uint32_t AvmCore::DEFAULT_VERBOSE_ON = (uint32_t)~0 & ~(nanojit::LC_FragProfile); // LC_FragProfile changes generated code!?!
    #else
        const uint32_t AvmCore::DEFAULT_VERBOSE_ON = (uint32_t)~0;
    #endif

    static bool substrMatches(const char* pattern, const char* p, const char* e)
    {
        ptrdiff_t const patlen = VMPI_strlen(pattern);
        return (e-p) >= patlen && !VMPI_strncmp(p, pattern, patlen);
    }

    /*static*/ uint32_t AvmCore::parseVerboseFlags(const char* p, char*& badFlag)
    {
        uint32_t r = 0;
        badFlag = 0;

        for (;;)
        {
            const char* e = p;
            // stop on null or end-of-line... use >=32 to catch those plus other unlikely/uninteresting cases
            while (*e >= 32 && *e != ',')
                e++;

            if (substrMatches("parse", p, e))
                r |= VB_parse;
            else if (substrMatches("verify", p, e))
                r |= VB_verify;
            else if (substrMatches("interp", p, e))
                r |= VB_interp;
            else if (substrMatches("traits", p, e))
                r |= VB_traits;
            else if (substrMatches("builtins", p, e))
                r |= VB_builtins;
            else if (substrMatches("execpolicy", p, e))
                r |= VB_execpolicy;
            else if (substrMatches("memstats", p, e))
                MMgc::GCHeap::GetGCHeap()->Config().gcstats = true;
            else if (substrMatches("sweep", p, e))
                MMgc::GCHeap::GetGCHeap()->Config().autoGCStats = true;
            else if (substrMatches("occupancy", p, e))
                MMgc::GCHeap::GetGCHeap()->Config().verbose = true;
#ifdef VMCFG_NANOJIT
            else if (substrMatches("jit", p, e))
                 r |= VB_jit | LC_AfterDCE | LC_Native;
            else if (substrMatches("lircfg-bb", p, e))
                r |= VB_jit | VB_lircfg | VB_lircfg_bb;
            else if (substrMatches("lircfg-ins", p, e))
                r |= VB_jit | VB_lircfg | VB_lircfg_ins;
            else if (substrMatches("lircfg", p, e))
                r |= VB_jit | VB_lircfg;  // unfortunately we have to turn on VB_jit to get names @todo fix that!
            else if (substrMatches("opt", p, e))
                r |= VB_jit | LC_Liveness | LC_ReadLIR | LC_AfterSF;
            else if (substrMatches("lir", p, e))
                r |= VB_jit | LC_ReadLIR;
            else if (substrMatches("regs", p, e))
                r |= VB_jit | LC_Activation | LC_RegAlloc;
            else if (substrMatches("raw", p, e))
                r |= VB_raw;
            else if (substrMatches("bytes", p, e))
                r |= VB_jit | LC_Native | LC_Bytes;
#endif /* VMCFG_NANOJIT */
            else
                badFlag = (char*)p;
            if (*e < 32)
                break;
            p = e+1;
        }

        return r;
    }

    const char* AvmCore::identifyDomain(Domain* domain) {
        return domain == builtinDomain  ? "builtin" : NULL;
    }

    static void parseVerboseRestrictions(const char* s, MethodRecognizerSet* list, PrintWriter& console)
    {
        while(*s)
        {
            s = (*s == '\"' || *s == ',') ? s+1 : s;
            if (!*s)
                break;

            MethodRecognizer* r = MethodRecognizer::parse(&s, ',');
            list->add( r );
            if (r != NULL)
                console << "verbose only for " << r << "\n";
            else
                console << "warning: not able to parse verbose only rule :" << s << "\n";
            if (*s) s++;
        }
    }

    /**
     * If verbosity is restricted per-method, then scan each MethodRecognizer seeing
     * seeing if we get a match , otherwise just check the bit.
     */
    bool AvmCore::isVerbose(uint32_t b, MethodInfo* info)
    {
        bool v = (b & config.verbose_vb) ? true : false;
        if (v && info && config.verboseOnlyString)
        {
            MethodRecognizerSet* list = &_verboseRestrictedTo;
            if (list->isEmpty())
                parseVerboseRestrictions(config.verboseOnlyString, list, console); // no good way to init it prior to this call since verboseOnlyString update occurs post-ctor

            // disabled by default
            v = false;
            for(uint32_t i=0,n=list->length(); !v && i<n; i++)
            {
                MethodRecognizer* r = list->get(i);
                v = (r) ? r->matches(info) : false;
            }
        }
        return v;
    }

#endif

    // a single string with characters 0x00...0x7f (inclusive)
    static const char* const k_cachedChars =
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
        "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
        "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
        "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"
        "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
        "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
        "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F";
    AvmCore::AvmCore(GC* g, ApiVersionSeries apiVersionSeries)
        : GCRoot(g)
        , enterEventLoop(false)
        , inEventLoop(false)
#ifdef DEBUGGER
        , _sampler(NULL)
#endif
        , currentMethodFrame(NULL)
        , livePools(NULL)
        , m_activeApiVersionSeries(apiVersionSeries)
        , m_activeApiVersionSeriesMask(1 << apiVersionSeries)
#ifdef VMCFG_NANOJIT
        , m_flushBindingCachesNextSweep(false)
#endif
        , console(NULL)
        , gc(g)
#ifdef VMCFG_STACK_METRICS
        , minStack(~uintptr_t(0))
        , maxStack(0)
#endif
#ifdef _DEBUG
        , codeContextThread(VMPI_currentThread())
#endif
        , bugzilla444630(false)
#ifdef DEBUGGER
        , langID(-1)
        , passAllExceptionsToDebugger(false)
#endif
        , exec(NULL)
        , gcInterface(g)
#ifdef DEBUGGER
        , _debugger(NULL)
        , _profiler(NULL)
#endif
        , m_tbCache(QCache::create(CacheSizes::DEFAULT_BINDINGS, g))
        , m_tmCache(QCache::create(CacheSizes::DEFAULT_METADATA, g))
        , m_msCache(QCache::create(CacheSizes::DEFAULT_METHODS, g))
        , m_domainMgr(NULL)
        , m_domainEnvs(gc, 1) 
#ifdef AVMPLUS_VERBOSE
        , _verboseRestrictedTo(gc,0)
#endif
    {
        // sanity check for all our types
        MMGC_STATIC_ASSERT(sizeof(int8_t) == 1);
        MMGC_STATIC_ASSERT(sizeof(uint8_t) == 1);
        MMGC_STATIC_ASSERT(sizeof(int16_t) == 2);
        MMGC_STATIC_ASSERT(sizeof(uint16_t) == 2);
        MMGC_STATIC_ASSERT(sizeof(int32_t) == 4);
        MMGC_STATIC_ASSERT(sizeof(uint32_t) == 4);
        MMGC_STATIC_ASSERT(sizeof(int64_t) == 8);
        MMGC_STATIC_ASSERT(sizeof(uint64_t) == 8);
        MMGC_STATIC_ASSERT(sizeof(intptr_t) == sizeof(void *));
        MMGC_STATIC_ASSERT(sizeof(uintptr_t) == sizeof(void *));
#ifdef AVMPLUS_64BIT
        MMGC_STATIC_ASSERT(sizeof(intptr_t) == 8);
        MMGC_STATIC_ASSERT(sizeof(uintptr_t) == 8);
#else
        MMGC_STATIC_ASSERT(sizeof(intptr_t) == 4);
        MMGC_STATIC_ASSERT(sizeof(uintptr_t) == 4);
#endif

#ifdef VMCFG_TELEMETRY_SAMPLER
        // intialize the sampler, off by default
        samplerEnabled = false;
        sampler = NULL;
        sampleTicks = 0;
#endif

        // set default mode flags
        config.verbose_vb = verbose_default;
        config.verboseOnlyString = NULL;

        // policy rule overrides 0=none
        config.compilePolicyRules = NULL;

        // default to recording method names, if possible.
        // (subclass might change this in its ctor if it wants to conserve memory.)
        config.methodNames = methodNames_default;
        config.oldVectorMethodNames = oldVectorMethodNames_default;

        config.verifyall = verifyall_default;
        config.verifyonly = verifyonly_default;

        // jit flag forces use of jit-compiler instead of interpreter
        config.runmode = runmode_default;
        config.osr_enabled = osr_enabled_default;
        config.osr_threshold = osr_threshold_default;
        config.jitprof_level = jitprof_level_default;
        config.jitordie = jitordie_default;

        config.interrupts = interrupts_default;

        gcInterface.SetCore(this);
        xmlEntities                 = NULL;
        exceptionFrame              = NULL;
        exceptionAddr               = NULL;
        builtinPool                 = NULL;
        builtinDomain               = NULL;
        builtinBugCompatibility     = NULL;

        m_versionedURIs = HeapHashtable::create(gc);
#ifdef _DEBUG
        m_unversionedURIs = HeapHashtable::create(gc);
#endif

        m_domainMgr = new(gc) DomainMgr(this);

        GetGC()->SetGCContextVariable(MMgc::GC::GCV_AVMCORE, this);

        minstack           = 0;

#ifdef DEBUGGER
        callStack          = NULL;
#endif

        interrupted     = NotInterrupted;
        pending_interrupt     = NotInterrupted;

        strings         = NULL;
        numStrings      = 0;
        namespaces      = NULL;
        numNamespaces   = 0;
        stringCount     = 0;
        deletedCount    = 0;
        nsCount         = 0;

        currentMethodInfoCount = 0;
        numStrings = 1024; // power of 2
        numNamespaces = 1024;  // power of 2
        strings = mmfx_new_array(GCRoot::GCMember<String>, numStrings);
        namespaces = mmfx_new_array(GCRoot::GCMember<Namespace>, numNamespaces);
        console.setCore(this);

        kconstructor = internConstantStringLatin1("constructor");
        kundefined = internConstantStringLatin1("undefined");
        knull = internConstantStringLatin1("null");
        ktrue = internConstantStringLatin1("true");
        kfalse = internConstantStringLatin1("false");
        ktoString = internConstantStringLatin1("toString");
        ktoLocaleString = internConstantStringLatin1("toLocaleString");
        kvalueOf = internConstantStringLatin1("valueOf");
        kvalue = internConstantStringLatin1("value");
        klength = internConstantStringLatin1("length");

        kobject = internConstantStringLatin1("object");
        kboolean = internConstantStringLatin1("boolean");
        kvoid = internConstantStringLatin1("void");
        knumber = internConstantStringLatin1("number");
#ifdef VMCFG_FLOAT
        kfloat = internConstantStringLatin1("float");
        kfloat4 = internConstantStringLatin1("float4");
        knumeric = internConstantStringLatin1("numeric[INTERNAL]");
#endif
        kstring = internConstantStringLatin1("string");
        kxml = internConstantStringLatin1("xml");
        kfunction = internConstantStringLatin1("function");
        kglobal = internConstantStringLatin1("global");
        kcallee = internConstantStringLatin1("callee");

        kuri = internConstantStringLatin1("uri");
        kprefix = internConstantStringLatin1("prefix");
        kNaN = doubleToAtom(MathUtils::kNaN);
#ifdef VMCFG_FLOAT
        kFltNaN = floatToAtom( (float) MathUtils::kNaN );
        kFltOne = floatToAtom( 1.0f );
        kFltMinusOne = floatToAtom( -1.0f );
        kFlt4NaN = float4Atom(  AtomConstants::undefinedAtom );
#endif
        kNeedsDxns = internConstantStringLatin1("NeedsDxns");
        kVersion = internConstantStringLatin1("Version");

        kanonymousFunc = newConstantStringLatin1("<anonymous>");

        klittleEndian = internConstantStringLatin1("littleEndian");
        kbigEndian = internConstantStringLatin1("bigEndian");
        kparent = internConstantStringLatin1("parent");
        kattribute = internConstantStringLatin1("attribute");
        kcomment = internConstantStringLatin1("comment");
        kprocessing_instruction = internConstantStringLatin1("processing-instruction");
        kelement = internConstantStringLatin1("element");
        ktext = internConstantStringLatin1("text");
        kattributeAdded = internConstantStringLatin1("attributeAdded");
        kattributeRemoved = internConstantStringLatin1("attributeRemoved");
        kattributeChanged = internConstantStringLatin1("attributeChanged");
        knodeAdded = internConstantStringLatin1("nodeAdded");
        knodeRemoved = internConstantStringLatin1("nodeRemoved");
        knodeChanged = internConstantStringLatin1("nodeChanged");
        knamespaceAdded = internConstantStringLatin1("namespaceAdded");
        knamespaceRemoved = internConstantStringLatin1("namespaceRemoved");
        knamespaceSet = internConstantStringLatin1("namespaceSet");
        knameSet = internConstantStringLatin1("nameSet");
        ktextSet = internConstantStringLatin1("textSet");
        klocalName = internConstantStringLatin1("localName");
        kindex = internConstantStringLatin1("index");
        kinput = internConstantStringLatin1("input");
        kemptyCtor = internConstantStringLatin1("emptyCtor");
        kMath = internConstantStringLatin1("Math");
        kNumber = internConstantStringLatin1("Number");

        kAsterisk = internConstantStringLatin1("*");
        kColon = internConstantStringLatin1(":");
        kUnderscore = internConstantStringLatin1("_");
        kXML1998NS = internConstantStringLatin1("http://www.w3.org/XML/1998/namespace");
        kzero = internConstantStringLatin1("0");
        kClassS = internConstantStringLatin1("Class$");
#ifdef VMCFG_FLOAT
        kVectorFloat = internConstantStringLatin1("Vector.<float>");
        kVectorFloat4 = internConstantStringLatin1("Vector.<float4>");
#endif
        kVectorNumber = internConstantStringLatin1("Vector.<Number>");
        kVectorint = internConstantStringLatin1("Vector.<int>");
        kVectoruint = internConstantStringLatin1("Vector.<uint>");
        kVectorAny = internConstantStringLatin1("Vector.<*>");
#ifdef VMCFG_FLOAT
        kx = internConstantStringLatin1("x");
        ky = internConstantStringLatin1("y");
        kz = internConstantStringLatin1("z");
        kw = internConstantStringLatin1("w");
#endif
        kExecPolicy = internConstantStringLatin1("ExecPolicy");
        kOSR = internConstantStringLatin1("OSR");

        for (int i = 0; i < 128; i++)
        {
            AvmAssert(k_cachedChars[i] == i);
            // call String::createLatin1() with an explicit length of 1; required
            // when singleChar==0, because in that case we need a string
            // which is a single character with value 0
            cachedChars[i] = internString(String::createLatin1(this, &k_cachedChars[i], 1));
        }

        workerStates[0] = internConstantStringLatin1("none");
        workerStates[1] = internConstantStringLatin1("new");
        workerStates[2] = internConstantStringLatin1("canstart");
        workerStates[3] = internConstantStringLatin1("starting");
        workerStates[4] = internConstantStringLatin1("running");
        workerStates[5] = internConstantStringLatin1("finishing");
        workerStates[6] = internConstantStringLatin1("terminated");
        workerStates[7] = internConstantStringLatin1("failed");
        workerStates[8] = internConstantStringLatin1("aborted");
        workerStates[9] = internConstantStringLatin1("exception");


        booleanStrings[0] = kfalse;
        booleanStrings[1] = ktrue;

        // init kEmptyString last, so that StringObject can use it as a sentinel for
        // determining if all the cached strings are valid.
        kEmptyString = internConstantStringLatin1("");

        // create public namespace -- note that this
        publicNamespace = internNamespace(newNamespace(kEmptyString, Namespace::NS_Public, kApiVersion_VM_ALLVERSIONS));

        _emptySupertypeList = Traits::allocSupertypeList(gc, 0);

        // the public namespace (empty uri) is always versioned.
        this->m_versionedURIs->get_ht()->add(kEmptyString->atom(), trueAtom);

        // cache public namespaces. (This creates namespaces that aren't in the active
        // series, but that's ok.)
        this->publicNamespaces = NamespaceSet::_create(GetGC(), kApiVersion_count);
        for (int i = 0; i < kApiVersion_count; ++i)
        {
            Namespacep ns = this->internNamespace(this->newNamespace(kEmptyString, Namespace::NS_Public, ApiVersion(i)));
            publicNamespaces->_initNsAt(i, ns);
        }
    }

    AvmCore* AvmCore::getActiveCore() {
        return GC::GetActiveGC()->core();
    }

    AvmCore::~AvmCore()
    {
#ifdef DEBUGGER
        if (gc)
        {
            gc->SetAttachedSampler(NULL);
        }
        delete _sampler;
        _sampler = NULL;
#endif

        m_tbCache->flush();
        m_tmCache->flush();
        m_msCache->flush();

        m_tbCache = NULL;
        m_tmCache = NULL;
        m_msCache = NULL;

        delete exec;
        delete m_domainMgr;

        // Free the numbers and strings tables
        mmfx_delete_array(strings);
        if (gc)
        {
            gc->SetGCContextVariable(GC::GCV_AVMCORE, NULL);
        }

        strings = NULL;
        // important for potential callback invocation of presweep
        numStrings = 0;

        mmfx_delete_array(namespaces);
        namespaces = NULL;
        numNamespaces = 0;

#ifdef DEBUGGER
        delete _profiler;
        _profiler = NULL;
#endif
        LivePoolNode* node = livePools;
        while (node)
        {
            LivePoolNode* next = node->next;
            delete node;
            node = next;
        }
        livePools = NULL;

#ifdef DEBUGGER
        while (callStack != NULL)
        {
            // note that reset() sets callStack to callStack->next,
            // so this loop will terminate
            if (callStack->reset() == false) {
                break;
            }
        }
#endif

#ifdef AVMPLUS_VERBOSE
        while(!_verboseRestrictedTo.isEmpty())
            mmfx_delete( _verboseRestrictedTo.removeLast() );
#endif
    }

    void AvmCore::initBuiltinPool(
#ifdef DEBUGGER
                                  int tracelevel
#endif
    )
    {
        // Create the singleton ExecMgr instance.
        exec = new (gc) BaseExecMgr(this);

        #ifdef DEBUGGER
        _debugger = createDebugger(tracelevel);
        _profiler = createProfiler();
        #endif

        builtinDomain = Domain::newDomain(this, NULL);

        // builtins always use BugCompatibility::kLatest
        builtinBugCompatibility = createBugCompatibility(BugCompatibility::kLatest);

#ifdef VMCFG_AOT
        NativeInitializer ninit(this,
                                avmplus::NativeID::builtin_versioned_uris,
                                &builtin_aotInfo,
                                avmplus::NativeID::builtin_abc_method_count,
                                avmplus::NativeID::builtin_abc_class_count);
        ninit.fillInClasses(avmplus::NativeID::builtin_classEntries);
        ninit.fillInMethods(avmplus::NativeID::builtin_methodEntries);
        builtinPool = ninit.parseBuiltinABC(builtinDomain);
#else
        builtinPool = AVM_INIT_BUILTIN_ABC(builtin, this);
#endif
        AvmAssert(builtinPool->isBuiltin);

        // Set the the non-interruptable bit on all builtin functions.
        for(int i=0, size=builtinPool->methodCount(); i<size; i++)
            builtinPool->getMethodInfo(i)->makeNonInterruptible();

        for(int i=0, size=builtinPool->classCount(); i<size; i++)
            builtinPool->getClassTraits(i)->init->makeNonInterruptible();

        for(int i=0, size=builtinPool->scriptCount(); i<size; i++)
            builtinPool->getScriptTraits(i)->init->makeNonInterruptible();

#if defined(DEBUGGER) && !defined(VMCFG_AOT)
        // sampling can begin now, requires builtinPool
        if (_debugger)
        {
            _sampler = createSampler();
            AvmAssert(_sampler != NULL);
            _sampler->initSampling();
        }
#endif
    }

#ifdef DEBUGGER
    Debugger* AvmCore::createDebugger(int /*tracelevel*/)
    {
        return NULL;
    }

    Profiler* AvmCore::createProfiler()
    {
        return NULL;
    }

    Sampler* AvmCore::createSampler()
    {
        return (new Sampler(this));
    }
#endif

    /*virtual*/ void AvmCore::setStackBase()
    {
        // nothing
    }

#ifdef _DEBUG
    /*virtual*/ void AvmCore::tryHook()
    {
        // nothing
    }
#endif

    /*virtual*/ void AvmCore::catchHook(CatchAction action)
    {
        (void)action;
        // nothing
    }

    /*static*/ void AvmCore::readOperands(const uint8_t* &pc, unsigned int& imm32, int& imm24, unsigned int& imm32b, int& imm8 )
    {
        AbcOpcode opcode = (AbcOpcode)*pc++;
        int op_count = opcodeInfo[opcode].operandCount;

        imm8 = pc[0];
        if( opcode == OP_pushbyte || opcode == OP_debug )
        {
            // these two operands have a byte as their first operand, which is not encoded
            // with the variable length encoding scheme for bigger numbers, because it will
            // never be larger than a byte.
            --op_count;
            pc++;
        }

        if( op_count > 0 )
        {
            if( opcode >= OP_ifnlt && opcode <= OP_lookupswitch )
            {
                imm24 = AvmCore::readS24(pc);
                pc += 3;
            }
            else
            {
                imm32 = AvmCore::readU32(pc);
            }

            if( opcode == OP_debug )
            {
                --op_count; //OP_debug has a third operand of a byte
                pc++;
            }
            if( op_count > 1 )
            {
                imm32b = AvmCore::readU32(pc);
            }
        }
    }

    static void initScriptActivationTraits(AvmCore* core, Toplevel* toplevel, MethodInfo* method)
    {
#ifdef VMCFG_AOT
        PoolObject* pool = method->pool();
        const AOTInfo* aotInfo = pool->aotInfo;
        if (method->needActivation()) {
            Traits* activationTraits = method->activationTraits();
            AvmAssert(activationTraits != NULL);
            AvmAssert(method->method_id() < (int)aotInfo->nActivationTraits);
            aotInfo->activationTraits[method->method_id()] = activationTraits;
            if (aotInfo->activationInfo[method->method_id()].initHandler != NULL) {
                // NativeMethodInfo.handler is a union of
                // pointer to function and pointer to member function.
                // Zero the structure so that the entire thing is
                // initialized.
                // See bugzilla#647660
                NativeMethodInfo compiledMethodInfo = {{0}, 0, 0};
                compiledMethodInfo.thunker = aotThunker;
                AvmThunkNativeHandler nhandler;
                nhandler.function = aotInfo->activationInfo[method->method_id()].initHandler;
                activationTraits->init = MethodInfo::create(core->GetGC(), MethodInfo::kInitMethodStub, activationTraits, &compiledMethodInfo, nhandler, aotInfo->activationInfo[method->method_id()].initMethodId);
            }
            method->activationTraits()->resolveSignatures(toplevel);
        }
        method->declaringTraits()->initActivationTraits();
#else
        (void)core;
        (void)toplevel;
        (void)method;
#endif
    }

    ScriptEnv* AvmCore::initOneScript(Toplevel* toplevel, AbcEnv* abcEnv, Traits* scriptTraits)
    {
        VTable* scriptVTable = this->newVTable(scriptTraits, toplevel->objectClass->ivtable(), toplevel);
        const ScopeTypeChain* scriptSTC = scriptTraits->declaringScope();
        if (!scriptSTC)
        {
            scriptSTC = ScopeTypeChain::createEmpty(this->GetGC(), scriptTraits);
            scriptTraits->setDeclaringScopes(scriptSTC);
        }
        ScopeChain* scriptScope = ScriptEnv::createScriptScope(scriptSTC, scriptVTable, abcEnv);
        ScriptEnv* scriptEnv = ScriptEnv::create(this->GetGC(), scriptTraits->init, scriptScope);
        scriptVTable->init = scriptEnv;

        initScriptActivationTraits(this, toplevel, scriptTraits->init);
        return scriptEnv;
    }

    ScriptEnv* AvmCore::initAllScripts(Toplevel* toplevel, AbcEnv* abcEnv)
    {
        PoolObject* pool = abcEnv->pool();

        // The list of ScriptEnvs that correspond to the MethodInfos the DomainMgr
        // already knows about for the Pool/Domain that correspond to the AbcEnv/DomainEnv.
        // Note that the order of the list doesn't matter (DomainMgr will use a hashtable
        // to do the relevant association).
        GCList<ScriptEnv> envs(GetGC(), pool->scriptCount());

        // some code relies on the final script being initialized first, so we
        // must continue that behavior
        uint32_t const last = pool->scriptCount()-1;
        ScriptEnv* main = initOneScript(toplevel, abcEnv, pool->getScriptTraits(last));
        envs.add(main);

        // skip the final one, it's already been done
        for (uint32_t i=0; i < last; i++)
        {
            ScriptEnv* scriptEnv = initOneScript(toplevel, abcEnv, pool->getScriptTraits(i));
            envs.add(scriptEnv);
        }

        this->domainMgr()->addNamedScriptEnvs(abcEnv, envs);

        exec->notifyAbcPrepared(toplevel, abcEnv);

        return main;
    }

    Atom AvmCore::callScriptEnvEntryPoint(ScriptEnv* main)
    {
        main->initGlobal();

        Atom result = 0; // init to zero to make GCC happy

        #ifndef DEBUGGER
        result = main->coerceEnter(main->global->atom());
        #else
        TRY(this, kCatchAction_Rethrow)
        {
            result = main->coerceEnter(main->global->atom());
        }
        CATCH(Exception* exception)
        {
            // Re-throw exception
            Isolate* isolate = getIsolate();
            if (isolate->getAggregate()->queryState(isolate) == Isolate::FINISHING) {
                // Don't rethrow, we're finishing and probably not setup to handle the exception.
            } else {
                this->throwException(exception);
            }
        }
        END_CATCH
        END_TRY
        #endif

        return result;
    }

    Toplevel* AvmCore::initToplevel(ICodeContextCreator& codeContextCreator)
    {
        DomainEnv* builtinDomainEnv = DomainEnv::newDomainEnv(this, builtinDomain, NULL);

        // Use codeContextCreator so that subclasses can construct a variant if appropriate;
        // if they are, we can't use a "naked" CodeContext anywhere, as debugger code might walk
        // thru it and all CodeContexts that it finds are the subclass.
        CodeContext* builtinCodeContext = codeContextCreator.create(builtinDomainEnv, builtinBugCompatibility);

        AvmAssert(builtinPool != NULL);
        AvmAssert(builtinPool->scriptCount() != 0);

        AbcEnv* abcEnv = AbcEnv::create(GetGC(), builtinPool, builtinCodeContext);
        Toplevel* toplevel = createToplevel(abcEnv);

        // save toplevel since it was initially null
        builtinDomainEnv->setToplevel(toplevel);

        AvmCore* core = this;

        // adds clarity to what is usually just global$init()
        SAMPLE_FRAME("[newclass]", core);

        MMgc::GC* gc = core->GetGC();
        PoolObject* pool = abcEnv->pool();
        AvmAssert(pool == core->builtinPool);
        Namespace* dxns = core->dxns();
        AvmAssert(dxns->isPublic());

        uint32_t const last = pool->scriptCount()-1;
        Traits* scriptTraits = pool->getScriptTraits(last);

        Traits* object_ctraits = core->traits.object_ctraits;
        Traits* class_ctraits = core->traits.class_ctraits;
        AvmAssert(object_ctraits->init != NULL);
        AvmAssert(class_ctraits->init != NULL);

        Traits* object_itraits = object_ctraits->itraits;
        Traits* class_itraits = class_ctraits->itraits;
        AvmAssert(object_itraits == core->traits.object_itraits);
        AvmAssert(class_itraits == core->traits.class_itraits);
        AvmAssert(object_itraits->init != NULL);
        AvmAssert(class_itraits->init != NULL);

        // ------------------ construct the vtables

        // Object derives from... nothing
        VTable* object_ivtable = core->newVTable(object_itraits, NULL, toplevel);

        // Class derives from Object
        VTable* class_ivtable = core->newVTable(class_itraits, object_ivtable, toplevel);

        // Object$ derives from Class
        VTable* object_cvtable = core->newVTable(object_ctraits, class_ivtable, toplevel);
        object_cvtable->ivtable = object_ivtable;

        // Class$ derives from Class
        VTable* class_cvtable = core->newVTable(class_ctraits, class_ivtable, toplevel);
        class_cvtable->ivtable = class_ivtable;

        // global script derives from Object
        VTable* scriptVTable = this->newVTable(scriptTraits, object_ivtable, toplevel);

        // ------------------ construct the ScopeTypeChains and ScopeChains

        const ScopeTypeChain* scriptSTC = ScopeTypeChain::createEmpty(gc, scriptTraits);
        ScopeChain* scriptScope = ScriptEnv::createScriptScope(scriptSTC, scriptVTable, abcEnv);

        Traits* object_cstc_types[] = { scriptTraits, object_ctraits, NULL };
        const ScopeTypeChain* object_cstc = ScopeTypeChain::createExplicit(gc, object_ctraits, object_cstc_types);
        ScopeChain* object_cscope = ScopeChain::create(gc, object_cvtable, abcEnv, object_cstc, NULL, dxns);

        Traits* object_istc_types[] = { scriptTraits, object_ctraits, object_itraits, NULL };
        const ScopeTypeChain* object_istc = ScopeTypeChain::createExplicit(gc, object_itraits, object_istc_types);
        ScopeChain* object_iscope = ScopeChain::create(gc, object_ivtable, abcEnv, object_istc, NULL, dxns);

        Traits* class_cstc_types[] = { scriptTraits, object_ctraits, class_ctraits, NULL };
        const ScopeTypeChain* class_cstc = ScopeTypeChain::createExplicit(gc, class_ctraits, class_cstc_types);
        ScopeChain* class_cscope = ScopeChain::create(gc, class_cvtable, abcEnv, class_cstc, NULL, dxns);

        Traits* class_istc_types[] = { scriptTraits, object_ctraits, class_ctraits, class_itraits, NULL };
        const ScopeTypeChain* class_istc = ScopeTypeChain::createExplicit(gc, class_itraits, class_istc_types);
        ScopeChain* class_iscope = ScopeChain::create(gc, class_ivtable, abcEnv, class_istc, NULL, dxns);

        // ------------------ resolve all the vtables
        
        object_ivtable->resolveSignatures(object_iscope);
        class_ivtable->resolveSignatures(class_iscope);
        object_cvtable->resolveSignatures(object_cscope);
        class_cvtable->resolveSignatures(class_cscope);
        scriptVTable->resolveSignatures(scriptScope);

        // ------------------ create Object$ and Class$ instances

        AvmAssert(object_ctraits->getCreateClassClosureProc() == ObjectClass::createClassClosure);
        ClassClosure* object_class = ObjectClass::createClassClosure(object_cvtable);
        object_class->initPrototypeConstructor();

        AvmAssert(class_ctraits->getCreateClassClosureProc() == ClassClass::createClassClosure);
        ClassClosure* class_class = ClassClass::createClassClosure(class_cvtable);
        class_class->initPrototypeConstructor();

        object_class->setDelegate(class_class->prototypePtr());
        class_class->prototypePtr()->setDelegate(object_class->prototypePtr());

        class_class->setDelegate(class_class->prototypePtr());

        // ------------------ create global script instance

        ScriptObject* global = ScriptObject::create(gc, scriptVTable, object_class->prototypePtr());

        // ------------------ fix up the scopechains appropriately

        Atom global_atom = global->atom();

        object_cscope->setScope(gc, 0, global_atom);

        object_iscope->setScope(gc, 0, global_atom);
        object_iscope->setScope(gc, 1, object_class->atom());

        class_cscope->setScope(gc, 0, global_atom);
        class_cscope->setScope(gc, 1, object_class->atom());

        class_iscope->setScope(gc, 0, global_atom);
        class_iscope->setScope(gc, 1, object_class->atom());
        class_iscope->setScope(gc, 2, class_class->atom());

        // ------------------ run the init-methods for Object$ and Class$

        bool deferredInterrupt = false;
#ifdef VMCFG_VERIFYALL
        if (!config.verifyonly && !config.verifyall)
#endif
        {
            Isolate* isolate = getIsolate();
            if (isolate != NULL)
            {
                Isolate::State state = isolate->getAggregate()->queryState(isolate);
                if (state == Isolate::STARTING) 
                {
                    isolate->getAggregate()->stateTransition(isolate, Isolate::RUNNING);
                } 
                else if (isolate->isInterrupted()) 
                {
                    // We'll still run the initialization in interrupted state so that
                    // whe have a properly initialized error object to throw.
                    deferredInterrupt = true;
                }
            }
            
            object_cvtable->init->coerceEnter(object_class->atom());
            class_cvtable->init->coerceEnter(class_class->atom());
        }

        // ------------------ set up the init-method for the global script
        ScriptEnv* main = ScriptEnv::create(gc, scriptTraits->init, scriptScope);
        main->global = global;

        scriptVTable->init = main;
#ifdef VMCFG_AOT
        initScriptActivationTraits(this, toplevel, scriptTraits->init);
#endif

        // ------------------ register the names in the DomainMgr (must be done before calling the init method)

        GCList<ScriptEnv> envs(GetGC(), pool->scriptCount());
        envs.add(main);
        // skip the final one: it's already been done
        for (uint32_t i=0; i < last; i++)
        {
            ScriptEnv* scriptEnv = initOneScript(toplevel, abcEnv, pool->getScriptTraits(i));
            envs.add(scriptEnv);
        }
        this->domainMgr()->addNamedScriptEnvs(abcEnv, envs);

        // ------------------ create the builtin class manifest

        toplevel->init_mainEntryPoint(main, builtinClassManifest::create(main));

        // ------------------ run the init-method for global

#ifdef VMCFG_VERIFYALL
        if (!config.verifyonly && !config.verifyall)
#endif
        {
            main->coerceEnter(global->atom());
        }
  
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=664045
        // Force the built-in error classes to be initialized before any 
        // non-builtin (user) code can be executed. 

#ifdef VMCFG_VERIFYALL
        if (!config.verifyonly && !config.verifyall)
#endif
            (void)toplevel->errorClass();
#endif

#ifdef VMCFG_VERIFYALL
        if (config.verifyall)
        {
            // We have to jump thru some hoops here in an ugly way: 
            // in verifyall mode, the MethodInfo invokers aren't filled in 
            // properly, and require notifyAbcPrepared to be called first.
            // This breaks the sequenced bootstrapping above, but it turns out
            // that we don't need the sequence above in verifyall mode.
            exec->notifyAbcPrepared(toplevel, abcEnv);
            
            object_cvtable->init->coerceEnter(object_class->atom());
            class_cvtable->init->coerceEnter(class_class->atom());
            main->coerceEnter(global->atom());
        }
        else
#endif
        {
            // We don't really need to call this, since the only implementation
            // is a no-op when !config.verifyall... but let's do it anyway, in case
            // additional work is done in the future.
            exec->notifyAbcPrepared(toplevel, abcEnv);
        }

        return toplevel;
    }

    Atom AvmCore::handleActionPool(PoolObject* pool,
                                   Toplevel* toplevel,
                                   CodeContext* codeContext)
    {
        AvmAssert(toplevel != NULL);

        if (pool->scriptCount() == 0)
            toplevel->throwVerifyError(kMissingEntryPointError);

        AvmAssert(codeContext != NULL);
        AvmAssert(codeContext->domainEnv() != NULL);
        AvmAssert(codeContext->domainEnv()->domain() == pool->domain);

        AbcEnv* abcEnv = AbcEnv::create(GetGC(), pool, codeContext);
        ScriptEnv* main = initAllScripts(toplevel, abcEnv);

#ifdef VMCFG_VERIFYALL
        AvmAssert(config.verifyonly || toplevel->objectClass != NULL);
#else
        AvmAssert(toplevel->objectClass != NULL);
#endif

		return callScriptEnvEntryPoint(main);
    }

    PoolObject* AvmCore::parseActionBlock(ScriptBuffer code,
                                          int /*start*/,
                                          Toplevel* toplevel,
                                          Domain* domain,
                                          const NativeInitializer* ninit,
                                          ApiVersion apiVersion,
                                          Stringp name)
    {
        (void)name;
        
        // parse constants and attributes.
        PoolObject* pool = AbcParser::decodeAbc(this,
                                                code,
                                                toplevel,
                                                domain,
                                                ninit,
                                                apiVersion);

        #ifdef DEBUGGER
        if (_debugger)
        {
            _debugger->processAbc(pool, code, name);
        }
        #endif

        return pool;
    }

    ScriptEnv* AvmCore::prepareActionBlock(ScriptBuffer code,
                                          Toplevel* toplevel,
                                          Domain* domain,
                                          CodeContext* codeContext,
                                          ApiVersion apiVersion)
    {
        PoolObject* pool = parseActionBlock(code, 0, toplevel, domain, NULL, apiVersion);
        if (pool->scriptCount() == 0)
        {
            toplevel->throwVerifyError(kMissingEntryPointError);
        }
        AvmAssert(codeContext != NULL);
        AvmAssert(codeContext->domainEnv() != NULL);
        AvmAssert(codeContext->domainEnv()->domain() == pool->domain);

        AbcEnv* abcEnv = AbcEnv::create(GetGC(), pool, codeContext);
        ScriptEnv* entryPoint = initAllScripts(toplevel, abcEnv);
        return entryPoint;
    }

    Atom AvmCore::handleActionBlock(ScriptBuffer code,
                                         int start,
                                         Toplevel* toplevel,
                                         const NativeInitializer* ninit,
                                         CodeContext *codeContext,
                                         ApiVersion apiVersion,
                                         Stringp name)
    {
        AvmAssert(codeContext != NULL);
        AvmAssert(toplevel != NULL);

        Domain* domain = codeContext->domainEnv()->domain();

        // parse constants and attributes.
        PoolObject *pool = parseActionBlock(code,
                                start,
                                toplevel,
                                domain,
                                ninit,
                                apiVersion,
                                name);
        return handleActionPool(pool, toplevel, codeContext);
    }

    int AvmCore::evaluateScriptBuffer(ScriptBuffer& buffer, bool enter_debugger_on_enter)
    {
        (void)buffer;
        (void)enter_debugger_on_enter;
        AvmAssert(false); // override me
        return -1;
    }

#ifdef VMCFG_EVAL
    Atom AvmCore::handleActionSource(String* code,
                                     String* filename,
                                     Toplevel* toplevel,
                                     const NativeInitializer* ninit,
                                     CodeContext *codeContext,
                                     ApiVersion apiVersion)
    {
        AvmAssert(toplevel != NULL);
        ScriptBuffer buffer = avmplus::compileProgram(this, toplevel, code, filename);
        return handleActionBlock(buffer, 0, toplevel, ninit, codeContext, apiVersion);
    }
#endif // VMCFG_EVAL

#ifdef VMCFG_AOT
    void AvmCore::handleAOT(Toplevel* toplevel, CodeContext* codeContext)
    {
        DomainEnv* domainEnv = codeContext->domainEnv();
        Domain* domain = domainEnv->domain();

        for(uint32_t i=0; i<nAOTInfos; i++)
        {
            ScriptBuffer code = ScriptBuffer(new (GetGC()) ReadOnlyScriptBufferImpl(aotInfos[i].abcBytes, aotInfos[i].nABCBytes));
            NativeInitializer ninit(this, NULL, &aotInfos[i], 0, 0);

            PoolObject *userPool = parseActionBlock(code, 0, toplevel, domain, &ninit, getDefaultAPI());

            #ifdef DEBUGGER
            AbcParser::addAOTDebugInfo(userPool);
            #endif

            handleActionPool(userPool, toplevel, codeContext);
        }
    }
#endif // VMCFG_AOT

/*
TODO: UPDATE THIS COMMENT, IT IS NO LONGER TRUE/RELIABLE
True source is the ABC Extensions Spec for float/float4.

11.9.3 The Abstract Equality Comparison Algorithm
The comparison x == y, where x and y are values, produces true or false. Such a comparison is performed as
follows:
E4X - 0a. if x is XMLList, call [[Equals]] method of x with argument y and return
E4X - 0b. if y is XMLList, call [[Equals]] method of y with argument x and return
1. If Type(x) is different from Type(y), go to step 14.
E4X - 1a. If x is XML and y is XML
    if x.class = text/attribute && y.class == text/attribute, do string compare
    else to x.[[Equals]](y)
E4X - 1b If x and y are QName
    return uri=uri and localname==localName
E4X - 1c if x and y are Namespaces
    return uri == uri
2. If Type(x) is Undefined, return true.
3. If Type(x) is Null, return true.
4. If Type(x) is not Number, go to step 11.
5. If x is NaN, return false.
6. If y is NaN, return false.
7. If x is the same number value as y, return true.
8. If x is +0 and y is -0, return true.
9. If x is -0 and y is +0, return true.
10. Return false.
11. If Type(x) is String, then return true if x and y are exactly the same sequence of characters (same length and
same characters in corresponding positions). Otherwise, return false.
12. If Type(x) is Boolean, return true if x and y are both true or both false. Otherwise, return false.
13. Return true if x and y refer to the same object or if they refer to objects joined to each other (section 13.1.2).
Otherwise, return false.

// Different types below here
E4X 14a. if (x is xml) and x.hasSimpleContent = true or (y is xml) and y.hasSimlpeContent = true
    return the results of tostring(x) and tostring(y)

14. If x is null and y is undefined, return true.
15. If x is undefined and y is null, return true.
16. If Type(x) is Number and Type(y) is String,
return the result of the comparison x == ToNumber(y).
17. If Type(x) is String and Type(y) is Number,
return the result of the comparison ToNumber(x) == y.
18. If Type(x) is Boolean, return the result of the comparison ToNumber(x) == y.
19. If Type(y) is Boolean, return the result of the comparison x == ToNumber(y).
20. If Type(x) is either String or Number and Type(y) is Object,
return the result of the comparison x == ToPrimitive(y).
21. If Type(x) is Object and Type(y) is either String or Number,
return the result of the comparison ToPrimitive(x) == y.
22. Return false.
    */

    Atom AvmCore::equals(Atom lhs, Atom rhs)
    {
        if (isNull(lhs)) lhs = 0;
        if (isNull(rhs)) rhs = 0;

        int ltype = (int)atomKind(lhs);
        int rtype = (int)atomKind(rhs);

        // See E4X 11.5.1, pg 53.
        if ((ltype == kObjectType) && (isXMLList(lhs)))
            return atomToXMLList(lhs)->_equals(rhs);
        else if ((rtype == kObjectType) && (isXMLList(rhs)))
            return atomToXMLList (rhs)->_equals(lhs);

        if (ltype == rtype)
        {
            // same type
            switch (ltype)
            {
            case 0:   // kUnusedAtomTag
                return trueAtom;
            case kSpecialBibopType:
                {
#ifdef VMCFG_FLOAT
                    // if either is undefined, return false
                    if (lhs == AtomConstants::undefinedAtom || rhs == AtomConstants::undefinedAtom) 
                        return lhs==rhs ? trueAtom : falseAtom;
                     
                    uint8_t lt = bibopKind(lhs) , rt = bibopKind(rhs) ;
                    if ((lt== kBibopFloatType) && (rt== kBibopFloatType)) 
                    {
                        float l = atomToFloat(lhs);
                        float r = atomToFloat(rhs);
                        //  if either arg is NaN, C++ == returns false, which matches ECMA.
                        return l == r ? trueAtom : falseAtom;
                    }

                    AvmAssertMsg(lt==kBibopFloat4Type || rt == kBibopFloat4Type, "Unhandled bibopKind");
                    float4_decl_v(lhs);
                    float4_decl_v(rhs);
                    return f4_eq_i(lhsv, rhsv) ? trueAtom:falseAtom;
#else
                    return trueAtom;
#endif // VMCFG_FLOAT
                }
            case kStringType:
                if (lhs == rhs) return trueAtom;
                return (*atomToString(lhs) == *atomToString(rhs)) ? trueAtom : falseAtom;
            case kBooleanType:
            case kIntptrType:
                return lhs == rhs ? trueAtom : falseAtom;
            case kNamespaceType:
                // E4X 11.5.1, pg 53
                return atomToNamespace(lhs)->EqualTo(atomToNamespace(rhs))? trueAtom : falseAtom;
            case kObjectType:
            {
                // E4X 11.5.1, pg 53
                if (lhs == rhs)
                    return trueAtom;
                if (isXML(lhs) && isXML(rhs))
                {
                    XMLObject *x = atomToXMLObject (lhs);
                    XMLObject *y = atomToXMLObject (rhs);
                    if ((((x->getClass() & (E4XNode::kText | E4XNode::kCDATA | E4XNode::kAttribute))) && y->hasSimpleContent()) ||
                        (((y->getClass() & (E4XNode::kText | E4XNode::kCDATA | E4XNode::kAttribute))) && x->hasSimpleContent()))
                    {
                        return ((*string(lhs) == *string(rhs)) ? trueAtom : falseAtom);
                    }
                    else
                    {
                        return x->getNode()->_equals(x->toplevel(), this, y->getNode()) ? trueAtom : falseAtom;
                    }
                }
                else if (isQName(lhs) && isQName(rhs))
                {
                    QNameObject *qn1 = atomToQName (lhs);
                    QNameObject *qn2 = atomToQName (rhs);
                    return (((qn1->getURI() == qn2->getURI()) && (qn1->get_localName() == qn2->get_localName()))? trueAtom : falseAtom);
                }
                else
                {
                    return falseAtom;
                }
            }
            case kDoubleType:
                // C++ portability note -- if either arg is NaN, java == returns false, which matches ECMA.
                return atomToDouble(lhs) == atomToDouble(rhs) ? trueAtom : falseAtom;
            }
        }
        else
        {   
            if (isNullOrUndefined(lhs) && isNullOrUndefined(rhs))
                return trueAtom;
#ifdef VMCFG_FLOAT
            if( isFloat4(lhs) || isFloat4(rhs) ){
                if (isNullOrUndefined(lhs) || isNullOrUndefined(rhs))
                    return falseAtom;
                float4_decl_v(lhs);
                float4_decl_v(rhs);
                return f4_eq_i(lhsv, rhsv) ? trueAtom : falseAtom;
            }
#endif
            if (ltype == kIntptrType && rtype == kDoubleType)
                return ((double)atomGetIntptr(lhs)) == atomToDouble(rhs) ? trueAtom : falseAtom;
            if (ltype == kDoubleType && rtype == kIntptrType)
                return atomToDouble(lhs) == ((double)atomGetIntptr(rhs)) ? trueAtom : falseAtom;

#ifdef VMCFG_FLOAT
            if(isNumberOrFloat(lhs) && isNumberOrFloat(rhs))
            {
                AvmAssert(isFloat(lhs) || isFloat(rhs));
                return number(lhs) == number(rhs) ? trueAtom : falseAtom;
            }
#endif 
            // 16. If Type(x) is Numeric and Type(y) is String,
            // return the result of the comparison x == ToNumeric(y).
            // Note: while it's tempting to blindly follow the spec and recursively call equals(),
            // the truth is that ToNumeric(string) == ToNumber(string), and we always end up doing
            // a number-to-number comparison.
            // (important remark: lhs can't be a float4, it would've been already handled)
            if (isNumeric(lhs) && isString(rhs))
            { 
#ifdef VMCFG_FLOAT
                if(isFloat(lhs))
                    return ((double)atomToFloat(lhs)) == number(rhs) ? trueAtom : falseAtom;
                else
#endif // VMCFG_FLOAT
                    return number_d(lhs) == number(rhs) ? trueAtom : falseAtom;
            }

            // 17. If Type(x) is String and Type(y) is Numeric,
            // return the result of the comparison ToNumeric(x) == y.
            // See note on the previous point
            if (isString(lhs) && isNumeric(rhs))
            {
#ifdef VMCFG_FLOAT
                if(isFloat(rhs))
                    return ((double)atomToFloat(rhs)) == number(lhs) ? trueAtom : falseAtom;
                else
#endif // VMCFG_FLOAT
                    return number(lhs) == number_d(rhs) ? trueAtom : falseAtom;
            }

            // E4X 11.5.1, step 4.  Placed slightly lower then in the spec
            // to handle quicker cases earlier.  No cases above should be comparing
            // an object to a non-object
            if (((ltype == kObjectType) && isXML(lhs) && atomToXMLObject(lhs)->hasSimpleContent()) ||
                ((rtype == kObjectType) && isXML(rhs) && atomToXMLObject(rhs)->hasSimpleContent()))
            {
                return ((*string(lhs) == *string(rhs)) ? trueAtom : falseAtom);
            }

            // 18. If Type(x) is Boolean, return the result of the comparison ToNumber(x) == y.
            if (ltype == kBooleanType)
                return equals((lhs&~7)|kIntptrType, rhs);  // equal(toInteger(lhs), rhs)

            // 19. If Type(y) is Boolean, return the result of the comparison x == ToNumber(y).
            if (rtype == kBooleanType)
                return equals(lhs, (rhs&~7)|kIntptrType);  // equal(lhs, toInteger(rhs))

            // 20. If Type(x) is either String or Number and Type(y) is Object,
            // return the result of the comparison x == ToPrimitive(y).

            if ((isString(lhs) || isNumeric(lhs)) && rtype == kObjectType)
                return equals(lhs, atomToScriptObject(rhs)->defaultValue());

            // 21. If Type(x) is Object and Type(y) is either String or Number,
            // return the result of the comparison ToPrimitive(x) == y.
            if ((isString(rhs) || isNumeric(rhs)) && ltype == kObjectType)
                return equals(atomToScriptObject(lhs)->defaultValue(), rhs);
        }
        return falseAtom;
    }

    /**
     * this is the abstract relational comparison algorithm according to ECMA 262 11.8.5
     * @param lhs
     * @param rhs
     * @return trueAtom, falseAtom, or undefinedAtom
     */
#if defined(_MSC_VER) && defined(AVMPLUS_IA32)
    // Force this function to have a frame pointer so the doubles used are 8-byte aligned
    #pragma optimize("y", off)
#endif
    Atom AvmCore::compare(Atom lhs, Atom rhs)
    {
        // fixme - toprimitive must take number hint, so "7" becomes 7
        if (atomIsBothIntptr(lhs, rhs))
        {
            // fast path for integers
            return lhs < rhs ? trueAtom : falseAtom;
        }

        lhs = primitive(lhs);
        rhs = primitive(rhs);

        if (isString(lhs) && isString(rhs))
        {
            // string compare. todo optimize!
            // we already know they are strings, call atomToString() rather than string()
            return *atomToString(lhs) < *atomToString(rhs) ? trueAtom : falseAtom;
        }

        // numeric compare
        // We only do float comparison if both LHS and RHS are floats,
        // but in this case, float and double comparison are identical.
        // Float4s are compared as Numbers (i.e. converted to NaN).
        double     dx = number(lhs);
        double     dy = number(rhs);

        if (MathUtils::isNaN(dx)) return undefinedAtom;
        if (MathUtils::isNaN(dy)) return undefinedAtom;
        return dx < dy ? trueAtom : falseAtom;
    }
#if defined(_MSC_VER) && defined(AVMPLUS_IA32)
    #pragma optimize("", on)
#endif

    Atom AvmCore::stricteq(Atom lhs, Atom rhs)
    {
        if (isNull(lhs)) return isNull(rhs) ? trueAtom : falseAtom;
        if (isNull(rhs)) return falseAtom; // We already know that lhs is not null

        int ltype = atomKind(lhs);
        int rtype = atomKind(rhs);
        if (ltype == rtype)
        {
            // same type
            switch (ltype)
            {
            case kSpecialBibopType:
                if(lhs == undefinedAtom && rhs == undefinedAtom) 
                    return trueAtom;
#ifdef VMCFG_FLOAT
                if(isFloat(lhs) && isFloat(rhs))
                    return atomToFloat(lhs) == atomToFloat(rhs) ? trueAtom : falseAtom;
                if(isFloat4(lhs) && isFloat4(rhs))
                    return f4_eq_i(atomToFloat4(lhs), atomToFloat4(rhs)) ? trueAtom : falseAtom;
#endif // VMCFG_FLOAT
                return falseAtom; // one bibop, other "undefined"; or one float, the other float4.
            case kStringType:
                return (lhs==rhs || *atomToString(lhs) == *atomToString(rhs)) ? trueAtom : falseAtom;
            case kBooleanType:
            case kIntptrType:
            case kNamespaceType:
                return lhs == rhs ? trueAtom : falseAtom;
            case kObjectType:
            {
                if (lhs == rhs)
                    return trueAtom;
                if (isXML(lhs) && isXML(rhs))
                {
                    E4XNode *lhn = atomToXML(lhs);
                    E4XNode *rhn = atomToXML(rhs);
                    return ((lhn == rhn) ? trueAtom : falseAtom);
                }
                return falseAtom;
            }
            case kDoubleType:
                // C++ portability note -- if either arg is NaN, java == returns false, which matches ECMA.
                return atomToDouble(lhs) == atomToDouble(rhs) ? trueAtom : falseAtom;
            }
        }
        // Sometimes ints can hide in double atoms (neg zero for one); also, we may have comparison with float
        else if (isNumber(lhs) && isNumber(rhs))
        {
            return number(lhs) == number(rhs) ? trueAtom : falseAtom;
        }

        return falseAtom;
    }

    /**
     * throwException will throw an exception.  This function
     * never returns.  The exception to be thrown is specified
     * by exception.
     */
    void AvmCore::throwException(Exception *exception)
    {
        #ifdef DEBUGGER
        if (_debugger && !(exception->flags & Exception::SEEN_BY_DEBUGGER))
        {
            // I'm going to set the SEEN_BY_DEBUGGER flag now, before calling
            // filterException(), just to avoid reentrancy problems (we don't
            // want to end up back here with the same Exception object and get
            // stuck in an infinite loop).
            exception->flags |= Exception::SEEN_BY_DEBUGGER;

            bool willBeCaught = willExceptionBeCaught(exception);

            if (passAllExceptionsToDebugger || !willBeCaught)
            {
                // filterException() returns 'true' if it somehow let the user know
                // about the exception, 'false' if it ignored the exception.
                if (_debugger->filterException(exception, willBeCaught))
                    exception->flags |= Exception::SEEN_BY_DEBUGGER;
                else
                    exception->flags &= ~Exception::SEEN_BY_DEBUGGER;
            }
            else
            {
                exception->flags &= ~Exception::SEEN_BY_DEBUGGER;
            }
        }
        #endif

#ifdef VMCFG_TELEMETRY_SAMPLER
        // Send the exception info along with the stack trace over telemetry
        if (!(exception->flags & Exception::SUPPRESS_ERROR_REPORT) && getTelemetry() && getTelemetry()->IsActive()) {
            StringBuffer exceptionStringBuffer(this);

            bool wroteErrString = false;

            if (atomKind(exception->atom) == kObjectType) {
                // string() is not very explanatory for most Error objects.
                // Get the full class name including the namespace instead.
                ScriptObject *so = atomToScriptObject(exception->atom);
                if (so) {
                    so->traits()->print(exceptionStringBuffer, true);
                    wroteErrString = true;
                }
            }

            if (!wroteErrString) {
                // backup, just convert atom to a string, it will say "Error"
                exceptionStringBuffer << StUTF8String(string(exception->atom)).c_str();
            }

            exceptionStringBuffer << '\n';

            // Get the stack trace
            GetStackTrace(exceptionStringBuffer);

            // Send the metric as a string
            TELEMETRY_STRING(getTelemetry(), ".as.exception", exceptionStringBuffer.c_str());
        }
#endif

        // exceptionFrame should not be NULL; if it is,
        // you are missing a TRY/CATCH block around
        // a call to an AVM+ method that throws an
        // exception.
        AvmAssert(exceptionFrame != NULL);
        exceptionFrame->throwException(exception);
    }

    /**
     * throwAtom will throw an exception.  This function
     * never returns.  The exception to be thrown is specified
     * by atom.
     */
    void AvmCore::throwAtom(Atom atom)
    {
        throwException(new (GetGC()) Exception(this, atom));
    }

#ifdef DEBUGGER
    bool AvmCore::willExceptionBeCaught(Exception* exception)
    {
        ExceptionFrame* ef;

        for (ef = exceptionFrame; ef != NULL; ef = ef->prevFrame)
        {
            switch (ef->catchAction)
            {
                // The CATCH block will consume any exception that occurs, and will not treat it as
                // an error, so exceptions should not be reported to the debugger.
                case kCatchAction_Ignore:
                    return true;

                // The CATCH block will rethrow any exception that occurs; so, we will 'continue',
                // which will take us back to the 'for' loop to keep going up the exception stack,
                // until we find a frame with some other value.
                case kCatchAction_Rethrow:
                    continue; // return to the 'for' loop

                // The CATCH block will treat any exception that occurs as an error -- probably by
                // calling uncaughtException, but possibly by some other means.  So, exceptions
                // should be reported to the debugger.
                case kCatchAction_ReportAsError:
                    return false;

                // This is the unfortunate, but rare, case where we can't tell in advance how the
                // CATCH block will handle exceptions.  In this case, we wil not report the
                // exception to the debugger.
                case kCatchAction_Unknown:
                    return true;

                // The CATCH block will walk up the exception frames that were defined in
                // ActionScript code -- e.g. "try { } catch (e:Error) { }" -- looking for one
                // which catch this exception.  So, we will do the same thing, and only report
                // the error to the debugger if there is no catch block for it.
                case kCatchAction_SearchForActionScriptExceptionHandler:
                {
                    CallStackNode* callStackNode;

                    // Walk all the way up the stack, one frame at a time, looking for
                    // one which will catch this exception.
                    for (callStackNode = callStack; callStackNode; callStackNode = callStackNode->next())
                    {
                        MethodInfo* info = callStackNode->info();

#ifdef VMCFG_AOT
                        // bparadie 2009-11-12: In LLVMEmitter exceptfilt will be set to either 0 = catch nothing, -1 = catch anything,
                        // or the pointer of the exception handler. exceptfilt is being passed to abcOP_debugEnter as the 5th parameter
                        // and ends up in callStackNode.m_eip. In other words kCatchAction_SearchForActionScriptExceptionHandler in
                        // AvmCore::willExceptionBeCaught() can return true if callStackNode->eip() != 0.
                        if (info && info->isAotCompiled() && callStackNode->eip() != 0)
                            return true;
#endif
                        // native methods don't have exception handlers
                        if (info && info->isNative())
                            continue;

                        // Note: we only care if the info actually has an exception handler or not...
                        // if WORD_CODE is enabled and this function is jitted, info->word_code_exceptions() will
                        // be null even if we have an exception handler. So always look at info->abc_exceptions()
                        // since it is (currently) always correct.
                        const bool hasExceptionHandler = info && info->abc_exceptions() != NULL;
                        if (hasExceptionHandler && callStackNode->eip() && *callStackNode->eip())
                        {
                            // Check if this particular frame of the callstack
                            // is going to catch the exception.
                            int32_t ordinal; // unused
                            if (findExceptionHandlerNoRethrow(info, *callStackNode->eip(), exception, &ordinal) != NULL)
                                return true;
                        }
                    }

                    // No ActionScript handler was found, so the exception is going to be
                    // re-thrown; so, 'continue' will get us back to the 'for' loop
                    continue;
                }

                default:
                    AvmAssert(false);
                    break;
            }
        }

        return false;
    }

    /**
     * Returns LANG_xx enum
     */
    int AvmCore::determineLanguage()
    {
        if (langID < 0)
        {
            // @todo system probe to determine language, then return one of our known enums
            langID = LANG_en;
        }
        return langID;
    }

#ifndef VMCFG_DEBUGGER_STUB
    String* AvmCore::findErrorMessage(int errorID,
                                      int* mapTable,  /* 2 ints per entry i, i+1 */
                                      const char** errorTable,
                                      int numErrors)
    {
        // Above that, we must binary search.
        int lo = 0;
        int hi = numErrors-1;

        while (lo <= hi) {
            int pivot = (lo+hi)>>1;
            int testID = mapTable[2*pivot];
            if (testID == errorID) {
                lo = pivot;
                break;
            } else if (errorID < testID) {
                hi = pivot-1;
            } else {
                lo = pivot+1;
            }
        }

        int index = mapTable[2*lo+1];
        int ident = mapTable[2*lo];

        if (ident == errorID) {
            return newStringUTF8(errorTable[index]);
        } else {
            return NULL;
        }
    }
#endif
#endif

    String* AvmCore::getErrorMessage(int errorID)
    {
        Stringp buffer = newConstantStringLatin1("Error #");
        buffer = concatStrings(buffer, internInt(errorID));

        #if defined(DEBUGGER) && !defined(VMCFG_DEBUGGER_STUB)
        if (_debugger)
        {
            // errorConstants is declared char* but is encoded as UTF8
            Stringp out = findErrorMessage(errorID,
                                           errorMappingTable,
                                           errorConstants[determineLanguage()],
                                           kNumErrorConstants);
            if (out)
            {
                buffer = concatStrings(buffer, newConstantStringLatin1(": "));
                buffer = concatStrings(buffer, out);
            }
            else
            {
                AvmAssertMsg(0, "errorID not found in the message table; check ErrorConstants.cpp you may need to regenerate it");
            }
        }
        #endif

        return buffer;
    }

    String* AvmCore::atomToErrorString(Atom a)
    {
        String* out = NULL;
    #ifdef DEBUGGER
        StringBuffer sb(this); // 256B gc alloc occurs here.
        sb << asAtom(a);
        out = sb.toString();
    #else
        (void)a;
        out = kEmptyString;
    #endif /* DEBUGGER */
        return out;
    }

    String* AvmCore::toErrorString(int d)
    {
    #ifdef DEBUGGER
        return MathUtils::convertIntegerToStringBase10(this, d, MathUtils::kTreatAsSigned);
    #else
        (void)d;
        return kEmptyString;
    #endif /* DEBUGGER */
    }

    String* AvmCore::toErrorString(const char* s)
    {
        String* out = NULL;
    #ifdef DEBUGGER
        if (s)
            out = this->newStringUTF8(s);
        else
            out = kEmptyString;
    #else
        out = kEmptyString;
        (void)s;
    #endif /* DEBUGGER */
        return out;
    }

    String* AvmCore::toErrorString(const wchar* s)
    {
        String* out = NULL;
    #ifdef DEBUGGER
        if (s)
            out = this->newStringUTF16(s);
        else
            out = kEmptyString;
    #else
        out = kEmptyString;
        (void)s;
    #endif /* DEBUGGER */
        return out;
    }

    String* AvmCore::toErrorString(MethodInfo* m)
    {
        String* s = NULL;
    #ifdef DEBUGGER
        if (m) {
            StringBuffer sb(this); // 256B gc alloc occurs here.
            m->print(sb);
            s = sb.toString();
        } else {
            s = kEmptyString;
        }
    #else
        s = kEmptyString;
        (void)m;
    #endif /* DEBUGGER */
        return s;
    }

    String* AvmCore::toErrorString(const Multiname& n)
    {
        return toErrorString(&n);
    }

    String* AvmCore::toErrorString(const Multiname* n)
    {
        String* s = NULL;
    #ifdef DEBUGGER
        if (n) {
            StringBuffer sb(this); // 256B gc alloc occurs here.
            sb << Multiname::FormatNameOnly(n);
            s = sb.toString();
        } else {
            s = kEmptyString;
        }
    #else
        s = kEmptyString;
        (void)n;
    #endif /* DEBUGGER */
        return s;
    }

    String* AvmCore::toErrorString(Namespacep ns)
    {
        String* s = NULL;
    #ifdef DEBUGGER
        if (ns) {
            StringBuffer sb(this); // 256B gc alloc occurs here.
            ns->print(sb);
            s = sb.toString();
        } else {
            s = kEmptyString;
        }
    #else
        s = kEmptyString;
        (void)ns;
    #endif /* DEBUGGER */
        return s;
    }

    String* AvmCore::toErrorString(const Traits* t)
    {
        #ifndef DEBUGGER
        (void)t;
        return kEmptyString;
        #else
        if (!t)
        {
            return newConstantStringLatin1("*");
        }

        StringBuffer sb(this); // 256B gc alloc occurs here.
        if (t->base == traits.class_itraits)
        {
            t = t->itraits;
            sb << "class ";
        }

        Namespacep ns = t->ns();
        if (ns != NULL && !ns->getURI()->isEmpty())
            sb << ns << ".";

        Stringp n = t->name();
        if (n)
            sb << n;
        else
            sb << "(null)";

        Stringp s = sb.toString();
        return s;
        #endif /* DEBUGGER */
    }

    String* AvmCore::formatErrorMessageV( int errorID, Stringp arg1, Stringp arg2, Stringp arg3)
    {
        Stringp out = NULL;

        Stringp errorMessage = getErrorMessage(errorID);
        if (errorMessage)
        {
            #ifdef DEBUGGER
            StUTF8String errorUTF8(errorMessage);
            const char *format = errorUTF8.c_str();

            // This block is enclosed in {} to force
            // StringBuffer destructor to unwind.
            {
                StringBuffer buffer(this);
                buffer.formatP( format, arg1, arg2, arg3);
                out = buffer.toString();
            }
            #else
            /**
             * We can skip all the above processing since getErrorMessage() only
             * returns the error code.  Likewise the toErrorString() methods return kEmptyString
             */
            (void)arg1;
            (void)arg2;
            (void)arg3;
            out = errorMessage;
            #endif /* DEBUGGER*/
        }
        else
        {
            AvmAssertMsg(errorMessage != NULL, "contract with getErrorMessage() broken, we should always get a string!");
            out = kEmptyString;
        }
        return out;
    }

    void AvmCore::throwErrorV(ClassClosure *type, int errorID, Stringp arg1, Stringp arg2, Stringp arg3)
    {
        AvmAssertMsg(type != NULL, "type should never be NULL - internal bootstrap error if it is.");

        Stringp out = formatErrorMessageV( errorID, arg1, arg2, arg3);
        Atom args[3] = { nullObjectAtom, out->atom(), intToAtom(errorID) };
        throwAtom(type->construct(2, args));
    }

    /*static*/ Atom AvmCore::booleanAtom(Atom atom)
    {
        return boolean(atom) == 0 ? falseAtom : trueAtom;
    }

    /*static*/ int AvmCore::boolean(Atom atom)
    {
        if (!AvmCore::isNullOrUndefined(atom))
        {
            switch (atomKind(atom))
            {
            case kIntptrType:
                return atomGetIntptr(atom) != 0;
            case kDoubleType: {
                double d = atomToDouble(atom);
                return !MathUtils::isNaN(d) && d != 0.0;
            }
            case kBooleanType:
                return (atom & ~7) != 0;
            case kStringType:
                return !isNull(atom) && atomToString(atom)->length() > 0;
#ifdef VMCFG_FLOAT
            case kSpecialBibopType:
                {   /* we already know it's not "undefined"! */
                    if( atom == undefinedAtom )
                        return 0;
                    uint8_t bKind = bibopKind(atom);
                    if(bKind == kBibopFloatType)
                    {
                        float f = atomToFloat(atom);
                        return f != 0.0 && !MathUtils::isNaNf(f) ;
                    }
                    else if(bKind == kBibopFloat4Type)
                    {
                        return 0;
                    }
                    else {
                        AvmAssertMsg(false, "Unhandled bibop kind in ToBoolean()!");
                        return 0;
                    }
                }
#endif // VMCFG_FLOAT
            default:
                // kObjectType, kNamespaceType
                return !isNull(atom);
            }
        }
        else
        {
            return false;
        }
    }

    /*
        ToPrimitive:

        Return a default value for the Object. The default value of an object is
        retrieved by calling the internal [[DefaultValue]] method of the object,
        passing the optional hint PreferredType. The behaviour of the
        [[DefaultValue]] method is defined by this specification for all native
        ECMAScript objects (section 8.6.2.6).
    */
    /*static*/ Atom AvmCore::primitive(Atom atom)
    {
        return isObject(atom) ? atomToScriptObject(atom)->defaultValue() : atom;
    }

    Atom AvmCore::numberAtom(Atom atom)
    {
        return numericAtomImpl<true>(atom);
    }
    Atom AvmCore::numericAtom(Atom atom)
    {
        return numericAtomImpl<false>(atom);
    }

    template<bool newTypes2Double> Atom AvmCore::numericAtomImpl(Atom atom )
    {
        if (!isNull(atom))
        {
            switch (atomKind(atom))
            {
            case kSpecialBibopType:
#ifdef VMCFG_FLOAT
                if(atom == undefinedAtom)
                    return kNaN;
                if(!newTypes2Double) 
                    return atom; // Float or Float4; stays unchanged in toNumeric
                if(isFloat(atom))
                    return doubleToAtom(atomToFloat(atom));
                AvmAssert(isFloat4(atom));
#endif // VMCFG_FLOAT
                return kNaN;
            case kStringType:
                return doubleToAtom(atomToString(atom)->toNumber());
            default:
                AvmAssert(false);
            case kBooleanType:
                return (atom&~7) | kIntptrType;
            case kDoubleType:
            case kIntptrType:
                return atom;
            case kNamespaceType:
                // return ToNumber(namespace->uri)
                return numberAtom(atomToNamespace(atom)->getURI()->atom());
            case kObjectType:
                return numericAtomImpl<newTypes2Double>(
                        atomToScriptObject(atom)->defaultValue());
            }
        }
        return zeroIntAtom;
    }

    /*static*/ double AvmCore::number(Atom atom)
    {
        for (;;)
        {
            const int kind = atomKind(atom);

            // kIntptrType is by far the most common
            if (kind == kIntptrType)
                return (double) atomGetIntptr(atom);

            // kDoubleType is next most common
            if (kind == kDoubleType)
                return atomToDouble(atom);

            if (AvmCore::isNull(atom))
                return 0.0;

            // all other cases are relatively rare
            switch (kind)
            {
            case kStringType:
                return atomToString(atom)->toNumber();
            default:
                AvmAssert(false);
                // should never get here, but fall through to NaN if we do.
            case kSpecialBibopType:
#ifdef VMCFG_FLOAT
                if (atom == undefinedAtom)
                    return MathUtils::kNaN;
                if (bibopKind(atom) == kBibopFloatType)
                    return atomToFloat(atom);
                AvmAssert(isFloat4(atom));
#endif
                return MathUtils::kNaN;
           case kBooleanType:
                return atom == trueAtom ? 1.0 : 0.0;
            case kNamespaceType:
                atom = atomToNamespace(atom)->getURI()->atom();
                break;  // continue loop, effectively a tailcall
            case kObjectType:
                atom = AvmCore::atomToScriptObject(atom)->defaultValue();
                break;  // continue loop, effectively a tailcall
            }
        }

        //AvmAssert(0); // can't get here
        //return 0.0;
    }

    Stringp AvmCore::intern(Atom atom)
    {
        if (!isNull(atom))
        {
            switch (atomKind(atom))
            {
            case kBooleanType:
                return booleanStrings[atom>>3];
            case kStringType:
                return internString(atom);
            case kNamespaceType:
                return atomToNamespace(atom)->getURI();
            case kSpecialBibopType:
#ifdef VMCFG_FLOAT
                if(atom==undefinedAtom)
                   return kundefined;
                if (bibopKind(atom)==kBibopFloatType)
                    return internFloat(atomToFloat(atom));
                if (bibopKind(atom)==kBibopFloat4Type)
                    return internFloat4(atomToFloat4(atom));
                AvmAssert(!"Unknown bibopKind");
#endif // VMCFG_FLOAT
                return kundefined;
            case kObjectType:
                return internString(atomToScriptObject(atom)->toString());
            case kIntptrType:
#ifdef AVMPLUS_64BIT
                return atomCanBeInt32(atom) ?
                        internInt((int32_t)atomGetIntptr(atom)) :
                        internDouble((double)atomGetIntptr(atom));
#else
                // atomCanBeInt32() is always true for 32-bit;
                // use explicit ifdef to avoid compiler warnings.
                return internInt((int32_t)atomGetIntptr(atom));
#endif
            case kDoubleType:
            default: // number
                return internDouble(atomToDouble(atom));
            }
        }
        else
        {
            return knull;
        }
    }

    Namespacep AvmCore::internNamespace(Namespacep ns)
    {
        if (ns->isPrivate())
        {
            // only intern namespaces with interned uri's.  this means anonymous
            // namespaces with null prefixes are always unique and can't be shared.
            return ns;
        }

        int i = findNamespace(ns);
        if (namespaces[i] == NULL)
        {
            // first time we've seen this namespace.  intern it
            nsCount++;
            namespaces[i] = ns;
            return ns;
        }

        // found the namespace, indexing by URI. return the interned copy.
        return namespaces[i];
    }

    Namespacep AvmCore::cloneNamespace(Namespacep ns)
    {
        // FIXME things get interesting if ns is from a different AvmCore 
        // and the core starts moving string internals (e.g., it's dynamicizing)
        Stringp prefix = ns->hasPrefix() ? internString(AvmCore::atomToString(ns->getPrefix())) : NULL;
        Stringp uri = ns->getURI();
        if (uri != NULL) {
            uri = internString(uri->clone(this));
        }
        return Namespace::create(gc, prefix ? prefix->toAtom() : undefinedAtom, uri, ns->getType(), ns->getApiVersion());
    }



#ifdef AVMPLUS_VERBOSE
    /* static */
    void AvmCore::formatMultiname(PrintWriter& out, uint32_t index, PoolObject* pool)
    {
        if (index > 0 && index < pool->cpool_mn_offsets.length())
        {
            Multiname name;
            pool->parseMultiname(name, index);
            out << name;
        }
        else
        {
            out << "invalid multiname index " << (uint32_t)index;
        }
    }

    void AvmCore::formatOpcode(PrintWriter& buffer, const uint8_t *pc, const uint8_t *code_end, AbcOpcode opcode, ptrdiff_t off, PoolObject* pool)
    {
        pc++;
        switch (opcode)
        {
            case OP_debugfile:
            case OP_pushstring:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->constantStringCount)
                {
                    buffer << " \"" << pool->getString(index) << "\"";
                }
                else
                {
                    buffer << " invalid string index=" << index;
                }
                break;
            }
            case OP_pushbyte:
                buffer << opcodeInfo[opcode].name << " " << int(int8_t(*pc));
                break;
            case OP_pushint:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->cpool_int.length())
                {
                    buffer << " " << pool->cpool_int[index];
                }
                else
                {
                    buffer << " invalid index=" << index;
                }
                break;
            }
            case OP_pushuint:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->cpool_uint.length())
                {
                    buffer << " " << (double)pool->cpool_uint[index];
                }
                else
                {
                    buffer  << " invalid index=" << index;
                }
                break;
            }
            case OP_pushdouble:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->cpool_double.length())
                {
                    buffer << " " << pool->cpool_double[index]->value;
                }
                else
                {
                    buffer << " invalid_index=" << index;
                }
                break;
            }
#ifdef VMCFG_FLOAT
            case OP_pushfloat:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->cpool_float.length())
                {
                    buffer << " " << pool->cpool_float[index]->value;
                }
                else
                {
                    buffer << " invalid_index=" << index;
                }
                break;
            }
            case OP_pushfloat4:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->cpool_float4.length())
                {
                    const float* pf4 = (const float*) pool->cpool_float4[index];
                    buffer << " " << pf4[0] << "," << pf4[1] << "," << pf4[2] << "," << pf4[3] ;
                }
                else
                {
                    buffer << " invalid_index=" << index;
                }
                break;
            }
#endif // VMCFG_FLOAT
            case OP_pushnamespace:
            {
                buffer << opcodeInfo[opcode].name;
                uint32_t index = readU32(pc);
                if (index > 0 && index < pool->cpool_ns.length())
                {
                    buffer << " " << pool->cpool_ns[index]->getURI();
                }
                else
                {
                    buffer << " invalid index=" << index;
                }
                break;
            }
            case OP_getsuper:
            case OP_setsuper:
            case OP_getproperty:
            case OP_setproperty:
            case OP_initproperty:
            case OP_findpropstrict:
            case OP_findproperty:
            case OP_finddef:
            case OP_deleteproperty:
            case OP_istype:
            case OP_coerce:
            case OP_astype:
            {
                buffer << opcodeInfo[opcode].name << " ";
                formatMultiname(buffer, readU32(pc), pool);
                break;
            }
            case OP_callproperty:
            case OP_callpropvoid:
            case OP_callproplex:
            case OP_callsuper:
            case OP_callsupervoid:
            {
                uint32_t index = readU32(pc);
                int argc = readU32(pc);
                buffer << opcodeInfo[opcode].name << " ";
                formatMultiname(buffer, index, pool);
                buffer << " " << argc;
                break;
            }
            case OP_callstatic:
            case OP_newfunction:
            {
                uint32_t method_id = readU32(pc);
                buffer << opcodeInfo[opcode].name;
                if (method_id >= pool->methodCount())
                    buffer << " invalid";
                buffer << " method_id=" << method_id;
                if (opcode == OP_callstatic)
                {
                    buffer << " argc=" << (int)readU32(pc); // argc
                }
                if (method_id < pool->methodCount())
                {
                    Stringp fname = pool->getMethodInfo(method_id)->getMethodName();
                    if (fname)
                        buffer << " " << fname;
                    else
                        buffer << " null";
                }
                break;
            }

            case OP_newclass:
            {
                uint32_t id = readU32(pc);
                if (id < pool->classCount()) {
                    Traits* c = pool->getClassTraits(id);
                    buffer << opcodeInfo[opcode].name << " " << c;
                }
                break;
            }
            case OP_lookupswitch:
            {
                ptrdiff_t target = off + readS24(pc);
                pc += 3;
                // maxindex is really a U30, so strip off the two hi bits
                // (don't bother trying to throw an error, Verifier will handle that)
                uint32_t maxindex = readU32(pc) & 0x3FFFFFF;
                buffer << opcodeInfo[opcode].name << " default:" << (int)target << " maxcase:"<<maxindex;
                for (uint32_t i=0; i <= maxindex; i++)
                {
                    if (pc > code_end)
                    {
                        buffer << " invalid";
                        break;
                    }
                    target = off + readS24(pc);
                    pc += 3;
                    buffer << " " << (int)target;
                }
                break;
            }

            case OP_ifnlt:
            case OP_ifnle:
            case OP_ifngt:
            case OP_ifnge:
            case OP_jump:
            case OP_iftrue:
            case OP_iffalse:
            case OP_ifeq:
            case OP_ifge:
            case OP_ifgt:
            case OP_ifle:
            case OP_iflt:
            case OP_ifne:
            case OP_ifstricteq:
            case OP_ifstrictne:
            {
                int imm24 = 0, imm8 = 0;
                unsigned int imm30 = 0, imm30b = 0;
                const uint8_t* p2 = pc-1;
                readOperands(p2, imm30, imm24, imm30b, imm8);
                int insWidth = (int)(p2-pc);

                ptrdiff_t target = off + insWidth + imm24 + 1;
                buffer << opcodeInfo[opcode].name << " " << (double)target;
                break;
            }
            default:
                switch (opcodeInfo[opcode].operandCount) {
                default:
                    buffer << opcodeInfo[opcode].name;
                    break;
                case 1:
                {
                    buffer << opcodeInfo[opcode].name
                    << ' '
                    << (int)readU32(pc);
                }
                    break;
                case 2:
                {
                    int first = readU32(pc);
                    int second = readU32(pc);
                    buffer << opcodeInfo[opcode].name
                    << ' '
                    << first
                    << ' '
                    << second;
                }
                    break;
            }
        }
    }

#ifdef VMCFG_WORDCODE
    void AvmCore::formatBits(PrintWriter& buffer, uint32_t bits)
    {
        Atom a = (Atom)(intptr_t)(int32_t)bits;
        if (isUndefined(a))
            buffer << "undefined";
        else if (isBoolean(a))
            buffer << (boolean(a) ? "true" : "false");
        else if (atomIsIntptr(a))
            buffer << (double)atomGetIntptr(a);
        else
            buffer << "[unknown: " << bits << "]";
    }

    void AvmCore::formatOpcode(PrintWriter& buffer, const uintptr_t *pc, const uintptr_t *code_end, WordOpcode opcode, ptrdiff_t off, PoolObject* pool)
    {
        pc++;
        switch (opcode)
        {
            case WOP_debugfile:
            case WOP_pushstring: {
                buffer << wopAttrs[opcode].name;
                uint32_t index = (uint32_t)*pc++;
                if (index > 0 && index < pool->constantStringCount)
                    buffer << " \"" << pool->getString(index) << "\"";
                else
                    buffer << " OUT OF RANGE: " << index;
                break;
            }

            case WOP_pushbits: {
                uint32_t bits = (uint32_t)*pc++;
                buffer << wopAttrs[opcode].name << " ";
                formatBits(buffer, bits);
                break;
            }

            case WOP_push_doublebits: {
                double_overlay d;
                d.bits32[0] = (uint32_t)*pc++;
                d.bits32[1] = (uint32_t)*pc++;
                buffer << wopAttrs[opcode].name << " " << d.value;
                break;
            }

            case WOP_pushdouble: {
                buffer << wopAttrs[opcode].name;
                uint32_t index = (uint32_t)*pc++;
                if (index < pool->cpool_double.length())
                    buffer << " " << pool->cpool_double[index]->value;
                else
                    buffer << " OUT OF RANGE: " << index;
                break;
            }

            case WOP_pushnamespace: {
                buffer << wopAttrs[opcode].name;
                uint32_t index = (uint32_t)*pc++;
                if (index > 0 && index < pool->cpool_ns.length())
                    buffer << " " << pool->cpool_ns[index]->getURI();
                else
                    buffer << " OUT OF RANGE: " << index;
                break;
            }

            case WOP_findpropglobal:
            case WOP_findpropglobalstrict: {
                buffer << wopAttrs[opcode].name << " ";
                formatMultiname(buffer, (uint32_t)*pc++, pool);
                buffer << " " << (uint32_t)*pc++;
                break;
            }

            case WOP_getsuper:
            case WOP_setsuper:
            case WOP_getproperty:
            case WOP_setproperty:
            case WOP_initproperty:
            case WOP_findpropstrict:
            case WOP_findproperty:
            case WOP_finddef:
            case WOP_deleteproperty:
            case WOP_istype:
            case WOP_coerce:
            case WOP_astype: {
                buffer << wopAttrs[opcode].name << " ";
                formatMultiname(buffer, (uint32_t)*pc++, pool);
                break;
            }

            case WOP_callproperty:
            case WOP_callpropvoid:
            case WOP_callproplex:
            case WOP_callsuper:
            case WOP_callsupervoid: {
                uint32_t index = (uint32_t)*pc++;
                int argc = (int)*pc++;
                buffer << wopAttrs[opcode].name << " ";
                formatMultiname(buffer, index, pool);
                buffer << " " << argc;
                break;
            }

            case WOP_callstatic:
            case WOP_newfunction: {
                uint32_t method_id = (uint32_t)*pc++;
                buffer << wopAttrs[opcode].name;
                if (method_id < pool->methodCount())
                    buffer << " invalid";
                buffer << " method_id=" << method_id;
                if (opcode == WOP_callstatic)
                    buffer << " argc=" << (uint32_t)*pc++; // argc
                if (method_id < pool->methodCount())
                    buffer << " " << pool->getMethodInfo(method_id)->getMethodName();
                break;
            }

            case WOP_newclass: {
                uint32_t id = (uint32_t)*pc++;
                if (id < pool->classCount())
                {
                    Traits* c = pool->getClassTraits(id);
                    buffer << wopAttrs[opcode].name << " " << c;
                }
                else
                {
                    buffer << wopAttrs[opcode].name << " invalid class index " << id;
                }
                break;
            }

            case WOP_lookupswitch: {
                ptrdiff_t target = off + *pc++;
                uint32_t maxindex = (uint32_t)*pc++;
                buffer << wopAttrs[opcode].name << " default:" << (int)target << " maxcase:"<<maxindex;
                for (uint32_t i=0; i <= maxindex; i++)
                {
                    if (pc > code_end)
                    {
                        buffer << " invalid";
                        break;
                    }
                    target = off + *pc++;
                    buffer << " " << (int)target;
                }
                break;
            }

            case WOP_ifnlt:
            case WOP_ifnle:
            case WOP_ifngt:
            case WOP_ifnge:
            case WOP_jump:
            case WOP_iftrue:
            case WOP_iffalse:
            case WOP_ifeq:
            case WOP_ifge:
            case WOP_ifgt:
            case WOP_ifle:
            case WOP_iflt:
            case WOP_ifne:
            case WOP_ifstricteq:
            case WOP_ifstrictne: {
                int offset = (int)*pc++;
                buffer << wopAttrs[opcode].name << " " << (uint32_t)(off + 2 + offset);
                break;
            }

#ifdef VMCFG_WORDCODE_PEEPHOLE
            case WOP_subtract_lb:
            case WOP_multiply_lb:
            case WOP_divide_lb:
            case WOP_bitand_lb:
            case WOP_bitor_lb:
            case WOP_bitxor_lb: {
                uint32_t r1 = (uint32_t)*pc++;
                uint32_t b1 = (uint32_t)*pc++;
                buffer << wopAttrs[opcode].name << " " << r1 << " ";
                formatBits(buffer, b1);
                break;
            }

            case WOP_iflt_ll:
            case WOP_ifnlt_ll:
            case WOP_ifle_ll:
            case WOP_ifnle_ll:
            case WOP_ifgt_ll:
            case WOP_ifngt_ll:
            case WOP_ifge_ll:
            case WOP_ifnge_ll:
            case WOP_ifeq_ll:
            case WOP_ifne_ll:
            case WOP_ifstricteq_ll:
            case WOP_ifstrictne_ll: {
                uint32_t r1 = (uint32_t)*pc++;
                uint32_t r2 = (uint32_t)*pc++;
                int offset = (int)*pc++;
                buffer << wopAttrs[opcode].name << " " << r1 << " " << r2 << " " << (uint32_t)(off + 4 + offset);
                break;
            }

            case WOP_iflt_lb:
            case WOP_ifnlt_lb:
            case WOP_ifle_lb:
            case WOP_ifnle_lb:
            case WOP_ifgt_lb:
            case WOP_ifngt_lb:
            case WOP_ifge_lb:
            case WOP_ifnge_lb:
            case WOP_ifeq_lb:
            case WOP_ifne_lb:
            case WOP_ifstricteq_lb:
            case WOP_ifstrictne_lb: {
                uint32_t r1 = (uint32_t)*pc++;
                uint32_t b1 = (uint32_t)*pc++;
                int offset = (int)*pc++;
                buffer << wopAttrs[opcode].name << " " << r1 << " ";
                formatBits(buffer, b1);
                buffer << " " << (uint32_t)(off + 4 + offset);
                break;
            }
#endif // VMCFG_WORDCODE_PEEPHOLE

            default:
                switch (wopAttrs[opcode].width) {
                case 0: {
                    buffer << "UNKNOWN: " << (uint32_t)opcode;
                    break;
                }
                default:
                    buffer << wopAttrs[opcode].name;
                    break;
                case 2:
                {
                    buffer << wopAttrs[opcode].name
                    << ' '
                    << (int)*pc++;
                    break;
                }
                case 3:
                {
                    int first = (int)*pc++;
                    int second = (int)*pc++;
                    buffer << wopAttrs[opcode].name
                    << ' '
                    << first
                    << ' '
                    << second;
                    break;
                }
            }
        }
    }
#endif // VMCFG_WORDCODE
#endif // AVMPLUS_VERBOSE

    ExceptionHandler* AvmCore::findExceptionHandler(MethodInfo *info,
                                                    intptr_t pc,
                                                    Exception *exception)
    {
        int32_t ordinal; // unused
        ExceptionHandler* handler = findExceptionHandlerNoRethrow(info, pc, exception, &ordinal);
        if (handler)
            return handler;

        // We don't have a matching exception.
        throwException(exception);
        return NULL;// not reached
    }

    ExceptionHandler* AvmCore::findExceptionHandlerNoRethrow(MethodInfo *info,
                                                             intptr_t pc,
                                                             Exception *exception,
                                                             int32_t *ordinal)
    {
        // If this exception is an EXIT_EXCEPTION, it cannot
        // be caught by AS code.  Exit immediately.
        if (exception->flags & Exception::EXIT_EXCEPTION)
        {
            *ordinal = -1;
            return NULL;
        }

        // Search the exception table for a catch clause
        // such that pc is between "from" and "to" and
        // the thrown atom matches the required type.

        //[ed] we only call this from methods with catch blocks, when exceptions != NULL
        AvmAssert(info->abc_exceptions() != NULL);
#ifdef VMCFG_WORDCODE
        // This is hacky and will go away.  If the target method was not jitted, use
        // word_code.exceptions, otherwise use info->exceptions.  methods may or may
        // not be JITted based on memory, configuration, or heuristics.

        ExceptionHandlerTable* exceptions;
        if (info->isInterpreted())
            exceptions = info->word_code_exceptions();
        else
            exceptions = info->abc_exceptions();
        AvmAssert(exceptions != NULL);
#else
        ExceptionHandlerTable* exceptions = info->abc_exceptions();
#endif

        int exception_count = exceptions->exception_count;
        Atom atom = exception->atom;

        for (int i = 0; i < exception_count; i++)
        {
            ExceptionHandler* handler = &exceptions->exceptions[i];

            // verifier makes sure type is valid, resolves to Traits*
            if (pc >= handler->from && pc < handler->to && istype(atom, handler->traits))
            {
                #ifdef AVMPLUS_VERBOSE
                if (isVerbose((uint32_t)~0)) // any verbose flag enabled we emit this...
                {
                    console << "enter " << info << " catch " << handler->traits << '\n';
                }
                #endif // DEBUGGER

                *ordinal = i;
                return handler;
            }
        }

        // We don't have a matching exception.
        *ordinal = -1;
        return NULL;
    }

    Atom AvmCore::increment_number_d(Atom a, int delta)
    {
        // Argument must be a number atom.  Float and Float4 are not legal.
        AvmAssert(isNumber(a));
        AvmAssert(delta == 1 || delta == -1);

        if (atomIsIntptr(a)) {
            // If the value is small enough for kIntptrType representation,
            // the result of an intptr_t addition is guaranteed not to overflow.
            intptr_t result = atomGetIntptr(a) + delta;
            if (atomIsValidIntptrValue(result))
                return atomFromIntptrValue(result);
            else
                return doubleToAtom(double(result));
        } else
            return doubleToAtom(atomToDouble(a) + delta);
    }

    // The result is always in the int32 range, wrapping if needed.
    Atom AvmCore::increment_i(Atom a, int delta)
    {
        switch (atomKind(a))
        {
        case kBooleanType:
            return intToAtom(int32_t((intptr_t)a >> 3) + int32_t(delta));
        case kIntptrType:
            return intToAtom(int32_t(atomGetIntptr(a)) + int32_t(delta));
        case kDoubleType:
            return intToAtom(int32_t(atomToDouble(a)) + int32_t(delta));
#ifdef VMCFG_FLOAT
        case kSpecialBibopType:
            if (isFloat(a))
                return intToAtom(int32_t(atomToFloat(a)) + int32_t(delta));
            // fall thru for the other cases of bibopType
#endif
        default:
            return intToAtom(integer(a) + int32_t(delta));
        }
    }

    // check for the easy cases with bitmasks
    /*static*/ const int AvmCore::k_atomDoesNotNeedCoerce_Masks[8] =
    {
        (1<<BUILTIN_null),                                                                  // kUnusedAtomTag -- recycle for null checking
        (1<<BUILTIN_object),                                                                // kObjectType
        (1<<BUILTIN_string) | (1<<BUILTIN_object),                                          // kStringType
        (1<<BUILTIN_namespace) | (1<<BUILTIN_object),                                       // kNamespaceType
        IFFLOAT(0, (1<<BUILTIN_void)),                                                      // kSpecialBibopType
        (1<<BUILTIN_boolean) | (1<<BUILTIN_object),                                         // kBooleanType
        (1<<BUILTIN_number) | (1<<BUILTIN_object),                                          // kIntptrType
        (1<<BUILTIN_number) | (1<<BUILTIN_object)                                           // kDoubleType
    };

    /*static*/ bool AvmCore::istype(Atom atom, Traits* itraits)
    {
        if (!itraits)
            return true;

        const int bt = itraits->builtinType;

        // conceptually, this is atomDoesNotNeedCoerce() inlined, so we don't have to
        // re-grab bt and kind

        // cheat and use "kUnusedAtomTag" for all null values (streamlines the test)
        AvmAssert(atomKind(atom) != kUnusedAtomTag);
        const int kind = isNull(atom) ? kUnusedAtomTag : atomKind(atom);

        if ((1<<bt) & k_atomDoesNotNeedCoerce_Masks[kind])
            return true;

        // repeated if-else is better than switch here
        if (kind == kObjectType)
        {
            return atomToScriptObject(atom)->traits()->subtypeof(itraits);
        }

        if (kind == kIntptrType)
        {
            // ISSUE need special support for number value ranges
            if (bt == BUILTIN_uint)
            {
                return atomCanBeUint32(atom);
            }
            if (bt == BUILTIN_int)
            {
                return atomCanBeInt32(atom);
            }
        }

        if (kind == kDoubleType)
        {
            // ISSUE there must be a better way...
            if (bt == BUILTIN_int)
            {
                const double d = atomToDouble(atom);
                const int32_t i = int32_t(d);
                return d == (double)i;
            }
            if (bt == BUILTIN_uint)
            {
                const double d = atomToDouble(atom);
                const uint32_t u = (uint32_t)d;
                return d == (double)u;
            }
        }

#ifdef VMCFG_FLOAT
        if(kind == kSpecialBibopType){
            if(atom==undefinedAtom)
            {
                AvmAssert(bt!=BUILTIN_void);
                return false;
            }
            uint8_t bk = bibopKind(atom);
            if( bk == kBibopFloatType )
                return ( bt == BUILTIN_float || bt==BUILTIN_object ) ;

            AvmAssert( bk == kBibopFloat4Type );
            return ( bt == BUILTIN_float4 || bt==BUILTIN_object ) ;
        }
#endif

        return false;
    }

    Stringp AvmCore::coerce_s(Atom atom)
    {
        if (isNullOrUndefined(atom))
            return NULL;
        return string(atom);
    }

    Stringp AvmCore::string(Atom atom)
    {
        if (!isNull(atom))
        {
            switch (atomKind(atom))
            {
            case kNamespaceType:
                return atomToNamespace(atom)->getURI();
            case kObjectType:
                return atomToScriptObject(atom)->toString();
            case kStringType:
                return atomToString(atom);
            case kSpecialBibopType:
#ifdef VMCFG_FLOAT
                if(atom == undefinedAtom)
                    return kundefined;
                if(isFloat(atom))
                    return floatToString(atomToFloat(atom));
                else 
                {
                    AvmAssert(isFloat4(atom));
                    return float4ToString(atomToFloat4(atom));
                }
#else
                return kundefined;
#endif // VMCFG_FLOAT
            case kBooleanType:
                return booleanStrings[atom>>3];
            case kIntptrType: {
                return MathUtils::convertIntegerToStringRadix(this, atomGetIntptr(atom), 10, MathUtils::kTreatAsSigned);
            }
            case kDoubleType:
            default: // number
                return doubleToString(atomToDouble(atom));
            }
        }
        else
        {
            return knull;
        }
    }

    void AvmCore::setConsoleStream(GCOutputStream *stream)
    {
        console.setOutputStream(stream);
    }

    void AvmCore::setConsoleStream(NonGCOutputStream *stream)
    {
        console.setOutputStream(stream);
    }

    /*static*/ bool AvmCore::isBuiltinType(Atom atm, BuiltinType bt)
    {
        return isObject(atm) && Traits::getBuiltinType(atomToScriptObject(atm)->traits()) == bt;
    }

    /*static*/ bool AvmCore::isBuiltinTypeMask(Atom atm, int btmask)
    {
        return isObject(atm) && ((1<<Traits::getBuiltinType(atomToScriptObject(atm)->traits())) & btmask) != 0;
    }

    /*static*/ bool AvmCore::isDictionary(Atom atm)
    {
        return isObject(atm) && atomToScriptObject(atm)->vtable->traits->isDictionary();
    }

    // Tables are from http://www.w3.org/TR/2004/REC-xml-20040204/#NT-NameChar
    // E4X 13.1.2.1, pg 63
    /* BaseChar = */
    static const wchar letterTable[] = {
        0x0041, 0x005A,
        0x0061, 0x007A,
        0x00C0, 0x00D6,
        0x00D8, 0x00F6,
        0x00F8, 0x00FF,
        0x0100, 0x0131,
        0x0134, 0x013E,
        0x0141, 0x0148,
        0x014A, 0x017E,
        0x0180, 0x01C3,
        0x01CD, 0x01F0,
        0x01F4, 0x01F5,
        0x01FA, 0x0217,
        0x0250, 0x02A8,
        0x02BB, 0x02C1,
        0x0386, 0x0386, // single
        0x0388, 0x038A,
        0x038C, 0x038C, // single
        0x038E, 0x03A1,
        0x03A3, 0x03CE,
        0x03D0, 0x03D6,
        0x03DA, 0x03DA, // single
        0x03DC, 0x03DC, // single
        0x03DE, 0x03DE, // single
        0x03E0, 0x03E0, // single
        0x03E2, 0x03F3,
        0x0401, 0x040C,
        0x040E, 0x044F,
        0x0451, 0x045C,
        0x045E, 0x0481,
        0x0490, 0x04C4,
        0x04C7, 0x04C8,
        0x04CB, 0x04CC,
        0x04D0, 0x04EB,
        0x04EE, 0x04F5,
        0x04F8, 0x04F9,
        0x0531, 0x0556,
        0x0559, 0x0559, // single
        0x0561, 0x0586,
        0x05D0, 0x05EA,
        0x05F0, 0x05F2,
        0x0621, 0x063A,
        0x0641, 0x064A,
        0x0671, 0x06B7,
        0x06BA, 0x06BE,
        0x06C0, 0x06CE,
        0x06D0, 0x06D3,
        0x06D5, 0x06D5, // single
        0x06E5, 0x06E6,
        0x0905, 0x0939,
        0x093D, 0x093D, // single
        0x0958, 0x0961,
        0x0985, 0x098C,
        0x098F, 0x0990,
        0x0993, 0x09A8,
        0x09AA, 0x09B0,
        0x09B2, 0x09B2, // single
        0x09B6, 0x09B9,
        0x09DC, 0x09DD,
        0x09DF, 0x09E1,
        0x09F0, 0x09F1,
        0x0A05, 0x0A0A,
        0x0A0F, 0x0A10,
        0x0A13, 0x0A28,
        0x0A2A, 0x0A30,
        0x0A32, 0x0A33,
        0x0A35, 0x0A36,
        0x0A38, 0x0A39,
        0x0A59, 0x0A5C,
        0x0A5E, 0x0A5E, // single
        0x0A72, 0x0A74,
        0x0A85, 0x0A8B,
        0x0A8D, 0x0A8D, // single
        0x0A8F, 0x0A91,
        0x0A93, 0x0AA8,
        0x0AAA, 0x0AB0,
        0x0AB2, 0x0AB3,
        0x0AB5, 0x0AB9,
        0x0ABD, 0x0ABD, // single
        0x0AE0, 0x0AE0, // single
        0x0B05, 0x0B0C,
        0x0B0F, 0x0B10,
        0x0B13, 0x0B28,
        0x0B2A, 0x0B30,
        0x0B32, 0x0B33,
        0x0B36, 0x0B39,
        0x0B3D, 0x0B3D, // single
        0x0B5C, 0x0B5D,
        0x0B5F, 0x0B61,
        0x0B85, 0x0B8A,
        0x0B8E, 0x0B90,
        0x0B92, 0x0B95,
        0x0B99, 0x0B9A,
        0x0B9C, 0x0B9C, // single
        0x0B9E, 0x0B9F,
        0x0BA3, 0x0BA4,
        0x0BA8, 0x0BAA,
        0x0BAE, 0x0BB5,
        0x0BB7, 0x0BB9,
        0x0C05, 0x0C0C,
        0x0C0E, 0x0C10,
        0x0C12, 0x0C28,
        0x0C2A, 0x0C33,
        0x0C35, 0x0C39,
        0x0C60, 0x0C61,
        0x0C85, 0x0C8C,
        0x0C8E, 0x0C90,
        0x0C92, 0x0CA8,
        0x0CAA, 0x0CB3,
        0x0CB5, 0x0CB9,
        0x0CDE, 0x0CDE, // single
        0x0CE0, 0x0CE1,
        0x0D05, 0x0D0C,
        0x0D0E, 0x0D10,
        0x0D12, 0x0D28,
        0x0D2A, 0x0D39,
        0x0D60, 0x0D61,
        0x0E01, 0x0E2E,
        0x0E30, 0x0E30, //single
        0x0E32, 0x0E33,
        0x0E40, 0x0E45,
        0x0E81, 0x0E82,
        0x0E84, 0x0E84, // single
        0x0E87, 0x0E88,
        0x0E8A, 0x0E8A, // single
        0x0E8D, 0x0E8D, // single
        0x0E94, 0x0E97,
        0x0E99, 0x0E9F,
        0x0EA1, 0x0EA3,
        0x0EA5, 0x0EA5, // single
        0x0EA7, 0x0EA7, // single
        0x0EAA, 0x0EAB,
        0x0EAD, 0x0EAE,
        0x0EB0, 0x0EB0, // single
        0x0EB2, 0x0EB3,
        0x0EBD, 0x0EBD, // single
        0x0EC0, 0x0EC4,
        0x0F40, 0x0F47,
        0x0F49, 0x0F69,
        0x10A0, 0x10C5,
        0x10D0, 0x10F6,
        0x1100, 0x1100, // single
        0x1102, 0x1103,
        0x1105, 0x1107,
        0x1109, 0x1109, // single
        0x110B, 0x110C,
        0x110E, 0x1112,
        0x113C, 0x113C, // single
        0x113E, 0x113E, // single
        0x1140, 0x1140, // single
        0x114C, 0x114C, // single
        0x114E, 0x114E, // single
        0x1150, 0x1150, // single
        0x1154, 0x1155,
        0x1159, 0x1159, // single
        0x115F, 0x1161,
        0x1163, 0x1163, // single
        0x1165, 0x1165, // single
        0x1167, 0x1167, // single
        0x1169, 0x1169, // single
        0x116D, 0x116E,
        0x1172, 0x1173,
        0x1175, 0x1175, // single
        0x119E, 0x119E, // single
        0x11A8, 0x11A8, // single
        0x11AB, 0x11AB, // single
        0x11AE, 0x11AF,
        0x11B7, 0x11B8,
        0x11BA, 0x11BA, // single
        0x11BC, 0x11C2,
        0x11EB, 0x11EB, // single
        0x11F0, 0x11F0, // single
        0x11F9, 0x11F9, // single
        0x1E00, 0x1E9B,
        0x1EA0, 0x1EF9,
        0x1F00, 0x1F15,
        0x1F18, 0x1F1D,
        0x1F20, 0x1F45,
        0x1F48, 0x1F4D,
        0x1F50, 0x1F57,
        0x1F59, 0x1F59, // single
        0x1F5B, 0x1F5B, // single
        0x1F5D, 0x1F5D, // single
        0x1F5F, 0x1F7D,
        0x1F80, 0x1FB4,
        0x1FB6, 0x1FBC,
        0x1FBE, 0x1FBE, // single
        0x1FC2, 0x1FC4,
        0x1FC6, 0x1FCC,
        0x1FD0, 0x1FD3,
        0x1FD6, 0x1FDB,
        0x1FE0, 0x1FEC,
        0x1FF2, 0x1FF4,
        0x1FF6, 0x1FFC,
        0x2126, 0x2126, // single
        0x212A, 0x212B,
        0x212E, 0x212E, // single
        0x2180, 0x2182,
        0x3041, 0x3094,
        0x30A1, 0x30FA,
        0x3105, 0x312C,
        0xAC00, 0xD7A3,
        //[86]      Ideographic    ::=
        0x4E00, 0x9FA5,
        0x3007, 0x3007, // single
        0x3021, 0x3029
        };

    /*static*/ bool AvmCore::isLetter(wchar c)
    {
        int x = sizeof(letterTable) / (sizeof(wchar));
        for (int i = 0; i < x; i += 2)
        {
            if (c >= letterTable[i] && c <= letterTable[i+1])
                return true;
        }
        return false;
    }

//[87]      CombiningChar      ::=
    static const wchar combiningCharTable[] = {
        0x0300, 0x0345,
        0x0360, 0x0361,
        0x0483, 0x0486,
        0x0591, 0x05A1,
        0x05A3, 0x05B9,
        0x05BB, 0x05BD,
        0x05BF, 0x05BF, // single
        0x05C1, 0x05C2,
        0x05C4, 0x05C4, // single
        0x064B, 0x0652,
        0x0670, 0x0670, // single
        0x06D6, 0x06DC,
        0x06DD, 0x06DF,
        0x06E0, 0x06E4,
        0x06E7, 0x06E8,
        0x06EA, 0x06ED,
        0x0901, 0x0903,
        0x093C, 0x093C, // single
        0x093E, 0x094C,
        0x094D, 0x094D, // single
        0x0951, 0x0954,
        0x0962, 0x0963,
        0x0981, 0x0983,
        0x09BC, 0x09BC, // single
        0x09BE, 0x09BE, // single
        0x09BF, 0x09BF, // single
        0x09C0, 0x09C4,
        0x09C7, 0x09C8,
        0x09CB, 0x09CD,
        0x09D7, 0x09D7, // single
        0x09E2, 0x09E3,
        0x0A02, 0x0A02, // single
        0x0A3C, 0x0A3C, // single
        0x0A3E, 0x0A3E, // single
        0x0A3F, 0x0A3F, // single
        0x0A40, 0x0A42,
        0x0A47, 0x0A48,
        0x0A4B, 0x0A4D,
        0x0A70, 0x0A71,
        0x0A81, 0x0A83,
        0x0ABC, 0x0ABC, // single
        0x0ABE, 0x0AC5,
        0x0AC7, 0x0AC9,
        0x0ACB, 0x0ACD,
        0x0B01, 0x0B03,
        0x0B3C, 0x0B3C, // single
        0x0B3E, 0x0B43,
        0x0B47, 0x0B48,
        0x0B4B, 0x0B4D,
        0x0B56, 0x0B57,
        0x0B82, 0x0B83,
        0x0BBE, 0x0BC2,
        0x0BC6, 0x0BC8,
        0x0BCA, 0x0BCD,
        0x0BD7, 0x0BD7, // single
        0x0C01, 0x0C03,
        0x0C3E, 0x0C44,
        0x0C46, 0x0C48,
        0x0C4A, 0x0C4D,
        0x0C55, 0x0C56,
        0x0C82, 0x0C83,
        0x0CBE, 0x0CC4,
        0x0CC6, 0x0CC8,
        0x0CCA, 0x0CCD,
        0x0CD5, 0x0CD6,
        0x0D02, 0x0D03,
        0x0D3E, 0x0D43,
        0x0D46, 0x0D48,
        0x0D4A, 0x0D4D,
        0x0D57, 0x0D57, // single
        0x0E31, 0x0E31, // single
        0x0E34, 0x0E3A,
        0x0E47, 0x0E4E,
        0x0EB1, 0x0EB1, // single
        0x0EB4, 0x0EB9,
        0x0EBB, 0x0EBC,
        0x0EC8, 0x0ECD,
        0x0F18, 0x0F19,
        0x0F35, 0x0F35, // single
        0x0F37, 0x0F37, // single
        0x0F39, 0x0F39, // single
        0x0F3E, 0x0F3E, // single
        0x0F3F, 0x0F3F, // single
        0x0F71, 0x0F84,
        0x0F86, 0x0F8B,
        0x0F90, 0x0F95,
        0x0F97, 0x0F97, // single
        0x0F99, 0x0FAD,
        0x0FB1, 0x0FB7,
        0x0FB9, 0x0FB9, // single
        0x20D0, 0x20DC,
        0x20E1, 0x20E1, // single
        0x302A, 0x302F,
        0x3099, 0x3099, // single
        0x309A, 0x309A // single
        };
    /*static*/ bool AvmCore::isCombiningChar(wchar c)
    {
        int x = sizeof(combiningCharTable) / (sizeof(wchar));
        for (int i = 0; i < x; i += 2)
        {
            if (c >= combiningCharTable[i] && c <= combiningCharTable[i+1])
                return true;
        }
        return false;
    }

//[88]      Digit      ::=
    static const wchar digitTable[] = {
        0x0030, 0x0039,
        0x0660, 0x0669,
        0x06F0, 0x06F9,
        0x0966, 0x096F,
        0x09E6, 0x09EF,
        0x0A66, 0x0A6F,
        0x0AE6, 0x0AEF,
        0x0B66, 0x0B6F,
        0x0BE7, 0x0BEF,
        0x0C66, 0x0C6F,
        0x0CE6, 0x0CEF,
        0x0D66, 0x0D6F,
        0x0E50, 0x0E59,
        0x0ED0, 0x0ED9,
        0x0F20, 0x0F29};

    /*static*/ bool AvmCore::isDigit(wchar c)
    {
        int x = sizeof(digitTable) / (sizeof(wchar));
        for (int i = 0; i < x; i += 2)
        {
            if (c >= digitTable[i] && c <= digitTable[i+1])
                return true;
        }
        return false;
    }

    static const wchar extenderTable[] = {
        0x00B7, 0x00B7, // single
        0x02D0, 0x02D0, // single
        0x02D1, 0x02D1, // single
        0x0387, 0x0387, // single
        0x0640, 0x0640, // single
        0x0E46, 0x0E46, // single
        0x0EC6, 0x0EC6, // single
        0x3005, 0x3005, // single
        0x3031, 0x3035,
        0x309D, 0x309E,
        0x30FC, 0x30FE};
    /*static*/ bool AvmCore::isExtender(wchar c)
    {
        int x = sizeof(extenderTable) / (sizeof(wchar));
        for (int i = 0; i < x; i += 2)
        {
            if (c >= extenderTable[i] && c <= extenderTable[i+1])
                return true;
        }
        return false;
    }

    bool AvmCore::isXMLName(Atom arg)
    {
        if (isNullOrUndefined(arg))
            return false;

        StringIndexer p(string(arg));

        // http://www.w3.org/TR/2004/REC-xml-20040204/#NT-NameChar

        // Name is (Letter | _ or :) followed by arbitrary number of NameChar

        if (!p->length())
            return false;

        // According to the Mozilla testcase...
        // e4x excludes ':'

        wchar c = p[0];
        if (!isLetter(c) && c != '_' /*&& c != ':'*/)
            return false;

        for (int i = 1; i < p->length(); i++)
        {
            wchar c = p[i];

            if (isDigit(c) || isLetter(c) || (c == '.') || (c == '-') || (c == '_') || /*(c != ':') ||*/
                isCombiningChar (c) || isExtender(c))
                continue;

            return false;
        }

        return true;
    }

    Stringp AvmCore::ToXMLString (Atom a)
    {
        if (!isNull(a))
        {
            switch (atomKind(a))
            {
            case kStringType:
                return EscapeElementValue (string(a), true);
            case kObjectType:
            case kNamespaceType:
                if (isXML(a))
                {
                    XMLObject *x = atomToXMLObject (a);
                    return x->toXMLString();
                }
                else if (isXMLList(a))
                {
                    XMLListObject *x = atomToXMLList (a);
                    return x->toXMLString();
                }
                else
                {
                    // !!@ to primitive (hint string first)
                    // !!@ namespace case falls into this as well
                    return EscapeElementValue (string(a), true);
                }
                break;
            case kSpecialBibopType:
                if(a != undefinedAtom)
                    return string(a);
                return kundefined;
            case kIntptrType:
            case kBooleanType:
            case kDoubleType:
            default:
                return string(a);
            }
        }
        else
        {
            return knull;
        }
    }

    Stringp AvmCore::EscapeElementValue(const Stringp _s, bool removeLeadingTrailingWhitespace)
    {
        StringIndexer s(_s);
        StringBuffer output(this);

        int i = 0;
        int last = s->length() - 1;
        if (removeLeadingTrailingWhitespace)
        {
            // finding trailing whitespace
            while (last >= 0)
            {
                if (!String::isSpace(s[last]))
                    break;

                last--;
            }

            if (last < 0)
                return kEmptyString;

            // skip leading whitespace
            for (i = 0; i <= last; i++)
            {
                if (!String::isSpace(s[i]))
                    break;
            }
        }

        while (i <= last)
        {
            switch (s[i])
            {
            case '<':
                output << "&lt;";
                break;
            case '>':
                output << "&gt;";
                break;
            case '&':
                output << "&amp;";
                break;
            case 0x0000:
                output << "&#x0;"; // extension of ECMA-357
                break;
            default:
                output << (s[i]);
            }

            i++;
        }

        return newStringUTF8(output.c_str(), output.length());
    }

    Stringp AvmCore::EscapeAttributeValue(Atom v)
    {
        StringBuffer output(this);

        StringIndexer s(string(v));

        for (int i = 0; i < s->length(); i++)
        {
            switch (s[i])
            {
            case '"':
                output << "&quot;";
                break;
            case '<':
                output << "&lt;";
                break;
            case '&':
                output << "&amp;";
                break;
            case 0x000a:
                output << "&#xA;";
                break;
            case 0x000d:
                output << "&#xD;";
                break;
            case 0x0009:
                output << "&#x9;";
                break;
            case 0x0000:
                output << "&#x0;"; // extension of ECMA-357
                break;
            default:
                output << (s[i]);
            }
        }

        return newStringUTF8(output.c_str(), output.length());
    }

    /*static*/ XMLObject* AvmCore::atomToXMLObject(Atom atm)
    {
        return isXML(atm) ? (XMLObject*)(atomToScriptObject(atm)) : NULL;
    }

    /*static*/ E4XNode* AvmCore::atomToXML(Atom atm)
    {
        return isXML(atm) ? ((XMLObject*)(atomToScriptObject(atm)))->getNode() : NULL;
    }

    /*static*/ XMLListObject* AvmCore::atomToXMLList(Atom atm)
    {
        return isXMLList(atm) ? (XMLListObject*)(atomToScriptObject(atm)) : NULL;
    }

    /*static*/ QNameObject* AvmCore::atomToQName(Atom atm)
    {
        return isQName(atm) ? (QNameObject*)(atomToScriptObject(atm)) : NULL;
    }

    Stringp AvmCore::_typeof (Atom arg)
    {
        if (!isNull(arg))
        {
            switch (atomKind(arg))
            {
            default:
            case kObjectType:
                if (isXMLorXMLList(arg))
                {
                    return kxml;
                }
                else if (isFunction(arg))
                {
                    return kfunction; // No special type code for functions, but we need to
                                    //  special case to return 'function' here.
                }
                // else fall thru and return kobject

            case kNamespaceType:
                return kobject;

            case kBooleanType:
                return kboolean;

            case kSpecialBibopType:
#ifdef VMCFG_FLOAT
                if(arg == undefinedAtom)
                    return kundefined;
                if (isFloat(arg))
                    return kfloat;
                if (isFloat4(arg))
                    return kfloat4;
                AvmAssertMsg(false, "Unknown bibop type");
#endif // VMCFG_FLOAT
                return kundefined;
                
            case kIntptrType:
            case kDoubleType:
                return knumber;


            case kStringType:
                return kstring;

            }
        }
        else
        {
            // typeof(null) = "object"
            return kobject;
        }
    }

    const BugCompatibility* AvmCore::createBugCompatibility(BugCompatibility::Version v)
    {
        return new (GetGC()) BugCompatibility(v);
    }

    void AvmCore::addLivePool(PoolObject* pool)
    {
        // Ordering is significant (Bugzilla 574427): GetWeakRef can trigger a collection,
        // and AvmCore::presweep may update the livePools list, specifically it may delete
        // the first element of that list.  Thus we should not touch that value until after
        // we've gotten the weak ref.

        LivePoolNode* node = new LivePoolNode(GetGC());
        node->pool = pool->GetWeakRef();
        node->next = livePools;
        livePools = node;
    }

    void AvmCore::presweep()
    {
        LivePoolNode** prev = &livePools;
        LivePoolNode* node = livePools;
        while (node)
        {
            PoolObject* pool = (PoolObject*)(void*)(node->pool->peek());
            if (pool)
            {
                // pool is still alive -- if about to get collected, dynamicize the strings
                // and remove it from the active list
                if (!GetGC()->GetMark(pool))
                {
                    pool->dynamicizeStrings();
                    *prev = node->next;
                    delete node;
                    node = *prev;
                    continue;
                }
            }
            else
            {
                // pool is dead -- should have already removed it?
                AvmAssert(0);
            }
            prev = &node->next;
            node = node->next;
        }

        // clear out the string table
        {
            for (int i=0, n=numStrings; i < n; i++)
            {
                if (strings[i] > AVMPLUS_STRING_DELETED && !GetGC()->GetMark(strings[i]))
                {
                    strings[i] = AVMPLUS_STRING_DELETED;
                    deletedCount++;
                    stringCount--;
                }
            }
        }


        // do the same for the namespaces
        {
            bool rehashFlag = false;
            for (int i=0, n=numNamespaces; i < n; i++)
            {
                if (namespaces[i] != NULL && !GetGC()->GetMark(namespaces[i]))
                {
                    rehashFlag = true;
                    namespaces[i] = NULL;
                    nsCount--;
                }
            }

            // if any interned strings were freed, rehash the intern table
            // todo - make this less aggressive
            if (rehashFlag)
                rehashNamespacesIfPossible(numNamespaces);
        }

        // Clear out the regex compile cache, just to prevent big strings or regexes from
        // hanging around.

        m_regexCache.clear();

#ifdef DEBUGGER
        if (_sampler)
            _sampler->presweep();
#endif
    }

    void AvmCore::postsweep()
    {
#ifdef VMCFG_NANOJIT
        if (m_flushBindingCachesNextSweep)
        {
            for (LivePoolNode* node = livePools; node != NULL; node = node->next)
            {
                // Use 'peek' here because we're in postsweep where 'get' could cause trouble;
                // we're OK because the reference does not escape.
                PoolObject* pool = (PoolObject*)(void*)(node->pool->peek());
                if (pool && pool->codeMgr)
                    pool->codeMgr->flushBindingCaches();
            }
            m_flushBindingCachesNextSweep = false;
        }
#endif
#ifdef DEBUGGER
        if (_sampler)
            _sampler->postsweep();
#endif
    }

    REALLY_INLINE int AvmCore::numStringsCheckLoadBalance()
    {
        int m = numStrings;
        // 80% load factor
        if (5*(stringCount+deletedCount+1) > 4*m) {
            if (2*stringCount > m) // 50%
                rehashStrings(m = m << 1);
            else
                rehashStrings(m);
        }
        return m;
    }

    int AvmCore::findString(Stringp s)
    {
        int m = numStringsCheckLoadBalance();

        // compute the hash function
        int hashCode = s->hashCode();

        int bitMask = m - 1;

        // find the slot to use
        int i = (hashCode&0x7FFFFFFF) & bitMask;
        int n = 7;
        Stringp k;
        if (!deletedCount)
        {
            while ((k=strings[i]) != NULL && !k->equals(s)) {
                i = (i + (n++)) & bitMask; // quadratic probe
            }
        }
        else
        {
            int iFirstDeletedSlot = -1;
            while ((k=strings[i]) != NULL)
            {
                if (k == AVMPLUS_STRING_DELETED)
                {
                    if (iFirstDeletedSlot < 0)
                    {
                        iFirstDeletedSlot = i;
                    }
                }
                else if (k->equals(s))
                {
                    break;
                }
                i = (i + (n++)) & bitMask; // quadratic probe
            }

            if ((k == NULL) && (iFirstDeletedSlot >= 0))
                i = iFirstDeletedSlot;
        }
        return i;
    }

    int AvmCore::findStringLatin1(const char* s, int len)
    {
        int m = numStringsCheckLoadBalance();

        // compute the hash function
        int hashCode = String::hashCodeLatin1(s, len);

        int bitMask = m - 1;

        // find the slot to use
        int i = (hashCode&0x7FFFFFFF) & bitMask;
        int n = 7;
        Stringp k;
        if (!deletedCount)
        {
            while ((k=strings[i]) != NULL && !k->equalsLatin1(s,len)) {
                i = (i + (n++)) & bitMask; // quadratic probe
            }
        }
        else
        {
            int iFirstDeletedSlot = -1;
            while ((k=strings[i]) != NULL)
            {
                if (k == AVMPLUS_STRING_DELETED)
                {
                    if (iFirstDeletedSlot < 0)
                    {
                        iFirstDeletedSlot = i;
                    }
                }
                else if (k->equalsLatin1(s, len))
                {
                    break;
                }
                i = (i + (n++)) & bitMask; // quadratic probe
            }

            if ((k == NULL) && (iFirstDeletedSlot >= 0))
                i = iFirstDeletedSlot;
        }
        return i;
    }

    int AvmCore::findStringUTF16(const wchar* s, int len)
    {
        int m = numStringsCheckLoadBalance();

        // compute the hash function
        int hashCode = String::hashCodeUTF16(s, len);

        int bitMask = m - 1;

        // find the slot to use
        int i = (hashCode&0x7FFFFFFF) & bitMask;
        int n = 7;
        Stringp k;
        if (!deletedCount)
        {
            while ((k=strings[i]) != NULL && !k->equalsUTF16(s,len)) {
                i = (i + (n++)) & bitMask; // quadratic probe
            }
        }
        else
        {
            int iFirstDeletedSlot = -1;
            while ((k=strings[i]) != NULL)
            {
                if (k == AVMPLUS_STRING_DELETED)
                {
                    if (iFirstDeletedSlot < 0)
                    {
                        iFirstDeletedSlot = i;
                    }
                }
                else if (k->equalsUTF16(s, len))
                {
                    break;
                }
                i = (i + (n++)) & bitMask; // quadratic probe
            }

            if ((k == NULL) && (iFirstDeletedSlot >= 0))
                i = iFirstDeletedSlot;

        }
        return i;
    }

    int AvmCore::findStringWithIndex(uint32_t value)
    {
        int m = numStringsCheckLoadBalance();

        // compute the hash function
        int hashCode = String::hashCodeUInt(value);

        int bitMask = m - 1;

        // find the slot to use
        int i = (hashCode&0x7FFFFFFF) & bitMask;
        int n = 7;
        Stringp k;
        uint32_t recv;
        if (!deletedCount)
        {
            while ((k=strings[i]) != NULL) {
                if (k->parseIndex(recv) && recv == value)
                    break;
                i = (i + (n++)) & bitMask; // quadratic probe
            }
        }
        else
        {
            int iFirstDeletedSlot = -1;
            while ((k=strings[i]) != NULL)
            {
                if (k == AVMPLUS_STRING_DELETED)
                {
                    if (iFirstDeletedSlot < 0)
                    {
                        iFirstDeletedSlot = i;
                    }
                }
                else if (k->parseIndex(recv) && recv == value)
                {
                    break;
                }
                i = (i + (n++)) & bitMask; // quadratic probe
            }

            if ((k == NULL) && (iFirstDeletedSlot >= 0))
                i = iFirstDeletedSlot;
        }
        return i;
    }


    /**
     * namespace hash search.  interned namespaces are as unique as their
     * uri.  We assume uri's are already interned, so interning a namespace
     * is quick because uri's can be compared quickly.
     */
    int AvmCore::findNamespace(Namespacep ns, bool canRehash)
    {
        int m = numNamespaces;
        // 80% load factor
        if (canRehash && nsCount*5 >= 4*m) {
            rehashNamespaces(m = m << 1);
        }

        // compute the hash function
        int hashCode = (int)(((uintptr_t)ns->getURI())>>3);  // FIXME possibly hash api mask too

        int bitMask = m - 1;

        // find the slot to use
        int i = (hashCode&0x7FFFFFFF) & bitMask;
        int n = 7;
        Namespacep k;
        while ((k=namespaces[i]) != NULL && (k->m_uriAndType != ns->m_uriAndType || k->getApiVersion() != ns->getApiVersion())) {
            i = (i + (n++)) & bitMask; // quadratic probe
        }
        return i;
    }

    Namespacep AvmCore::gotNamespace(uintptr_t uriAndType, ApiVersion apiVersion)
    {
        int m = numNamespaces;

        // compute the hash function
        int hashCode = (int)(((uintptr_t)uriAndType)>>3);  // FIXME possibly hash api mask too

        int bitMask = m - 1;

        // find the slot to use
        int i = (hashCode&0x7FFFFFFF) & bitMask;
        int n = 7;
        Namespacep k;
        while ((k=namespaces[i]) != NULL &&
               (k->m_uriAndType != uriAndType ||
                k->getApiVersion() != apiVersion)) {
            i = (i + (n++)) & bitMask; // quadratic probe
        }
        return namespaces[i];
    }

    // note, this assumes Latin-1, not UTF8.
    Stringp AvmCore::internStringLatin1(const char* s, int len)
    {
        if (len < 0) len = String::Length(s);
        int i = findStringLatin1(s, len);
        Stringp other;
        if ((other=strings[i]) <= AVMPLUS_STRING_DELETED)
        {
            if (other == AVMPLUS_STRING_DELETED)
            {
                deletedCount--;
                AvmAssert(deletedCount >= 0);
            }

#ifdef DEBUGGER
            GCRoot::GCMember<String> *oldStrings = strings;
#endif

            other = newStringLatin1(s, len);

#ifdef DEBUGGER
            // re-find if String ctor caused rehash
            if(strings != oldStrings)
                i = findStringLatin1(s, len);
#endif
            strings[i] = other;
            stringCount++;
            other->setInterned();
        }
        return other;
    }

    Stringp AvmCore::internSubstring(Stringp s, int32_t start, int32_t end)
    {
        // this is (currently) only called by XMLParser, which only passes us
        // valid values -- skip the checks in the name of performance.
        AvmAssert(start < end);
        AvmAssert(start >= 0 && start <= s->length());
        AvmAssert(end >= 0 && end <= s->length());

        if (start == 0 && end == s->length())
            return internString(s);

        int32_t const len = end - start;
        String::Width const w = s->getWidth();

        int i;
        {
            // enclose this in braces to ensure we don't use Pointers
            // across an allocation (eg, substring())
            String::Pointers ptrs(s);
            i = (w == String::k8) ?
                findStringLatin1((const char*)ptrs.p8 + start, len) :
                findStringUTF16(ptrs.p16 + start, len);
        }

        Stringp other;
        if ((other=strings[i]) <= AVMPLUS_STRING_DELETED)
        {
            if (other == AVMPLUS_STRING_DELETED)
            {
                deletedCount--;
                AvmAssert(deletedCount >= 0);
            }

#ifdef DEBUGGER
            GCRoot::GCMember<String>* oldStrings = strings;
#endif

            other = s->substring(start, end);
            /*
                substring() attempts to make a dependent string when possible;
                normally this is highly desirable, but for interned strings,
                we must be cautious: we usually call this function from XML parsing,
                and it would be bad if we forced a giant string of XML to be
                held in memory because of one tiny dependent string in the intern table.
                The simplest fix is to call fixDependentString(): if the
                string is dependent, it's converted to dynamic (unless the length of the
                dependent string is very close to the size of the master string, in which
                case there's no memory to be saved by doing the conversion).
            */
            other->fixDependentString();

#ifdef DEBUGGER
            // re-find if String ctor caused rehash
            if (strings != oldStrings)
            {
                String::Pointers ptrs(s);
                i = (w == String::k8) ?
                    findStringLatin1((const char*)ptrs.p8 + start, len) :
                    findStringUTF16(ptrs.p16 + start, len);
            }
#endif

            strings[i] = other;
            stringCount++;
            other->setInterned();
        }
        return other;
    }


    // note, this assumes Latin-1, not UTF8.
    Stringp AvmCore::internConstantStringLatin1(const char* s)
    {
        return internString(newConstantStringLatin1(s));
    }

    /**
     * intern the given string atom which has already been allocated
     * @param atom
     * @return
     */
    Stringp AvmCore::internString(Stringp o)
    {
        if (o->isInterned())
            return o;

        int i = findString(o);
        Stringp other;
        if ((other=strings[i]) <= AVMPLUS_STRING_DELETED)
        {
            if (other == AVMPLUS_STRING_DELETED)
            {
                deletedCount--;
                AvmAssert(deletedCount >= 0);
            }
            stringCount++;
            // Prevent dependent strings from keeping a huge master in memory
            o->fixDependentString();
            o->setInterned();
            strings[i] = o;
            return o;
        }
        else
        {
            return other;
        }
    }

    // Identical to internString plus clone minus the isInterned() check - for the receiver
    // it doesn't matter if the string is interned or not.
    Stringp AvmCore::internForeignString(Stringp o) 
    {
        // this can go in parallel 
        int i = findString(o);
        Stringp other;
        if ((other=strings[i]) <= AVMPLUS_STRING_DELETED)
        {
            if (other == AVMPLUS_STRING_DELETED)
            {
                deletedCount--;
                AvmAssert(deletedCount >= 0);
            }
            stringCount++;
            // Make copy now, an interned string doesn't exist yet
            o = o->clone(this);
            // Prevent dependent strings from keeping a huge master in memory
            o->fixDependentString();
            o->setInterned();
            strings[i] = o;
            return o;
        }
        else
        {
            return other;
        }
    }


    Stringp AvmCore::internString(Atom atom)
    {
        AvmAssert(isString(atom));
        Stringp s = atomToString(atom);
        return s->isInterned() ? s : internString(s);
    }

    Stringp AvmCore::internInt(int value)
    {
        // todo, bug 561092: avoid string buffer conversion by
        // calculating hashcode from int value directly.
        //
        // below: manual inlining and specialization of
        // MathUtils::convertIntegerToStringBase10.
        char buffer[MathUtils::kMinSizeForInt32_t_base10_toString];
        int32_t len = MathUtils::kMinSizeForInt32_t_base10_toString;
        intptr_t wideVal = (intptr_t)value;
        MathUtils::UnsignedTreatment treatAs = MathUtils::kTreatAsSigned;
        char *p = MathUtils::convertIntegerToStringBuffer(wideVal, buffer,
                                                          len, 10, treatAs);
        return internStringLatin1( p, len );
    }

    Stringp AvmCore::internUint32 (uint32_t ui)
    {
        if (ui & 0x80000000)
            return internDouble(ui);
        else
            return internInt((int)ui);
    }

    bool AvmCore::isInternedUint(uint32_t ui, Stringp *result)
    {
        int iSlotForIndex;
        if (ui & 0x80000000) {
            // FIXME bug 561092: is there a more direct route to slot here?
            Stringp s = MathUtils::convertDoubleToString(this, double(ui));
            iSlotForIndex = findString(s);
        } else {
            iSlotForIndex = findStringWithIndex(ui);
        }

        Stringp other = strings[iSlotForIndex];
        if (other <= AVMPLUS_STRING_DELETED)
        {
            return false;
        }
        else
        {
            *result = other;
            return true;
        }
    }

#ifdef VMCFG_FLOAT
    Stringp AvmCore::internFloat(float f)
    {
        // we call internDouble, because according to the spec, 
        // we must match the behaviour of Number. 
        return internDouble(f);
    }

    Stringp AvmCore::internFloat4(const float4_t& f4)
    {
        return internString(float4ToString(f4));
    }
#endif // VMCFG_FLOAT

    Stringp AvmCore::internDouble(double d)
    {
        return internString(MathUtils::convertDoubleToString(this, d));
    }

#ifdef DEBUGGER
    Stringp AvmCore::findInternedString(const char *cs, int len8)
    {
        // NOTE: this works in strict UTF-8 conversion mode
        int32_t len16 = UnicodeUtils::Utf8ToUtf16((const uint8_t*)cs, len8, NULL, 0, true);
        AvmAssertMsg(len16 >= 0, "Malformed UTF-8 sequence");
        // use alloca to avoid heap allocations where possible
        MMgc::GC::AllocaAutoPtr _buffer;
        wchar *buffer = (wchar*) avmStackAllocArray(this, _buffer, (len16+1), sizeof(wchar));

        if(!buffer) {
            AvmAssertMsg(false, "alloca failed!");
            return NULL;
        }

        UnicodeUtils::Utf8ToUtf16((const uint8_t*)cs, len8, buffer, len16, true);
        buffer[len16] = 0;
        int i = findStringUTF16(buffer, len16);
        Stringp other;
        if ((other=strings[i]) > AVMPLUS_STRING_DELETED)
        {
            return other;
        }
        return NULL;
    }
#endif

    Stringp AvmCore::internStringUTF8(const char* cs, int len8, bool constant, bool strict)
    {
        Stringp s = String::createUTF8(this, (const utf8_t*)cs, len8, String::kAuto, constant, strict);
        // createUTF8() will return NULL if we pass it invalid UTF8 data and its "strict" arg is true
        // (which it is by default) -- check and bail if that happens.
        if (!s) return NULL;
        int i = findString(s);
        Stringp other;
        if ((other=strings[i]) <= AVMPLUS_STRING_DELETED)
        {
            if (other == AVMPLUS_STRING_DELETED)
            {
                deletedCount--;
                AvmAssert(deletedCount >= 0);
            }

            s->setInterned();
            stringCount++;
            strings[i] = s;
            return s;
        }
        else
            return other;
    }

    Stringp AvmCore::internStringUTF16(const wchar* s, int len)
    {
        if (len < 0) len = String::Length(s);
        int i = findStringUTF16(s, len);
        Stringp other;
        if ((other=strings[i]) <= AVMPLUS_STRING_DELETED)
        {
            if (other == AVMPLUS_STRING_DELETED)
            {
                deletedCount--;
                AvmAssert(deletedCount >= 0);
            }

#ifdef DEBUGGER
            GCRoot::GCMember<String> *oldStrings = strings;
#endif

            other = newStringUTF16(s, len);

#ifdef DEBUGGER
            // re-find if String ctor caused rehash
            if(strings != oldStrings)
                i = findStringUTF16(s, len);
#endif
            strings[i] = other;
            stringCount++;
            other->setInterned();
        }
        return other;
    }

    void AvmCore::rehashStrings(int newlen)
    {
        // rehash

        GCRoot::GCMember<String> *oldStrings = strings;
        strings = mmfx_new_array(GCRoot::GCMember<String>, newlen);

        int oldStringCount = numStrings;
        numStrings = newlen;

#ifdef _DEBUG // debug sanity checks
        int oldDeletedCount = deletedCount;
        int computedDeleteCount = 0;
        int computedStringCount = 0;
#endif

        deletedCount = 0;

        // Inlined and optimized our findString routine.  We know that there
        // are no duplicated strings in our intern string table so we don't
        // need to call Equals.  All we need to do is find the first blank
        // spot available.
        int m = numStrings;
        int bitMask = m - 1;

        for (int i=0; i < oldStringCount; i++)
        {
            Stringp o = oldStrings[i];
            if (o > AVMPLUS_STRING_DELETED)
            {
                // compute the hash function
                int hashCode = (int) o->hashCode();

                // find the slot to use
                int j = (hashCode&0x7FFFFFFF) & bitMask;
                int n = 7;
                while (strings[j] != NULL) {
                    j = (j + (n++)) & bitMask; // quadratic probe
                }

                strings[j] = o;
#ifdef _DEBUG
                computedStringCount++;
#endif
            }
#ifdef _DEBUG
            else if (o == AVMPLUS_STRING_DELETED)
            {
                computedDeleteCount++;
            }
#endif
        }

#ifdef _DEBUG
        AvmAssert(computedStringCount == stringCount);
        AvmAssert(oldDeletedCount == computedDeleteCount);
#endif

        // Clear oldStrings so it can be collected.
        mmfx_delete_array(oldStrings);
    }

    void AvmCore::rehashNamespacesIfPossible(int newlen)
    {
        rehashNamespaces(newlen, true);
    }

    void AvmCore::rehashNamespaces(int newlen, bool canFail/*=false*/)
    {
        // rehash

        GCRoot::GCMember<Namespace> *old = namespaces;
        int oldCount = numNamespaces;
        if (canFail)
        {
            AvmAssert(newlen <= oldCount); // failure senseless when growing.

            // alloc failure when shrinking ==> can recover without OOM Abort.
            namespaces = mmfx_new_array_opt(GCRoot::GCMember<Namespace>, newlen,
                                            kCanFail);
            if (namespaces == NULL)
            {
                namespaces = old;
                return;
            }
        }
        else
        {
            namespaces = mmfx_new_array(GCRoot::GCMember<Namespace>, newlen);
        }
        numNamespaces = newlen;

        for (int i=0; i < oldCount; i++)
        {
            Namespacep o = old[i];
            if (o != NULL)
                namespaces[findNamespace(o, /*canRehash = */ false)] = o;
        }

        // Clear old namespaces table so it can be collected.
        mmfx_delete_array(old);
    }

    ScriptBufferImpl* AvmCore::newScriptBuffer(size_t size)
    {
        return new (GetGC(), size) BasicScriptBufferImpl(size);
    }

    VTable* AvmCore::newVTable(Traits* traits, VTable* base, Toplevel* toplevel)
    {
        const uint32_t count = traits->getTraitsBindings()->methodCount;
        size_t extraSize = sizeof(MethodEnv*)*(count > 0 ? count-1 : 0);
        return new (GetGC(), MMgc::kExact, extraSize) VTable(traits, base, toplevel);
    }

    RegExpObject* AvmCore::newRegExp(RegExpClass* regexpClass,
                                  Stringp pattern,
                                  Stringp options)
    {
        return RegExpObject::create(GetGC(), regexpClass->ivtable(), regexpClass->prototypePtr(), pattern, options);
    }

    Namespacep AvmCore::newNamespace(Atom prefix, Atom uri, Namespace::NamespaceType type)
    {
        // E4X - this is 13.2.3, step 3 - prefix IS specified
        Atom p;
        Stringp u;
        if (isQName(uri) && !isNull(atomToQName(uri)->getURI()))
        {
            u = atomToString(atomToQName(uri)->getURI());
        }
        else
        {
            u = internString(string(uri));
        }
        if (u->isEmpty())
        {
            if (prefix == undefinedAtom)
                p = kEmptyString->atom();
            else if (!string (prefix)->length())
                p = kEmptyString->atom();
            else
            {
                // !!@ throw correct type error
                //typeErrorClass()->throwError(kConvertUndefinedToObjectError);
                return NULL;
            }
        }
        else if (prefix == undefinedAtom)
        {
            p = undefinedAtom;
        }
        else if (prefix != kEmptyString->atom() && !isXMLName (prefix))
        {
            p = undefinedAtom;
        }
        else
        {
            p = internString(string(prefix))->atom();
        }

        // called from AS so need to get the api version off the call stack
        ApiVersion const apiVersion = this->getApiVersionFromCallStack();
        AvmAssert(isValidApiVersion(apiVersion));
        Namespacep ns = Namespace::create(GetGC(), p, u, type, apiVersion);
        return ns;
    }

    Namespacep AvmCore::newNamespace(Atom uri, Namespace::NamespaceType type)
    {
        // prefix and uri must be interned!
        // E4X - this is 13.2.2, step 3 - "prefix not specified"
        Atom p;
        Stringp u;
        if (isNamespace (uri))
        {
            Namespacep ns = atomToNamespace(uri);
            p = ns->getPrefix();
            u = ns->getURI();
        }
        else if (isObject(uri) && isQName(uri) && !isNull(atomToQName(uri)->getURI()))
        {
            p = undefinedAtom;
            u = atomToString(atomToQName(uri)->getURI());
        }
        else
        {
            u = string(uri);
            p = u->isEmpty() ? kEmptyString->atom() : undefinedAtom;
            u = internString(u);
        }
        // called from AS so need to get the api version off the call stack
        ApiVersion const apiVersion = this->getApiVersionFromCallStack();
        AvmAssert(isValidApiVersion(apiVersion));
        Namespacep ns = Namespace::create(GetGC(), p, u, type, apiVersion);
        return ns;
    }

    Namespacep AvmCore::newNamespace(Stringp uri, Namespace::NamespaceType type, ApiVersion apiVersion)
    {
        apiVersion = getValidApiVersion(apiVersion);
        Atom prefix = uri->isEmpty() ? kEmptyString->atom() : undefinedAtom;
        Namespacep ns = Namespace::create(GetGC(), prefix, uri, type, apiVersion);
        return ns;
    }

    Namespacep AvmCore::newAnyNamespace(Stringp uri, Namespace::NamespaceType type)
    {
        // kApiVersion_VM_INTERNAL matches all versions, so that's what we want,
        // not kApiVersion_VM_ALLVERSIONS, amusingly enough. (But: only for NS_Public
        // namespaces, since all other forms are always nonversioned and require
        // ALLVERSIONS at all times.)
        ApiVersion const apiVersion = (type == Namespace::NS_Public) ? kApiVersion_VM_INTERNAL : kApiVersion_VM_ALLVERSIONS;
        Atom prefix = uri->isEmpty() ? kEmptyString->atom() : undefinedAtom;
        Namespacep ns = Namespace::create(GetGC(), prefix, uri, type, apiVersion);
        return ns;
    }

    Namespacep AvmCore::newPublicNamespace(Stringp uri)
    {
        ApiVersion const apiVersion = this->getApiVersionFromCallStack();
        AvmAssert(isValidApiVersion(apiVersion));
        return newNamespace(uri, Namespace::NS_Public, apiVersion);
    }

    Atom AvmCore::uintToAtom(uint32_t n)
    {
        return (atomIsValidIntptrValue_u(n) ?  // always true on 64-bit
                atomFromIntptrValue_u(n) :
                allocDouble(n));
    }

    Atom AvmCore::intToAtom(int32_t n)
    {
        return (atomIsValidIntptrValue(n) ? // always true on 64-bit
                atomFromIntptrValue(n) :
                allocDouble(n));
    }

    REALLY_INLINE bool atomIsValidIntptrValueAndEqualTo(const intptr_t ival, const double orig)
    {
        // ival's intptr_t type is critical; implicitly casts from
        // int32_t in most invocations of this function here.

        return ((((ival << atomSignExtendShift) >> atomSignExtendShift) == orig)
                && !((ival == 0)
                     && MathUtils::isNegZero(orig)));
    }

#if defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64)

    // ignore warning that inline asm disables global optimization in this function
    #ifdef _MSC_VER
    #pragma warning(disable: 4740)
    #endif
    Atom AvmCore::doubleToAtom_sse2(double n)
    {
        // handle integer values w/out allocation
        // this logic rounds in the wrong direction for E3, but
        // we never use a rounded value, only cleanly converted values.
#if defined(WIN32) || defined(__ICC)
    #ifdef AVMPLUS_AMD64

        int32_t const intval = _mm_cvttsd_si32(_mm_set_sd(n));
        if (atomIsValidIntptrValueAndEqualTo(intval, n))
            return atomFromIntptrValue(intval);
        return allocDouble(n);

    #else // x86
        int id3;
        __asm {
            movsd xmm0,n
            cvttsd2si ecx,xmm0
            shl ecx,3       // intval<<3
            mov eax,ecx
            sar ecx,3       // intval>>3
            cvtsi2sd xmm1,ecx
            ucomisd xmm0,xmm1
            jne d2a_alloc   // < or >
            jp  d2a_alloc   // unordered
            mov id3,eax
        }

        if (id3 != 0 || !MathUtils::isNegZero(n))
        {
            return id3 | kIntptrType;
        }
        else
        {
            __asm d2a_alloc:
            return allocDouble(n);
        }
    #endif

#elif defined(_MAC)
        // MacTel always has SSE2 available
        int32_t const intval = _mm_cvttsd_si32(_mm_set_sd(n));
        if (atomIsValidIntptrValueAndEqualTo(intval, n))
            return atomFromIntptrValue(intval);

        return allocDouble(n);

#elif defined(SOLARIS)

        return AvmCore::doubleToAtom(n); // This needs to be optimized for solaris.

#elif defined(AVMPLUS_UNIX)

    #ifdef AVMPLUS_AMD64

        int32_t const intval = _mm_cvttsd_si32(_mm_set_sd(n));
        if (atomIsValidIntptrValueAndEqualTo(intval, n))
            return atomFromIntptrValue(intval);

        return allocDouble(n);

    #else // not AVMPLUS_AMD64
        int id3;
        asm("movups %1, %%xmm0;"
            "cvttsd2si %%xmm0, %%ecx;"
            "shl $0x3, %%ecx;"
            "mov %%ecx, %%eax;"
            "sar $0x3, %%ecx;"
            "cvtsi2sd %%ecx, %%xmm1;"
            "ucomisd %%xmm1, %%xmm0;"
            "jne d2a_alloc;"
            "jp d2a_alloc;"
            "movl %%eax, %0" : "=r" (id3) : "m" (n));

        if (id3 != 0 || !MathUtils::isNegZero(n))
        {
            return id3 | kIntptrType;
        }

        asm("d2a_alloc:");
        return allocDouble(n);
    #endif // AVMPLUS_AMD64
#endif // defined(AVMPLUS_UNIX)
    }
#endif // x86 or x64

#ifndef AVMPLUS_SSE2_ALWAYS
    Atom AvmCore::doubleToAtom(double n)
    {
        // There is no need for special logic for NaN or +/-Inf since we don't
        // ever test for those values in coreplayer.  As far as we're concerned
        // they are regular numeric values.

        // handle integer values w/out allocation
    #if defined(WIN32) && !defined(_ARM_)
        #ifdef AVMPLUS_AMD64
        int intval = _mm_cvttsd_si32(_mm_set_sd(n));
        #else
        // this logic rounds in the wrong direction for E3, but
        // we never use a rounded value, only cleanly converted values.
        int intval;
        _asm {
            fld [n];
            fistp [intval];
        }
        #endif
    #elif defined(_MAC) && (defined (AVMPLUS_IA32) || defined(AVMPLUS_AMD64))
        int intval = _mm_cvttsd_si32(_mm_set_sd(n));
    #else
        int intval = int32_t(n);
    #endif

        // make sure n is integer value that fits in 29 bits
        if (atomIsValidIntptrValueAndEqualTo(intval, n))
            return atomFromIntptrValue(intval);

        return allocDouble(n);
    }
#endif // not AVMPLUS_SSE2_ALWAYS

    Stringp AvmCore::newConstantStringLatin1(const char* s)
    {
        return String::createLatin1(this, s, String::Length(s), String::k8, true);
    }

    Stringp AvmCore::newStringLatin1(const char* s, int len)
    {
        return String::createLatin1(this, s, len);
    }

    Stringp AvmCore::newStringUTF8(const char* s, int len, bool strict)
    {
        return String::createUTF8(this, (const utf8_t*)s, len, String::kDefaultWidth, false, strict);
    }

    Stringp AvmCore::newStringUTF16(const wchar* s, int len, bool strict)
    {
        return String::createUTF16(this, s, len, String::kDefaultWidth, false, strict);
    }

    // "swap16" is apparently reserved/defined in some build environments...
    inline uint16_t avmSwap16(const uint16_t c)
    {
        const uint16_t hi = (c >> 8);
        const uint16_t lo = (c & 0xff);
        return (lo << 8) | hi;
    }

    Stringp AvmCore::newStringEndianUTF16(bool littleEndian, const wchar* s, int len, bool strict)
    {
    #ifdef VMCFG_LITTLE_ENDIAN
        const bool nativeIsLE = true;
    #else
        const bool nativeIsLE = false;
    #endif
        if (littleEndian == nativeIsLE)
        {
            return newStringUTF16(s, len, strict);
        }
        else
        {
            if (len < 0 && s != NULL)
                len = String::Length(s);

            if (s == NULL || len == 0)
                return this->kEmptyString;

            MMgc::GC::AllocaAutoPtr _swapped;
            wchar* swapped = (wchar*)avmStackAllocArray(this, _swapped, len, sizeof(wchar));
            for (int32_t i = 0; i < len; i++)
            {
                swapped[i] = avmSwap16(s[i]);
            }
            return newStringUTF16(swapped, len, strict);
        }
    }

    Stringp AvmCore::concatStrings(Stringp s1, Stringp s2)
    {
        if (!s1) s1 = knull;
        if (!s2) s2 = knull;
        return String::concatStrings(s1, s2);
    }

    Stringp AvmCore::intToString(int value)
    {
        return MathUtils::convertIntegerToStringBase10(this, value, MathUtils::kTreatAsSigned);
    }

    Stringp AvmCore::uintToString(uint32_t value)
    {
        return MathUtils::convertIntegerToStringBase10(this, value, MathUtils::kTreatAsUnsigned);
    }

    Stringp AvmCore::doubleToString(double d)
    {
        return MathUtils::convertDoubleToString(this, d, MathUtils::DTOSTR_NORMAL,15);
    }

#ifdef VMCFG_FLOAT
    Stringp AvmCore::floatToString(float f)
    {
        return doubleToString(f);                    
    }
    
    Stringp AvmCore::float4ToString(const float4_t& f)
    {
        float x = f4_x(f), y = f4_y(f), z = f4_z(f), w = f4_w(f);
        return floatToString(x)
            ->appendLatin1(",")
            ->append(floatToString(y))
            ->appendLatin1(",")
            ->append(floatToString(z))
            ->appendLatin1(",")
        ->append(floatToString(w));
    }
#endif
    
    StackTrace* AvmCore::newStackTrace()
    {
        if (callStack)
        {
            int depth = callStack->depth();
            StackTrace* stackTrace = StackTrace::create(GetGC(), depth);
            CallStackNode *curr = callStack;
            StackTrace::Element *element = stackTrace->elements;
            while (curr)
            {
                element->set(*curr);
                element++;
                curr = curr->next();
            }
            return stackTrace;
        }
        else if (currentBugCompatibility()->bugzilla619148) // Stack traces in release builds
        {
            int depth = 0;
            MethodFrame* methodFrame = currentMethodFrame;
            while (methodFrame != NULL) {
                if (methodFrame->env() != NULL)
                    depth++;
                methodFrame = methodFrame->next;
            }
            StackTrace* stackTrace = StackTrace::create(GetGC(), depth);
            StackTrace::Element *element = stackTrace->elements;
            methodFrame = currentMethodFrame;
            while (methodFrame != NULL && depth > 0) {
                if (methodFrame->env() != NULL) {
                    element->set(methodFrame->env()->method);
                    element++;
                    depth--;
                }
                methodFrame = methodFrame->next;
            }
            return stackTrace;
        }
        return NULL;
    }

    #ifdef DEBUGGER
    #ifdef _DEBUG
    void AvmCore::dumpStackTrace()
    {
        StringBuffer buffer(this);
        buffer << "Stack Trace:\n" << newStackTrace()->format(this) << '\n';
        AvmDebugMsg(false, (const char*)buffer.c_str());
    }
    #endif
    #endif /* DEBUGGER */

    /*static*/ int32_t AvmCore::integer(Atom atom)
    {
        const int kind = atomKind(atom);
        if (kind == kIntptrType)
        {
            AvmAssert(int32_t(atomGetIntptr(atom)) == (int32_t)integer_d(number(atom)));
            return int32_t(atomGetIntptr(atom));
        }
        else if (kind == kBooleanType)
        {
            return int32_t(atom>>3);
        }
        else
        {
            // TODO optimize the code below.
            return (int32_t)integer_d(number(atom));
        }
    }

#ifdef VMCFG_FLOAT
    /*static*/ float AvmCore::singlePrecisionFloat(Atom atom)
    {
        const int kind = atomKind(atom);
        if (kind == kSpecialBibopType )
        {
            if( atom == AtomConstants::undefinedAtom )
                return (float) MathUtils::kNaN;
            if(bibopKind(atom) == kBibopFloatType)
                return atomToFloat(atom);
            AvmAssert(bibopKind(atom) == kBibopFloat4Type);
            return (float) MathUtils::kNaN;
        }
        else
        {
            // TODO optimize the code below if needed
            return (float)number(atom);
        }
    }

    /*static*/ void AvmCore::float4(float4_t* retval, Atom atom)
    {
        
        if (isFloat4(atom))
        {
            *retval = atomToFloat4(atom);
            return;
        }

        float f = singlePrecisionFloat(atom);
        float4_t f4 = {f,f,f,f};
        *retval = f4;
    }
#endif // VMCFG_FLOAT

#ifndef AVMPLUS_SSE2_ALWAYS
    /*static*/ int AvmCore::integer_d(double d)
    {
        // Try a simple case first to see if we have a in-range float value

#if defined(AVMPLUS_IA32)
        // x86 int32_t(double) returns 0x80000000 if d is not in a valid integer range
        int intval = int32_t(d);
        if (intval != (int)0x80000000)
            return intval;
#elif defined(AVMPLUS_MIPS)
        // mips int32_t(double) returns 0x7fffffff if d is not in a valid integer range
        int intval = int32_t(d);
        if (intval != 0x7fffffff)
            return intval;
#elif defined(AVMPLUS_SPARC) || defined(AVMPLUS_ARM)
        // an int cast which should give:
        // +/-0.0:0 +/-nan:0 +/-ind:0 -inf:0x80000000 +inf:0x7fffffff den:0 >=0x7fffffff:x7fffffff <=0x80000000:0x80000000
        int intval = int32_t(d);
        if (intval != 0x7fffffff && intval != (int)0x80000000)
            return intval;
#endif

        return doubleToInt32(d);
    }
#endif // not AVMPLUS_SSE2_ALWAYS

#if defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64)
    int AvmCore::integer_d_sse2(double d)
    {
        int intval;
        #ifdef WIN32
        #ifdef AVMPLUS_AMD64
        intval = _mm_cvttsd_si32(_mm_set_sd(d));
        if (intval != (int)0x80000000)
            return intval;
        #else
        _asm {
            cvttsd2si eax,d
            mov intval,eax
        }
        if (intval != 0x80000000)
            return intval;
        #endif
        #elif defined(_MAC) && (defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64))
        intval = _mm_cvttsd_si32(_mm_set_sd(d));
        if (intval != (int)0x80000000)
            return intval;
        #elif defined(SOLARIS)
        #elif defined(AVMPLUS_UNIX)
        asm("movups %1, %%xmm0;"
            "cvttsd2si %%xmm0, %%eax;"
            "movl %%eax, %0" : "=r" (intval) : "m" (d) : "%eax");
        if (intval != (int) 0x80000000)
            return intval;
        #endif

        return doubleToInt32(d);
    }
#endif // AVMPLUS_IA32 or AVMPLUS_AMD64


#if !defined(AVMPLUS_IA32) && !defined(AVMPLUS_AMD64)
    int AvmCore::doubleToInt32(double d)
    {
        // From the ES3 spec, 9.5
        //  2.  If Result(1) is NaN, +0, -0, +Inf, or -Inf, return +0.
        //  3.  Compute sign(Result(1)) * floor(abs(Result(1))).
        //  4.  Compute Result(3) modulo 2^32; that is, a finite integer value k of Number
        //  type with positive sign and less than 2^32 in magnitude such the mathematical
        //  difference of Result(3) and k is mathematically an integer multiple of 2^32.
        //  5.  If Result(4) is greater than or equal to 2^31, return Result(4)- 2^32,
        //  otherwise return Result(4).

        // step 2
        if (MathUtils::isNaN(d) || MathUtils::isInfinite(d) || d == 0) {
            return 0;
        }

        // step 3 (round towards 0)
        double ad = d < 0.0 ? MathUtils::floor(-d) : MathUtils::floor(d);

        // step 4
        if (ad > 4294967295.0)
            ad = MathUtils::mod(ad,4294967296.0); // ad %= 0x10000000

        // step 5
        if (ad >= (double)2147483648.0)
        {
            // This case is a large negative number that overflows back to a positive
            // number.  This code has been tweaked to work on both Mac and Windows.  Mac
            // is particularly sensitive to edge numbers such as 0x80000000 when converting
            // doubles to ints).
            if (d < 0.0)
            {
                int intVal = int32_t (ad - 2147483648.0);
                return 0x80000000 - intVal;
            }
            // This case is a large positive number overflowing to negative.
            else
            {
                int intVal = int32_t (ad - 2147483648.0);
                return 0x80000000 + intVal;
            }
        }
        else
        {
            return int32_t(d < 0.0 ? -ad : ad);
        }
    }
#else
        // From the ES3 spec, 9.5
        //  2.  If Result(1) is NaN, +0, -0, +Inf, or -Inf, return +0.
        //  3.  Compute sign(Result(1)) * floor(abs(Result(1))).
        //  4.  Compute Result(3) modulo 2^32; that is, a finite integer value k of Number
        //  type with positive sign and less than 2^32 in magnitude such the mathematical
        //  difference of Result(3) and k is mathematically an integer multiple of 2^32.
        //  5.  If Result(4) is greater than or equal to 2^31, return Result(4)- 2^32,
        //  otherwise return Result(4).
#if defined(AVMPLUS_AMD64)
    #define DBLTOINT32_INT64 1
#else
    #define DBLTOINT32_INT64 0
#endif

    typedef union {
        double d;
        uint64_t i;
#if defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64)
        struct {
            uint32_t il, ih;
        } i32;
#else
#error("this routine does not work in PowerPC processors");
        struct {
            uint32_t ih, il;
        } i32;
#endif
    } double_int;

    #if DBLTOINT32_INT64
    int AvmCore::doubleToInt32(double d)
    {
        double_int du, duh, two32;
        uint64_t sign_d;
        int64_t MASK;
        uint32_t DI_H, u_tmp, expon, shift_amount;

        //  Algorithm Outline
        //  Step 1.  If d is NaN, +/-Inf or |d|>=2^84 or |d|<1, then return 0
        //  All of this is implemented based on an exponent comparison.
        //  Step 2.  If |d|<2^31, then return (int)d
        //  The cast to integer (conversion in RZ mode) returns the correct result.
        //  Step 3. If |d|>=2^32, d:=fmod(d, 2^32) is taken  -- but without a call
        //  Step 4. If |d|>=2^31, then the fractional bits are cleared before
        //  applying the correction by 2^32:  d - sign(d)*2^32
        //  Step 5.  Return (int)d

        du.d = d;
        DI_H = du.i32.ih;

        u_tmp = (DI_H & 0x7ff00000) - 0x3ff00000;
        if(u_tmp >= (0x45300000-0x3ff00000)) {
            // d is Nan, +/-Inf or +/-0, or |d|>=2^(32+52) or |d|<1, in which case result=0
            return 0;
        }

        if(u_tmp < 0x01f00000) {
            // |d|<2^31
            return int32_t(d);
        }

        if(u_tmp > 0x01f00000) {
            // |d|>=2^32
            expon = u_tmp >> 20;
            shift_amount = expon - 21;
            duh.i = du.i;
            MASK = 0x8000000000000000ll;
            MASK = MASK >> shift_amount;
            duh.i &= (uint64_t)MASK;
            du.d -= duh.d;
        }

        DI_H = du.i32.ih;

        // eliminate fractional bits
        u_tmp = (DI_H & 0x7ff00000);
        if(u_tmp >= 0x41e00000) {
            // |d|>=2^31
            expon = u_tmp >> 20;
            shift_amount = expon - (0x3ff - 11);
            MASK = 0x8000000000000000ll;
            MASK = MASK >> shift_amount;
            du.i &= (uint64_t)MASK;
            sign_d = du.i & 0x8000000000000000ull;
            two32.i = 0x41f0000000000000ull ^ sign_d;
            du.d -= two32.d;
        }

        return int32_t(du.d);
    }
    #else // DBLTOINT32_INT64
    int AvmCore::doubleToInt32(double d)
    {
        double_int du, duh, two32;
        uint32_t DI_H, u_tmp, expon, shift_amount;
        int32_t mask32;

        //  Algorithm Outline
        //  Step 1.  If d is NaN, +/-Inf or |d|>=2^84 or |d|<1, then return 0
        //  All of this is implemented based on an exponent comparison.
        //  Step 2.  If |d|<2^31, then return (int)d
        //  The cast to integer (conversion in RZ mode) returns the correct result.
        //  Step 3. If |d|>=2^32, d:=fmod(d, 2^32) is taken  -- but without a call
        //  Step 4. If |d|>=2^31, then the fractional bits are cleared before
        //  applying the correction by 2^32:  d - sign(d)*2^32
        //  Step 5.  Return (int)d

       du.d = d;
       DI_H = du.i32.ih;

       u_tmp = (DI_H & 0x7ff00000) - 0x3ff00000;
       if(u_tmp >= (0x45300000-0x3ff00000)) {
           // d is Nan, +/-Inf or +/-0, or |d|>=2^(32+52) or |d|<1, in which case result=0
           return 0;
       }

       if(u_tmp < 0x01f00000) {
           // |d|<2^31
           return int32_t(d);
       }

       if(u_tmp > 0x01f00000) {
           // |d|>=2^32
           expon = u_tmp >> 20;
           shift_amount = expon - 21;
           duh.i = du.i;
           mask32 = 0x80000000;
           if(shift_amount<32) {
               mask32 >>= shift_amount;
               duh.i32.ih = du.i32.ih & mask32;
               duh.i32.il = 0;
           }
           else {
               mask32 >>= (shift_amount-32);
               duh.i32.ih = du.i32.ih;
               duh.i32.il = du.i32.il & mask32;
           }
           du.d -= duh.d;
       }

        DI_H = du.i32.ih;

        // eliminate fractional bits
        u_tmp = (DI_H & 0x7ff00000);
        if(u_tmp >= 0x41e00000) {
            // |d|>=2^31
            expon = u_tmp >> 20;
            shift_amount = expon - (0x3ff - 11);
            mask32 = 0x80000000;
            if(shift_amount<32) {
                mask32 >>= shift_amount;
                du.i32.ih &= mask32;
                du.i32.il = 0;
            }
            else {
                mask32 >>= (shift_amount-32);
                du.i32.il &= mask32;
            }
            two32.i32.ih = 0x41f00000 ^ (du.i32.ih & 0x80000000);
            two32.i32.il = 0;
            du.d -= two32.d;
        }

        return int32_t(du.d);
    }
   #endif // DBLTOINT32_INT64
#endif // !(defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64))

    ApiVersion AvmCore::getApiVersionFromCallStack()
    {
        for (MethodFrame* f = currentMethodFrame; f != NULL; f = f->next)
        {
            CodeContext* cc = f->cc();
            if (cc)
            {
                // There is no user code on the stack, but we entered actionscript on behalf of some toplevel,
                // so use its version. There might be user code below an EnterCodeContext, but it would be
                // irrelevant because it represents an entirely different entry into the VM.
                ApiVersion apiVersion = cc->domainEnv()->toplevel()->abcEnv()->pool()->getApiVersion();
                AvmAssert(isValidApiVersion(apiVersion));
                return apiVersion;
            }
            else
            {
                MethodEnv* env = f->env();
                if (env && !env->method->pool()->isBuiltin)
                {
                    ApiVersion apiVersion = env->method->pool()->getApiVersion();
                    AvmAssert(isValidApiVersion(apiVersion));
                    return apiVersion;
                }
            }
        }
        ApiVersion const apiVersion = this->getDefaultAPI();
        AvmAssert(isValidApiVersion(apiVersion));
        return apiVersion;
    }

    bool AvmCore::isNamespaceVisibleToApiVersionFromCallStack(Namespace* ns)
    {
        return ns->getApiVersion() <= getApiVersionFromCallStack();
    }

    void AvmCore::setDxns(MethodFrame* f, String* internedUri)
    {
        Namespace* ns = newPublicNamespace(internedUri);
        f->setDxns(ns);
    }

    void AvmCore::setDxnsLate(MethodFrame* f, Atom uri)
    {
        String* internedUri = intern(uri);
        Namespace* ns = newPublicNamespace(internedUri);
        f->setDxns(ns);
    }

    Namespace* AvmCore::dxns() const
    {
        // if currentMethodFrame is null, then no AS3 code is executing -- we are being
        // called directly from builtin code. We really want to return a non-NULL namespace
        // in this case, so that native code that calls (say) XML::construct directly
        // can succeed (otherwise, getDefaultNamespace() would throw exceptions).
        if (!currentMethodFrame)
            return publicNamespace;

        // NULL is ok to return here -- Toplevel::getDefaultNamespace() will throw
        return MethodFrame::findDxns(currentMethodFrame);
    }

    // static
    Namespace* MethodFrame::findDxns(const MethodFrame* start)
    {
        for (const MethodFrame* f = start; f != NULL; f = f->next) {
            // explicit dxns set here? if so, it's the winner
            if (f->envOrCodeContext & DXNS_NOT_NULL)
                return f->dxns;

            // if not -- is this a frame with an env? if so, return its default ns, even if null.
            MethodEnv* env = f->env();
            if (env)
                return env->scope()->getDefaultNamespace();
        }
        return NULL;
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    /*static*/
    REALLY_INLINE void decr_atom(Atom const a)
    {
        // contrary to what you might think from the name, "kUnusedAtomTag" is in fact occasionally used
        // in player code, so be sure *not* to include it.
        int const RC_TYPE_MASK = (1 << kObjectType) | (1 << kStringType) | (1 << kNamespaceType);
        int const aKind = (1 << atomKind(a));
        if (aKind & RC_TYPE_MASK)
        {
            MMgc::RCObject* rcptr = (MMgc::RCObject*)atomPtr(a);
            if (rcptr)
                rcptr->DecrementRef();
        }
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    /*static*/
    REALLY_INLINE void incr_atom(MMgc::GC *gc, const void* container, Atom const a)
    {
        int const RC_TYPE_MASK = (1 << kObjectType) | (1 << kStringType) | (1 << kNamespaceType);
        int const GC_TYPE_MASK = RC_TYPE_MASK | (1 << kDoubleType) FLOAT_ONLY(| (1<< kSpecialBibopType)) ;

        // assume existing contents of address are potentially uninitialized,
        // so don't bother even making an assertion.
        int const aKind = (1 << atomKind(a));
        if (aKind & GC_TYPE_MASK  FLOAT_ONLY(&& a!=AtomConstants::undefinedAtom) ) 
        {
            if (aKind & RC_TYPE_MASK)
            {
                MMgc::RCObject* rcptr = (MMgc::RCObject*)atomPtr(a);
                if (rcptr)
                    rcptr->IncrementRef();
                // fall through to InlineWriteBarrierTrap()
            }
#ifdef DEBUG
            else
            {
                AvmAssert(!gc->IsRCObjectSafe(atomPtr(a)));
            }
#endif
            if (gc->BarrierActive())
                gc->InlineWriteBarrierTrap(container);
        }
#ifdef DEBUG
        else
        {
            // Its illegal for kUnusedAtomTag, kSpecialType and
            // kBooleanType to carry a GC pointer.
            AvmAssert(atomKind(a) == kIntptrType || !gc->IsPointerToGCPage(atomPtr(a)));
        }
#endif
    }

    /*static*/
    void AvmCore::decrementAtomRegion(Atom* arr, int length)
    {
        Atom* end = arr + length;
        while (arr < end)
        {
            decr_atom(*arr);
            *arr++ = 0;
        }
    }

    /*static*/
    void AvmCore::decrementAtomRegion_null(Atom* arr, int length)
    {
        Atom* end = arr + length;
        while (arr < end)
        {
            decr_atom(*arr);
            *arr++ = nullObjectAtom;
        }
    }

    /*static*/ void AvmCore::atomWriteBarrier(MMgc::GC *gc, const void *container, Atom *address, Atom atomNew)
    {
        decr_atom(*address);
        incr_atom(gc, container, atomNew);
        *address = atomNew;
    }

    /*static*/ void AvmCore::atomWriteBarrier_ctor(MMgc::GC *gc, const void *container, Atom *address, Atom atomNew)
    {
        incr_atom(gc, container, atomNew);
        *address = atomNew;
    }

    /*static*/ void AvmCore::atomWriteBarrier_dtor(Atom *address)
    {
        decr_atom(*address);
        *address = 0;
    }

#ifdef _DEBUG
    Atom AvmCore::genericObjectToAtom(const void* obj)
    {
        // We get a null obj here through ElementE4XNode::_insert
        if (obj != NULL)
        {
            GC* gc = GC::GetGC(obj);
            AvmAssert(!gc->IsRCObjectSafe(obj));
        }
        // return a non-RC atom, makes atomWriteBarrier do the right thing
        return (Atom)obj|kDoubleType;
    }
#endif

    void AvmCore::oom(MemoryStatus)
    {
        // on kReserve ditch native pages and switch from JIT to interpreter
        // on kEmpty ditch WORDCODE and switch to abc interpreter
    }

    CodeContext* AvmCore::codeContext() const
    {
        for (MethodFrame* f = currentMethodFrame; f != NULL; f = f->next)
        {
            // has someone overridden the CodeContext at this level (via EnterCodeContext)?
            CodeContext* cc = f->cc();
            if (cc)
                return cc;

            MethodEnv* env = f->env();
            if (env && !env->method->pool()->isBuiltin)
                return env->codeContext();
        }

        return NULL;
    }

    const BugCompatibility* AvmCore::currentBugCompatibility() const
    {
        CodeContext* cc = this->codeContext();
        if (cc != NULL)
            return cc->bugCompatibility();

        // If CodeContext is NULL, then we're not executing any user code,
        // so just use the BugCompatibility of the builtin pool...
        // which should always be builtinBugCompatibility.
        return builtinBugCompatibility;
    }

    void AvmCore::setStackLimit(uintptr_t p)
    {
        stack_limit = p;
        if (interrupted == NotInterrupted)
            minstack = p;
    }

#ifdef VMCFG_TELEMETRY_SAMPLER
    // Tells the sampler to take a stack sample. This is called only when
    // the sampler is enabled.
    void AvmCore::takeSample()
    {
        if (sampler) sampler->takeSample();
    }

    // A static wrapper for takeSample(), used by the jit.
    /* static */
    void AvmCore::takeSampleWrapper(AvmCore *theCore)
    {
        if (theCore->sampler) theCore->sampler->takeSample();
    }

    void AvmCore::enableSampler(telemetry::ISampler* pSampler) {
        sampler = pSampler;
        samplerEnabled = pSampler != 0;
        sampleTicks = 0;
    }

    void AvmCore::requestSample() {
        sampleTicks = 1;
    }

    void AvmCore::clearSampleRequest() {
        sampleTicks = 0;
    }

    bool AvmCore::isCallStackEmpty() {
        for (MethodFrame* frame = currentMethodFrame; frame != NULL; frame = frame->next) {
            MethodEnv* env = frame->env();
            if (env && env->method) {
                return false;
            }
        }
        return true;
    }

    unsigned int AvmCore::recordCallStack(telemetry::FunctionHandle* outFrameBuffer, unsigned int maxDepth)
    {
        unsigned int nFramesWritten = 0;

        for (MethodFrame* curFrame = currentMethodFrame;
             curFrame != NULL;
             curFrame = curFrame->next) {
            MethodEnv* env = curFrame->env();
            if (env && env->method) {
                if (nFramesWritten == maxDepth) {
                    // more stack frames than maxDepth.
                    // return 1 greater than the max length so we know there is more..
                    return ++nFramesWritten;
                } else {
                    outFrameBuffer[nFramesWritten++] = env->method;
                }
            }
        }

        return nFramesWritten;
    }

    avmplus::Stringp AvmCore::functionHandleToString(telemetry::FunctionHandle handle)
    {
        return ((MethodInfo*)handle)->getMethodName();
    }

    void AvmCore::GetStackTrace(StringBuffer &buffer)
    {
        for (MethodFrame* curFrame = currentMethodFrame;
             curFrame != NULL;
             curFrame = curFrame->next) {
            MethodEnv* env = curFrame->env();
            if (env && env->method) {
                avmplus::Stringp pStrFunc = functionHandleToString(env->method);
                avmplus::StUTF8String methodNameUtf8(pStrFunc);
                buffer << methodNameUtf8.c_str() << '\n';
            }
        }
    }
#endif

    /* static */
    void AvmCore::handleStackOverflowMethodEnv(MethodEnv* env)
    {
        handleStackOverflowToplevel(env->toplevel());
    }

    /* static */
    void AvmCore::handleStackOverflowToplevel(Toplevel* toplevel)
    {
        // this could be a real stack overflow, or an interrupt that
        // used the stack overflow handler as a way to take control of AS3.
        AvmCore *core = toplevel->core();
        if (core->interrupted) {
            AvmAssert(core->interrupted != SafepointPoll);
            handleInterruptToplevel(toplevel, true);
            // never returns
        }

        // invoke host's stack overflow handler
        core->stackOverflow(toplevel);
    }

#ifdef VMCFG_STACK_METRICS
    void AvmCore::initStackMetrics()
    {
        minStack = ~uintptr_t(0);
        maxStack = 0;
    }

    uintptr_t AvmCore::getMinStack()
    {
        return minStack;
    }
    
    uintptr_t AvmCore::getMaxStack()
    {
        return maxStack;
    }

    void AvmCore::recordStackPointer()
    {
        char* dummy;
        uintptr_t sp = (uintptr_t)&dummy;
        if (sp < minStack) minStack = sp;
        if (sp > maxStack) maxStack = sp;
    }

    // A static wrapper for recordStackPointer(), used by the jit.
    /* static */
    void AvmCore::recordStackPointerWrapper(AvmCore *theCore)
    {
        theCore->recordStackPointer();
    }
#endif

#ifdef VMCFG_EPOC_EMULATOR

    // Current (June 2010) Symbian emulator can not handle PCREContext because the constructor
    // calls OS methods, disable for emulator build.
    void AvmCore::checkPCREStackOverflow()
    {
    }
    void AvmCore::setPCREContext(Toplevel* /*env*/)
    {
    }
    void AvmCore::releasePCREContext()
    {
    }

#else

    static GCThreadLocal<Toplevel*> PCREContext;

    /* static */
    void AvmCore::checkPCREStackOverflow()
    {
        // Stopgap measure 2009-10-29:  The PCREContext can be NULL when host code (*cough* Flash Player *cough*)
        // calls directly into the PCRE engine.  We need to fix that, but for the moment we allow it to happen:
        // it's no worse than before.
        if (PCREContext != NULL)
            (PCREContext->core())->stackCheck(PCREContext);
    }

    /* static */
    void AvmCore::setPCREContext(Toplevel* env)
    {
        PCREContext = env;
    }

    /* static */
    void AvmCore::releasePCREContext()
    {
        PCREContext.destroy();
    }

#endif // VMCFG_EPOC_EMULATOR

    void AvmCore::clearInterrupt()
    {
        interrupted = NotInterrupted;
    }

    void AvmCore::raiseInterrupt(InterruptReason reason)
    {
        AvmAssert(reason != NotInterrupted);
        AvmAssert(reason != SafepointPoll); // Don't use this for safepoints
		// If this core is already suppose to be in a safepoint poll state, then pend the reason for
		// examination after the safepoint.
		if (interrupted == SafepointPoll) {
			pending_interrupt = reason;
		}
		else {
			interrupted = reason;
		}
	}

    /* static */
    void AvmCore::handleInterruptMethodEnv(MethodEnv *env)
    {
        handleInterruptToplevel(env->toplevel(), !env->method->isNonInterruptible());
    }

    /* static */
    void AvmCore::handleInterruptToplevel(Toplevel *toplevel, bool canUnwindStack)
    {
        AvmCore *core = toplevel->core();
        InterruptReason reason = core->interrupted;
        core->interrupted = NotInterrupted;
        if (reason == SafepointPoll)
        {
            SAFEPOINT_POLL();
            if (core->pending_interrupt != NotInterrupted && canUnwindStack) {
                reason = core->pending_interrupt;
                core->pending_interrupt = NotInterrupted;
                core->interrupt(toplevel, reason);
                // Doesn't return.
                AvmAssert(false);
            } else {
                // No pending interrupt, or there is one, but we can't unwind.
                // We'll unwind next time we get interrupted and can unwind.
                return; // This branch does return.
            }

        } else if (canUnwindStack) {
            // There might be a pending terminating interrupt but we don't care.
            core->pending_interrupt = NotInterrupted;
            core->interrupt(toplevel, reason);
            // interrupt() must not return from here!
            AvmAssert(false);
        } else {
            // We'll act on the interrupt next time we're in safepoint.
            core->pending_interrupt = reason;
            // Interrupted a noninterruptible method and it's not a safepoint poll point.
            // Ignore and return to caller, interrupt is pending.
        }
    }

    void AvmCore::addVersionedURIs(char const* const* uris)
    {
        while (*uris != NULL)
        {
            String* uri = internConstantStringLatin1(*uris++);
#ifdef _DEBUG
            if (m_unversionedURIs->get_ht()->get(uri->atom()) != undefinedAtom)
            {
                VMPI_log(StUTF8String(uri).c_str());
                AvmAssert(!"a URI that is used in unversioned form must not later be used in versioned form");
            }
#endif
            m_versionedURIs->get_ht()->add(uri->atom(), trueAtom);
        }
    }

    Namespacep AvmCore::findPublicNamespace()
    {
        ApiVersion apiVersion = this->getApiVersionFromCallStack();
        AvmAssert(isValidApiVersion(apiVersion));
        return publicNamespaces->nsAt(apiVersion);
    }

// NOTE: this is a temporary fix until OOM handling hooks in to Avm exceptions generally;
// see https://bugzilla.mozilla.org/show_bug.cgi?id=611078
#ifdef DEBUGGER
    AvmCoreAutoEnter::AvmCoreAutoEnter(AvmCore* core)
        : m_ef(GCHeap::GetGCHeap()->GetEnterFrame())
        , m_savedCore(core)
        , m_savedCallStack(core ? core->callStack : NULL)
    {
        if (m_ef)
            m_ef->AddAbortUnwindObject(this);
    }

    AvmCoreAutoEnter::~AvmCoreAutoEnter()
    {
        if (m_ef)
            m_ef->RemoveAbortUnwindObject(this);
    }

    void AvmCoreAutoEnter::Unwind()
    {
        if (m_savedCore)
            m_savedCore->callStack = m_savedCallStack;
    }
#endif


    // global helpers

    bool AvmCore::getBaseURIAndApiVersion(Stringp& uri, ApiVersion& apiVersion)
    {
        if (!uri->isEmpty())
        {
            int const idx = uri->length() - 1;
            int const mark = uri->charAt(idx);
            // if mark is not recognized as a valid version marker, then ignore it
            if (mark >= AvmCore::MIN_API_MARK &&
                mark < AvmCore::MIN_API_MARK + kApiVersion_count)
            {
                apiVersion = ApiVersion(mark - AvmCore::MIN_API_MARK);
                uri = (idx > 0) ? internSubstring(uri, 0, idx) : Stringp(kEmptyString);
#ifdef VMCFG_VERIFYALL
                // You can use -Dverifyonly to simply verify a SWF, but not execute any code;
                // as part of this you have to also specify avmglue.abc, eg
                //
                //      avm -Dverifyonly avmglue.abc something.swf
                //
                // The problem is that avmglue.abc contains version-marked URIs,
                // but avmshell doesn't know which of those are "supposed" to be versioned
                // (e.g., flash.debugger). For -Dverifyonly, however, it doesn't really
                // matter much, so rather than jump thru hoops to figure out the proper
                // URIs for this, just skip the assert.
                if (!config.verifyonly)
                {
                    AvmAssert(isVersionedURI(uri));
                }
#else
                AvmAssert(isVersionedURI(uri));
#endif
                return true;
            }
            // this is not the case: we can legitimately see unmarked URIs for "versioned uris".
            // AvmAssert(!isVersionedURI(uri));
        }
        return false;
    }

    Namespacep AvmCore::getVersionedNamespace(Namespacep ns, ApiVersion apiVersion)
    {
        if (ns->getType() != Namespace::NS_Public || !isVersionedURI(ns->getURI()))
            return ns;

        Namespacep ns2 = gotNamespace(ns->m_uriAndType, apiVersion);
        if (ns2 == NULL)
        {
            ns2 = internNamespace(newNamespace(ns->getURI(), ns->getType(), apiVersion));
        }

        return ns2;
    }

    bool AvmCore::isVersionedURI(Stringp uri)
    {
        AvmAssert(uri != NULL);
        AvmAssert(uri->isInterned());
        bool const r = m_versionedURIs->get_ht()->get(uri->atom()) != undefinedAtom;
#ifdef _DEBUG
        if (!r)
            m_unversionedURIs->get_ht()->add(uri->atom(), trueAtom);
#endif
        return r;
    }

    /*static*/ bool AvmCore::parseApiVersion(const char* p, ApiVersion& apiVersion, ApiVersionSeries& apiVersionSeries)
    {
        for (int i = 0; i < kApiVersion_count; ++i)
        {
            if (VMPI_strcmp(p, kApiVersionNames[i]) == 0)
            {
                apiVersion = ApiVersion(i);
                // This is not terribly efficient, but doesn't need to be.
                uint32_t m = kApiVersionSeriesMembership[i];
                apiVersionSeries = ApiVersionSeries(0);
                while ((m & 1) == 0)
                {
                    apiVersionSeries = ApiVersionSeries(int(apiVersionSeries) + 1);
                    m >>= 1;
                }
                return true;
            }
        }
        return false;
    }

    BugCompatibility::BugCompatibility(BugCompatibility::Version v)
    {
#ifdef DEBUG
        // Prevent the name table from getting out of sync with the enum.

        for ( int i=0 ; i < VersionCount ; i++)
            AvmAssert(kNames[i] != 0);
#endif

        // We rely on the fact that we are allocated pre-zeroed by MMgc,
        // thus ensuring that the default state of all compatibility bits is zero.
        // Thus we only need to set bits based on versions, we don't have to clear anything.

        // Primordial version did not fix any bugs (it only introduced them)
        // if (v >= kSWF9) { nothing to do }

        // kSWF10 is also a no-op, since the one bug that used to live here actually needs
        // to be controlled by a global (!) flag for backwards compatibility in this case.
        // see class BugCompatibility for more information.
        // if (v >= kSWF10) { nothing to do here, either }

        if (v >= kSWF11)
        {
            bugzilla444630  = 1;    // Entities are not escaped when appending String content to XML
            bugzilla456852a = 1;    // Different runtime error for invalid constructor call when -Ojit set
            bugzilla456852b = 1;    // Different runtime error for negative vector index when -Ojit set
            bugzilla504525  = 1;    // Vector.concat processes arguments in reverse order
            bugzilla513018  = 1;    // parseFloat accepts illegal number syntax
            bugzilla524122  = 1;    // Incorrect optimization for integers in numeric sort
            bugzilla526662  = 1;    // XMLParser stops at NUL char
            bugzilla539094  = 1;    // OSR (On-Stack replacement of interpreted methods by JIT-ted methods)
            bugzilla551587  = 1;    // MathClass:_min() does not correctly handle -0
            bugzilla558863  = 1;    // in operator on bytearray throws exception for non-natural number
            bugzilla585791  = 1;    // String.localeCompare with a null String object returns 0
            bugzilla598683  = 1;    // Bad XML with unterminated node with namespace not throwing correctly
            bugzilla609416  = 1;    // encodeURIComponent and decodeURIComponent give wrong output when input contains surrogate pairs
        }

        // if (v >= kSWF12) { nothing to do here }

        if (v >= kSWF13)    /* Serrano */
        {
            bugzilla532454  = 1;    // Sort incorrectly converts result of user-supplied comparison function to integer
            bugzilla574600  = 1;    // Vector.<>.AS3::map returns no value
            bugzilla654807  = 1;    // sealed subclasses of Array act sort-of-dynamic
        }

        if (v >= kSWF14) /* Anza, tentative! */
        {
            bugzilla678952 = 1;     // Operations on Vector.<C> do not make use of or provide information about C
        }

        if (v >= kSWF15)    /* Brannan */
        {
            bugzilla513039 = 1;     // Number.toFixed(0) returns incorrect numbers, rounding issues
            bugzilla661330 = 1;     // Array.length behavior doesn't follow ECMA262 near 2^32-1
        }

        if (v >= kSWF16)    /* Cyril */
        {
        }

        if (v >= kSWF17)    /* Dolores */
        {
        }

        if (v >= kSWF18)    /* Ellis */
        {
            bugzilla619148 = 1;     // Stack traces in release builds
        }
        
        if (v >= kSWF19)    /* Folsom */
        {
        }
    }

    /*static*/ uint32_t const BugCompatibility::kNames[BugCompatibility::VersionCount] =
    {
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
		18,
		19
    };
}
