/*
ADOBE SYSTEMS INCORPORATED
Copyright 2012 Adobe Systems Incorporated
All Rights Reserved.

NOTICE: Adobe permits you to use, modify, and distribute this file
in accordance with the terms of the license agreement accompanying it.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "plugin-api.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <limits.h>
#include <sys/wait.h>
#include <arpa/inet.h> // htonl
#include <pthread.h>

#include "SetAlchemySDKLocation.c"

int execjava(std::vector<std::string> &args)
{
    char ** newargs = (char **)malloc((args.size()+1) * sizeof(char*));
    newargs[args.size()] = NULL;
    for(int i=0; i<args.size(); i++) {
        newargs[i] = (char*)args[i].c_str();
#ifdef __CYGWIN__
        if(newargs[i][0] == '/') {
            newargs[i] = javapath(newargs[i]);
        }
#endif
    }
    return execvp("java", (char* const*)newargs);
}

namespace
{
static const char *PLUGIN_NAME = "makeswf";

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

int api_version = 0;
int gold_version = 0;

struct claimed_file {
    void *handle;
    char *contents;
    uint32_t contentsLength;
    std::vector<ld_plugin_symbol> syms;
};

#ifndef DEFTMPDIR
#define DEFTMPDIR "/tmp"
#endif

std::string output_name = "";
std::string tmpdir = DEFTMPDIR;
std::string sysroot = "";
std::string swfpreloader = "";
std::vector<std::string> jvmopts;
std::list<claimed_file> abcs;
std::vector<std::string> appended_abcs;
bool enablevmargs, appendabcs, make_projector, make_swc, swf_ns, all_symbols_read = false;
std::string swc_namespace, swf_namespace;
std::map<uint16_t, const std::string> symbolclass_entries;
bool enable_debugger = false, disable_telemetry = false, use_pthreads = false;
bool run_swfresolve = true;
std::vector<std::string> vmargs;
uint32_t swf_width = 1024, swf_height = 512, swf_version = 18;
}

namespace options
{
// Additional options to pass into the code generator.
// Note: This array will contain all plugin options which are not claimed
// as plugin exclusive to pass to the code generator.
// For example, "generate-api-file" and "as"options are for the plugin
// use only and will not be passed.
static std::vector<std::string> extra;

static void process_plugin_option(const char* opt_)
{
    if (opt_ == NULL) {
        return;
    }
    static bool saw_appendabc = false;
    static bool saw_preloader = false;
    
    std::string opt(opt_);
    if (opt.find("sysroot=") == 0) {
        sysroot = opt.substr(strlen("sysroot="));
    } else if (opt == "disablevmargs") {
        enablevmargs = false;
    } else if (opt.find("vmarg=") == 0) {
        vmargs.push_back(opt.substr(6));
    } else if (opt == "enabledebugger") {
        enable_debugger = true;
    } else if (opt == "disable-telemetry") {
        disable_telemetry = true;
    } else if (opt.find("jvmopt=") == 0) {
        jvmopts.push_back(opt.substr(strlen("jvmopt=")));
    } else if (opt == "swfpreloader") {
        saw_preloader = true;
    } else if (opt == "pthread") {
        use_pthreads = true;
    } else if (opt == "disableprojector") {
        make_projector = false;
    } else if (opt.find("emit-swc=") == 0) {
        make_swc = true;
	swc_namespace = opt.substr(strlen("emit-swc="));
    } else if (opt.find("swf-ns=") == 0) {
        swf_ns = true;
	swf_namespace = opt.substr(strlen("swf-ns="));
    } else if (opt.find("swf-version=") == 0) {
        swf_version = atoi(opt.substr(strlen("swf-version=")).c_str());
    } else if (opt.find("symbolclass=") == 0) {
        std::string spec = opt.substr(strlen("symbolclass="));
        size_t colon = spec.find(':');

        if(colon != std::string::npos && colon > 0) {
            unsigned tagID = (unsigned)atoi(spec.substr(0, colon).c_str());
            uint16_t tagID16 = (uint16_t)tagID;

            if(tagID != tagID16) {
                (*message)(LDPL_WARNING, "symbolclass tag ID %d will be truncated", tagID);
            }

            if(symbolclass_entries.find(tagID16) != symbolclass_entries.end()) {
                (*message)(LDPL_WARNING, "symbolclass tag ID %d redefined", tagID);
            }

            std::string clazz = spec.substr(colon+1);

            symbolclass_entries.insert(std::make_pair(tagID16, clazz));
        } else {
            (*message)(LDPL_WARNING, "Couldn't parse symbolclass specifier: '%s'", spec.c_str());
        }
    } else if (opt.find("swf-size=") == 0) {
        std::string spec = opt.substr(strlen("swf-size="));
        size_t xpos = spec.find('x');

        if(xpos != std::string::npos && xpos > 0) {
            swf_width = (unsigned)atoi(spec.substr(0, xpos).c_str());
            swf_height = (unsigned)atoi(spec.substr(xpos+1).c_str());
        } else {
            (*message)(LDPL_WARNING, "Couldn't parse swf size specifier: '%s'", spec.c_str());
        }
    } else if (opt.find("appendabc") == 0) {
        saw_appendabc = true;
    } else if (saw_appendabc) {
        appended_abcs.push_back(opt);
        saw_appendabc = false;
    } else if (saw_preloader) {
        swfpreloader = opt;
        saw_preloader = false;
    }
}
}

static ld_plugin_status claim_file_hook(const ld_plugin_input_file *file,
                                        int *claimed);
static ld_plugin_status all_symbols_read_hook(void);
static ld_plugin_status cleanup_hook(void);

extern "C" ld_plugin_status onload(ld_plugin_tv *tv);
ld_plugin_status onload(ld_plugin_tv *tv)
{
    // We're given a pointer to the first transfer vector. We read through them
    // until we find one where tv_tag == LDPT_NULL. The REGISTER_* tagged values
    // contain pointers to functions that we need to call to register our own
    // hooks. The others are addresses of functions we can use to call into gold
    // for services.

    if(sysroot.length() == 0) {
        sysroot = getenv("FLASCC_INTERNAL_SDK_ROOT");
    }

    enablevmargs = true;
    appendabcs = false;
    make_projector = true;
    make_swc = false;
    swf_ns = false;
    bool registeredClaimFile = false;

    for (; tv->tv_tag != LDPT_NULL; ++tv) {
        switch (tv->tv_tag) {
        case LDPT_API_VERSION:
            api_version = tv->tv_u.tv_val;
            break;
        case LDPT_GOLD_VERSION:  // major * 100 + minor
            gold_version = tv->tv_u.tv_val;
            break;
        case LDPT_OUTPUT_NAME:
            output_name = tv->tv_u.tv_string;
            break;
        case LDPT_LINKER_OUTPUT:
            switch (tv->tv_u.tv_val) {
            case LDPO_REL:  // .o
            case LDPO_DYN:  // .so
                break;
            case LDPO_EXEC:  // .exe
                break;
            default:
                (*message)(LDPL_ERROR, "Unknown output file type %d",
                           tv->tv_u.tv_val);
                return LDPS_ERR;
            }
            // TODO: add an option to disable PIC.
            //output_type = LTO_CODEGEN_PIC_MODEL_DYNAMIC_NO_PIC;
            break;
        case LDPT_OPTION:
            options::process_plugin_option(tv->tv_u.tv_string);
            break;
        case LDPT_REGISTER_CLAIM_FILE_HOOK: {
            ld_plugin_register_claim_file callback;
            callback = tv->tv_u.tv_register_claim_file;

            if ((*callback)(claim_file_hook) != LDPS_OK) {
                return LDPS_ERR;
            }

            registeredClaimFile = true;
        }
        break;
        case LDPT_REGISTER_ALL_SYMBOLS_READ_HOOK: {
            ld_plugin_register_all_symbols_read callback;
            callback = tv->tv_u.tv_register_all_symbols_read;

            if ((*callback)(all_symbols_read_hook) != LDPS_OK) {
                return LDPS_ERR;
            }

        }
        break;
        case LDPT_REGISTER_CLEANUP_HOOK: {
            ld_plugin_register_cleanup callback;
            callback = tv->tv_u.tv_register_cleanup;

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
        case LDPT_SET_ALWAYS_RESCAN:
            tv->tv_u.tv_set_always_rescan(); // call the function to set the flag
            break;
        case LDPT_MESSAGE:
            message = tv->tv_u.tv_message;
            break;
        default:
            break;
        }
    }

    if(use_pthreads && swf_version < 18) {
        (*message)(LDPL_WARNING, "SWF version must be at least 18 to use pthreads, your SWF version will be forced to 18.");
        swf_version = 18;
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

// state for abcnm process running as a "server" -- handling multiple abcs
static pthread_mutex_t abcnmMutex = PTHREAD_MUTEX_INITIALIZER;
static int abcnmPID = -1;
static int abcnmOut[2];
static int abcnmIn[2];
static FILE *abcnmFIn;

        
static bool checkForJVMError(const std::string& str)
{
    return str.find("Error: Could not create the Java Virtual Machine") != std::string::npos ||
            str.find("Error occurred during initialization of VM") != std::string::npos;
}

// extract symbols from the abc
static ld_plugin_status extract_syms(std::vector<ld_plugin_symbol> *syms, const char *buf, size_t nbyte)
{
    if(pthread_mutex_lock(&abcnmMutex)) {
        (*message)(LDPL_ERROR, "Failed to lock abcnm mutex");
        return LDPS_ERR;
    }

    if(abcnmPID < 0) { // abcnm process not created yet
        if(pipe(abcnmOut) || pipe(abcnmIn)) {
            (*message)(LDPL_ERROR, "Failed to create pipes for abcnm");
            pthread_mutex_unlock(&abcnmMutex);
            return LDPS_ERR;
        }

        if((abcnmPID = fork()) < 0) {
            close(abcnmOut[0]);
            close(abcnmOut[1]);
            close(abcnmIn[0]);
            close(abcnmIn[1]);
            (*message)(LDPL_ERROR, "Failed to create abcnm fork");
            pthread_mutex_unlock(&abcnmMutex);
            return LDPS_ERR;
        }
        if(!abcnmPID) { // fork for exec-ing abcnm
            close(abcnmOut[1]);
            close(abcnmIn[0]);
            if(dup2(abcnmOut[0], 0) < 0 ||
                    dup2(abcnmIn[1], 1) < 0) {
                fprintf(stderr, "Failed to redirect IO for abcnm\n");
                exit(-1);
            }
            close(abcnmOut[0]);
            close(abcnmIn[1]);

            std::string cp = sysroot + "/usr/lib";
            std::string alctool = sysroot + "/usr/lib/alctool.jar";
            std::vector<std::string> args;
            args.push_back("java");
            if(!jvmopts.size())
              args.push_back("-Xmx1G");
            else for(unsigned int i=0; i<jvmopts.size(); i++) {
                args.push_back(jvmopts[i]);
            }
            args.push_back("-cp");
            args.push_back(cp);
            args.push_back("-jar");
            args.push_back(alctool);
            args.push_back("-abcnm");
            args.push_back("-s");
            int r = execjava(args);
            fprintf(stderr, "Failed to exec abcnm");
            exit(-1);
        }

        close(abcnmOut[0]);
        close(abcnmIn[1]);
        abcnmFIn = fdopen(abcnmIn[0], "r");
        if(!abcnmFIn) {
            close(abcnmIn[0]);
            close(abcnmOut[1]);
            abcnmPID = -1;
            (*message)(LDPL_ERROR, "Failed to fdopen abcnm output");
            pthread_mutex_unlock(&abcnmMutex);
            return LDPS_ERR;
        }
    }

    // write bytes!
    uint32_t size = htonl(nbyte);

    // write chunk size
    if(write(abcnmOut[1], &size, sizeof(size)) != sizeof(size)) {
        (*message)(LDPL_ERROR, "Failed to write abc size to abcnm");
        pthread_mutex_unlock(&abcnmMutex);
        return LDPS_ERR;
    }

    while(nbyte > 0) {
        ssize_t written = write(abcnmOut[1], buf, nbyte);

        if(written < 1) {
            (*message)(LDPL_ERROR, "Failed to write abc data to abcnm");
            pthread_mutex_unlock(&abcnmMutex);
            return LDPS_ERR;
        }
        nbyte -= written;
        buf += written;
    }

    size = 0;
    if(write(abcnmOut[1], &size, sizeof(size)) != sizeof(size)) {
        (*message)(LDPL_ERROR, "Failed to write abc eof to abcnm");
        pthread_mutex_unlock(&abcnmMutex);
        return LDPS_ERR;
    }

    syms->clear();

    char symEntry[16384];

    while(fgets(symEntry, sizeof(symEntry), abcnmFIn)) {
        if(!symEntry[0] || symEntry[0] == '\n' && !symEntry[1]) { // empty line... done!
            break;
        }

        // validate the format...
        // should be 9 spaces, one symbol type char, another space, and the non-zero length symbol
        bool fmtOk = true;

        for(int i = 0; i < 9; i++)
            if(symEntry[i] != ' ') {
                fmtOk = false;
            }
        if(fmtOk && !isalpha(symEntry[9])) {
            fmtOk = false;
        }
        if(fmtOk && symEntry[10] != ' ') {
            fmtOk = false;
        }
        if(fmtOk && symEntry[11] == 0) {
            fmtOk = false;
        }
        if(!fmtOk) {
            if(checkForJVMError(std::string(&symEntry[0]))) {
                (*message)(LDPL_ERROR, 
                    "Error: Unable to launch the Java Virtual Machine. \n"
                    "This usually means you have a 32bit JVM installed or have set your Java heap size too large.\n"
                    "Try lowering the Java heap size by passing \"-jvmopt=-Xmx1G\" to gcc/g++.");
            } else {
                (*message)(LDPL_ERROR, "Couldn't parse abcnm output: '%s'", symEntry);
            }
            while(fgets(symEntry, sizeof(symEntry), abcnmFIn)) {}
            pthread_mutex_unlock(&abcnmMutex);
            return LDPS_ERR;
        }

        char symType = symEntry[9];

        switch(symType) {
        case 't':
        case 'd':
            continue; // ld doesn't care about these
        }

        char *end = symEntry + strlen(symEntry) - 1;

        if(*end == '\n') {
            *end = 0;
        }

        syms->push_back(ld_plugin_symbol());

        ld_plugin_symbol *sym = &syms->back();

        sym->name = strdup(symEntry + 11);
        sym->version = NULL;
        sym->visibility = LDPV_DEFAULT;
        sym->size = 0;
        sym->comdat_key = NULL;
        sym->resolution = LDPR_UNKNOWN;

        switch(symType) { // symbol type
        default:
            (*message)(LDPL_WARNING, "Unrecognized symbol type '%c' (assuming definition)", symType);
            // fall through
        case 'T':
        case 'D':
            sym->def = LDPK_DEF;
            break;
        case 'U':
            sym->def = LDPK_UNDEF;
            break;
        case 'W':
            sym->def = LDPK_WEAKDEF;
            break;
        case 'C':
            sym->def = LDPK_COMMON;
            break;
        }
    }

    pthread_mutex_unlock(&abcnmMutex);
    return LDPS_OK;
}

/// claim_file_hook - called by gold to see whether this file is one that
/// our plugin can handle. We'll try to open it and register all the symbols
/// with add_symbol if possible.
static ld_plugin_status claim_file_hook(const ld_plugin_input_file *file, int *claimed)
{
    char *buf = (char*)malloc(file->filesize);
    if (!buf) {
        (*message)(LDPL_ERROR,
                   "Failed to allocate buffer for file of size: %d\n",
                   file->filesize);
        return LDPS_ERR;
    }

    if (file->offset) {
        // Gold has found what might be IR part-way inside of a file, such as
        // an .a archive.
        if (lseek(file->fd, file->offset, SEEK_SET) == -1) {
            (*message)(LDPL_ERROR,
                       "Failed to seek to archive member of %s at offset %d: %d\n",
                       file->name,
                       file->offset, strerror(errno));
            free(buf);
            return LDPS_ERR;
        }


        if (read(file->fd, buf, file->filesize) != file->filesize) {
            (*message)(LDPL_ERROR,
                       "Failed to read archive member of %s at offset %d: %s\n",
                       file->name,
                       file->offset,
                       strerror(errno));
            free(buf);
            return LDPS_ERR;
        }
    } else {
        lseek(file->fd, 0, SEEK_SET);
        read(file->fd, buf, file->filesize);
    }

    if(   file->filesize > 4 &&
            buf[0] == 0x10 &&
            buf[1] == 0x00 &&
            buf[2] == 0x2E &&
            buf[3] == 0x00) {
        *claimed = 1;
        claimed_file *cf = NULL;

        abcs.resize(abcs.size() + 1);
        cf = &abcs.back();

        //cf->name = file->name;
        cf->handle = file->handle;
        cf->contents = buf;
        cf->contentsLength = file->filesize;

        if(!all_symbols_read) {
            ld_plugin_status extractSymStatus = extract_syms(&cf->syms, buf, file->filesize);

            if(extractSymStatus != LDPS_OK) {
                free(buf);
                return extractSymStatus;
            }

            if (!cf->syms.empty()) {
                if ((*add_symbols)(cf->handle, cf->syms.size(), &cf->syms[0]) != LDPS_OK) {
                    (*message)(LDPL_ERROR, "Unable to add symbols!");
                    return LDPS_ERR;
                }
            }
        }

        return LDPS_OK;
    }

    // wasn't a valid abc file
    free(buf);
    return LDPS_OK;
}

static ld_plugin_status all_symbols_read_hook(void)
{
    all_symbols_read = true;
    /*
    fprintf(stderr, "all_symbols_read_hook\n");
    for (std::list<claimed_file>::iterator I = abcs.begin(), E = abcs.end(); I != E; ++I) {
        (*get_symbols)(I->handle, I->syms.size(), &I->syms[0]);
        for (unsigned i = 0, e = I->syms.size(); i != e; i++) {
            fprintf(stderr, "sym: %s %d\n", I->syms[i].name, I->syms[i].resolution);
        }
    }
    */
    return LDPS_OK;
}

char* readContents(const char *path, uint32_t &len)
{
    len = 0;
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) {
        return NULL;
    }

    f.seekg(0,std::ios::end);
    len = (uint32_t)f.tellg();
    f.seekg(0,std::ios::beg);

    char *buffer = new char[len];
    f.read(&buffer[0], len);
    return buffer;
}

class BitWriter
{
    char _accum;
    char _accumCount;
    std::string _buf;

public:
    BitWriter() : _accum(0), _accumCount(0) {}

    std::string bytes() const {
        std::string result = _buf;
        if(_accumCount) {
            result += (_accum << (8-_accumCount));
        }
        return result;
    }

    void write(int bit) {
        _accum <<= 1;
        _accum |= (bit ? 1 : 0);
        if(++_accumCount == 8) {
            _buf += _accum;
            _accumCount = 0;
            _accum = 0;
        }
    }

    void write(uint64_t val, int size) {
        while(size--) {
            write((val >> size) & 1);
        }
    }
};

static std::string rect(unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax)
{
    BitWriter bw;

    bw.write(31, 5);
    bw.write(xMin, 31);
    bw.write(xMax, 31);
    bw.write(yMin, 31);
    bw.write(yMax, 31);

    return bw.bytes();
}

static ld_plugin_status cleanup_hook(void)
{
    // clean up after abcnm
    if(!pthread_mutex_destroy(&abcnmMutex) && abcnmPID >= 0) {
        close(abcnmOut[1]);
        fclose(abcnmFIn);
        int dummy;
        wait(&dummy);
    }

    std::ofstream outputfile;
    if(make_projector) {
        outputfile.open(output_name.c_str(), std::ios::out | std::ios::binary);
        std::string avmshellpath;
        if(enable_debugger) {
            avmshellpath = sysroot + "/usr/bin/avmshell-release-debugger";
        } else {
            avmshellpath = sysroot + "/usr/bin/avmshell";
        }
        std::ifstream avmshell(avmshellpath.c_str(), std::ios::in | std::ios::binary);
        std::copy(
            std::istreambuf_iterator<char>(avmshell),
            std::istreambuf_iterator<char>( ),
            std::ostreambuf_iterator<char>(outputfile));
    }

    char resolveTmpPath[PATH_MAX];
    strcpy(resolveTmpPath,
           (tmpdir + "/swfresolve.in.XXXXXX").substr(0, PATH_MAX-1).c_str());
    mktemp(resolveTmpPath);

    std::ofstream swfStream;
    swfStream.open(resolveTmpPath, std::ios::out | std::ios::binary);

    int swfstart = outputfile.tellp();
    char swfHeader[8];
    swfHeader[0] = 'F';
    swfHeader[1] = 'W';
    swfHeader[2] = 'S';
    swfHeader[3] = swf_version;
    swfHeader[4] = 0x0;    // ignored; swfresolve doesn't care about size bytes
    swfHeader[5] = 0x0;
    swfHeader[6] = 0x0;
    swfHeader[7] = 0x0;
    swfStream.write(swfHeader, sizeof(swfHeader));

    std::string rectBytes = rect(0, 20*swf_width, 0, 20*swf_height);

    swfStream.write(rectBytes.data(), rectBytes.size());
    swfStream.put(30); // framerate
    swfStream.put(0);
    swfStream.put(1); // framecount
    swfStream.put(0);

    for(int i=0; i<appended_abcs.size(); i++) {
        abcs.resize(abcs.size() + 1);
        claimed_file *cf = &abcs.back();
        cf->contents = readContents(appended_abcs[i].c_str(), cf->contentsLength);
        if(cf->contentsLength == 0) {
            fprintf(stderr, "Can't append empty ABC file: '%s'\n", appended_abcs[i].c_str());
            abort();
        }
    }

    char fileattributesRecord[10];
    memset(fileattributesRecord, 0, sizeof(fileattributesRecord));
    *(uint16_t*)&fileattributesRecord[0] = (69 << 6) | 63;
    *(uint32_t*)&fileattributesRecord[2] = 4;
    *(uint32_t*)&fileattributesRecord[6] = 8; // AS3
    swfStream.write(fileattributesRecord, sizeof(fileattributesRecord));

    if(!disable_telemetry) {
        char EnableTelemetry[4] = {};
        *(uint16_t*)&EnableTelemetry[0] = (93 << 6) | 2;
        *(uint16_t*)&EnableTelemetry[2] = 0;
        swfStream.write(EnableTelemetry, sizeof(EnableTelemetry));
    }

    //printf("filling swf with %d abcs\n", (int)abcs.size());
    int abccount = 0;
    for (std::list<claimed_file>::iterator I = abcs.begin(), E = abcs.end(); I != E; ++I) {
        claimed_file &cf = *I;
        std::ostringstream abcnamebuilder;
        abcnamebuilder << "ABC" << abccount++;
        std::string abcname = abcnamebuilder.str();

        char doABCHeader[10];
        *(uint16_t*)&doABCHeader[0] = (82 << 6) | 63; // DoABC, extended length
        *(uint32_t*)&doABCHeader[2] = (cf.contentsLength + 4 + abcname.length() + 1);
        *(uint32_t*)&doABCHeader[6] = 0; // flags

        swfStream.write(doABCHeader, sizeof(doABCHeader));
        swfStream.write(abcname.c_str(), abcname.length());

        char nullbyte = 0;
        swfStream.write(&nullbyte, 1);
        swfStream.write(cf.contents, cf.contentsLength);
    }

    if(symbolclass_entries.size() > 0) {
        //printf("%d SymbolClass entries\n", (int)symbolclass_entries.size());

        char symbolclassHeader[8];
        uint32_t dataLen = 0;

        std::map<uint16_t, const std::string>::const_iterator i, e;

        i = symbolclass_entries.begin();
        e = symbolclass_entries.end();
        for(; i != e; i++) {
            //printf("SymbolClass entry: %d '%s'\n", i->first, i->second.c_str());
            dataLen += 3 + i->second.length(); // tag, string, null term
        }

        *(uint16_t*)&symbolclassHeader[0] = (76 << 6) | 63; // SymbolClass, extended length
        *(uint32_t*)&symbolclassHeader[2] = 2 + dataLen;
        *(uint16_t*)&symbolclassHeader[6] = (uint16_t)symbolclass_entries.size();

        swfStream.write(symbolclassHeader, sizeof(symbolclassHeader));

        i = symbolclass_entries.begin();
        e = symbolclass_entries.end();
        for(; i != e; i++) {
            uint16_t tagID = i->first;
            const std::string &clazz = i->second;

            swfStream.write((char *)&tagID, sizeof(tagID));
            swfStream.write(clazz.c_str(), clazz.length()+1);
        }
    }

    if(enable_debugger) {
        char enabledebugger2Record[9];
        *(uint16_t*)&enabledebugger2Record[0] = (64 << 6) | 63;
        *(uint32_t*)&enabledebugger2Record[2] = 3;
        *(uint16_t*)&enabledebugger2Record[6] = 0;
        *(uint8_t*)&enabledebugger2Record[8] = 0;
        swfStream.write(enabledebugger2Record, sizeof(enabledebugger2Record));
    }

    uint16_t suffixTags[2] = {
        1 << 6, // ShowFrame
        0       // End
    };
    swfStream.write((char*)&suffixTags[0], sizeof(suffixTags));
    swfStream.close();

    // Run SwfResolve/SwcGen
    {
        int pid = fork();

        if(pid < 0) {
            (*message)(LDPL_ERROR,
                       "Failed to fork for swfresolve\n");
            return LDPS_ERR;
        }

        if(!pid) {
            std::string cp = sysroot + "/usr/lib";
            std::string alctool = sysroot + "/usr/lib/alctool.jar";
            std::vector<std::string> args;
            args.push_back("java");
            for(unsigned int i=0; i<jvmopts.size(); i++) {
                args.push_back(jvmopts[i]);
            }
            args.push_back("-cp");
            args.push_back(cp);
            args.push_back("-jar");
            args.push_back(alctool);
            args.push_back("-swfresolve");
            if(make_swc)
	    {
                args.push_back("-swc");
                args.push_back(swc_namespace);
	    }
            else if(swf_ns)
	    {
                args.push_back("-swfns");
                args.push_back(swf_namespace);
	    }
            args.push_back("-forceordering");
            if(!enable_debugger)
                args.push_back("-optimize");
            args.push_back("-o");
            if(make_projector) {
                args.push_back(resolveTmpPath);
            } else {
                args.push_back(output_name);
                if(!enable_debugger)
                    args.push_back("-swfstrip");
                if(swfpreloader.size() > 0) {
                    args.push_back("-swfpreloader");
                    args.push_back(swfpreloader);
                }
            }
            args.push_back(resolveTmpPath);
            int r = execjava(args);
            fprintf(stderr, "Failed to exec swfresolve");
            exit(-1);
        } else {
            wait(&pid);
        }
    }

    if(make_projector) {
        std::ifstream resolvedSwf(resolveTmpPath, std::ios::in | std::ios::binary);

        std::copy(
        std::istreambuf_iterator<char>(resolvedSwf),
        std::istreambuf_iterator<char>( ),
        std::ostreambuf_iterator<char>(outputfile));

    
        int swfend = (int)outputfile.tellp();
        int swfsize = swfend - swfstart;

        // vm arguments, null separated
        if(enablevmargs) {
            for(int i=0; i<vmargs.size(); i++) {
                outputfile << vmargs[i] << 0x0;
            }
        }
        int vmarglen = (int)outputfile.tellp() - swfend;

        // avmshell projector magic bytes
        char projectorHeader[4];
        projectorHeader[0] = 0x56;
        projectorHeader[1] = 0x34;
        projectorHeader[2] = 0x12;
        projectorHeader[3] = 0xFA;
        outputfile.write(&projectorHeader[0], sizeof(projectorHeader));

        // length of the whole swf
        outputfile.write((char*)&swfsize, sizeof(swfsize));

        // length of the vm arg string (or INT_MAX if we want to enable normal
        // avmshell argument parsing
        vmarglen = enablevmargs ? vmargs.size() : INT_MAX;
        outputfile.write((char*)&vmarglen, sizeof(vmarglen));
        outputfile.close();
        resolvedSwf.close();
    }
    unlink(resolveTmpPath);

    return LDPS_OK;
}
