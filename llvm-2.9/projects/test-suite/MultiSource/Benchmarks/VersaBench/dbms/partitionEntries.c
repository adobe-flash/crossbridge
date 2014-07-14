/*
 *
 *  Name:           partitionEntries
 *  Input:          input list of index entries, I
 *                  integer fan value, fan
 *  Output:         output list of index entries, A
 *                  output list of index entries, B
 *  Return:         void
 *  Description:    Separate input list of index entries into two groups.  The 
 *                  method used for partitioning the entries is extremely 
 *                  implementation dependent.  The basic idea is to set-up the 
 *                  two output index entry lists to have minimal bounding 
 *                  index keys which will improve later queries on the index, 
 *                  since fewer branches of the index tree will need to be 
 *                  traversed to satisfy the query command.  However, the 
 *                  method itself is probably the most computationally 
 *                  expensive of the insertion subroutines, because multiple 
 *                  loops through the index entry lists and volume 
 *                  calculations are required for true "minimal" bounding index 
 *                  keys to be determined.  If multiple branch searches is not 
 *                  prohibitive, i.e., a parallel search is possible or query 
 *                  response time is not time consuming relative to an insert 
 *                  operation, then the partition subroutine can use a 
 *                  sub-minimal approach.  In fact, the partition can simply 
 *                  split the input list into two equal groups ignoring the 
 *                  bounding index keys completely.  The effect will be to 
 *                  cause new traversals of the index to descend multiple 
 *                  branches, but this trade-off may be acceptable for a given 
 *                  implementation.
 *
 *                  On entrance, the input list, I, should have at least 
 *                  MINIMUM_FAN_SIZE entries and nor more than 2 * fan entries, 
 *                  and the output lists, A and B, should be empty(NULL).  On 
 *                  exit, the input list, I, is empty(NULL) and the output 
 *                  lists, A and B, have the partitioned entries (both are 
 *                  non-NULL).  Each list is ready for insertion into an 
 *                  IndexNode.
 *  Calls:          errorMessage()
 *                  keyUnion()
 *                  penalty()
 *                  volume()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                   */
#include <stdlib.h>         /* for NULL definition            */
#include "dataManagement.h" /* for primitive type definitions */
#include "errorMessage.h"   /* for errorMessage() definition  */
#include "index.h"          /* for IndexEntry definition      */
#include "indexKey.h"       /* for IndexKey definition        */

/*
 *  Function prototypes
 */
extern Float penalty( IndexEntry A, IndexEntry B );
extern Float volume( IndexKey key );
extern void keyUnion( IndexKey *A, IndexKey *B, IndexKey *U );

void partitionEntries( IndexEntry *I,   /*  input entry list to partition */
                       Int fan,         /*  fan or order of index         */
                       IndexEntry **A,  /*  1st group partitioned entries */
                       IndexEntry **B ) /*  2nd group partitioned entries */
{   /* beginning of partitionEntries()  */
    IndexEntry *entry1;        /* looping entry to find first entry of groups */
    IndexEntry *entry2;        /* looping entry to find first entry of groups */
    IndexEntry *currentEntry;  /* looping entry for input list                */
    IndexEntry *previousEntry; /* looping entry for input list                */
    IndexEntry *tempA;         /* temp entry to find first entry of group A   */
    IndexEntry *tempB;         /* temp entry to find first entry of group B   */
    IndexKey    unionAB;       /* union of first entries of group A and B     */
    Float       volumeAB;      /* volume of first entries of group A and B    */
    Int         sizeOfGroupA;  /* current size of group A                     */
    Int         sizeOfGroupB;  /* current size of group B                     */

    static Char name[] = "partitionEntries";

    assert( I && I->next );     /*  need at least two entries to partition  */
    assert( MINIMUM_FAN_SIZE > 1 );
    assert( fan >= MINIMUM_FAN_SIZE );

    /*
     *  Find "worst" combination of all entries in I.  The worst combination is 
     *  the one which produces the largest bounding index key, i.e., the 
     *  largest hyper-cube volume.  The two entries which form the worst combo 
     *  will be the first entries into the two groups, A and B.  The method 
     *  used to find the worst combo is straight forward enumeration of all 
     *  possible combinations.  Note that only forward combinations are 
     *  necessary since the volume( union( A, B ) ) is the same as the 
     *  volume( union( B, A ) ).  The first candidate pair for the worst case 
     *  are chosen as the first and second entries of the input list, I.
     */
    *A       = I;
    *B       = I->next;
    keyUnion( &(*A)->key, &(*B)->key, &unionAB );
    volumeAB = volume( unionAB );

    /*
     *  A double loop through the input list, I, is used to find all 
     *  combinations.
     */
    for ( entry1 = I; entry1 != NULL; entry1 = entry1->next ) {
        for ( entry2 = entry1->next; entry2 != NULL; entry2 = entry2->next ) {
            IndexKey    tempKey;
            Float       tempVolume;
            
            /*
             *  If this combination produces a worse penalty, then replace old 
             *  candidates with new pair.
             */
            keyUnion( &entry1->key, &entry2->key, &tempKey );
            tempVolume = volume( tempKey );

            if ( tempVolume > volumeAB ) {
               *A       = entry1;
               *B       = entry2;
               unionAB  = tempKey;
               volumeAB = tempVolume;
            }   /*  end of if ( tempVolume > volumeAB )   */
        }   /*  end of loop for entry2  */
    }   /* end of loop for entry1   */

    /*
     *  The entry pointers, A and B, now point to the first entries of the 
     *  two groups which are forming during the partition. Remove them from the 
     *  input list, I. Set the size of each group to one for the initial 
     *  entries.  The entries of I which correspond to A and B are 
     *  found by comparing pointer values.
     */
    currentEntry  = I;      /*  current entry  (starts at beginning of I)   */
    previousEntry = NULL;   /*  previous entry (NULL for first entry)       */
    while ( currentEntry != NULL ) {
        if ( currentEntry == *A || currentEntry == *B ) {
            /*
             *  Found A or B in list as currentEntry. Remove current entry from 
             *  list, checking for special case where current entry is the head 
             *  of list, i.e., no previous entry.
             */
            if ( previousEntry == NULL ) {
                /*
                 *  No previous pointer means that the current entry is the 
                 *  head of the list.  Removing the entry means updating I and 
                 *  reseting the values for currentEntry and previousEntry.
                 */
                I             = currentEntry->next;
                currentEntry  = I;
                previousEntry = NULL;
            }   /*  end of if ( previousEntry == NULL )    */
            else {
                /*
                 *  Remove current entry by setting previous entry's pointer to 
                 *  skip the current.
                 */
                previousEntry->next = currentEntry->next;
                
                /*
                 *  Setup entries for next loop. Since an entry was removed 
                 *  from the list, the previous entry, previousEntry, does not 
                 *  change.
                 */
                currentEntry = previousEntry->next;
            }   /*  end of else */           
        }   /*  end of if ( currentEntry == A || currentEntry == B )    */
        else {
            /*
             *  Did not find either A or B, so just set up for next loop
             */
            previousEntry = currentEntry;
            currentEntry  = currentEntry->next;
        }   /*  end of else */
    }   /* end of loop for currentEntry   */

    /*
     *  Finish up by fixing the next pointers for both A and B to NULL since 
     *  their the first and only entries in the lists, and setting the size of 
     *  each group to one.
     */
    (*A)->next   = NULL;
    (*B)->next   = NULL;
    sizeOfGroupA = 1;
    sizeOfGroupB = 1;

    /*
     *  The first entries of groups A and B are now assigned and removed from 
     *  the input list, I.  The "volume" for A and B is the "worst" possible 
     *  for all combinations of the entries of I.
     *
     *  Assign all remaining entries of I to each group based on penalty.  The 
     *  current implementation finds the penalty of the entry with the first 
     *  entries into the group, i.e., A or B.  Other methods are possible, 
     *  including using the penalty of the current group rather than the first 
     *  entry into that group.
     */
    tempA = *A;
    tempB = *B;
    while ( I != NULL ) {
        /*
         *  If neither group is full, add according to penalty
         */
        if ( sizeOfGroupA < fan && sizeOfGroupB < fan ) {
            if ( penalty( **A, *I ) < penalty( **B, *I ) ) {
                /*
                 *  Place current entry into group A, incrementing counter
                 */
                tempA->next = I;            /*  add current entry to group A  */
                I           = I->next;      /*  increment current entry       */
                tempA       = tempA->next;  /*  increment group A ptr         */
                tempA->next = NULL;         /*  remove new entry from group I */

                sizeOfGroupA++;
            }   /*  end of if ( penalty( I, A ) < penalty( I, B ) ) */
            else {
                /*
                 *  Place current entry into group B, incrementing counter
                 */
                tempB->next = I;            /*  add current entry to group B  */
                I           = I->next;      /*  increment current entry       */
                tempB       = tempB->next;  /*  increment group B ptr         */
                tempB->next = NULL;         /*  remove new entry from group I */

                sizeOfGroupB++;
            }   /*  end of else */
        }   /*  end of if ( sizeOfGroupA < fan && sizeOfGroupB < fan )  */
        /*
         *  If group A is full and there is room on group B, then add entry to 
         *  group B
         */
        else if ( sizeOfGroupA >= fan && sizeOfGroupB < fan ) {
            /*
             *  Place entry into group B, incrementing counter
             */
            tempB->next = I;            /*  add current entry to group B  */
            I           = I->next;      /*  increment current entry       */
            tempB       = tempB->next;  /*  increment group B ptr         */
            tempB->next = NULL;         /*  remove new entry from group I */
            
            sizeOfGroupB++;
        }   /*  end of if ( sizeOfGroupA >= fan )   */
        /*
         *  If group B is full and there is room on group A, then add entry to 
         *  group A
         */
        else if ( sizeOfGroupB >= fan && sizeOfGroupA < fan ) {
            /*
             *  Place current entry into group A, incrementing counter
             */
            tempA->next = I;            /*  add current entry to group A  */
            I           = I->next;      /*  increment current entry       */
            tempA       = tempA->next;  /*  increment group A ptr         */
            tempA->next = NULL;         /*  remove new entry from group I */

            sizeOfGroupA++;
        }   /*  end of if ( sizeOfGroupB >= fan )   */
        else {
            /*
             *  Special(error) case when both groups are full and no where to 
             *  place entry.  Simply ignore entry and try to continue.
             */
            I = I->next;
            
            errorMessage( "too many entries to partition", REPLACE );
            errorMessage( name, PREPEND );
            
        }
    }   /* end of loop for currentEntry   */
    
    return;
}   /*  end paritionEntries()   */
