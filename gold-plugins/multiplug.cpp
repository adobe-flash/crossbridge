// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "plugin-api.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <limits.h>
#include <dlfcn.h>

namespace
{
static const char *PLUGIN_NAME = "multiplug";

ld_plugin_status discard_message(int level, const char *format, ...)
{
    // Die loudly. Recent versions of Gold pass ld_plugin_message as the first
    // callback in the transfer vector. This should never be called.
    abort();
}

ld_plugin_add_symbols add_symbols = NULL;
ld_plugin_get_symbols get_symbols = NULL;
ld_plugin_add_input_file add_input_file = NULL;
ld_plugin_add_input_library add_input_library = NULL;
ld_plugin_set_extra_library_path set_extra_library_path = NULL;
ld_plugin_message message = discard_message;

std::string sysroot = "";
std::string libext = SOEXT;
std::vector<void*> pluginHandles;
std::vector<ld_plugin_claim_file_handler> claimFileHandlers;
std::vector<ld_plugin_cleanup_handler> cleanupHandlers;
std::vector<ld_plugin_all_symbols_read_handler> allSymbolsReadHandlers;
}

static ld_plugin_status claim_file_hook(const ld_plugin_input_file *file, int *claimed);
static ld_plugin_status all_symbols_read_hook(void);
static ld_plugin_status cleanup_hook(void);
extern "C" ld_plugin_status onload(ld_plugin_tv *tv);

ld_plugin_status onload(ld_plugin_tv *_tv)
{
    if(sysroot.length() == 0) {
        sysroot = getenv("FLASCC_INTERNAL_SDK_ROOT");
    }

    ld_plugin_get_claim_file get_claim_file;
    ld_plugin_get_all_symbols_read get_all_symbols_read;
    ld_plugin_get_cleanup get_cleanup;
    for (ld_plugin_tv *tv = _tv; tv->tv_tag != LDPT_NULL; ++tv) {
        switch (tv->tv_tag) {
        case LDPT_GET_CLAIM_FILE_HOOK:
            get_claim_file = tv->tv_u.tv_get_claim_file;
            break;
        case LDPT_GET_ALL_SYMBOLS_READ_HOOK:
            get_all_symbols_read = tv->tv_u.tv_get_all_symbols_read;
            break;
        case LDPT_GET_CLEANUP_HOOK:
            get_cleanup = tv->tv_u.tv_get_cleanup;
            break;
        }
    }

    std::vector<std::string> plugins;
    plugins.push_back(sysroot + "/usr/lib/LLVMgold" + libext);
    plugins.push_back(sysroot + "/usr/lib/makeswf" + libext);

    for(int i=0; i<2; i++) {
        void *handle = dlopen(plugins[i].c_str(), RTLD_NOW);
        if(!handle) {
            fprintf(stderr, "Failed to load plugin %s: %s\n", plugins[i].c_str(), dlerror());
            abort();
        }
        ld_plugin_onload plugin = (ld_plugin_onload)dlsym(handle, "onload");
        plugin(_tv);
        pluginHandles.push_back(handle);

        ld_plugin_claim_file_handler cfh = get_claim_file();
        if(cfh) {
            claimFileHandlers.push_back(cfh);
        }
        ld_plugin_all_symbols_read_handler asrh = get_all_symbols_read();
        if(asrh) {
            allSymbolsReadHandlers.push_back(asrh);
        }
        ld_plugin_cleanup_handler ch = get_cleanup();
        if(ch) {
            cleanupHandlers.push_back(ch);
        }
    }

    bool registeredClaimFile = false;
    for (ld_plugin_tv *tv = _tv; tv->tv_tag != LDPT_NULL; ++tv) {
        switch (tv->tv_tag) {
        case LDPT_REGISTER_CLAIM_FILE_HOOK: {
            ld_plugin_register_claim_file callback = tv->tv_u.tv_register_claim_file;
            if ((*callback)(claim_file_hook) != LDPS_OK) {
                return LDPS_ERR;
            }
            registeredClaimFile = true;
        }
        break;
        case LDPT_REGISTER_ALL_SYMBOLS_READ_HOOK: {
            ld_plugin_register_all_symbols_read callback = tv->tv_u.tv_register_all_symbols_read;
            if ((*callback)(all_symbols_read_hook) != LDPS_OK) {
                return LDPS_ERR;
            }
        }
        break;
        case LDPT_REGISTER_CLEANUP_HOOK: {
            ld_plugin_register_cleanup callback = tv->tv_u.tv_register_cleanup;
            if ((*callback)(cleanup_hook) != LDPS_OK) {
                return LDPS_ERR;
            }
        }
        break;
        case LDPT_ADD_SYMBOLS:
            add_symbols = tv->tv_u.tv_add_symbols;
            break;
        case LDPT_GET_SYMBOLS:
            get_symbols = tv->tv_u.tv_get_symbols;
            break;
        case LDPT_ADD_INPUT_FILE:
            add_input_file = tv->tv_u.tv_add_input_file;
            break;
        case LDPT_ADD_INPUT_LIBRARY:
            add_input_library = tv->tv_u.tv_add_input_file;
            break;
        case LDPT_SET_EXTRA_LIBRARY_PATH:
            set_extra_library_path = tv->tv_u.tv_set_extra_library_path;
            break;
        case LDPT_MESSAGE:
            message = tv->tv_u.tv_message;
            break;
        default:
            break;
        }
    }

    if (!registeredClaimFile) {
        (*message)(LDPL_ERROR, "register_claim_file not passed to %s.", PLUGIN_NAME);
        return LDPS_ERR;
    }
    if (!add_symbols) {
        (*message)(LDPL_ERROR, "add_symbols not passed to %s.", PLUGIN_NAME);
        return LDPS_ERR;
    }

    return LDPS_OK;
}

static ld_plugin_status claim_file_hook(const ld_plugin_input_file *file, int *claimed)
{
    for(int i=0; i<claimFileHandlers.size(); i++) {
        ld_plugin_status s = claimFileHandlers[i](file, claimed);
        if (*claimed) {
            return s;
        }
    }
    return LDPS_OK;
}

static ld_plugin_status all_symbols_read_hook(void)
{
    for(int i=0; i<allSymbolsReadHandlers.size(); i++) {
        ld_plugin_status s = allSymbolsReadHandlers[i]();
    }
    return LDPS_OK;
}

static ld_plugin_status cleanup_hook(void)
{
    for(int i=0; i<cleanupHandlers.size(); i++) {
        ld_plugin_status s = cleanupHandlers[i]();
    }
    return LDPS_OK;
}
