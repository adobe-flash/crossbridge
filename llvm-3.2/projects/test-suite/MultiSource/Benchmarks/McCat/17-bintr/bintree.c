
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

/***************************************************************************/
/*                                                                                                             */
/*  Filename : bintree.c                                                                                       */
/*                                                                                                             */
/*  Author   : Frederic Bergeron (91 485 12)                                                                   */
/*                                                                                                             */
/*  Date     : 1996/09/26                                                                                      */
/*                                                                                                             */
/***************************************************************************/


#include "bintree.h"


/***************************************************************************/
struct binaryTree*	createBinaryTree(	int newValue	)
/***************************************************************************/
{
	struct binaryTree*	newTree = NULL;

	newTree = (struct binaryTree*) malloc( sizeof( struct binaryTree ) );
	newTree->value = newValue;
	newTree->left = newTree->right = NULL;
	
	return newTree;
}
/***************************************************************************/


/***************************************************************************/
void	printBinaryTree(	struct binaryTree*	tree	)
/***************************************************************************/
{
	if ( tree == NULL )
		printf( "X" );
	else {
		printf( "(%d <L ", tree->value );
		printBinaryTree( tree->left );
		printf( " L> <R " );
		printBinaryTree( tree->right );
		printf( " R>)\n" );
	}
}
/***************************************************************************/


/***************************************************************************/
void	printSortedBinaryTree(	struct binaryTree*	tree	)
/***************************************************************************/
{
	if ( tree ) {
		printSortedBinaryTree( tree->left );
		printf( "%d ", tree->value );
		printSortedBinaryTree( tree->right );
	}
}
/***************************************************************************/


/***************************************************************************/
struct binaryTree* insertSortedBinaryTree( int			newValue,	
					   struct binaryTree**	sortedTree	)
/***************************************************************************/
{
	struct binaryTree*	newTree = createBinaryTree( newValue );
	struct binaryTree*	temp = NULL;
	int			done = FALSE;
	
	if ( *sortedTree == NULL )
		*sortedTree = newTree;
	else {
		temp = *sortedTree;
	
		do {
			if ( newValue < temp->value )
				if ( temp->left == NULL ) {
					temp->left = newTree;
					done = TRUE;
				}
				else 
					temp = temp->left;
			else if ( newValue > temp->value )
				if ( temp->right == NULL ) {
					temp->right = newTree;
					done = TRUE;
				}
				else
					temp = temp->right;
			else 
				done = TRUE; 
		} while ( !done );
	}
					
	return newTree;
}
/***************************************************************************/


/***************************************************************************/
double 	getArithmeticMeanBinaryTree( struct binaryTree* tree )
/**************************************************************************************************************/
{
  return (double) getSumBinaryTree( tree ) / 
  	 (double) getSizeBinaryTree( tree );
}
/**************************************************************************************************************/


/**************************************************************************************************************/
double 	getArithmeticMeanOptimized( struct binaryTree* tree)
/**************************************************************************************************************/
{
  double sum = 0.0, count = 0.0;

  getArithmeticMeanOptimizedRecurs( tree, &sum, &count );
  
  return sum / count;
}
/**************************************************************************************************************/


/**************************************************************************************************************/
void getArithmeticMeanOptimizedRecurs( 	struct binaryTree* 	tree, 
					double* 		sum, 
					double* 		count )
/**************************************************************************************************************/
{
  if ( tree ) {
    *sum = *sum + tree->value;
    *count = *count + 1;
    getArithmeticMeanOptimizedRecurs( tree->left, sum, count );
    getArithmeticMeanOptimizedRecurs( tree->right, sum, count );
  }
}
/**************************************************************************************************************/


/**************************************************************************************************************/
int memberOfBinaryTree( struct binaryTree* tree, int searchedValue )
/**************************************************************************************************************/
{
  int found = FALSE;

  memberOfBinaryTreeRecurs( tree, searchedValue, &found );

  return found;
}
/**************************************************************************************************************/


/*************************************************************************************************************/
void memberOfBinaryTreeRecurs( 	struct binaryTree* 	tree, 
				int 			searchedValue, 
				int* 			found )
/*************************************************************************************************************/
{
  if (tree) {
        
    if (tree->value == searchedValue)
      *found = TRUE;
    else {
      memberOfBinaryTreeRecurs( tree->left, searchedValue, found );
      if ( ! (*found) )      
	memberOfBinaryTreeRecurs( tree->right, searchedValue, found );
	
    }
  }
}
/*************************************************************************************************************/


/**************************************************************************************************************/
int memberOfSortedBinaryTree( struct binaryTree* tree, int searchedValue )
/**************************************************************************************************************/
{
  struct binaryTree* seeker = tree;
  int                found = FALSE;

  while ((!found) && (seeker)) {
    if (searchedValue < seeker->value)
      seeker = seeker->left;
    else if (searchedValue > seeker->value)
      seeker = seeker->right;
    else
      found = TRUE;
  }

  return found;
}
/**************************************************************************************************************/


/**************************************************************************************************************/
int getSizeBinaryTree( struct binaryTree* tree )
/**************************************************************************************************************/
{
  if ( tree )
    return 1 + 
    	   getSizeBinaryTree( tree->left ) + 
    	   getSizeBinaryTree( tree->right );
  else
    return 0;
}
/**************************************************************************************************************/


/**************************************************************************************************************/
double getSumBinaryTree( struct binaryTree* tree )
/**************************************************************************************************************/
{
  if ( tree )
    return (double) tree->value + 
           getSumBinaryTree( tree->left ) + 
           getSumBinaryTree( tree->right );
  else
    return (double) 0;
}
/**************************************************************************************************************/


