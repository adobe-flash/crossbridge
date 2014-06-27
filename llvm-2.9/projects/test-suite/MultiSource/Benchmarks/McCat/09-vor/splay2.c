
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
#include "headers.h"

CHsplay_node *CHfind(CHsplay_node *root, key key)
{
  if (((root->element.key.radius < key.radius) ||
       ((root->element.key.radius == key.radius) &&
	(root->element.key.angle > key.angle)) ||
       ((root->element.key.radius == key.radius) &&
	(root->element.key.angle == key.angle) &&
	(root->element.key.number < key.number))) &&
      (root->right != NULL))
    return CHfind(root->right,key);
  else if (((root->element.key.radius > key.radius) ||
	    ((root->element.key.radius == key.radius) &&
	     (root->element.key.angle < key.angle)) ||
	    ((root->element.key.radius == key.radius) &&
	     (root->element.key.angle == key.angle) &&
	     (root->element.key.number > key.number))) &&
	   (root->left != NULL))
    return CHfind(root->left,key);
  else
    return root;
}
      
/*
  if ((root->element.key.radius < key.radius) && (root->right != NULL))
    return CHfind(root->right,key);
  else if ((root->element.key.radius > key.radius) && (root->left != NULL))
    return CHfind(root->left,key);
  else {
    if ((root->element.key.angle > key.angle) && (root->right != NULL))
      return CHfind(root->right,key);
    else if ((root->element.key.angle > key.angle) && (root->left != NULL))
      return CHfind(root->left,key);
    else {
      if ((root->element.key.number < key.number) && (root->right != NULL))
	return CHfind(root->right,key);
      else if ((root->element.key.number > key.number) && (root->left != NULL))
	return CHfind(root->left,key);
      else
	return root; }
  }
}
*/
CHsplay_node *CHrotate(CHsplay_node *sn)
{
  CHsplay_node *temp;
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

void *CHsplay(CHsplay_node **root, key key)
{
  CHsplay_node *temp;

  temp=CHfind(*root,key);
  while (temp->father != NULL) {
      if (temp->father->father==NULL) { /* CHrotate x */
	  temp=CHrotate(temp); }
      else if ((temp->father->left == temp) && 
	       (temp->father->father->left == temp->father))
	{ /* x and y both left children, CHrotate y, CHrotate x */
	  CHrotate(temp->father);
	  temp=CHrotate(temp); }
      else if ((temp->father->right == temp) && 
	       (temp->father->father->right == temp->father))
	{ /* x and y both right children, CHrotate y, CHrotate x */
	  CHrotate(temp->father);
	  temp=CHrotate(temp); }
      else { /* CHrotate x, CHrotate x */
	  temp=CHrotate(temp);
	  temp=CHrotate(temp); }
    }
  (*root)=temp;
}

void CHtraverse(CHsplay_node *root)
{
  if (root != NULL) {
      CHtraverse(root->left);
      printf("(%d,%d)  key: (%f,%f,%d)\n",((root->element.point)->node.x),
	     ((root->element.point)->node.y),(root->element.key.radius),
	     (root->element.key.angle),(root->element.key.number));
      CHtraverse(root->right); }
}

void CHfree_tree(CHsplay_node *root)
{
  if (root != NULL) {
      CHfree_tree(root->left);
      CHfree_tree(root->right);
      free(root); }
}

CHsplay_node *CHcreate_node(CHpoints *p)
{
  CHsplay_node *new_node;
  dpoint c;
  key key;

  if (!(new_node = (CHsplay_node *)malloc(sizeof(CHsplay_node)))) {
    printf("Can't create node\n");
    exit(0); }
  new_node->left = NULL;
  new_node->right = NULL;
  new_node->father = NULL;
  c=centre(before(p)->node,p->node,next(p)->node);
  key.radius=radius2(p->node,c);
  key.angle=angle(before(p),p,next(p));
  key.number=p->number;
  new_node->element.key = key;
  new_node->element.point = p;
  return new_node;
}

/* Operations */

CHsplay_node *CHinit(void)
{
  return NULL;
}

void *CHinsert(CHsplay_node **root, CHpoints *p) {
  CHsplay_node *temp;
  temp = CHcreate_node(p);

  if (*root != NULL) {
    CHsplay(root,temp->element.key);
    if (((*root)->element.key.radius > temp->element.key.radius) ||
	(((*root)->element.key.radius == temp->element.key.radius) &&
	 ((*root)->element.key.angle < temp->element.key.angle)) ||
	((((*root)->element.key.radius == temp->element.key.radius) &&
	 ((*root)->element.key.angle == temp->element.key.angle)) &&
	 (((*root)->element.key.number > temp->element.key.number)))) {
      temp->left = (*root)->left;
      if (temp->left != NULL) 
	temp->left->father = temp; 
      temp->right = (*root);
      (*root)->left = NULL; }
    else {
      temp->right = (*root)->right;      
      if (temp->right != NULL) 
	temp->right->father = temp;
      temp->left = (*root);
      (*root)->right = NULL; }
    (*root)->father = temp; }
  *root = temp;
}

CHpoints *CHdelete_max(CHsplay_node **root) {
  CHsplay_element max_elm;
  CHsplay_node *max_node;
  key key;
  
  key.radius=((double)3.40282346638528860e+38);
  key.angle=1000;
  key.number=1000;
  if (*root != NULL) {
    CHsplay(root,key);
    max_elm=(*root)->element;
    max_node=*root;
    *root = (*root)->left;
    if (*root)
      (*root)->father = NULL;
    free(max_node); }
  else {  
      printf("No elements in tree! [CHdelete_max]\n");
      return 0; }
  return max_elm.point;
}

void CHdelete(CHsplay_node **root, key key) {
  CHsplay_node *node,*tmp1,*tmp2;
  struct key tmp_key;

  if (*root != NULL) {
    CHsplay(root,key);           /* Splay around the key we want to delete */
    node=*root;                           /* Temporary pointer to be freed */
    tmp1=(*root)->left;                                   /* Left sub tree */
    tmp2=(*root)->right;                                 /* Right sub tree */
    
    if ((tmp1 == NULL) && (tmp2 == NULL))              /* No elements left */
      (*root)=NULL;
    else if ((tmp1 == NULL) && (tmp2 != NULL)) {   /* right sub tree empty */
      (*root)=tmp2;
      (*root)->father=NULL; }
    else if ((tmp1 != NULL) && (tmp2 == NULL)) {    /* left sub tree empty */
      (*root)=tmp1;
      (*root)->father=NULL; }
    else {                                 /* both sub trees are non-empty */
      tmp_key.radius=((double)3.40282346638528860e+38);
      tmp_key.angle=1000;
      tmp_key.number=1000;

      tmp1->father=NULL;
      CHsplay(&tmp1,tmp_key);          /* make tree without right sub tree */
      tmp_key.radius=-1;
      tmp_key.angle=-1;
      tmp_key.number=-1;

      tmp2->father=NULL;                /* make tree without left sub tree */
      CHsplay(&tmp2,tmp_key);
          
      tmp1->right=tmp2;
      tmp1->right->father=tmp1;
      (*root)=tmp1; }
    free(node);
  }
  else
    printf("No elements in tree! [CHdelete]\n");
}
