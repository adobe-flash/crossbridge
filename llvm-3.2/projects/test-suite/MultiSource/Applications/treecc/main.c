/*
 * main.c - Main program entry point for "treecc".
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "parse.h"
#include "errors.h"
#include "gen.h"
#include "options.h"

#ifdef	__cplusplus
extern	"C" {
#endif

static void Usage(char *progname);
static void Version(void);
static int ExtraOptions(TreeCCContext *context, char **options, int num);
static char *GetDefault(const char *filename, const char *extension);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	char *opt;
	char *outputFile = NULL;
	char *headerFile = NULL;
	char *extension = "c";
	char *outputDir = NULL;
	int forceCreate = 0;
	TreeCCInput input;
	TreeCCContext *context;
	int sawStdin = 0;
	int generateOutput = 1;
	FILE *file;
	int len, result;
	char **options = (char **)malloc(sizeof(char *) * argc);
	int num_options = 0;

	/* Allocate the array for external "%option" values */
	options = (char **)malloc(sizeof(char *) * argc);
	if(!options)
	{
		TreeCCOutOfMemory(0);
	}

	/* Parse the command-line options */
	while(argc > 1 && argv[1][0] == '-')
	{
		if(argv[1][1] == '\0')
		{
			/* The input is stdin */
			break;
		}
		else if(argv[1][1] == '-')
		{
			/* The option begins with "--" */
			if(argv[1][2] == '\0')
			{
				/* End of options, and start of filenames */
				--argc;
				++argv;
				break;
			}
			if(!strcmp(argv[1], "--output"))
			{
				--argc;
				++argv;
				if(argc <= 1)
				{
					Usage(progname);
					return 1;
				}
				outputFile = argv[1];
			}
			else if(!strcmp(argv[1], "--header"))
			{
				--argc;
				++argv;
				if(argc <= 1)
				{
					Usage(progname);
					return 1;
				}
				headerFile = argv[1];
			}
			else if(!strcmp(argv[1], "--output-dir"))
			{
				--argc;
				++argv;
				if(argc <= 1)
				{
					Usage(progname);
					return 1;
				}
				outputDir = argv[1];
			}
			else if(!strcmp(argv[1], "--skeleton-dir"))
			{
				/* This option is obsolete: we still parse it just in
				   case there are older build systems that expect it */
				--argc;
				++argv;
				if(argc <= 1)
				{
					Usage(progname);
					return 1;
				}
			}
			else if(!strcmp(argv[1], "--extension"))
			{
				--argc;
				++argv;
				if(argc <= 1)
				{
					Usage(progname);
					return 1;
				}
				extension = argv[1];
			}
			else if(!strcmp(argv[1], "--option"))
			{
				--argc;
				++argv;
				if(argc <= 1)
				{
					Usage(progname);
					return 1;
				}
				options[num_options++] = argv[1];
			}
			else if(!strcmp(argv[1], "--force-create"))
			{
				forceCreate = 1;
			}
			else if(!strcmp(argv[1], "--no-output"))
			{
				generateOutput = 0;
			}
			else if(!strcmp(argv[1], "--help"))
			{
				Usage(progname);
				return 1;
			}
			else if(!strcmp(argv[1], "--version"))
			{
				Version();
				return 0;
			}
			else
			{
				Usage(progname);
				return 1;
			}
		}
		else
		{
			/* Single-character option */
			opt = argv[1] + 1;
			while(*opt != '\0')
			{
				switch(*opt++)
				{
					case 'o':
					{
						if(*opt != '\0')
						{
							outputFile = opt;
							opt = "";
						}
						else if(argc <= 2)
						{
							Usage(progname);
							return 1;
						}
						else
						{
							--argc;
							++argv;
							outputFile = argv[1];
						}
					}
					break;

					case 'h':
					{
						if(*opt != '\0')
						{
							headerFile = opt;
							opt = "";
						}
						else if(argc <= 2)
						{
							Usage(progname);
							return 1;
						}
						else
						{
							--argc;
							++argv;
							headerFile = argv[1];
						}
					}
					break;

					case 'd':
					{
						if(*opt != '\0')
						{
							outputDir = opt;
							opt = "";
						}
						else if(argc <= 2)
						{
							Usage(progname);
							return 1;
						}
						else
						{
							--argc;
							++argv;
							outputDir = argv[1];
						}
					}
					break;

					case 's':
					{
						/* This option is obsolete: we still parse it just in
						   case there are older build systems that expect it */
						if(*opt != '\0')
						{
							opt = "";
						}
						else if(argc <= 2)
						{
							Usage(progname);
							return 1;
						}
						else
						{
							--argc;
							++argv;
						}
					}
					break;

					case 'e':
					{
						if(*opt != '\0')
						{
							extension = opt;
							opt = "";
						}
						else if(argc <= 2)
						{
							Usage(progname);
							return 1;
						}
						else
						{
							--argc;
							++argv;
							extension = argv[1];
						}
					}
					break;

					case 'O':
					{
						if(*opt != '\0')
						{
							options[num_options++] = opt;
							opt = "";
						}
						else if(argc <= 2)
						{
							Usage(progname);
							return 1;
						}
						else
						{
							--argc;
							++argv;
							options[num_options++] = argv[1];
						}
					}
					break;

					case 'f':
					{
						forceCreate = 1;
					}
					break;

					case 'n':
					{
						generateOutput = 0;
					}
					break;

					case 'v':
					{
						Version();
						return 0;
					}
					/* Not reached */

					default:
					{
						Usage(progname);
						return 1;
					}
					/* Not reached */
				}
			}
		}
		--argc;
		++argv;
	}
	if(argc <= 1)
	{
		Usage(progname);
		return 1;
	}

	/* Determine the default output and header filenames */
	if(!outputFile)
	{
		if(*extension == '.')
		{
			++extension;
		}
		outputFile = GetDefault(argv[1], extension);
	}
	if(!headerFile)
	{
		headerFile = GetDefault(outputFile, "h");
	}

	/* Initialize the input routines */
	TreeCCOpen(&input, progname, NULL, NULL);

	/* Determine the default Java output directory */
	if(!outputDir)
	{
		len = strlen(argv[1]);
		while(len > 0 && argv[1][len - 1] != '/' && argv[1][len - 1] != '\\')
		{
			--len;
		}
		if(len > 0)
		{
			--len;
		}
		if(len > 0)
		{
			outputDir = (char *)malloc(len + 1);
			if(!outputDir)
			{
				TreeCCOutOfMemory(&input);
			}
			strncpy(outputDir, argv[1], len);
			outputDir[len] = '\0';
		}
	}

	/* Create the parsing context */
	context = TreeCCContextCreate(&input);
	if(!context)
	{
		TreeCCOutOfMemory(&input);
	}
	context->force = forceCreate;
	context->outputDirectory = outputDir;

	/* Process additional options from the command-line */
	if(!ExtraOptions(context, options, num_options))
	{
		return 1;
	}
	free(options);

	/* Create the default source and header streams */
	context->sourceStream = TreeCCStreamCreate(context, outputFile,
											   NULL, 0);
	context->sourceStream->defaultFile = 1;
	context->headerStream = TreeCCStreamCreate(context, headerFile,
											   NULL, 1);
	context->headerStream->defaultFile = 1;

	/* Process the input files */
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Parse stdin, but only once */
			if(!sawStdin)
			{
				TreeCCOpen(&input, progname, stdin, "stdin");
				TreeCCParse(context);
				TreeCCClose(&input, 0);
				sawStdin = 1;
			}
		}
		else
		{
			/* Parse some other file */
			if((file = fopen(argv[1], "r")) == NULL)
			{
				perror(argv[1]);
				input.errors = 1;
			}
			else
			{
				TreeCCOpen(&input, progname, file, argv[1]);
				TreeCCParse(context);
				TreeCCClose(&input, 1);
			}
		}
		--argc;
		++argv;
	}

	/* Validate node and operation coverage */
	TreeCCNodeValidate(context);
	TreeCCOperationValidate(context);

	/* Abort if we encountered errors during parsing and validation */
	if(input.errors)
	{
		TreeCCContextDestroy(context);
		return 1;
	}

	/* Generate the output files */
	result = 0;
	if(generateOutput)
	{
		TreeCCStream *stream;
		TreeCCGenerate(context);
		stream = context->streamList;
		while(stream != 0)
		{
			if(!TreeCCStreamFlush(stream))
			{
				result = 1;
			}
			stream = stream->nextStream;
		}
	}

	/* Done */
	TreeCCContextDestroy(context);
	return result;
}

static void Usage(char *progname)
{
	fprintf(stderr, "TREECC " VERSION " - Tree Compiler-Compiler\n");
	fprintf(stderr, "Copyright (c) 2001, 2002 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] input ...\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "    -o file, --output file\n");
	fprintf(stderr, "        Set the name of the output file.\n");
	fprintf(stderr, "    -h file, --header file\n");
	fprintf(stderr, "        Set the name of the header output file.\n");
	fprintf(stderr, "    -d dir,  --output-dir file\n");
	fprintf(stderr, "        Set the name of the Java output directory.\n");
	fprintf(stderr, "    -e ext,  --extension ext\n");
	fprintf(stderr, "        Set the output file extension (default is \".c\").\n");
	fprintf(stderr, "    -f,      --force-create\n");
	fprintf(stderr, "        Force the creation of unchanged output files.\n");
	fprintf(stderr, "    -O opt,  --option opt\n");
	fprintf(stderr, "        Set a treecc source option value.\n");
	fprintf(stderr, "    --help\n");
	fprintf(stderr, "        Print this help message.\n");
	fprintf(stderr, "    -v,      --version\n");
	fprintf(stderr, "        Print the program version.\n");
}

static void Version(void)
{
	printf("TREECC " VERSION " - Tree Compiler-Compiler\n");
	printf("Copyright (c) 2001, 2002 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("TREECC comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

/*
 * Process extra treecc "%option" values from the command-line.
 */
static int ExtraOptions(TreeCCContext *context, char **options, int num)
{
	char *name;
	char *value;
	while(num-- > 0)
	{
		name = *options++;
		value = name;
		while(*value != '\0' && *value != '=')
		{
			++value;
		}
		if(*value == '\0')
		{
			value = 0;
		}
		else
		{
			*value++ = '\0';
		}
		if(TreeCCOptionProcess(context, name, value) != TREECC_OPT_OK)
		{
			fprintf(stderr, "%s: unknown option or invalid value\n", name);
			return 0;
		}
	}
	return 1;
}

/*
 * Get a default output or header filename from an input filename.
 */
static char *GetDefault(const char *filename, const char *extension)
{
	int len;
	char *name;

	/* USe a default filename pattern if we are reading from stdin */
	if(!strcmp(filename, "-"))
	{
		filename = "yy_tree.tc";
	}

	/* Find the end of the input filename, without its extension */
	len = strlen(filename);
	while(len > 0 && filename[len - 1] != '/' && filename[len - 1] != '\\' &&
		  filename[len - 1] != '.')
	{
		--len;
	}

	/* Construct the new filename */
	if(len > 0 && filename[len - 1] == '.')
	{
		name = (char *)malloc(len + strlen(extension) + 1);
		if(!name)
		{
			TreeCCOutOfMemory((TreeCCInput *)0);
		}
		strncpy(name, filename, len);
		strcpy(name + len, extension);
	}
	else
	{
		len = strlen(filename);
		name = (char *)malloc(len + strlen(extension) + 2);
		if(!name)
		{
			TreeCCOutOfMemory((TreeCCInput *)0);
		}
		strcpy(name, filename);
		name[len] = '.';
		strcpy(name + len + 1, extension);
	}
	return name;
}

#ifdef	__cplusplus
};
#endif
