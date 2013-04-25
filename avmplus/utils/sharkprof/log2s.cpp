/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <inttypes.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

static void warn(const char *tmpl, ...)
{
    va_list ap;
    va_start(ap, tmpl);
    vfprintf(stderr, tmpl, ap);
    fputs("\n", stderr);
    va_end(ap);
}

static void fatal(const char *tmpl, ...)
{
    va_list ap;
    va_start(ap, tmpl);
    vfprintf(stderr, tmpl, ap);
    fputs("\n", stderr);
    va_end(ap);
    exit(-1);
}

struct Chunk
{
    uint32_t tag;
    uint32_t size;
    char data[0];
};

struct DebugInfo
{
    const Chunk *info;

    uint64_t pc() const
    {
        return ((uint64_t *) info->data)[0];
    }

    uint32_t tag() const
    {
        return info->tag;
    }

    std::string file() const
    {
        if (tag() != 'file')
            fatal("DebugInfo::file() called on non-file debug info");

        return std::string(info->data + sizeof(uint64_t),
                           info->size - sizeof(uint64_t));
    }

    uint64_t line() const
    {
        if (tag() != 'line'
            )
            fatal("DebugInfo::line() called on non-linedebug info");

        return ((uint64_t *) info->data)[1];
    }

    bool operator<(const DebugInfo &other) const
    {
        int64_t pcPad = pc() - other.pc();

        if (pcPad)
            return pcPad < 0;

        if (tag() == 'file' && other.tag() != 'file')
            return true;

        return false;
    }
};

struct MethBlock
{
    const Chunk *meth;
    const Chunk *block;
    const Chunk *code;

    uint64_t start() const
    {
        return ((uint64_t *) block->data)[0];
    }

    uint64_t end() const
    {
        return ((uint64_t *) block->data)[1];
    }

    std::string name() const
    {
        return std::string(meth->data, meth->size);
    }

    const void *codeBytes() const
    {
        return code ? code->data : NULL;
    }

    const uint32_t codeLen() const
    {
        return code ? code->size : 0;
    }

    bool operator<(const MethBlock &other) const
    {
        return start() < other.start();
    }
};

static std::string quote(const std::string &s)
{
#ifdef __MACH__
    std::string result = "\"";
    std::string::const_iterator i = s.begin(), e = s.end();

    for (; i != e; i++) {
        if (*i != '\\' && isprint(*i))
            result += *i;
        else {
            char buf[8];

            sprintf(buf, "\\%03o", *i);
            result += buf;
        }
    }
    result += "\"";
    return result;
#else
#error foo
#endif
}

int main(int argc, char **argv)
{
    if (argc != 2)
        fatal("usage: %s inputLog", argv[0]);

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0)
        fatal("couldn't open %s", argv[1]);

    off_t len = lseek(fd, 0, SEEK_END);
    const void *cur = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    const void *end = (char *) cur + len;

    if (cur == MAP_FAILED)
        fatal("couldn't mmap %s", argv[1]);

    const Chunk *base = NULL;
    const Chunk *meth = NULL;

    std::vector<MethBlock> blocks;
    std::vector<DebugInfo> debugInfos;
    int pad_count = 0;

    while (cur < end) {
        const Chunk &chunk = *(const Chunk *) cur;

        switch (chunk.tag) {
        default:
            warn("unknown tag %08x", chunk.tag);
            break;
        case 'base':
            base = &chunk;
            break;
        case 'meth':
            meth = &chunk;
            break;
        case 'blok': {
            if (!meth)
                fatal("'blok' tag without 'meth'");

            MethBlock block;
            block.meth = meth;
            block.block = &chunk;
            block.code = NULL;
            blocks.push_back(block);
        }
            break;
        case 'code': {
            if (blocks.size() <= 0)
                fatal("'code' chunk without 'blok'");
            blocks.back().code = &chunk;
        }
            break;
        case 'file':
        case 'line': {
            DebugInfo info = { &chunk };
            debugInfos.push_back(info);
        }
            break;
        }
        cur = chunk.data + chunk.size;
    }

    printf(".text\n");
#ifdef __MACH__
    printf(".align 12\n");
#else
    printf(".align 4096\n");
#endif
    printf(".globl __jitStart\n");
    printf(".globl _jitStart\n");
    printf("__jitStart:\n");
    printf("_jitStart:\n");
    printf(".align 0\n");

    std::sort(blocks.begin(), blocks.end());
    std::sort(debugInfos.begin(), debugInfos.end());

    std::map<std::string, int> namesSeen;
    std::vector<MethBlock>::const_iterator i, e;
    std::vector<DebugInfo>::const_iterator di, de;

    if (!base)
        fatal("no 'base' found");

    uint64_t textOffs = 0x1000;
    uint64_t textLen = 256 * 1024 * 1024;

    if (base->size <= sizeof(uint64_t))
        fatal("base info too short");

    char path[4096];
    int path_len = base->size - sizeof(uint64_t);

    if (path_len >= sizeof(path))
        fatal("path to base library too long");

    memcpy(path, base->data + sizeof(uint64_t), path_len);
    path[path_len] = 0;
    void *hdl = dlopen(path, RTLD_NOW);
    if (!hdl)
        fatal("couldn't get info from base library %s", path);

    char *jit_start = (char*) dlsym(hdl, "_jitStart");
    if (!jit_start)
        fatal("couldn't find _jitStart in base library");
    Dl_info dlInfo;

    if (dladdr(jit_start, &dlInfo)) {
        if ((jit_start - (char *) dlInfo.dli_fbase) != textOffs)
            warn("_jitStart at unexpected location");
    } else {
        warn("couldn't validate _jitStart location");
    }

    char *jit_end = (char*) dlsym(hdl, "_jitEnd");
    if (!jit_end)
        fatal("couldn't find _jitEnd in base library");

    textLen = jit_end - jit_start;
    dlclose(hdl);

    uint64_t curAddr = *(uint64_t *) base->data + textOffs;
    uint64_t endAddr = curAddr + textLen;

    int dbgFile = 0;

    di = debugInfos.begin();
    de = debugInfos.end();

    for (i = blocks.begin(), e = blocks.end(); i != e; ++i) {
        int curDbgFile = 0;

        uint64_t block_start = i->start();  //###
        uint64_t block_end = i->end();      //###

        std::string name = i->name();
        int seen = namesSeen[name]++;

        if (seen) {
            char buf[16];

            sprintf(buf, " (%d)", seen);
            name += buf;
        }

        uint64_t pad = block_start - curAddr;

        if (pad) {
            printf("pad%d:\n", pad_count++);
            printf(".fill %lld,1,0xcc\n", pad);
        }

        curAddr += pad;

        while (di != de && di->pc() < curAddr) {
            warn("dropped debug info at %llx", di->pc());
            di++;
        }

        printf("%s:\n", quote(name).c_str());

        const void *code = i->codeBytes();

        if (code) {
            uint32_t codeLen = i->codeLen();
            if (codeLen != block_end - block_start) {
                printf("FAIL\n");
            }

            while (codeLen) {
                while (di != de && di->pc() == curAddr) {
                    switch (di->tag()) {
                    default:
                        warn("unknown debug tag %08x", di->tag());
                        break;
                    case 'file':
                        printf(".file %d %s\n", curDbgFile = ++dbgFile,
                               quote(di->file()).c_str());
                        break;
                    case 'line':
                        if (curDbgFile)
                            printf(".loc %d %lld 0\n", dbgFile, di->line());
                        break;
                    }
                    di++;
                }

                printf(".byte ");

                int nextDebug = (di == de) ? codeLen : di->pc() - curAddr;
                int nextDebugHere = (nextDebug > codeLen) ? codeLen : nextDebug;
                int atOnce = (nextDebugHere < 12) ? nextDebugHere : 12;

                while (atOnce--) {
                    printf("0x%02x%s", *(const unsigned char *) code,
                           atOnce ? ", " : "\n");
                    code = (const char *) code + 1;
                    codeLen--;
                    curAddr++;
                }
            }
        }
    }

    uint64_t endPad = endAddr - curAddr;

    if (endPad) {
        printf("pad%d:\n", pad_count++);
        printf(".fill %lld,1,0xcc\n", endPad);
    }
    printf(".globl __jitEnd\n");
    printf(".globl _jitEnd\n");
    printf("__jitEnd:\n");
    printf("_jitEnd:\n");
    return 0;
}
