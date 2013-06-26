/*
 *  Some basic 2-3 tree functions:
 *       1. tinsert( &root , value , property ) ;
 *          TNODEPTR root ;
 *          int value , property ;
 *   
 *       2. tdelete( &root , value , property ) ;
 *          TNODEPTR root ;
 *          int value , property ;
 *      
 *       3. tpop( &root , &node , &value , &property ) ;
 *          TNODEPTR root , node ;
 *          int value , property ;
 *      
 *       4. ttrim( &root , threshold ) ;
 *          TNODEPTR root ;
 *          int threshold ;
 *      
 *       5. tprint( root ) ;
 *          TNODEPTR root ;
 *
 *       6. tdiscard( root , count )
 *          TNODEPTR *root ;
 *          int count ;
 *
 *       7. int tprop( r , value )   --- returns FIRST property
 *          TNODEPTR r ;             associated with: value  of
 *          int value ;              tree with root: r  ---
 *
 *       8. PLISTPTR tplist( r , value )  ---- return ptr to FIRST
 *          TNODEPTR r ;                       property associated with
 *          int value ;                        value of tree with root r.
 *
 *    To use the 2-3 tree implementation, compile the file: 23tree.c
 *    with your source files and include the line: #include "23tree.h"
 *    in any of your source files which use the 2-3 tree functions.
 *    You may have several 2-3 trees active at once. Each tree must
 *    given a "root", whose type is necessarily: TNODEPTR.  That is,
 *    if you want 3 trees to be active, you must declare 3 roots in
 *    the following fashion:
 *        TNODEPTR root1 , root2 , root3 ;
 *    Finally, before you can use any of the functions, you must
 *    initialize the roots to NULL, as follows:
 *        root1 = (TNODEPTR) NULL ;
 *        root2 = (TNODEPTR) NULL ;
 *        root3 = (TNODEPTR) NULL ;
 */
#include "23tree.h"

void tdump( TNODEPTR node );
void tdelete( TNODEPTR *root , int value , int property );
void tpatch( TNODEPTR v , int value );
void makenode( TNODEPTR *v , int value , int property );
void addplist( TNODEPTR v , int property );
void tsubson( TNODEPTR *root , TNODEPTR l );


void tprint( TNODEPTR v )
{

int qbot , qtop ;
TNODEPTR node , *q , f ;

if( v == (TNODEPTR) NULL ) {
    return ;
}
qtop = 0 ;
qbot = 0 ;
q = (TNODEPTR *) malloc( 10001 * sizeof(TNODEPTR) ) ;
q[0] = v ;
while( qtop >= qbot ) {
    tdump( node = q[qbot++] ) ;
    if( node != v ) {
	f = node->father ;
	if( f->son1 != node && f->son2 != node && f->son3 != node ) {
	    printf("Error Type 1\n");
	}
	if( node->nsons != 0 ) {
	    if( node->son1->father->Lval != node->Lval ) {
		printf("Error Type 2\n");
	    }
	    if( node->son1->father->Mval != node->Mval ) {
		printf("Error Type 3\n");
	    }
	}
    }
    if( node->nsons > 0 ) {
	q[++qtop] = node->son1 ;
	q[++qtop] = node->son2 ;
	if( node->son3 != (TNODEPTR) NULL ) {
	    q[++qtop] = node->son3 ;
	}
    }
}
free(q) ;
/*
tmin( v, &node, &value, &property );
printf("\n\nMinimum Tree Value: %d\n", value );
*/
return ;
}


void tdump( TNODEPTR node )
{

PLISTPTR plptr ;

printf("\n");
if( node->nsons == 0 ) {
    printf("value: %d  ", node->value );
    plptr = node->plist ;
    for( ; plptr != (PLISTPTR) NULL; plptr = plptr->pnext ) {
	printf("%d ", plptr->property ) ;
    }
    printf("\n");
} else{
    /* printf("Lval: %d   Mval: %d\n", node->Lval , node->Mval );
    */
}
return ;
}



void tmin( TNODEPTR root , TNODEPTR *node , int *value , int *property )
{

TNODEPTR v ;

v = root ;
if( v == (TNODEPTR) NULL ) {
    *node = (TNODEPTR) NULL ;
} else {
    while( v->son1 != (TNODEPTR) NULL ) {
	v = v->son1 ;
    }
    *node = v ;
    *value = v->value ;
    *property = v->plist->property ;
}

return;
}



void tpop( TNODEPTR *root , TNODEPTR *node , int *value , int *property )
{

TNODEPTR v ;

v = *root ;
if( v == (TNODEPTR) NULL ) {
    *node = (TNODEPTR) NULL ;
} else {
    while( v->son1 != (TNODEPTR) NULL ) {
	v = v->son1 ;
    }
    *node = v ;
    *value = v->value ;
    *property = v->plist->property ;
    tdelete( root , *value , *property ) ;
}

return;
}



void tmax( TNODEPTR *root , TNODEPTR *node , int *value , int *property )
{

TNODEPTR v ;

v = *root ;
if( v == (TNODEPTR) NULL ) {
    *node = (TNODEPTR) NULL ;
} else {
    for( ; ; ) {
	if( v->nsons == 3 ) {
	    v = v->son3 ;
	} else if( v->nsons == 2 ) {
	    v = v->son2 ;
	} else {
	    break ;
	}
    }
    *node = v ;
    *value = v->value ;
    *property = v->plist->property ;
}

return;
}



void ttrim( TNODEPTR *root , int threshold )
{

TNODEPTR node ;
int property , value ;

for( ; ; ) {
    tmax( root , &node , &value , &property ) ;
    if( node == (TNODEPTR) NULL ) {
	break ;
    } else if( value < threshold ) {
	break ;
    } else {
	tdelete( root , value , property ) ;
    }
}

return;
}




void tdiscard( TNODEPTR *root , int count )
{

TNODEPTR node ;
int property , value , i ;

for( i = 1 ; i <= count ; i++ ) {
    tmax( root , &node , &value , &property ) ;
    if( node == (TNODEPTR) NULL ) {
	break ;
    } else {
	tdelete( root , value , property ) ;
    }
}
return;
}




TNODEPTR tsearch( int val , TNODEPTR r )
{

if( r->son1->nsons == 0 ) {
    return( r ) ;
} else {
    if( val <= r->Lval ) {
	return( tsearch( val , r->son1 ) ) ;
    } else if( r->nsons == 2 || val <= r->Mval ) {
	return( tsearch( val , r->son2 ) ) ;
    } else {
	return( tsearch( val , r->son3 ) ) ;
    }
}
}


void taddson( TNODEPTR *root , TNODEPTR r )
{

TNODEPTR v , nr , f ;

v = (TNODEPTR) malloc( sizeof( TNODE ) ) ;
v->son1 = r->son3 ;
v->son2 = r->son4 ;
v->son3 = (TNODEPTR) NULL ;
v->son4 = (TNODEPTR) NULL ;
v->father = r->father ;
v->nsons = 2 ;
r->nsons = 2 ;
r->son3 = (TNODEPTR) NULL ;
r->son4 = (TNODEPTR) NULL ;
v->son1->father = v ;
v->son2->father = v ;

f = r->son1 ;
while( f->nsons == 3 ) {
    f = f->son3 ;
}
if( f->nsons == 0 ) {
    r->Lval = f->value ;
} else {
    r->Lval = f->Mval ;
}
f = r->son2 ;
while( f->nsons == 3 ) {
    f = f->son3 ;
}
if( f->nsons == 0 ) {
    r->Mval = f->value ;
} else {
    r->Mval = f->Mval ;
}
f = v->son1 ;
while( f->nsons == 3 ) {
    f = f->son3 ;
}
if( f->nsons == 0 ) {
    v->Lval = f->value ;
} else {
    v->Lval = f->Mval ;
}
f = v->son2 ;
while( f->nsons == 3 ) {
    f = f->son3 ;
}
if( f->nsons == 0 ) {
    v->Mval = f->value ;
} else {
    v->Mval = f->Mval ;
}

if( r->father == (TNODEPTR) NULL ) {
    nr = (TNODEPTR) malloc( sizeof( TNODE ) ) ;
    nr->son1 = r ;
    nr->son2 = v ;
    nr->son3 = (TNODEPTR) NULL ;
    nr->son4 = (TNODEPTR) NULL ;
    nr->father = (TNODEPTR) NULL ;
    nr->nsons = 2 ;
    r->father = nr ;
    v->father = nr ;
    nr->Lval = r->Mval ;
    nr->Mval = v->Mval ;
    *root = nr ;
} else {
    f = r->father ;
    if( ++f->nsons == 4 ) {
	if( f->son1 == r ) {
	    f->son4 = f->son3 ;
	    f->son3 = f->son2 ;
	    f->son2 = v ;
	} else if( f->son2 == r ) {
	    f->son4 = f->son3 ;
	    f->son3 = v ;
	} else {
	    f->son4 = v ;
	}
	taddson( root , f ) ;
    } else {
	if( f->son1 == r ) {
	    f->son3 = f->son2 ;
	    f->son2 = v ;
	    f->Lval = r->Mval ;
	    f->Mval = v->Mval ;
	} else {
	    f->son3 = v ;
	    f->Mval = r->Mval ;
	    tpatch( f , v->Mval ) ;
	}
    }
}
return ;
}





void tinsert( TNODEPTR *root , int value , int property )
{

TNODEPTR v , nr , f , node ;

if( *root == (TNODEPTR) NULL ) {
    makenode( &v , value , property ) ;
    v->father = (TNODEPTR) NULL ;
    *root = v ;
} else if( (*root)->nsons == 0 ) {
    if( (*root)->value == value ) {
	addplist( *root , property ) ;
    } else {
	makenode( &v , value , property ) ;
	nr = (TNODEPTR) malloc( sizeof( TNODE ) ) ;
	if( value <= (*root)->value ) {
	    nr->Lval = value ;
	    nr->Mval = (*root)->value ;
	    nr->son1 = v ;
	    nr->son2 = *root ;
	    nr->son3 = (TNODEPTR) NULL ;
	    nr->son4 = (TNODEPTR) NULL ;
	} else {
	    nr->Mval = value ;
	    nr->Lval = (*root)->value ;
	    nr->son1 = *root ;
	    nr->son2 = v ;
	    nr->son3 = (TNODEPTR) NULL ;
	    nr->son4 = (TNODEPTR) NULL ;
	}
	nr->father = (TNODEPTR) NULL ;
	nr->nsons = 2 ;
	(*root)->father = nr ;
	v->father = nr ;
	*root = nr ;
    }
} else {
    f = tsearch( value , *root ) ;
    node = tfind( value , f ) ;
    if( node != (TNODEPTR) NULL ) {
	addplist( node , property ) ;
    } else {
	makenode( &v , value , property ) ;
	if( f->nsons == 2 ) {
	    if( value <= f->son1->value ) {
		f->son3 = f->son2 ;
		f->son2 = f->son1 ;
		f->son1 = v ;
		f->Mval = f->Lval ;
		f->Lval = value ;
	    } else if( value <= f->son2->value ) {
		f->son3 = f->son2 ;
		f->son2 = v ;
		f->Mval = value ;
	    } else {
		f->son3 = v ;
		tpatch( f , value ) ;
	    }
	    f->nsons = 3 ;
	    v->father = f ;
	} else {
	    if( value <= f->son1->value ) {
		f->son4 = f->son3 ;
		f->son3 = f->son2 ;
		f->son2 = f->son1 ;
		f->son1 = v ;
	    } else if( value <= f->son2->value ) {
		f->son4 = f->son3 ;
		f->son3 = f->son2 ;
		f->son2 = v ;
	    } else if( value <= f->son3->value ) {
		f->son4 = f->son3 ;
		f->son3 = v ;
	    } else {
		f->son4 = v ;
	    }
	    v->father = f ;
	    taddson( root , f ) ;
	}
    }
}
return ;
}



void tpatch( TNODEPTR v , int value )
{

TNODEPTR f ;

f = v->father ;
if( f != (TNODEPTR) NULL ) {
    if( f->son1 == v ) {
	f->Lval = value ;
    } else if( f->son2 == v ) {
	f->Mval = value ;
	if( f->son3 == (TNODEPTR) NULL ) {
	    tpatch( f , value ) ;
	}
    } else {
	tpatch( f , value ) ;
    }
} 
return ;
}




void tdelete( TNODEPTR *root , int value , int property )
{

TNODEPTR f , l , g , s ;
PLISTPTR plptr , prevptr ;

if( *root == (TNODEPTR) NULL ) {
} else if( (*root)->nsons == 0 ) {
    plptr = pfind( *root , property , &prevptr ) ;
    if( plptr != (PLISTPTR) NULL ) {
	if( prevptr != (PLISTPTR) NULL ) {
	    prevptr->pnext = plptr->pnext ;
	    free( plptr ) ;
	} else if( plptr->pnext != (PLISTPTR) NULL ) {
	    (*root)->plist = plptr->pnext ;
	    free( plptr ) ;
	} else {
	    free( plptr ) ;
	    free( *root ) ;
	    *root = (TNODEPTR) NULL ;
	}
    }
} else {
    l = tdsearch( value , *root ) ;
    if( l != (TNODEPTR) NULL ) {
	plptr = pfind( l , property , &prevptr ) ;
	if( plptr != (PLISTPTR) NULL ) {
	    if( prevptr != (PLISTPTR) NULL ) {
		prevptr->pnext = plptr->pnext ;
		free( plptr ) ;
	    } else if( plptr->pnext != (PLISTPTR) NULL ) {
		l->plist = plptr->pnext ;
		free( plptr ) ;
	    } else {
		free( plptr ) ;
		f = l->father ;
		if( f->nsons == 3 ) {
		    if( f->son1 == l ) {
			f->son1 = f->son2 ;
			f->son2 = f->son3 ;
			f->son3 = (TNODEPTR) NULL ;
			f->Lval = f->son1->value ;
			f->Mval = f->son2->value ;
		    } else if( f->son2 == l ) {
			f->son2 = f->son3 ;
			f->son3 = (TNODEPTR) NULL ;
			f->Mval = f->son2->value ;
		    } else {
			f->son3 = (TNODEPTR) NULL ;
			tpatch( f , value ) ;
		    }
		    free( l ) ;
		    f->nsons = 2 ;
		} else {
		    if( f == *root ) {
			if( (*root)->son1 == l ) {
			    (*root)->value = (*root)->son2->value ;
			    (*root)->plist = (*root)->son2->plist ;
			} else {
			    (*root)->value = (*root)->son1->value ;
			    (*root)->plist = (*root)->son1->plist  ;
			}
			free( (*root)->son1 ) ;
			free( (*root)->son2 ) ;
			(*root)->son1 = (TNODEPTR) NULL ;
			(*root)->son2 = (TNODEPTR) NULL ;
			(*root)->nsons = 0 ;
		    } else {
			if( f->son1 == l ) {
			    s = f->son2 ;
			} else {
			    s = f->son1 ;
			}
			if( f->father->son1 == f ) {
			    g = f->father->son2 ;
			    if( f->father->nsons == 2 ) {
				if( g->nsons == 2 ) {
				    g->nsons = 3 ;
				    g->son3 = g->son2 ;
				    g->son2 = g->son1 ;
				    g->son1 = s ;
				    s->father = g ;
				    free( l ) ;
				    g->Lval = s->value ;
				    g->Mval = g->son2->value ;
				    tsubson( root , f ) ;
				} else {
				    g->nsons = 2 ;
				    f->son1 = s ;
				    f->son2 = g->son1 ;
				    g->son1 = g->son2 ;
				    g->son2 = g->son3 ;
				    f->son2->father = f ;
				    g->son3 = (TNODEPTR) NULL ;
				    f->Lval = s->value ;
				    f->Mval = f->son2->value ;
				    g->Lval = g->son1->value ;
				    g->Mval = g->son2->value ;
				    g->father->Lval = f->Mval ;
				    free( l ) ;
				}
			    } else {
				if( g->nsons == 2 ) {
				    g->nsons = 3 ;
				    g->son3 = g->son2 ;
				    g->son2 = g->son1 ;
				    g->son1 = s ;
				    s->father = g ;
				    free( l ) ;
				    g->Lval = s->value ;
				    g->Mval = g->son2->value ;
				    f->father->son1 = f->father->son2 ;
				    f->father->son2 = f->father->son3 ;
				    f->father->son3 = (TNODEPTR) NULL ;
				    f->father->nsons = 2 ;
				    f->father->Lval = f->father->Mval ;
				    if( f->father->son2->nsons == 2 ) {
					f->father->Mval = 
						f->father->son2->Mval ;
				    } else {
					f->father->Mval =
					 f->father->son2->son3->value ;
				    }
				    free( f ) ;
				} else {
				    g->nsons = 2 ;
				    f->son1 = s ;
				    f->son2 = g->son1 ;
				    g->son1 = g->son2 ;
				    g->son2 = g->son3 ;
				    f->son2->father = f ;
				    g->son3 = (TNODEPTR) NULL ;
				    f->Lval = s->value ;
				    f->Mval = f->son2->value ;
				    g->Lval = g->son1->value ;
				    g->Mval = g->son2->value ;
				    g->father->Lval = f->Mval ;
				    free( l ) ;
				}
			    }
			} else if( f->father->son2 == f ) {
			    if( f->father->nsons == 2 ) {
				g = f->father->son1 ;
				if( g->nsons == 2 ) {
				    g->nsons = 3 ;
				    g->son3 = s ;
				    s->father = g ;
				    free( l ) ;
				    f->father->Lval = s->value ;
				    tsubson( root , f ) ;
				} else {
				    g->nsons = 2 ;
				    f->son1 = g->son3 ;
				    f->son2 = s ;
				    f->son1->father = f ;
				    g->son3 = (TNODEPTR) NULL ;
				    f->Lval = f->son1->value ;
				    f->Mval = s->value ;
				    f->father->Lval = g->Mval ;
				    f->father->Mval = f->Mval ;
				    tpatch( f->father , f->Mval ) ;
				    free( l ) ;
				}
			    } else {
				g = f->father->son3 ;
				if( g->nsons == 2 ) {
				    g->nsons = 3 ;
				    g->son3 = g->son2 ;
				    g->son2 = g->son1 ;
				    g->son1 = s ;
				    s->father = g ;
				    free( l ) ;
				    g->Lval = s->value ;
				    g->Mval = g->son2->value ;
				    g->father->Mval = g->son3->value ;
				    free( f ) ;
				    g->father->son2 = g->father->son3 ;
				    g->father->son3 = (TNODEPTR) NULL ;
				    g->father->nsons = 2 ;
				} else {
				    g->nsons = 2 ;
				    f->son1 = s ;
				    f->son2 = g->son1 ;
				    f->son2->father = f ;
				    g->son1 = g->son2 ;
				    g->son2 = g->son3 ;
				    g->son3 = (TNODEPTR) NULL ;
				    g->Lval = g->son1->value ;
				    g->Mval = g->son2->value ;
				    f->Lval = f->son1->value ;
				    f->Mval = f->son2->value ;
				    f->father->Mval = f->Mval ;
				    free( l ) ;
				}
			    }
			} else {
			    g = f->father->son2 ;
			    if( g->nsons == 2 ) {
				g->nsons = 3 ;
				g->son3 = s ;
				s->father = g ;
				free( l ) ;
				g->father->Mval = s->value ;
				tpatch( g->father , s->value ) ;
				g->father->son3 = (TNODEPTR) NULL ;
				g->father->nsons = 2 ;
				free( f ) ;
			    } else {
				g->nsons = 2 ;
				f->son1 = g->son3 ;
				f->son2 = s ;
				f->son1->father = f ;
				g->son3 = (TNODEPTR) NULL ;
				f->Lval = f->son1->value ;
				f->Mval = s->value ;
				g->father->Mval = g->Mval ;
				tpatch( f->father , s->value ) ;
				free( l ) ;
			    }
			}
		    }
		}
	    }
	}
    }
}
return ;
}


void tsubson( TNODEPTR *root , TNODEPTR l )
{

TNODEPTR f , s , g ;
int LMval ;

f = l->father ;
if( f == *root ) {
    if( (*root)->son1 == l ) {
	s = (*root)->son2 ;
    } else {
	s = (*root)->son1 ;
    }
    s->father = (TNODEPTR) NULL ;
    free( l ) ;
    free( *root ) ;
    *root = s ;
} else {
    if( f->son1 == l ) {
	s = f->son2 ;
	LMval = f->Mval ;
    } else {
	s = f->son1 ;
	LMval = f->Lval ;
    }
    if( f->father->son1 == f ) {
	g = f->father->son2 ;
	if( f->father->nsons == 2 ) {
	    if( g->nsons == 2 ) {
		g->nsons = 3 ;
		g->son3 = g->son2 ;
		g->son2 = g->son1 ;
		g->son1 = s ;
		s->father = g ;
		free( l ) ;
		g->Mval = g->Lval ;
		g->Lval = LMval ;
		tsubson( root , f ) ;
	    } else {
		g->nsons = 2 ;
		f->son1 = s ;
		f->son2 = g->son1 ;
		g->son1 = g->son2 ;
		g->son2 = g->son3 ;
		f->son2->father = f ;
		g->son3 = (TNODEPTR) NULL ;
		f->Lval = LMval ;
		f->Mval = g->Lval ;
		g->Lval = g->Mval ;
		g->Mval = g->father->Mval ;
		g->father->Lval = f->Mval ;
		free( l ) ;
	    }
	} else {
	    if( g->nsons == 2 ) {
		g->nsons = 3 ;
		g->son3 = g->son2 ;
		g->son2 = g->son1 ;
		g->son1 = s ;
		s->father = g ;
		free( l ) ;
		g->Mval = g->Lval ;
		g->Lval = LMval ;
		f->father->son1 = f->father->son2 ;
		f->father->son2 = f->father->son3 ;
		f->father->son3 = (TNODEPTR) NULL ;
		f->father->nsons = 2 ;
		f->father->Lval = f->father->Mval ;
		free( f ) ;
		f = g->father->son2 ;
		while( f->nsons == 3 ) {
		    f = f->son3 ;
		}
		if( f->nsons == 0 ) {
		    g->father->Mval = f->value ;
		} else {
		    g->father->Mval = f->Mval ;
		}
	    } else {
		g->nsons = 2 ;
		f->son1 = s ;
		f->son2 = g->son1 ;
		g->son1 = g->son2 ;
		g->son2 = g->son3 ;
		f->son2->father = f ;
		g->son3 = (TNODEPTR) NULL ;
		f->Lval = LMval ;
		f->Mval = g->Lval ;
		g->Lval = g->Mval ;
		g->Mval = g->father->Mval ;
		g->father->Lval = f->Mval ;
		free( l ) ;
	    }
	}
    } else if( f->father->son2 == f ) {
	if( f->father->nsons == 2 ) {
	    g = f->father->son1 ;
	    if( g->nsons == 2 ) {
		g->nsons = 3 ;
		g->son3 = s ;
		s->father = g ;
		free( l ) ;
		f->father->Lval = LMval ;
		tsubson( root , f ) ;
	    } else {
		g->nsons = 2 ;
		f->son1 = g->son3 ;
		f->son2 = s ;
		f->son1->father = f ;
		g->son3 = (TNODEPTR) NULL ;
		f->Lval = g->father->Lval ;
		f->Mval = LMval ;
		g->father->Lval = g->Mval ;
		g->father->Mval = LMval ;
		tpatch( f->father , f->Mval ) ;
		free( l ) ;
	    }
	} else {
	    g = f->father->son3 ;
	    if( g->nsons == 2 ) {
		g->nsons = 3 ;
		g->son3 = g->son2 ;
		g->son2 = g->son1 ;
		g->son1 = s ;
		s->father = g ;
		free( l ) ;
		g->father->Mval = g->Mval ;
		g->Mval = g->Lval ;
		g->Lval = LMval ;
		free( f ) ;
		g->father->son2 = g->father->son3 ;
		g->father->son3 = (TNODEPTR) NULL ;
		g->father->nsons = 2 ;
	    } else {
		g->nsons = 2 ;
		f->son1 = s ;
		f->son2 = g->son1 ;
		f->son2->father = f ;
		g->son1 = g->son2 ;
		g->son2 = g->son3 ;
		g->son3 = (TNODEPTR) NULL ;
		f->Lval = LMval ;
		f->Mval = g->Lval ;
		g->Lval = g->Mval ;
		g->father->Mval = f->Mval ;
		f = g->son2 ;
		while( f->nsons == 3 ) {
		    f = f->son3 ;
		}
		if( f->nsons == 0 ) {
		    g->Mval = f->value ;
		} else {
		    g->Mval = f->Mval ;
		}
		free( l ) ;
	    }
	}
    } else {
	g = f->father->son2 ;
	if( g->nsons == 2 ) {
	    g->nsons = 3 ;
	    g->son3 = s ;
	    s->father = g ;
	    free( l ) ;
	    g->father->Mval = LMval ;
	    tpatch( g->father , LMval ) ;
	    g->father->son3 = (TNODEPTR) NULL ;
	    g->father->nsons = 2 ;
	    free( f ) ;
	} else {
	    g->nsons = 2 ;
	    f->son1 = g->son3 ;
	    f->son2 = s ;
	    f->son1->father = f ;
	    g->son3 = (TNODEPTR) NULL ;
	    f->Lval = g->father->Mval ;
	    f->Mval = LMval ;
	    g->father->Mval = g->Mval ;
	    tpatch( f->father , LMval ) ;
	    free( l ) ;
	}
    }
}
return ;
}



TNODEPTR tdsearch( int value , TNODEPTR r )
{

if( r == (TNODEPTR) NULL ) {
    return( (TNODEPTR) NULL ) ;
} else if( r->nsons == 0 ) {
    return( r ) ;
} else {
    if( value <= r->Lval ) {
	return( tdsearch( value , r->son1 ) ) ;
    } else if( value <= r->Mval ) { 
	return( tdsearch( value , r->son2 ) ) ;
    } else {
	return( tdsearch( value , r->son3 ) ) ;
    }
}
}


TNODEPTR tfind( int value , TNODEPTR r )
{

if( r->son1->value == value ) {
    return( r->son1 ) ;
} else if( r->son2->value == value ) {
    return( r->son2 ) ;
} else if( r->nsons == 3 ) {
    if( r->son3->value == value ) {
	return( r->son3 ) ;
    }
} 
return( (TNODEPTR) NULL ) ;
}



void makenode( TNODEPTR *v , int value , int property )
{
*v = (TNODEPTR) malloc( sizeof( TNODE ) ) ;
(*v)->son1 = (TNODEPTR) NULL ;
(*v)->son2 = (TNODEPTR) NULL ;
(*v)->son3 = (TNODEPTR) NULL ;
(*v)->son4 = (TNODEPTR) NULL ;
(*v)->value = value ;
(*v)->nsons = 0 ;
(*v)->plist = (PLISTPTR) malloc( sizeof( PLIST ) ) ;
(*v)->plist->property = property ;
(*v)->plist->pnext = (PLISTPTR) NULL ;

return ;
}



void addplist( TNODEPTR v , int property ) 
{

PLISTPTR plptr ;

plptr = (PLISTPTR) malloc( sizeof( PLIST ) ) ;
plptr->pnext = v->plist ;
v->plist = plptr ;
v->plist->property = property ;

return ;
}



PLISTPTR pfind( TNODEPTR v , int property , PLISTPTR *prevptr ) 
{

PLISTPTR plptr ;

*prevptr = (PLISTPTR) NULL ;
plptr = v->plist ;
while( plptr != (PLISTPTR) NULL ) {
    if( plptr->property == property ) {
	return( plptr ) ;
    }
    *prevptr = plptr ;
    plptr = plptr->pnext ;
}

return( (PLISTPTR) NULL ) ;
}



int tprop( TNODEPTR r , int value )
{

TNODEPTR v , f ;

if( r == (TNODEPTR) NULL ) {
    return( -1 ) ;
} else if( r->nsons == 0 ) {
    if( r->value == value ) {
	return( r->plist->property ) ;
    } else {
	return( -1 ) ;
    }
} else {
    f = tsearch( value , r ) ;
    if( f == (TNODEPTR) NULL ) {
	return( -1 ) ;
    } else {
	v = tfind( value , f ) ;
	if( v == (TNODEPTR) NULL ) {
	    return( -1 ) ;
	} else {
	    return( v->plist->property ) ;
	}
    }
}
}



PLISTPTR tplist( TNODEPTR r , int value )
{

TNODEPTR v , f ;

if( r == (TNODEPTR) NULL ) {
    return( (PLISTPTR) NULL ) ;
} else if( r->nsons == 0 ) {
    if( r->value == value ) {
	return( r->plist ) ;
    } else {
	return( (PLISTPTR) NULL ) ;
    }
} else {
    f = tsearch( value , r ) ;
    if( f == (TNODEPTR) NULL ) {
	return( (PLISTPTR) NULL ) ;
    } else {
	v = tfind( value , f ) ;
	if( v == (TNODEPTR) NULL ) {
	    return( (PLISTPTR) NULL ) ;
	} else {
	    return( v->plist ) ;
	}
    }
}
}
