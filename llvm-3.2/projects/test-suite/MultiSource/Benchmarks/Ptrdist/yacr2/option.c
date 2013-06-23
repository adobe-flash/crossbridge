
/*
 *
 * option.c
 *
 */

#include <string.h>

#define OPTION_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "channel.h"


/*
 *
 * Code.
 *
 */

void
Option(int argc,
       char *argv[])
{
    /*
     * Check arguments.
     */
    if (argc != 2) {
	printf("\nUsage: yacr2 <filename>\n\n");
	exit(1);
    }

    /*
     * Specified options.
     */
    channelFile = argv[1];
}
