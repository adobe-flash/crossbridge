/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
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

#include <bfd.h>

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
        if (tag() != 'file'
            )
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

        if (tag() == 'file' && other.tag() != 'file'
            )
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

static void emit_so_bfd(const Chunk *base, uint64_t textOffs, uint64_t textLen,
                        std::vector<MethBlock> &blocks,
                        std::vector<DebugInfo> &debugInfos);

int main(int argc, char **argv)
{
    if (argc != 2)
        fatal("usage: %s inputLog", argv[0]);

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0)
        fatal("couldn't open %s", argv[1]);

    off_t len = lseek(fd, 0, SEEK_END);
    const void *start = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    const void *cur = start;
    const void *end = (char *) cur + len;

    if (cur == MAP_FAILED)
        fatal("couldn't mmap %s", argv[1]);

    const Chunk *base = NULL;
    const Chunk *meth = NULL;

    std::vector<MethBlock> blocks;
    std::vector<DebugInfo> debugInfos;

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

    std::sort(blocks.begin(), blocks.end());
    std::sort(debugInfos.begin(), debugInfos.end());

    if (!base)
        fatal("no 'base' found");

    uint64_t textOffs = 0x1000;
    uint64_t textLen = 64 * 1024 * 1024;

    if (base->size > sizeof(uint64_t)) {
        char path[4096];
        int len = base->size - sizeof(uint64_t);

        if (len >= sizeof(path))
            warn("path to base library too long");
        else {
            memcpy(path, base->data + sizeof(uint64_t), len);
            path[len] = 0;

            void *hdl = dlopen(path, RTLD_NOW);

            if (!hdl)
                warn("couldn't get info from base library %s (using defaults)",
                     path);
            else {
                void *start = dlsym(hdl, "_jitStart");

                if (!start)
                    warn("couldn't find _jitStart in base library");
                else {
                    Dl_info dlInfo;

                    if (dladdr(start, &dlInfo)) {
                        if (((char *) start - (char *) dlInfo.dli_fbase)
                                != textOffs)
                            warn("_jitStart at unexpected location");
                    } else
                        warn("couldn't validate _jitStart location");

                    void *end = dlsym(hdl, "_jitEnd");

                    if (!end)
                        warn("couldn't find _jitEnd in base library");
                    else
                        textLen = (char *) end - (char *) start;
                }
                dlclose(hdl);
            }
        }
    }

    emit_so_bfd(base, textOffs, textLen, blocks, debugInfos);
    return 0;
}

static asymbol *make_glob_symbol(bfd *abfd, const char *name, asection *section,
                                 uint64_t value)
{
    asymbol *result = bfd_make_empty_symbol(abfd);
    result->name = strdup(name);
    result->section = section;
    result->flags = BSF_GLOBAL;
    result->value = value;
    return result;
}

static int l2(uint64_t n)
{
    int result = 0;

    while (!(n & 1)) {
        result++;
        n >>= 1;
    }
    if (n != 1)
        warn("textOffs not a power of 2");
    return result;
}

static void emit_so_bfd(const Chunk *base, uint64_t textOffs, uint64_t textLen,
                        std::vector<MethBlock> &blocks,
                        std::vector<DebugInfo> &debugInfos)
{
    bfd_init();

    bfd *abfd = bfd_openw("jit.o",
#ifdef __MACH__
                          "mach-o-i386"
#else
                          "elf32-i386"
#endif
)                          ;

    bfd_set_format(abfd, bfd_object);
#ifdef __MACH__
    bfd_set_arch_mach(abfd, bfd_arch_i386, bfd_mach_i386_i386);
#else
    bfd_set_arch_mach(abfd, bfd_arch_i386, bfd_mach_i386_i386);
#endif

    asection *textSection = bfd_make_section(abfd, ".text");

    bfd_set_section_flags(
            abfd, textSection,
            SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE | SEC_HAS_CONTENTS);
#ifdef __MACH__
    bfd_set_section_size(abfd, textSection, textLen);
    // for mach-o, the .o we generate is linked into a .dylib
    // so just set the alignment to make sure things end up
    // in the right place after link
    textSection->alignment_power = l2(textOffs);

    static const uint64_t textAdj = 0;
#else
    bfd_set_section_size(abfd, textSection, textLen + textOffs);

    // for linux (and oprofile) the generated object is used
    // directly, so just offset the symbols
    uint64_t textAdj = textOffs;
#endif

    asymbol **symbols = (asymbol **) malloc(
            sizeof(asymbol *) * (6 + blocks.size()));
    uint64_t start = ((uint64_t *) base->data)[0] + textOffs;

    int n = 0;
    std::map<std::string, int> namesSeen;

    std::vector<MethBlock>::const_iterator i, e;

    symbols[n++] = make_glob_symbol(abfd, "__jitStart", textSection, textAdj);
    symbols[n++] = make_glob_symbol(abfd, "_jitStart", textSection, textAdj);

    for (i = blocks.begin(), e = blocks.end(); i != e; i++, n++) {
        std::string name = i->name();
        int seen = namesSeen[name]++;

        if (seen) {
            char buf[16];

            sprintf(buf, " (%d)", seen);
            name += buf;
        }
        symbols[n] = make_glob_symbol(abfd, name.c_str(), textSection,
                                      i->start() - start + textAdj);
    }
    symbols[n++] = make_glob_symbol(abfd, "__jitUnused", textSection,
                                    blocks.back().end() - start + textAdj);
    symbols[n++] = make_glob_symbol(abfd, "_jitUnused", textSection,
                                    blocks.back().end() - start + textAdj);
    symbols[n++] = make_glob_symbol(abfd, "__jitEnd", textSection,
                                    textLen + textAdj);
    symbols[n++] = make_glob_symbol(abfd, "_jitEnd", textSection,
                                    textLen + textAdj);

    bfd_set_symtab(abfd, symbols, n);

    for (i = blocks.begin(), e = blocks.end(); i != e; i++, n++)
        if (i->codeBytes())
            bfd_set_section_contents(abfd, textSection, i->codeBytes(),
                                     i->start() - start, i->codeLen());

    bfd_close(abfd);

    for (int n = 0; n < blocks.size(); n++)
        free((void *) symbols[n]->name);
    free(symbols);
}
