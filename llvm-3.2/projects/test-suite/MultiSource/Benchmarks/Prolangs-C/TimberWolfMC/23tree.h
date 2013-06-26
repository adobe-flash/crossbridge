#include "port.h"

typedef struct plist {
    int property ;
    struct plist *pnext  ;
} 
PLIST, *PLISTPTR ;

typedef struct tnode {
    int value ;
    int nsons ;
    struct tnode *son1  ;
    struct tnode *son2  ;
    struct tnode *son3  ;
    struct tnode *son4  ;
    struct tnode *father;
    int Lval  ;
    int Mval  ;
    PLISTPTR plist ;
} 
TNODE, *TNODEPTR ;

#define RAND ((randVar = randVar * 1103515245 + 12345) & 0x7fffffff)

TNODEPTR tsearch() ;
TNODEPTR tdsearch() ;
TNODEPTR tfind() ;
PLISTPTR pfind() ;
PLISTPTR tplist() ;
