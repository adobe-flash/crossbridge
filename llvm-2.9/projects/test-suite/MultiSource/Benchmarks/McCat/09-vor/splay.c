
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

#include <stdio.h>
#include <stdlib.h>
#include "struktur.h"

splay_node *find(splay_node *root, long key)
{
  if ((root->element.key < key) && (root->right != NULL))
    return find(root->right,key);
  else if ((root->element.key > key) && (root->left != NULL))
    return find(root->left,key);
  else
    return root;
}

splay_node *rotate(splay_node *sn)
{
  splay_node *temp;
  if(sn->father->left==sn)
    { /*left*/
      sn->father->left = sn->right;
      if (sn->right != NULL)
	sn->right->father = sn->father;
      sn->right = sn->father;
    }
  else
    { /*right*/
      sn->father->right = sn->left;
      if (sn->left != NULL)
	sn->left->father = sn->father;
      sn->left = sn->father;
    }
  temp = sn->father->father;
  sn->father->father = sn;
  if (temp != NULL)
    if (temp -> left == sn->father)
      temp->left = sn;
    else
      temp->right = sn;
  sn->father=temp;
  return sn;
}

void *splay(splay_node **root, long key)
{
  splay_node *temp;

  temp=find(*root,key);
  while (temp->father != NULL)
    {
      if (temp->father->father==NULL)
	{ /* rotate x */
	  temp=rotate(temp);
	}
      else if ((temp->father->left == temp) && 
	       (temp->father->father->left == temp->father))
	{ /* x and y both left children, rotate y, rotate x */
	  rotate(temp->father);
	  temp=rotate(temp);
	}
      else if ((temp->father->right == temp) && 
	       (temp->father->father->right == temp->father))
	{ /* x and y both right children, rotate y, rotate x */
	  rotate(temp->father);
	  temp=rotate(temp);
	}
      else
	{ /* rotate x, rotate x */
	  temp=rotate(temp);
	  temp=rotate(temp);
	}
    }
  (*root)=temp;
}

void free_tree(splay_node *root)
{
  if (root != NULL)
    {
      free_tree(root->left);
      free_tree(root->right);
      free(root);
    }
}

splay_node *create_node(point p)
{
  splay_node *new_node;
  
  if (!(new_node = (splay_node *)malloc(sizeof(splay_node)))) {
    printf("Can't create node\n");
    exit(0);
  }
  
  new_node->left = NULL;
  new_node->right = NULL;
  new_node->father = NULL;
  new_node->element.key = p.x*10000+p.y;
  new_node->element.p = p;
  return new_node;
}

/* Operations */

splay_node *init(void)
{
  return NULL;
}

void *insert(splay_node **root, point p)
{
  splay_node *temp;
  temp = create_node(p);
  if (*root != NULL)
    {
      splay(root,temp->element.key);
      if (temp->element.key == (*root)->element.key) 
	return 0;
      else if ((*root)->element.key > temp->element.key) {
	temp->left = (*root)->left;
	if (temp->left != NULL) temp->left->father = temp; 
	temp->right = (*root);
	(*root)->left = NULL;
      }
      else 
	{
	  temp->right = (*root)->right;      
	  if (temp->right != NULL) temp->right->father = temp;
	  temp->left = (*root);
	  (*root)->right = NULL;
	}
      (*root)->father = temp;
    }
  *root = temp;
}

point delete_min(splay_node **root)
{
  splay_element min_elm;
  splay_node *min_node;
  
  if (*root != NULL) {
    splay(root,-1);
    min_elm = (*root)->element;
    min_node = *root;
    *root = (*root)->right;
    if (*root)
      (*root)->father = NULL;
    free(min_node);
  }
  else 
    {  
      printf("No elements in tree! [delete_min]\n");
      return min_elm.p;
    }
  return min_elm.p;
}
