
/*
 *  Name:           openFiles
 *  Input:          number of command-line arguments, argc
 *                  array of command-line argument strings, argv
 *  Output:         input FILE pointer
 *                  output FILE pointer
 *                  metrics FILE pointer
 *  Return:         OPEN_FILES_SUCCESS, or
 *                  OPEN_FILES_USAGE_REQUEST
 *                  OPEN_FILES_USAGE_ERROR
 *                  OPEN_FILES_INPUT_FILE_ERROR,
 *                  OPEN_FILES_OUTPUT_FILE_ERROR,
 *                  OPEN_FILES_METRICS_FILE_ERROR,
 *  Description:    Routine opens the three files used by the application: 
 *                  input, output, and metrics.  The FILE pointers are returned 
 *                  open and at the beginning of the files.  Default values for 
 *                  the input, output, and metric files are stdin, stdout, and 
 *                  stderr respectively.  The command-line arguments are passed
 *                  to the routine for default overrides.  The usage for the
 *                  baseline application is:
 *
 *                      a.out -i name     : specify input file name
 *                            -o name     : specify output file name
 *                            -m name     : specify metrics file name
 *                            -h          : display usage and return
 *
 *                  The routine also initializes the output buffer by passing
 *                  the value of the output file pointer to initOutputBuffer().
 *  Calls:          errorMessage()
 *                  initOutputBuffer()
 *      System:     fopen()
 *                  fprintf()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *  06Jul99 Matthew Rivas   Modified command-line argument parsing and added 
 *                          usage parameter -h
 *  12Jul99 Matthew Rivas   Change file assertions to correct form
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                        */
#include <stdio.h>          /* for FILE definitions and fopen()    */
#include "dataManagement.h" /* for primitive type definitions      */
#include "errorMessage.h"   /* for errorMessage() definition       */
#include "openFiles.h"      /* for openFiles return codes          */

/*
 *  Function Prototypes
 */
extern void initOutputBuffer( FILE *file );

Int openFiles( Int    argc,         /*  command-line arguments to provide   */
               Char **argv,         /*  input values for file names         */
               FILE **inputFile,    /*  input file ptr - output             */
               FILE **outputFile,   /*  output file ptr - output            */
               FILE **metricsFile ) /*  metrics file ptr - output           */
{   /*  begin openFiles()   */
    Char        *inputFileName;     /*  character strings used to store name  */
    Char        *outputFileName;    /*  values parsed from input command-line */
    Char        *metricsFileName;   /*  arguments                             */

    static Char name[]  = "openFiles";

    assert( argc > 0 );
    assert( argv && argv[ 0 ] );
    assert( *inputFile == NULL );
    assert( *outputFile == NULL );
    assert( *metricsFile == NULL );

    /*
     *  Process input command-line arguments for the name of the input, output, 
     *  and metric files.  The names are set to NULL and if unchanged will 
     *  default to the stdin, stdout, and stderr, respectively.  If a file is 
     *  specified by the command-line options, the name is referenced by the 
     *  appropriate local variable and used during the opening of the file in 
     *  the next section.
     */
    inputFileName   = NULL;
    outputFileName  = NULL;
    metricsFileName = NULL;
    if ( argc > 1 ) {
        Int i; /* counter for current argument being parsed */
        
        i = 1; /* start with second argument - first is program name */
        while ( i < argc ) { /* parse command-line arguments until all parsed */
            if ( strlen( argv[ i ] ) == 2 && argv[ i ][ 0 ] == '-' ) {
                if ( argv[ i ][ 1 ] == 'i' ) {
                    if ( i + 1 < argc ) {
                        i++;
                        inputFileName = argv[ i ];
                    } /* end of if i+1 <= argc */
                    else {
                        errorMessage( "missing argument for -i", REPLACE );
                        errorMessage( name, PREPEND );

                        inputFileName   = NULL;
                        outputFileName  = NULL;
                        metricsFileName = NULL;

                        fprintf( stderr, "Usage: %s [-h], or\n", argv[ 0 ] );
                        fprintf( stderr, "       %s", argv[ 0 ] );
                        fprintf( stderr, " [-i <input file = stdin>]" );
                        fprintf( stderr, " [-o <output file = stdout>]" );
                        fprintf( stderr, " [-m <metrics file = stderr>]\n" );
                        return ( OPEN_FILES_USAGE_ERROR );
                    } /* end of if i+1 > argc */
                    i++;
                } /* end of if argv[ i ][ 1 ] = 'i' */
                else if ( argv[ i ][ 1 ] == 'o' ) {
                    if ( i + 1 < argc ) {
                        i++;
                        outputFileName = argv[ i ];
                    } /* end of if i+1 <= argc */
                    else {
                        errorMessage( "missing argument for -o", REPLACE );
                        errorMessage( name, PREPEND );

                        inputFileName   = NULL;
                        outputFileName  = NULL;
                        metricsFileName = NULL;

                        fprintf( stderr, "Usage: %s [-h], or\n", argv[ 0 ] );
                        fprintf( stderr, "       %s", argv[ 0 ] );
                        fprintf( stderr, " [-i <input file = stdin>]" );
                        fprintf( stderr, " [-o <output file = stdout>]" );
                        fprintf( stderr, " [-m <metrics file = stderr>]\n" );
                        return ( OPEN_FILES_USAGE_ERROR );
                    } /* end of if i+1 > argc */
                    i++;
                } /* end of if argv[ i ][ 1 ] = 'o' */
                else if ( argv[ i ][ 1 ] == 'm' ) {
                    if ( i + 1 < argc ) {
                        i++;
                        metricsFileName = argv[ i ];
                    } /* end of if i+1 <= argc */
                    else {
                        errorMessage( "missing argument for -m", REPLACE );
                        errorMessage( name, PREPEND );

                        inputFileName   = NULL;
                        outputFileName  = NULL;
                        metricsFileName = NULL;

                        fprintf( stderr, "Usage: %s [-h], or\n", argv[ 0 ] );
                        fprintf( stderr, "       %s", argv[ 0 ] );
                        fprintf( stderr, " [-i <input file = stdin>]" );
                        fprintf( stderr, " [-o <output file = stdout>]" );
                        fprintf( stderr, " [-m <metrics file = stderr>]\n" );
                        return ( OPEN_FILES_USAGE_ERROR );
                    } /* end of if i+1 > argc */
                    i++;
                } /* end of if argv[ i ][ 1 ] = 'm' */
                else if ( argv[ i ][ 1 ] == 'h' ) {
                    fprintf( stderr, "Usage: %s [-h], or\n", argv[ 0 ] );
                    fprintf( stderr, "       %s", argv[ 0 ] );
                    fprintf( stderr, " [-i <input file = stdin>]" );
                    fprintf( stderr, " [-o <output file = stdout>]" );
                    fprintf( stderr, " [-m <metrics file = stderr>]\n" );
                    return ( OPEN_FILES_USAGE_REQUEST );
                } /* end of if argv[ i ][ 1 ] = 'h' */
                else {
                    errorMessage( "unknown option", REPLACE );
                    errorMessage( name, PREPEND );

                    inputFileName   = NULL;
                    outputFileName  = NULL;
                    metricsFileName = NULL;

                    fprintf( stderr, "Usage: %s [-h], or\n", argv[ 0 ] );
                    fprintf( stderr, "       %s", argv[ 0 ] );
                    fprintf( stderr, " [-i <input file = stdin>]" );
                    fprintf( stderr, " [-o <output file = stdout>]" );
                    fprintf( stderr, " [-m <metrics file = stderr>]\n" );
                    return ( OPEN_FILES_USAGE_ERROR );
                } /* end of else - unknown option */
            } /* end of if strlen( argv ) > 1 && argv[i][0] = '-' */
            else {
                errorMessage( "incorrect format - unknown option", REPLACE );
                errorMessage( name, PREPEND );

                inputFileName   = NULL;
                outputFileName  = NULL;
                metricsFileName = NULL;
                
                fprintf( stderr, "Usage: %s [-h], or\n", argv[ 0 ] );
                fprintf( stderr, "       %s", argv[ 0 ] );
                fprintf( stderr, " [-i <input file = stdin>]" );
                fprintf( stderr, " [-o <output file = stdout>]" );
                fprintf( stderr, " [-m <metrics file = stderr>]\n" );
                return ( OPEN_FILES_USAGE_ERROR );
            } /* end of else - incorrect format */
        } /* end of option present with '-' in front */
    } /* end of if argc > 1 - command-line arguments present */
    
    /*
     *  Check each file name for either default value or no value, open each 
     *  file and check value of FILE pointer.  If a failure occurs, place 
     *  message in error buffer and return appropriate code.
     */
    if ( inputFileName == NULL ) {      /*  input file  */
        *inputFile = stdin;
    }   /*  end of if ( inputFileName == NULL )    */
    else {
        *inputFile = fopen( inputFileName, "r" );
        if ( *inputFile == NULL ) {
            errorMessage( "error opening input file", REPLACE );
            errorMessage( name, PREPEND );

            return ( OPEN_FILES_INPUT_FILE_ERROR );
        }   /*  end of if ( *inputFile == NULL )    */
    }   /*  end of branches for input file  */
    
    if ( outputFileName == NULL ) {      /*  output file    */
        *outputFile = stdout;
    }   /*  end of if ( outputFileName == NULL )    */
    else {
        *outputFile = fopen( outputFileName, "w" );
        if ( *outputFile == NULL ) {
            errorMessage( "error opening output file", REPLACE );
            errorMessage( name, PREPEND );

            return ( OPEN_FILES_OUTPUT_FILE_ERROR );
        }   /*  end of if ( *outputFile == NULL )    */
    }   /*  end of branches for output file  */

    initOutputBuffer( *outputFile );
    
    if ( metricsFileName == NULL ) {      /*  metrics file  */
        *metricsFile = stderr;
    }   /*  end of if ( metricsFileName == NULL )   */
    else {
        *metricsFile = fopen( metricsFileName, "w" );
        if ( *metricsFile == NULL ) {
            errorMessage( "error opening metrics file", REPLACE );
            errorMessage( name, PREPEND );

            return ( OPEN_FILES_METRICS_FILE_ERROR );
        }   /*  end of if ( *metricsFile == NULL )    */
    }   /*  end of branches for metrics file  */
    
    return ( OPEN_FILES_SUCCESS );
}   /*  end of openFiles()  */
