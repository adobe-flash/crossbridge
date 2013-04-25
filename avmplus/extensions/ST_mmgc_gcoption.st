// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category gcoption

// Bug 661286: MMgc generic option parsing
%%prefix
using namespace MMgc;

%%decls
    bool m_ret;
    bool m_wrong;
    GCHeap *m_heap;
    GCHeapConfig m_config_orig;
    bool isParamOption(const char* line) {
        return m_heap->config.IsGCOptionWithParam(line);
    }
    bool notParamOption(const char* line) {
        return !isParamOption(line);
    }
    bool approxEqual(double x, double y) {
        double delta1 = x - y;
        // abs() does not produce correct results at e.g. DBL_MAX.
        double delta2 = (delta1 > 0) ? delta1 : -delta1;
        return (delta2 < 0.0001);
    }
    void parseApply(const char* line, const char* optExtra=0)
    {
        m_ret = m_heap->config.ParseAndApplyOption(line, m_wrong, optExtra);
    }
    bool parsedCorrectly() {
        return m_ret && !m_wrong;
    }
    bool gcoptionButIncorrectFormat() {
        return m_ret && m_wrong;
    }

    void saveOrigHeapConfig() {
        memcpy(&m_config_orig, &m_heap->config, sizeof(GCHeapConfig));
    }

    void restoreHeapConfig() {
        memcpy(&m_heap->config, &m_config_orig, sizeof(GCHeapConfig));
    }

    bool configUnchanged() {
        const char* c1 = (const char*)&m_heap->config;
        const char* c2 = (const char*)&m_config_orig;
        bool noChangeSeen = true;
        for (size_t i=0; i < sizeof(GCHeapConfig); i++) {
            if (*c1 != *c2)
                noChangeSeen = false;
        }
        return noChangeSeen;
    }

%%prologue
{
    m_heap = GCHeap::GetGCHeap();
    saveOrigHeapConfig();
}

%%epilogue
{
    restoreHeapConfig();
}

%%test detect_parameterized_options
{
    %%verify isParamOption("-memlimit")
    %%verify isParamOption("-load")
    %%verify isParamOption("-loadCeiling")
    %%verify isParamOption("-gcwork")
    %%verify isParamOption("-gcstack")

    %%verify notParamOption("-memlimit=10")
    %%verify notParamOption("-load 1.5")
    %%verify notParamOption("-loadCeiling 1.5")
    %%verify notParamOption("-gcwork 1.5")
    %%verify notParamOption("-gcstack 10")

    %%verify notParamOption("-not_an_option_and_never_will_be")
    %%verify notParamOption("-not_an_option_and_never_will_be 10")
    %%verify notParamOption("-not_an_option_and_never_will_be=10")
          ;
}

%%test parse_memstats
{
    // sanity checks:
    // - make sure our configUnchanged check is sane
    // - make sure our restoreHeapConfig works.
    %%verify configUnchanged()
    memset(&m_heap->config, 0xfe, sizeof(GCHeapConfig));
    %%verify !configUnchanged()
    restoreHeapConfig();
    %%verify configUnchanged()
          ;

    parseApply("-memstats");
    %%verify parsedCorrectly()
    %%verify m_heap->config.gcstats && m_heap->config.autoGCStats
          ;
    restoreHeapConfig();

    parseApply("-memstats-verbose");
    %%verify parsedCorrectly()
    %%verify m_heap->config.gcstats && m_heap->config.autoGCStats && m_heap->config.verbose
          ;
    restoreHeapConfig();
}
%%test parse_memlimit
{
    parseApply("-memlimit   10");
    %%verify parsedCorrectly()
    %%verify m_heap->config.heapLimit == 10
          ;
    restoreHeapConfig();

    parseApply("-memlimit=11");
    %%verify parsedCorrectly()
    %%verify m_heap->config.heapLimit == 11
          ;
    restoreHeapConfig();

    parseApply("-memlimit = 12");
    %%verify parsedCorrectly()
    %%verify m_heap->config.heapLimit == 12
          ;
    restoreHeapConfig();

    parseApply("-memlimit");
    %%verify gcoptionButIncorrectFormat()
    %%verify m_heap->config.heapLimit == m_config_orig.heapLimit
          ;
    restoreHeapConfig();

    parseApply("-memlimit", "13");
    %%verify parsedCorrectly()
    %%verify m_heap->config.heapLimit == 13
          ;
    restoreHeapConfig();
}
%%test parse_gcbehavior_gcsummary_eagersweep
{
#ifdef MMGC_POLICY_PROFILING
    parseApply("-gcbehavior");
    %%verify parsedCorrectly()
    %%verify (m_heap->config.gcbehavior == 2)
          ;
    restoreHeapConfig();

    parseApply("-gcsummary");
    %%verify parsedCorrectly()
    %%verify (m_heap->config.gcbehavior == 1)
          ;
    restoreHeapConfig();
#endif

    parseApply("-eagersweep");
    %%verify parsedCorrectly()
    %%verify m_heap->config.eagerSweeping
          ;
    restoreHeapConfig();
}
%%test parse_load_gcwork
{
    parseApply("-load 7.0");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcLoad[0], 7.0)
          ;
    restoreHeapConfig();

    // test load with '<space><param>'
    parseApply("-load 6.0,10,5.0");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcLoad[0], 6.0)
    %%verify approxEqual(m_heap->config.gcLoadCutoff[0], 10.0)
    %%verify approxEqual(m_heap->config.gcLoad[1], 5.0)
          ;
    restoreHeapConfig();

    // test load with separate <param>
    parseApply("-load", "8.0,20.5,7.0");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcLoad[0], 8.0)
    %%verify approxEqual(m_heap->config.gcLoadCutoff[0], 20.5)
    %%verify approxEqual(m_heap->config.gcLoad[1], 7.0)
          ;
    restoreHeapConfig();

    // test load with '=<param>'
    parseApply("-load=10.0,30.5,9.0");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcLoad[0], 10.0)
    %%verify approxEqual(m_heap->config.gcLoadCutoff[0], 30.5)
    %%verify approxEqual(m_heap->config.gcLoad[1], 9.0)
          ;
    restoreHeapConfig();

    // Max load pairs is 7
    parseApply("-load 1.5,1.5,2,2,3,3,4,4,5,5,6,6,7,7,8,8");
    %%verify gcoptionButIncorrectFormat()
    %%verify configUnchanged()
    restoreHeapConfig();

    // Ensure that the last load value is ignored
    parseApply("-load=10.0,30.0,9.0,60.0");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcLoad[0], 10.0)
    %%verify approxEqual(m_heap->config.gcLoadCutoff[0], 30.0)
    %%verify approxEqual(m_heap->config.gcLoad[1], 9.0)
    %%verify !approxEqual(m_heap->config.gcLoadCutoff[1], 60.0)
          ;
    restoreHeapConfig();

    parseApply("-load");
    %%verify gcoptionButIncorrectFormat()
    %%verify configUnchanged()
          ;
    restoreHeapConfig();

    // L (load) must be > 1
    parseApply("-load 1,30");
    %%verify gcoptionButIncorrectFormat()
    %%verify configUnchanged()
          ;
    restoreHeapConfig();

    parseApply("-load badvalue");
    %%verify gcoptionButIncorrectFormat()
    %%verify configUnchanged()
          ;
    restoreHeapConfig();


    parseApply("-loadCeiling 11.5");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcLoadCeiling, 11.5)
          ;
    restoreHeapConfig();

    parseApply("-gcwork 12.5");
    %%verify gcoptionButIncorrectFormat()
          ;
    restoreHeapConfig();

    parseApply("-gcwork 0.123456");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcEfficiency, .123456)
          ;
    restoreHeapConfig();

    parseApply("-gcwork=0.23456");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcEfficiency, .23456)
          ;
    restoreHeapConfig();

    parseApply("-gcwork", "0.3456");
    %%verify parsedCorrectly()
    %%verify approxEqual(m_heap->config.gcEfficiency, .3456)
          ;
    restoreHeapConfig();

}
