/* -*- tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "eval.h"

#ifdef _MSC_VER   // Not ideal
#define WINDOWS_BUILD
#endif

#include <math.h>

// avmc - ahead-of-time ActionScript compiler based on the eval code in Tamarin.
//
// usage: avmc [option ...] <actionscript source file> ...
//
// options:
//   --parse   Only run the parser, do not generate code

class AvmcHostContext : public avmplus::RTC::HostContext {
public:
    AvmcHostContext();

	uint8_t* obtainStorageForResult(uint32_t nbytes);
	const wchar* readFileForEval(const wchar* basename, const wchar* filename, uint32_t* inputlen);
    const wchar* constructName(const wchar* basename, const wchar* filename);
    const char* latinizeName(const wchar* filename);
	void freeInput(const wchar* input);
	void doubleToString(double d, char* buf, size_t bufsiz);
	bool stringToDouble(const char* s, double* d);
	void throwInternalError(const char* msgz);
	void throwSyntaxError(const char* msgz);
	
	uint8_t* result;
	uint32_t result_size;
    uint8_t include_nesting;
    
    static const uint32_t include_nesting_limit = 10;
};

int fail(const char* fmt, ...);

int main (int argc, char** argv) 
{
	argc > 1 || fail("Usage: avmc <actionscript source file> ...");

	AvmcHostContext context;
    bool options = true;
	for ( int i=1 ; i < argc ; i++ )
	{
        // FIXME: want an option to control public_by_default (which is false, by default)
        if (options) {
            if (strcmp(argv[i], "--parse") == 0) {
                context.stopAfterParse = true;
                continue;
            }
            options = false;
        }

		char* filename = argv[i];
		FILE *fp = fopen(filename, "r");
		fp != NULL || fail("Could not open file for input: %s", filename);
		fseek(fp, 0, SEEK_END) == 0 || fail("Could not seek on input file: %s", filename);
		long int llen = ftell(fp);
		llen >= 0 || fail("Could not seek on input file: %s", filename);
		uint32_t len = (uint32_t)llen;
		fseek(fp, 0, SEEK_SET) == 0 || fail("Could not seek on input file: %s", filename);
		wchar *text = new wchar[len+1];
		text != NULL || fail("Out of memory");
		fread(text, 1, len, fp) == len || fail("Could not read from file: %s\n", filename);
		fclose(fp);
		wchar* wdest = text+len-1;
		char* csrc = (char*)text+len-1;
		while (wdest >= text)
			*wdest-- = *csrc--;
		text[len] = 0;
		wchar* wfilename = new wchar[strlen(filename)+1];
		wfilename != NULL || fail("Out of memory");
		wdest = wfilename;
		csrc = filename;
		while (*wdest++ = *csrc++)
			;
		avmplus::RTC::Compiler compiler(&context, wfilename, text, len+1, false);
		compiler.compile();
		delete [] text;
        if (!context.stopAfterParse) {
            char *dotpos = strrchr(filename, '.');
            if (dotpos && strcmp(dotpos, ".as") == 0)
                *dotpos = 0;
            char outfn[FILENAME_MAX];
            snprintf(outfn, sizeof(outfn), "%s.abc", filename);
            outfn[sizeof(outfn)-1] = 0;
            fp = fopen(outfn, "wb");
            fp != NULL || fail("Could not open file for output: %s\n", outfn);
            fwrite(context.result, 1, context.result_size, fp) == context.result_size || fail("Could not write to output: %s\n", outfn);
            fclose(fp);
        }
		delete [] context.result;
		context.result = NULL;
	}
	return 0;
}

int fail(const char* fmt, ...)
{
    va_list args;
    fprintf(stderr, "Error: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
	/*NOTREACHED*/
    return 0;
}

const double MathUtils::kNaN = nan("");

bool MathUtils::isNaN(double d)
{
	return isnan(d);
}

bool MathUtils::isInfinite(double d)
{
    return isinf(d);
}

AvmcHostContext::AvmcHostContext()
    : result(NULL)
    , result_size(0)
    , include_nesting(0)
{
}

uint8_t* AvmcHostContext::obtainStorageForResult(uint32_t nbytes)
{
	result = new uint8_t[nbytes];
	result != NULL || fail("Out of memory");
	result_size = nbytes;
	return result;
}

static size_t length(const wchar* s)
{
    const wchar* t = s;
    while (*t)
        t++;
    return t-s;
}

// The filename is relative to the base.  Find the last path separator
// in the base if any, truncate the string there, append the new filename.
// FIXME, filename sanitazion is significantly more complicated than this.

const wchar* AvmcHostContext::constructName(const wchar* basename, const wchar* filename)
{
    if (basename != NULL && filename[0] != '/' 
#ifdef WINDOWS_BUILD
        && filename[0] != '\\'
#endif
        )
    {
        const wchar *basesep = NULL;
        const wchar *p = basename;
        while (*p) {
#ifdef WINDOWS_BUILD
            if (*p == '/' || *p == '\\')
                basesep = p;
#else
            if (*p == '/')
                basesep = p;
#endif
            p++;
        }
        
        if (basesep == NULL)
            return filename;

        size_t fnlen = length(filename);
        wchar* canonical_filename = new wchar[basesep - basename + 2 + fnlen];
        VMPI_memcpy(canonical_filename, basename, sizeof(wchar)*(basesep - basename + 1));
        VMPI_memcpy(canonical_filename + (basesep - basename + 1), filename, sizeof(wchar)*(fnlen + 1));
        return canonical_filename;
    }

    return filename;
}

const char* AvmcHostContext::latinizeName(const wchar* filename)
{
    size_t fnlen = length(filename);
    const wchar* p = filename;
    char* buf = new char[fnlen+1];
    char* q = buf;

    while (*p) {
        if (*p & ~255) {
            delete [] buf;
            throwInternalError("File names are restricted to latin-1 at this time");
        }
        *q++ = (char)(*p++);
    }
    return buf;
}

// For the tracking of the levels of nesting (which is a dirty way of handling
// circularities in the include files) we depend on the parser calling freeInput as
// soon as it's finished consuming the input for a particular file, and not
// batching the freeInput calls at the end.
//
// The responsibility for freeing input buffers when errors are signaled falls to
// the host context; since this is a batch compiler and we just abort we don't
// need to do anything.

const wchar* AvmcHostContext::readFileForEval(const wchar* basename, const wchar* filename, uint32_t* inputlen)
{
    if (include_nesting == include_nesting_limit)
        throwInternalError("includes too deeply nested");
    include_nesting++;

    // Construct the canonical file name from basename and provided name.
    // For now restrict it to latin-1 names only so that we can use fopen().

    const wchar* canonical_filename = constructName(basename, filename);  // canonical_filename will be either 'filename' or a heap-allocated array
    const char* latin1_filename = latinizeName(canonical_filename);
    if (canonical_filename != filename)
        delete [] canonical_filename;

    FILE* fp = fopen(latin1_filename, "rb");
    if (fp == NULL)
        throwInternalError("Cannot open include file");

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // +1 for the terminating NUL.

    wchar* buf = new wchar[filesize+1];
    
    if (fread((char*)buf+filesize, 1, filesize, fp) != filesize)
        throwInternalError("Cannot read include file");

    fclose(fp);

    // It's completely ill-defined what the encoding of the file is.
    // Here we assume it's latin1, and we just expand it.

    wchar* p = buf;
    char* q = (char*)buf + filesize;
    for ( long i=0 ; i < filesize ; i++ )
        *p++ = *q++;

    // Terminate and return.  freeInput will be called to free the buffer after parsing.

    buf[filesize] = 0;
    *inputlen = (uint32_t)(filesize+1);
    return buf;
}

void AvmcHostContext::freeInput(const wchar* input)
{
    include_nesting--;
    delete [] input;
}

void AvmcHostContext::doubleToString(double d, char* buf, size_t bufsiz)
{
	snprintf(buf, bufsiz, "%f", d);		// not sure about this
	buf[bufsiz-1] = 0;
}

bool AvmcHostContext::stringToDouble(const char* s, double* d)
{
	*d = strtod(s, NULL);
	return true;
}

void AvmcHostContext::throwInternalError(const char* msgz)
{
	fprintf(stderr, "%s\n", msgz);
	exit(1);
}

void AvmcHostContext::throwSyntaxError(const char* msgz)
{
	fprintf(stderr, "%s\n", msgz);
	exit(1);
}

