
/*
 *  Name:           main
 *  Input:          command-line arguments (argc, argv)
 *  Output:         integer success or error code
 *  Return:         DIS_DATA_MANAGEMENT_SUCCESS, or
 *                  DIS_DATA_MANAGEMENT_ERROR
 *  Description:    The is the main() routine for the DIS Data Management 
 *                  Benchmark baseline application.  The application reads a 
 *                  data set from the specified input (default=stdin), displays 
 *                  the query command response to the specified output 
 *                  (default=stdout), and calculates and displays a set of 
 *                  metric calculations (default=stderr).  The format for the 
 *                  input and output files is given in the DIS Benchmark Suite 
 *                  specification document.  The application is made up of three
 *                  modules: Database, Input & Output, and Metrics.  A complete 
 *                  description of the modules and supporting routines are 
 *                  provided in the DIS Data Management Software Design document
 *                  provided with the baseline source code.  The routine has 
 *                  three main sections:
 *
 *                  (1) Initialization
 *                      The three modules are initialized by opening files, 
 *                      creating the first root node, reading the fan from the 
 *                      input, and starting the metric timing.
 *                  (2) Main Loop
 *                      The main loop consists of the input data set being 
 *                      processed line by line where a single line represents a 
 *                      command to the database.  The command is process as 
 *                      either an INSERT, QUERY, or DELETE.  If no commands are 
 *                      left in the database, then a value of NONE is specified 
 *                      by the getNextCommandCode() routine and the main loop 
 *                      exits.
 *
 *                  (3) Exit
 *                      The application exits by stopping the metric timing, 
 *                      calculating and displaying the statistical values, 
 *                      removing the memory allocated for the database index, 
 *                      and closing all files.
 *
 *  Calls:          clearLine()
 *                  closeFiles()
 *                  createIndexNode()
 *                  delete();
 *                  deleteIndexNode()
 *                  errorMessage()
 *                  flushErrorMessage()
 *                  flushOutputBuffer()
 *                  getDeleteCommand()
 *                  getInitCommand()
 *                  getInsertCommand()
 *                  getNextCommandCode()
 *                  getQueryCommand()
 *                  getTime()
 *                  initMetricsData()
 *                  insert()
 *                  openFiles()
 *                  outputMetricsData()
 *                  setMetricsData()
 *                  query()
 *                  updateMetricsData()
 *      System:     free()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  28May99 Matthew Rivas   Created
 *  06Jul99 Matthew Rivas   Modified return check for openFiles() routine to 
 *                          handle a usage request or error
 *  12Jul99 Matthew Rivas   Initialized input, output, and metric pointers
 *                          to NULL before passing to openFiles() routine to
 *                          prevent incorrect assertion
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <stdlib.h>             /*  for free() and NULL definitions           */
#include "dataObject.h"         /*  for DataObject definitions                */
#include "dataManagement.h"     /*  for primitive type definitions            */
#include "delete.h"             /*  for delete() and return codes             */
#include "errorMessage.h"       /*  for errorMessage() definitions            */
#include "index.h"              /*  for IndexNode definition                  */
#include "insert.h"             /*  for insert() and return codes             */
#include "getNextCommandCode.h" /*  for getNextCommandCode definitions        */
#include "getInitCommand.h"     /*  for getInitCommand() and return codes     */
#include "getInsertCommand.h"   /*  for getInsertCommand() and return codes   */
#include "getDeleteCommand.h"   /*  for getDeleteCommand() and return codes   */
#include "getQueryCommand.h"    /*  for getQueryCommand() and return codes    */
#include "metrics.h"            /*  for Metrics definition                    */
#include "openFiles.h"          /*  for openFiles() and return codes          */
#include "query.h"              /*  for query() and return codes              */

/*
 *  Exit status definitions
 */
#define DIS_DATA_MANAGEMENT_SUCCESS  0
#define DIS_DATA_MANAGEMENT_ERROR   -1

/*
 *  Function prototypes
 */
extern void clearLine( FILE *file );
extern void closeFiles( FILE *input, FILE *output, FILE *metrics );
extern void flushOutputBuffer( void );
extern void outputQuery( DataObject *dataObject );
extern void initMetricsData( Metrics *metrics );
extern void setMetricsData( Metrics *metrics, CommandType command );
extern void updateMetricsData( Metrics *metrics );
extern void outputMetricsData( FILE *file, Metrics *metrics );
extern Time getTime( void );

/*
 *  System Test Debug Prototypes
 */
extern Int countDataObjects( IndexNode *node );

/*
 *  Main program
 */
int main( int argc, Char *argv[] )
{   /*  begin main()    */
    Int         returnCode; /*  generic return code for error processing    */
    CommandType command;    /*  current command read/processing             */
    FILE        *input;     /*  input file                                  */
    FILE        *output;    /*  output file                                 */
    FILE        *metric;    /*  metrics file                                */
    Metrics     metrics;    /*  metrics data structure                      */
    IndexNode   *root;      /*  root node of index                          */
    Int         fan;        /*  fan or order of index - read from input     */
    Time        tempTime;   /*  time for keeping I/O timing statistics      */ 

    /*
     *                  Initialize Application
     *
     *      (1) Initialize the Metrics module: this consists of calling the 
     *          initMetricsData() routine to initialize the Metrics structure.  
     *      (2) Initialize the Input & Output module: this consists of calling 
     *          the openFiles() routine which opens the three files: input, 
     *          output, and metrics and also calls the initOutputBuffer() 
     *          routine to pass the output FILE ptr to the output buffer.  If an
     *          error occurred for any of the files, notify user and exit.  Note
     *          that the command-line arguments are passed directly to 
     *          openFiles() for default overrides on the file names.
     *      (3) Initialize the Database module:  this consists of creating the 
     *          initial root node and reading the INIT command from the input 
     *          file.  If the root node can't be created or an error occurred 
     *          during the reading of the INIT command, notify user and exit.
     */

    initMetricsData( &metrics ); /* (1) */

    input  = NULL;
    output = NULL;
    metric = NULL;
    returnCode = openFiles( argc, argv, &input, &output, &metric ); /* (2) */
    if ( returnCode != OPEN_FILES_SUCCESS ) {
        /*
         *  Check for a usage request, i.e., "-h" in the command-line and
         *  exit successfully if so. Otherwise, flush error message and return
         *  with error.
         */
        if ( returnCode == OPEN_FILES_USAGE_REQUEST ) {
            return ( DIS_DATA_MANAGEMENT_SUCCESS );        
        }   /*  end of returnCode == OPEN_FILES_USAGE_REQUEST */
        else {
            /*
             *  FATAL ERROR: can't proceed if unable to open files or incorrect 
             *               usage
             */
            errorMessage( argv[ 0 ], PREPEND );
            flushErrorMessage();

            return ( DIS_DATA_MANAGEMENT_ERROR );        
        }   /*  end of else */
    }

    root = createIndexNode( LEAF ); /* (3) */
    if ( root == NULL ) {
        /*
         *  FATAL ERROR: can't proceed if unable to create root node
         */
        errorMessage( "root node", PREPEND );
        errorMessage( argv[ 0 ], PREPEND );
        flushErrorMessage();

        return ( DIS_DATA_MANAGEMENT_ERROR );
    }

    returnCode = getNextCommandCode( input, &command );
    if ( returnCode == GET_NEXT_COMMAND_CODE_SUCCESS ) {
        if ( command == INIT ) {

            tempTime = getTime(); /* start I/O timing of command */
            returnCode = getInitCommand( input, &fan );
            metrics.inputTime += ( getTime() - tempTime );
            
            if ( returnCode != GET_INIT_SUCCESS ) { 
                errorMessage( "Can't read first command (INIT)", PREPEND );
                errorMessage( argv[ 0 ], PREPEND );
                flushErrorMessage();

                /*
                 *  FATAL ERROR: must have fan parameter to continue
                 */
                return ( DIS_DATA_MANAGEMENT_ERROR );
            }   /*  end error checks for getInitCommand */
        }   /*  end if command == INIT  */
        else {
            errorMessage( "First command is not INIT command", REPLACE );
            errorMessage( argv[ 0 ], PREPEND );
            flushErrorMessage();

            /*
             *  FATAL ERROR: must have fan parameter to continue
             */
            return ( DIS_DATA_MANAGEMENT_ERROR );
        }   /*  end command != INIT     */
    }   /*  end if returnCode == GET_NEXT_COMMAND_CODE_SUCCESS */
    else if ( returnCode == GET_NEXT_COMMAND_CODE_IO_ERROR ||
              returnCode == GET_NEXT_COMMAND_CODE_INVALID_COMMAND ) {
        errorMessage( "Can't read first command (INIT)", PREPEND );
        errorMessage( argv[ 0 ], PREPEND );
        flushErrorMessage();

        /*
         *  FATAL ERROR: must have fan parameter to continue
         */
        return ( DIS_DATA_MANAGEMENT_ERROR );
    }   /*  end return check for getNextCommandCode */
    clearLine( input );
    
    /*
     *                  Main Loop
     *
     *    The main loop of the DIS Data Management benchmark baseline
     *    application consists of getting the next command from the 
     *    input stream and processing the command.  Metric timing is
     *    done for each command processed.  The query command response
     *    is also display/flushed through each loop.  Extensive error
     *    checking is made during the loop.  The following is a
     *    description of the main loop actions:
     *
     *      (1) Get next command code from input stream
     *      (2) Start metric timing of command
     *      (3) Switch on command code
     *          (A) Insert
     *              (a) Get insert command from input stream: the
     *                  insert command consists of a new data object
     *                  to add to index.
     *              (b) Insert new data object into index: this may
     *                  cause tree "growth", so the root node may be
     *                  replaced or "updated" after call to insert(). 
     *                  There is no response for the insert command.
     *          (B) Query 
     *              (a) Get query command from input stream: the 
     *                  command consists of an index key and a list of
     *                  non-key DataAttribute's.  The memory for the
     *                  non-key character strings will need to be 
     *                  freed after the query() call (see 3.B.c)
     *              (b) Query current index placing responses
     *                  into output buffer via outputQuery()
     *                  routine. 
     *              (c) Free non-key DataAttribute character strings
     *                  allocated during the getQueryCommand() call.
     *          (C) Delete 
     *              (a) Get delete command from input stream: the 
     *                  command consists of an index key and a list of
     *                  non-key DataAttribute's.  The memory for the
     *                  non-key character strings will need to be 
     *                  freed after the delete() call (see 3.C.c)
     *              (b) Delete entries in current index which are 
     *                  consistent with input search values.  There is
     *                  no response to the delete command. 
     *              (c) Free non-key DataAttribute character strings
     *                  allocated during the getDeleteCommand() call.
     *      (3) Stop metric timing for command 
     *      (4) Flush output buffer of any query response
     */

	/*** VERSABENCH START ***/
	while ( command != NONE ) {
        returnCode = getNextCommandCode( input, &command );
        if ( returnCode == GET_NEXT_COMMAND_CODE_SUCCESS ) {
            setMetricsData( &metrics, command ); /* start timing */

            if ( command == INSERT ) {
                DataObject  *dataObject;

                tempTime = getTime(); /* start I/O timing of command */
                returnCode = getInsertCommand( input, &dataObject );
                metrics.inputTime += ( getTime() - tempTime );

                if ( returnCode == GET_INSERT_SUCCESS ) {
                    returnCode = insert( &root, dataObject, fan );
                    if ( returnCode == INSERT_INSERT_ENTRY_FAILURE_FATAL ){
                        errorMessage( argv[ 0 ], PREPEND );
                        flushErrorMessage();

                        /*
                         *  FATAL ERROR:
                         */
                        return ( DIS_DATA_MANAGEMENT_ERROR );
                    }   /*  end code == INSERT_INSERT_ENTRY_FAILURE_FATAL */
                    else if (returnCode==INSERT_INSERT_ENTRY_FAILURE_NON_FATAL){
                        errorMessage( argv[ 0 ], PREPEND );
                        flushErrorMessage();
                    }   /*  end code == INSERT_INSERT_ENTRY_FAILURE_NON_FATAL */
                    else if ( returnCode == INSERT_ALLOCATION_FAILURE ) {
                        errorMessage( argv[ 0 ], PREPEND );
                        flushErrorMessage();

                        /*
                         *  FATAL ERROR:
                         */
                        return ( DIS_DATA_MANAGEMENT_ERROR );
                    }   /*  end of if returnCode == INSERT_ALLOCATION_FAILURE */
                }   /*  end of if ( returnCode == GET_INSERT_SUCCESS )  */
                else {
                    errorMessage( argv[ 0 ], PREPEND );
                    flushErrorMessage();
                }   /*  end of returnCode != GET_INSERT_SUCCESS */
            }   /*  end command == INSERT   */
            else if ( command == QUERY ) {
                IndexKey        searchKey;
                DataAttribute   *searchNonKey;
                DataAttribute   *temp;

                tempTime = getTime(); /* start I/O timing of command */
                returnCode = getQueryCommand( input, &searchKey, &searchNonKey);
                metrics.inputTime += ( getTime() - tempTime );

                if ( returnCode == GET_QUERY_SUCCESS ) {

                    returnCode = query( root, &searchKey, searchNonKey, TRUE, 
                                        outputQuery );
                    if ( returnCode == QUERY_INVALID_KEY_SEARCH_VALUE ||
                         returnCode == QUERY_INVALID_NON_KEY_SEARCH_VALUE ) {
                        errorMessage( argv[ 0 ], PREPEND );
                        flushErrorMessage();
                    } /* end of if returnCode != QUERY_SUCCESS */
                }   /*  end of if ( returnCode == GET_QUERY_SUCCESS )  */
                else {
                    errorMessage( argv[ 0 ], PREPEND );
                    flushErrorMessage();
                }   /*  end of returnCode != GET_QUERY_SUCCESS */

                /*
                 *  Clean-up memory: the non-key search values read for the
                 *  QUERY command allocates memory for each value.  This memory
                 *  needs to be deallocated before the list leaves scope.
                 */
                temp = searchNonKey;
                while ( temp != NULL ) {
                    DataAttribute *next;
                    
                    next = temp->next;
                    free( temp->attribute.value.nonKey );
                    free( temp );
                    temp = next;                    
                }  /*  end of loop over searchNonKey list using temp */
            }   /*  end command == QUERY    */
            else if ( command == DELETE ) {
                IndexKey        searchKey;
                DataAttribute   *searchNonKey;
                DataAttribute   *temp;

                tempTime = getTime(); /* start I/O timing of command */
                returnCode = getDeleteCommand( input,&searchKey, &searchNonKey);
                metrics.inputTime += ( getTime() - tempTime );

                if ( returnCode == GET_DELETE_SUCCESS ) {
                    returnCode = delete( &root, &searchKey, searchNonKey );
                    if ( returnCode == DELETE_INVALID_KEY_SEARCH_VALUE ||
                         returnCode == DELETE_INVALID_NON_KEY_SEARCH_VALUE ) {
                        errorMessage( argv[ 0 ], PREPEND );
                        flushErrorMessage();
                   }  /* end of returnCode != DELETE_SUCCESS */
                }   /*  end of if ( returnCode == GET_INSERT_SUCCESS )  */
                else {
                    errorMessage( argv[ 0 ], PREPEND );
                    flushErrorMessage();
                }   /*  end of returnCode != GET_DELETE_SUCCESS */

                /*
                 *  Clean-up memory: the non-key search values read for the
                 *  DELETE command allocates memory for each value.  This memory
                 *  needs to be deallocated before the list leaves scope.
                 */
                temp = searchNonKey;
                while ( temp != NULL ) {
                    DataAttribute *next;
                    
                    next = temp->next;
                    free( temp->attribute.value.nonKey );
                    free( temp );
                    temp = next;                    
                }  /*  end of loop over searchNonKey list using temp */
            }   /*  end command == DELETE   */
            else if ( command == INIT ) {
                errorMessage( "Additional INIT command read", REPLACE );
                errorMessage( argv[ 0 ], PREPEND );
                flushErrorMessage();
            }   /*  end command == INIT     */
            else if ( command == INVALID ) {
                errorMessage( argv[ 0 ], PREPEND );
                flushErrorMessage();
            }   /*  end command == INVALID  */

            updateMetricsData( &metrics ); /* stop timing */

            tempTime = getTime(); /* start output timing of command */
            flushOutputBuffer();
            metrics.outputTime += ( getTime() - tempTime );

        }   /*  end of returnCode == GET_NEXT_COMMAND_CODE_SUCCESS */
        else if ( returnCode == GET_NEXT_COMMAND_CODE_IO_ERROR ) {
            errorMessage( argv[ 0 ], PREPEND );
            flushErrorMessage();
            return ( DIS_DATA_MANAGEMENT_ERROR );
        }   /*  else low-level I/O error for getNextCommandCode */
        else if ( returnCode == GET_NEXT_COMMAND_CODE_INVALID_COMMAND ) {
            errorMessage( argv[ 0 ], PREPEND );
            flushErrorMessage();
        }   /*  end return check for getNextCommandCode */

        clearLine( input );
    }   /*  end main loop   */

	/*** VERSABENCH END ***/
	
    /*
     *                  Exit Application
     *
     *      (1) Exit the Metrics module: this consists of calling the 
     *          outputMetricsData() routine to calculate and display the
     *          statistical metric values.
     *      (2) Exit the Database module:  this consists of deleting the
     *          index root node which will recursively delete all nodes,
     *          entries, and data objects currently in the index. 
     *      (3) Exit the Input & Output module: this consists of closing
     *          the three files used during the program execution.  Note
     *          that the three files may point to stdin, stdout, and 
     *          stderr, but no error occurs for closing these files. 
     */
	/*    outputMetricsData( metric, &metrics ); */

    deleteIndexNode( root );

    closeFiles( input, output, metric );

    exit ( DIS_DATA_MANAGEMENT_SUCCESS );
}   /*  end of main()   */
