/***************************************************************************/
/*                                                                         */
/*  Filename : bintree.h                                                   */
/*                                                                         */
/*  Author   : Frederic Bergeron (91 485 12)                               */
/*                                                                         */
/*  Date     : 1996/09/26                                                  */
/*                                                                         */
/***************************************************************************/

#ifndef _bintree
#define _bintree 

#include "general.h"

#include <stdlib.h>

struct binaryTree {
  int                 value;
  struct binaryTree*   left;
  struct binaryTree*  right;
};

struct binaryTree* createBinaryTree(int newValue);

void printBinaryTree(struct binaryTree* tree);

void printSortedBinaryTree(struct binaryTree*  tree);

struct binaryTree* insertSortedBinaryTree(int newValue, 
                                          struct binaryTree** tree);

double getArithmeticMeanBinaryTree(struct binaryTree* tree);

double getArithmeticMeanOptimized(struct binaryTree* tree);

void getArithmeticMeanOptimizedRecurs(struct binaryTree* tree, double* sum, 
                                      double* count);

int memberOfBinaryTree(struct binaryTree* tree, int searchedValue);

void memberOfBinaryTreeRecurs(struct binaryTree* tree, int searchedValue, 
                              int* found);

int memberOfSortedBinaryTree(struct binaryTree* tree, int searchedValue);

int getSizeBinaryTree(struct binaryTree* tree);

double getSumBinaryTree(struct binaryTree* tree);

#endif
