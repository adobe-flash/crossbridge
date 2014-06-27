#include "port.h"
/*
Macros to manage heap data and linked lists.

Assume:

  typedef struct Type Type;
  struct Type {
    Type *succ; };
  Type *node,*head;
  head = NULL;

has been elaborated where head is the head of the list.
Elaborating:

  node = ALLOCNODE(Type);
  INSERTNODE(Type,head,node,succ);

allocates and inserts a list node pointed to by node.
Elaborating:

  DELETENODE(Type,head,node,succ);
  FREENODE(Type,node);

deletes and frees the list node pointed to by node.

Copyright Ken Keller 1981
*/

#ifdef TEST99
#include "kenk.h"
#endif

#define ALLOCNODE(Type)\
(Type *)malloc(sizeof(Type))

#define INSERTNODE(Type,head,node,succ)\
{\
  if((head) == NULL) {\
    (head) = (node);\
    (node)->succ = NULL; }\
  else {\
    (node)->succ = (head);\
    (head) = (node); }\
}

#define DELETENODE(Type,head,node,succ)\
{\
  register Type *lastType,*type;\
\
  if((head) == NULL)\
    ;\
  lastType = NULL;\
  type = (head);\
  while(type->succ != NULL)\
    if(type == (node))\
      break;\
    else {\
      lastType = type;\
      type = type->succ; }\
  if(lastType == NULL)\
    (head) = (node)->succ;\
  else lastType->succ = (node)->succ;\
}

#define FREENODE(Type,node) free(node);

#ifdef TEST99
main()
{
  typedef struct Dragon Dragon;
  struct Dragon {
    Dragon *succ;
    int i; };
  Dragon *head = NULL;
  Dragon *node;
  int cmd,i;

  LOOP {
    printf("1 Insert node.\n");
    printf("2 Delete node.\n");
    printf("3 Print list.\n");
    scanf("%d",&cmd);
    switch(cmd) {
      case 1:
	printf("Integer?");
	scanf("%d",&i);
	node = ALLOCNODE(Dragon);
	INSERTNODE(Dragon,head,node,succ);
	node->i = i;
	break;
      case 2:
	if(head == NULL) {
	  printf("Can't.  List is empty.\n");
	  break; }
	printf("Integer?");
	scanf("%d",&i);
	node = head;
	while(node != NULL)
	  if(node->i == i) {
	    DELETENODE(Dragon,head,node,succ);
	    FREENODE(Dragon,node);
	    break; }
          else node = node->succ;
        if(node == NULL)
	  printf("Can't.  Not in list.\n");
	break;
      case 3:
	if(head == NULL) {
	  printf("List is empty.\n");
	  break; }
	node = head;
	while(node != NULL) {
	  printf("%d ",node->i);
	  node = node->succ; }
        printf("\n");
	break;
      default:
	printf("What?\n");
	break; }/*switch*/ }/*LOOP*/
}
#endif
