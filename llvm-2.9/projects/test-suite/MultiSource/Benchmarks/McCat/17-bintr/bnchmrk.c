
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

/**************************************************************************************/
/*                                                                                    */
/*  Title       : Assignment #1 - Benchmark for McCat compiler                        */
/*                                                                                    */
/*  Course      : CS621 - Advanced Compiler Techniques (Clark Verbrugge)              */
/*                                                                                    */
/*  Filename    : bnchmrk.c                                                           */
/*                                                                                    */
/*  Author      : Frederic Bergeron (91 485 12)                                       */
/*                                                                                    */
/*  Date        : 1996/10/01                                                          */
/*                                                                                    */
/*  Description : This program is a benchmark for the McCat Compiler.  Its main       */
/*                goal is to test recursive calls and dynamic allocation.  It         */
/*                uses a simple binary tree structure of integers.  It computes       */
/*                the arithmetic average of all the numbers in the tree and           */
/*                performs some research in the tree.                                 */
/*                                                                                    */
/**************************************************************************************/


#include "general.h"
#include "bintree.h"


#define MAX_SEARCHED_VALUES 	100
#define TRACE 			0


/**************************************************************************************/
/* Function declarations                                                              */
/**************************************************************************************/

void	fillTree(		struct binaryTree**	treeToFill	);
void    fillSearchValues( 	int 			arrayOfValues[] );

/**************************************************************************************/


/**************************************************************************************/
void	fillTree(	struct binaryTree**	treeToFill	)
/**************************************************************************************/
{
	int	number;

	printf( "Constructing tree\n\n\n" );
	
	scanf( "%d", &number );

	while (number != 0) {
	
		insertSortedBinaryTree( number, treeToFill ); 
		if (TRACE)
		  printf ("%d inserted\n", number);
		scanf( "%d", &number );
	}
	
	printf( "\n\nTree constructed\n\n\n" );
}
/**************************************************************************************/


/**************************************************************************************/
void 	fillSearchedValues( 	int 	arrayOfValues[] 	)
/**************************************************************************************/
{
	int	number, i = 0;

	printf( "Constructing array of values\n\n\n" );
	
	scanf( "%d", &number ); 

	while ( (i<MAX_SEARCHED_VALUES) && (number!=0) ) {
		
		arrayOfValues[i] = number;
		if (TRACE)
		  printf ("%d read; %d inserted\n", number, arrayOfValues[i] );
		scanf( "%d", &number );
		i++;
	}
	
	printf( "\n\nArray of values constructed\n\n\n" );
}
/**************************************************************************************/


/**************************************************************************************/
int main()
/**************************************************************************************/
{
	struct binaryTree*	tree = NULL;
        int                     searchedValues[MAX_SEARCHED_VALUES];
	int                     i;

	printf( "Beginning of program\n\n\n" );
	
	for( i=0; i<10; i++) 
		searchedValues[i]=0;
	
	fillTree( &tree );
        fillSearchedValues( searchedValues );
        
	if (TRACE) {
	  printBinaryTree( tree );
	  printf( "\n\n" );
	  printSortedBinaryTree( tree );
	  printf( "\n\n" );
	}

	printf( "Summary of sorted binary tree\n=============================\n\n" );
	printf( "Size                   : %d\n\n", getSizeBinaryTree( tree ) );
	printf( "Sum                    : %f\n\n", getSumBinaryTree( tree ) );
	printf( "Arithmetic Mean        : %f\n\n", getArithmeticMeanBinaryTree( tree ) );
//	printf( "Arithmetic Mean (opt.) : %f\n\n", getArithmeticMeanOptimized( tree ) );
	
	for( i=0; i<MAX_SEARCHED_VALUES; i++) {
		if (memberOfBinaryTree( tree, searchedValues[i] ))
			printf( "%d is in the tree.\n",	searchedValues[i] );
		else
			printf( "%d is NOT in the tree.\n", searchedValues[i] );
	}
	printf( "\n\n" );
//	for( i=0; i<MAX_SEARCHED_VALUES; i++) {
//		if (memberOfSortedBinaryTree( tree, searchedValues[i] ))
//			printf( "%d is in the tree.\n",	searchedValues[i] );
//		else
//			printf( "%d is NOT in the tree.\n", searchedValues[i] );
//	}
//	printf( "\n\n" );
	
	
	printf( "\n\nEnd of program\n" );
        return 0;
}
/**************************************************************************************/




