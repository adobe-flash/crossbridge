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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "zip.h"

struct map_entry {
    const char *realpath;
    const char *vfspath;
    const char *escaped_vfspath;
    struct map_entry *next;
};

static int gen_binarydatavfs(int);
static int gen_httpvfs(int);
static int gen_zipvfs(int);

static int (*generator_func)(int) = gen_zipvfs;
static struct map_entry *filemaps = NULL;
static struct map_entry *dirmaps = NULL;
static char *output_prefix = NULL;
static char *output_classname = "RootFSBackingStore";
static char *output_packagename = "com.adobe.flascc.vfs";

static void
add(struct map_entry **list, const char *realpath, const char *vfspath)
{
    struct map_entry *entry = malloc(sizeof(struct map_entry));
    entry->realpath = realpath;
    entry->vfspath = vfspath;

    int vfspath_len = strlen(entry->vfspath);
    char *escaped_vfspath = malloc(vfspath_len * 2 + 1);
    int i, j;
    for (i = j = 0; i < vfspath_len; i++) {
        if (vfspath[i] == '\"') {
            escaped_vfspath[j++] = '\\';
        }
        escaped_vfspath[j++] = vfspath[i];
    }
    escaped_vfspath[j] = '\0';
    entry->escaped_vfspath = escaped_vfspath;

    entry->next = *list;
    
    *list = entry;
}

static void 
add_dir(const char *realpath, const char *vfspath)
{
    add(&dirmaps, realpath, vfspath); 
}

static void
add_file(const char *realpath, const char *vfspath)
{
    add(&filemaps, realpath, vfspath); 
}

static int
read_files(const char *rootdir, const char *vfsdirname)
{
    DIR *root = opendir(rootdir);
    if (!root) {
        perror(rootdir);
        return 1;
    }
    
    char *fullname, *vfsfullname;
    struct dirent *ent = readdir(root);
    while (ent) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
            ent = readdir(root);
            continue;
        }

        asprintf(&fullname, "%s/%s", rootdir, ent->d_name);
        asprintf(&vfsfullname, "%s/%s", vfsdirname, ent->d_name);

        if (ent->d_type == DT_DIR) {
            if (read_files(fullname, vfsfullname)) {
                return 1;
            }
            add_dir(fullname, vfsfullname);
        } else if (ent->d_type == DT_LNK) {
            struct stat stats;
            if (stat(fullname, &stats)) {
                perror(fullname);
                return 1;
            }
            if (S_ISDIR(stats.st_mode)) {
                if (read_files(fullname, vfsfullname)) {
                    return 1;
                }
                add_dir(fullname, vfsfullname);
            } else {
                add_file(fullname, vfsfullname);
            }
        } else {
            add_file(fullname, vfsfullname);
        }

        ent = readdir(root);
    }

    if (closedir(root)) {
        perror(rootdir);
    }
    return 0;
}

static void
hex_encode(FILE *dest, const char *source)
{
    while (*source) {
        fprintf(dest, "%02X", *source);
        source++;
    }
}

static int
hex_encode_file(FILE *dest, const char *file)
{
    FILE *src = fopen(file, "r");
    if (!src) {
        perror(file);
        return 1;
    }

    int c;
    while ((c = fgetc(src)) != EOF) {
        fprintf(dest, "%02X", c);
    }

    if (ferror(src)) {
        perror(file);
        return 1;
    }
    fclose(src);
    return 0;
}

static void
insert_readonly_code(FILE *out)
{
    fprintf(out, "override public function get readOnly():Boolean {\n");
    fprintf(out, "\treturn true\n");
    fprintf(out, "}\n\n");
}

static int
gen_binarydatavfs(int readonly)
{
    int filecnt = 0;
    struct map_entry *ent;
    char *outfilename, *classfilename;
    FILE *outfile; // the AS file containing hex-encoded data of a real file
    FILE *bsclassfile; // AS file containing VFSBackingStore subclass 

    asprintf(&classfilename, "%s_%06d.as", output_prefix, filecnt++);
    bsclassfile = fopen(classfilename, "w");
    if (!bsclassfile) {
        perror(classfilename);
        return 1;
    }

    fprintf(bsclassfile, "package C_Run {}\n");
    fprintf(bsclassfile, "package %s {\n", output_packagename);
    fprintf(bsclassfile, "import com.adobe.flascc.vfs.*;\n");
    fprintf(bsclassfile, "import com.adobe.flascc.BinaryData\n");
    fprintf(bsclassfile, "public class %s extends InMemoryBackingStore {\n",
        output_classname);
    if (readonly) {
        insert_readonly_code(bsclassfile);
    }
    fprintf(bsclassfile, "public function %s() {\n", output_classname);
    
    for (ent = dirmaps; ent; ent = ent->next) {
        fprintf(bsclassfile, "\taddDirectory(\"%s\")\n", ent->escaped_vfspath);
    }

    for (ent = filemaps; ent; ent = ent->next) {
        asprintf(&outfilename, "%s_%06d.as", output_prefix, filecnt++);
        outfile = fopen(outfilename, "w");
        if (!outfile) {
            perror(outfilename);
            return 1;
        }
        
        fprintf(outfile, "package C_Run {\n");
        fprintf(outfile, "import com.adobe.flascc.BinaryData\n");
        fprintf(outfile, ";[HexData(\"");
        if (hex_encode_file(outfile, ent->realpath)) {
            return 1;
        }
        fprintf(outfile, "\")]\n");

        // The class name and vfspath, separated by a space, should
        // uniquely identify a file.
        char *outclass_and_vfspath;
        asprintf(&outclass_and_vfspath, "%s %s", 
            output_classname, ent->vfspath);
        
        fprintf(outfile, "public class ALC_FS_");
        hex_encode(outfile, outclass_and_vfspath);
        fprintf(outfile, " extends BinaryData {}\n");
        fprintf(outfile, "\n}\n");

        fprintf(bsclassfile, "\taddFile(\"%s\", new C_Run.ALC_FS_",
            ent->escaped_vfspath);
        hex_encode(bsclassfile, outclass_and_vfspath);
        free(outclass_and_vfspath);
        fprintf(bsclassfile, ")\n");
        
        if (fclose(outfile)) {
            perror(outfilename);
            return 1;
        }
        free(outfilename);
    }
        
    fprintf(bsclassfile, "}\n");    // constructor
    fprintf(bsclassfile, "\n}\n");  // class
    fprintf(bsclassfile, "\n}\n");  // package

    if (fclose(bsclassfile)) {
        perror(classfilename);
        return 1;
    }

    // Generate some makefile rules to compile these abcs
    char *makefilename;
    asprintf(&makefilename, "%s.rules", output_prefix); 
    FILE *makefile = fopen(makefilename, "w");
    if (!makefile) {
        perror(makefilename);
        return 1;
    }

    fprintf(makefile, "VFS_SRCS = ");
    int i;
    for (i = 0; i < filecnt; i++) {
        fprintf(makefile, "%s_%06d.as ", output_prefix, i);
    }
    fprintf(makefile, "\nVFS_ABCS := $(VFS_SRCS:.as=.abc)\n");

    if (fclose(makefile)) {
        perror(makefilename);
        return 1;
    }

    return 0;
}

static int 
gen_zipvfs(int readonly) 
{
    char *zipfilename = tmpnam(NULL);
    zipFile zf = zipOpen64(zipfilename, 0);
    if (!zf) {
        perror(zipfilename);
        return 1;
    }

    zip_fileinfo zi;
    bzero(&zi, sizeof(zi));

    char *buf = malloc(4096);
    struct map_entry *ent;
    for (ent = filemaps; ent; ent = ent->next) {
        int err = zipOpenNewFileInZip64(zf, ent->vfspath, &zi,
                NULL,0,NULL,0,NULL /* no comments */,
                Z_DEFLATED, Z_DEFAULT_COMPRESSION, 1);
        if (err != ZIP_OK) {
            fprintf(stderr, "error including %s in archive\n", ent->realpath);
            return 1;
        }

        FILE *infile = fopen(ent->realpath, "r");
        if (!infile) {
            perror(ent->realpath);
            return 1;
        }

        while (!feof(infile)) {
            size_t bytes_read = fread(buf, 1, 4096, infile);
            if (ferror(infile)) {
                perror(ent->realpath);
                return 1;
            } 
            err = zipWriteInFileInZip(zf, buf, bytes_read);
            if (err < 0) {
                fprintf(stderr, 
                    "error writing %s in the archive\n", ent->realpath);
                return 1;
            }
        } 

        if (zipCloseFileInZip(zf) != ZIP_OK) {
            fprintf(stderr, 
                    "error writing %s in the archive\n", ent->realpath);
            return 1;
        }

        fclose(infile);
    }
    free(buf);

    if (zipClose(zf, NULL)) {
        perror(zipfilename);
        return 1;
    }

    char *outfilename;
    FILE *outfile;
    asprintf(&outfilename, "%s%s.as", output_prefix, output_classname);
    outfile = fopen(outfilename, "w");
    if (!outfile) {
        perror(outfilename);
        return 1;
    }

    fprintf(outfile, "package %s {\n", output_packagename);
    fprintf(outfile, "import com.adobe.flascc.vfs.*;\n");
    fprintf(outfile, "import com.adobe.flascc.BinaryData\n");
    fprintf(outfile, "import com.adobe.flascc.vfs.zip.*\n");
    fprintf(outfile, ";[HexData(\"");
    if (hex_encode_file(outfile, zipfilename)) {
        return 1;
    }
    fprintf(outfile, "\")]\n");

    fprintf(outfile, "public class %sData extends BinaryData {}\n\n", 
        output_classname);
  
    fprintf(outfile, "public class %s extends InMemoryBackingStore {\n", 
        output_classname);
    if (readonly) {
        insert_readonly_code(outfile);
    }
    fprintf(outfile, "public function %s() {\n", output_classname);
    // Don't rely on the zip file to store directories, because the
    // zip library this program uses doesn't seem to support directories.
    for (ent = dirmaps; ent; ent = ent->next) {
        fprintf(outfile, "\taddDirectory(\"%s\")\n", ent->escaped_vfspath);
    }
    fprintf(outfile, "\tvar data = new %sData()\n", output_classname);
    fprintf(outfile, "\tvar zip = new ZipFile(data)\n");
    fprintf(outfile, "\tfor (var i = 0; i < zip.entries.length; i++) {\n");
    fprintf(outfile, "\t\tvar e = zip.entries[i]\n");
    fprintf(outfile, "\t\tif (!e.isDirectory()) {\n");
    fprintf(outfile, "\t\t\taddFile(e.name, zip.getInput(e))\n");
    fprintf(outfile, "\t\t}\n");
    fprintf(outfile, "\t}\n");
    fprintf(outfile, "}\n");
    fprintf(outfile, "}\n");

    fprintf(outfile, "\n}\n");

    if (fclose(outfile)) {
        perror(outfilename);
        return 1;
    }

    if (remove(zipfilename)) {
        perror(zipfilename);
        return 1;
    }
   
    free(outfilename);
    return 0;
}

static const int HTTP_MAX_CHUNK_SIZE = 1024 * 1024 * 10;

static int
gen_httpvfs(int readonly)
{
    int total_bytes = 0;
    FILE *manifest;
    char *manifestname;
    struct map_entry *ent;

    asprintf(&manifestname, "%s_manifest.as", output_prefix);
    manifest = fopen(manifestname, "w");
    if (!manifest) {
        perror(manifestname);
        return 1;
    }

    if (readonly) {
        insert_readonly_code(manifest);
    }

    fprintf(manifest, "public var vfsFiles:Array = [\n");
    char *in_buf = malloc(HTTP_MAX_CHUNK_SIZE);
    int filecnt = 0;
    for (ent = filemaps; ent; ent = ent->next) {
        FILE *infile = fopen(ent->realpath, "r");
        if (!infile) {
            perror(ent->realpath);
            return 1;
        }

        int chunkcnt = 0;
        struct stat stats;
        if (fstat(fileno(infile), &stats)) {
            perror(ent->realpath);
            return 1;
        }
        fprintf(manifest, "[\"%s\", %Ld, ", ent->escaped_vfspath, stats.st_size);

        while (1) {
            size_t bytes_in = fread(in_buf, 1, HTTP_MAX_CHUNK_SIZE, infile);
            total_bytes += bytes_in;
            char *chunkname;

            asprintf(&chunkname, "%s_file%dchunk%d", output_prefix, filecnt,
                chunkcnt++);
            FILE *chunk = fopen(chunkname, "w");
            if (!chunk) {
                perror(chunkname);
                return 1;
            }
          
            fwrite(in_buf, 1, bytes_in, chunk);
            if (ferror(chunk)) {
                perror(chunkname);
                return 1;
            }

            if (fclose(chunk)) {
                perror(chunkname);
                return 1;
            } else if (ferror(infile)) {
                perror(ent->realpath);
                return 1;
            }

            fprintf(manifest, "\"%s\"", chunkname);
            free(chunkname);

            if (feof(infile)) {
                break;
            } else {
                fprintf(manifest, ", ");
            }
        }
        fprintf(manifest, "], ");
        fclose(infile);
        filecnt++;
    }
    free(in_buf);

    fprintf(manifest, "\n]\n"); 
    fprintf(manifest, "public var vfsTotalSize:uint = %d\n", total_bytes);

    fprintf(manifest, "public function initDirs():void {\n");
    for (ent = dirmaps; ent; ent = ent->next) {
        fprintf(manifest, "\taddDirectory(\"%s\")\n", ent->escaped_vfspath);
    }
    fprintf(manifest, "}\n");

    if (fclose(manifest)) {
        perror(manifestname);
        return 1;
    }

    free(manifestname);
    return 0;
}

static void
print_usage ()
{
    static char *usage =
"usage: genfs [options] source-directory output-prefix\n"
"Options:\n"
"  --help\t\tShow this message\n"
"  --name=NAME\t\tGenerate an Actionscript class named NAME\n"
"  --read-only\t\tMake the generated filesystem read only\n"
"  --type=TYPE\t\tSpecify the type of filesystem to be generated\n"
"             \t\tPermissible types include: compress embed http\n";
    printf("%s", usage);
}

int
main (int argc, char **argv)
{
    static struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {"name", required_argument, 0, 'n'},
            {"read-only", no_argument, 0, 'r'},
            {"type", required_argument, 0, 't'},
            {0, 0, 0, 0}
        };
    int option_index = 0;
    int ro_flag = 0;
    while (1) {
        int c = getopt_long(argc, argv, "hn:rt:", long_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'h':
            print_usage();
            return 0;
        case 'n':
          {
            const char *lastDot = strrchr(optarg, '.');
            const char *className = lastDot ? lastDot + 1 : optarg;
            output_classname = malloc(strlen(className) + 1);
            strcpy(output_classname, className);
            if(lastDot)
            {
              output_packagename = calloc(1, lastDot - optarg + 1);
              strncpy(output_packagename, optarg, lastDot - optarg);
            }
            break;
          }
        case 'r':
            ro_flag = 1;
            break;
        case 't':
            if (!strcmp(optarg, "embed")) {
                generator_func = gen_binarydatavfs;
            } else if (!strcmp(optarg, "http")) {
                generator_func = gen_httpvfs;
            } else if (!strcmp(optarg, "compress")) {
                generator_func = gen_zipvfs;
            } else {
                print_usage();
                return 1;
            }
            break;
        case '?':
            break;
        default:
            abort();
        }

    }

    if (optind + 2 != argc) {
        print_usage();
        return 1;
    } else {
        output_prefix = argv[optind + 1];
    }

    if (read_files(argv[optind], "")) {
        return 1;
    } else {
        return generator_func(ro_flag);
    }
}

