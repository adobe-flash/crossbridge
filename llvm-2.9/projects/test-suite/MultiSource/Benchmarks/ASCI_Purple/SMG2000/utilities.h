
#include <HYPRE_config.h>

#include "HYPRE_utilities.h"

#ifndef hypre_UTILITIES_HEADER
#define hypre_UTILITIES_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
 *********************************************************************EHEADER*/
/******************************************************************************
 *
 * General structures and values
 *
 *****************************************************************************/

#ifndef hypre_GENERAL_HEADER
#define hypre_GENERAL_HEADER

/*--------------------------------------------------------------------------
 * Define various functions
 *--------------------------------------------------------------------------*/

#ifndef hypre_max
#define hypre_max(a,b)  (((a)<(b)) ? (b) : (a))
#endif
#ifndef hypre_min
#define hypre_min(a,b)  (((a)<(b)) ? (a) : (b))
#endif

#ifndef hypre_round
#define hypre_round(x)  ( ((x) < 0.0) ? ((int)(x - 0.5)) : ((int)(x + 0.5)) )
#endif

#endif
/*BHEADER**********************************************************************
 * (c) 1998   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
 *********************************************************************EHEADER*/
/******************************************************************************
 *
 *  Fake mpi stubs to generate serial codes without mpi
 *
 *****************************************************************************/

#ifndef hypre_MPISTUBS
#define hypre_MPISTUBS

#ifdef HYPRE_SEQUENTIAL

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 * Change all MPI names to hypre_MPI names to avoid link conflicts
 *
 * NOTE: MPI_Comm is the only MPI symbol in the HYPRE user interface,
 * and is defined in `HYPRE_utilities.h'.
 *--------------------------------------------------------------------------*/

#define MPI_Comm            hypre_MPI_Comm            
#define MPI_Group           hypre_MPI_Group            
#define MPI_Request         hypre_MPI_Request          
#define MPI_Datatype        hypre_MPI_Datatype         
#define MPI_Status          hypre_MPI_Status           
#define MPI_Op              hypre_MPI_Op               
#define MPI_Aint            hypre_MPI_Aint             

#define MPI_COMM_WORLD      hypre_MPI_COMM_WORLD       

#define MPI_BOTTOM  	    hypre_MPI_BOTTOM

#define MPI_DOUBLE          hypre_MPI_DOUBLE           
#define MPI_INT             hypre_MPI_INT              
#define MPI_CHAR            hypre_MPI_CHAR             
#define MPI_LONG            hypre_MPI_LONG             

#define MPI_SUM             hypre_MPI_SUM              
#define MPI_MIN             hypre_MPI_MIN              
#define MPI_MAX             hypre_MPI_MAX              
#define MPI_LOR             hypre_MPI_LOR              

#define MPI_UNDEFINED       hypre_MPI_UNDEFINED        
#define MPI_REQUEST_NULL    hypre_MPI_REQUEST_NULL        
#define MPI_ANY_SOURCE      hypre_MPI_ANY_SOURCE        

#define MPI_Init            hypre_MPI_Init             
#define MPI_Finalize        hypre_MPI_Finalize         
#define MPI_Abort           hypre_MPI_Abort         
#define MPI_Wtime           hypre_MPI_Wtime            
#define MPI_Wtick           hypre_MPI_Wtick            
#define MPI_Barrier         hypre_MPI_Barrier          
#define MPI_Comm_create     hypre_MPI_Comm_create      
#define MPI_Comm_dup        hypre_MPI_Comm_dup         
#define MPI_Comm_group      hypre_MPI_Comm_group       
#define MPI_Comm_size       hypre_MPI_Comm_size        
#define MPI_Comm_rank       hypre_MPI_Comm_rank        
#define MPI_Comm_free       hypre_MPI_Comm_free        
#define MPI_Group_incl      hypre_MPI_Group_incl       
#define MPI_Group_free      hypre_MPI_Group_free        
#define MPI_Address         hypre_MPI_Address        
#define MPI_Get_count       hypre_MPI_Get_count        
#define MPI_Alltoall        hypre_MPI_Alltoall        
#define MPI_Allgather       hypre_MPI_Allgather        
#define MPI_Allgatherv      hypre_MPI_Allgatherv       
#define MPI_Gather          hypre_MPI_Gather       
#define MPI_Scatter         hypre_MPI_Scatter       
#define MPI_Bcast           hypre_MPI_Bcast            
#define MPI_Send            hypre_MPI_Send             
#define MPI_Recv            hypre_MPI_Recv             
#define MPI_Isend           hypre_MPI_Isend            
#define MPI_Irecv           hypre_MPI_Irecv            
#define MPI_Send_init       hypre_MPI_Send_init             
#define MPI_Recv_init       hypre_MPI_Recv_init             
#define MPI_Irsend          hypre_MPI_Irsend             
#define MPI_Startall        hypre_MPI_Startall             
#define MPI_Probe           hypre_MPI_Probe             
#define MPI_Iprobe          hypre_MPI_Iprobe             
#define MPI_Test            hypre_MPI_Test             
#define MPI_Testall         hypre_MPI_Testall
#define MPI_Wait            hypre_MPI_Wait             
#define MPI_Waitall         hypre_MPI_Waitall          
#define MPI_Waitany         hypre_MPI_Waitany          
#define MPI_Allreduce       hypre_MPI_Allreduce        
#define MPI_Request_free    hypre_MPI_Request_free        
#define MPI_Type_contiguous hypre_MPI_Type_contiguous     
#define MPI_Type_vector     hypre_MPI_Type_vector     
#define MPI_Type_hvector    hypre_MPI_Type_hvector     
#define MPI_Type_struct     hypre_MPI_Type_struct      
#define MPI_Type_commit     hypre_MPI_Type_commit
#define MPI_Type_free       hypre_MPI_Type_free        

/*--------------------------------------------------------------------------
 * Types, etc.
 *--------------------------------------------------------------------------*/

/* These types have associated creation and destruction routines */
typedef int hypre_MPI_Comm;
typedef int hypre_MPI_Group;
typedef int hypre_MPI_Request;
typedef int hypre_MPI_Datatype;

typedef struct { int MPI_SOURCE; } hypre_MPI_Status;
typedef int  hypre_MPI_Op;
typedef int  hypre_MPI_Aint;

#define  hypre_MPI_COMM_WORLD 0

#define  hypre_MPI_BOTTOM  0x0

#define  hypre_MPI_DOUBLE 0
#define  hypre_MPI_INT 1
#define  hypre_MPI_CHAR 2
#define  hypre_MPI_LONG 3

#define  hypre_MPI_SUM 0
#define  hypre_MPI_MIN 1
#define  hypre_MPI_MAX 2
#define  hypre_MPI_LOR 3

#define  hypre_MPI_UNDEFINED -9999
#define  hypre_MPI_REQUEST_NULL  0
#define  hypre_MPI_ANY_SOURCE    1

/*--------------------------------------------------------------------------
 * Prototypes
 *--------------------------------------------------------------------------*/

/* mpistubs.c */
int hypre_MPI_Init( int *argc , char ***argv );
int hypre_MPI_Finalize( void );
int hypre_MPI_Abort( hypre_MPI_Comm comm , int errorcode );
double hypre_MPI_Wtime( void );
double hypre_MPI_Wtick( void );
int hypre_MPI_Barrier( hypre_MPI_Comm comm );
int hypre_MPI_Comm_create( hypre_MPI_Comm comm , hypre_MPI_Group group , hypre_MPI_Comm *newcomm );
int hypre_MPI_Comm_dup( hypre_MPI_Comm comm , hypre_MPI_Comm *newcomm );
int hypre_MPI_Comm_size( hypre_MPI_Comm comm , int *size );
int hypre_MPI_Comm_rank( hypre_MPI_Comm comm , int *rank );
int hypre_MPI_Comm_free( hypre_MPI_Comm *comm );
int hypre_MPI_Comm_group( hypre_MPI_Comm comm , hypre_MPI_Group *group );
int hypre_MPI_Group_incl( hypre_MPI_Group group , int n , int *ranks , hypre_MPI_Group *newgroup );
int hypre_MPI_Group_free( hypre_MPI_Group *group );
int hypre_MPI_Address( void *location , hypre_MPI_Aint *address );
int hypre_MPI_Get_count( hypre_MPI_Status *status , hypre_MPI_Datatype datatype , int *count );
int hypre_MPI_Alltoall( void *sendbuf , int sendcount , hypre_MPI_Datatype sendtype , void *recvbuf , int recvcount , hypre_MPI_Datatype recvtype , hypre_MPI_Comm comm );
int hypre_MPI_Allgather( void *sendbuf , int sendcount , hypre_MPI_Datatype sendtype , void *recvbuf , int recvcount , hypre_MPI_Datatype recvtype , hypre_MPI_Comm comm );
int hypre_MPI_Allgatherv( void *sendbuf , int sendcount , hypre_MPI_Datatype sendtype , void *recvbuf , int *recvcounts , int *displs , hypre_MPI_Datatype recvtype , hypre_MPI_Comm comm );
int hypre_MPI_Gather( void *sendbuf , int sendcount , hypre_MPI_Datatype sendtype , void *recvbuf , int recvcount , hypre_MPI_Datatype recvtype , int root , hypre_MPI_Comm comm );
int hypre_MPI_Scatter( void *sendbuf , int sendcount , hypre_MPI_Datatype sendtype , void *recvbuf , int recvcount , hypre_MPI_Datatype recvtype , int root , hypre_MPI_Comm comm );
int hypre_MPI_Bcast( void *buffer , int count , hypre_MPI_Datatype datatype , int root , hypre_MPI_Comm comm );
int hypre_MPI_Send( void *buf , int count , hypre_MPI_Datatype datatype , int dest , int tag , hypre_MPI_Comm comm );
int hypre_MPI_Recv( void *buf , int count , hypre_MPI_Datatype datatype , int source , int tag , hypre_MPI_Comm comm , hypre_MPI_Status *status );
int hypre_MPI_Isend( void *buf , int count , hypre_MPI_Datatype datatype , int dest , int tag , hypre_MPI_Comm comm , hypre_MPI_Request *request );
int hypre_MPI_Irecv( void *buf , int count , hypre_MPI_Datatype datatype , int source , int tag , hypre_MPI_Comm comm , hypre_MPI_Request *request );
int hypre_MPI_Send_init( void *buf , int count , hypre_MPI_Datatype datatype , int dest , int tag , hypre_MPI_Comm comm , hypre_MPI_Request *request );
int hypre_MPI_Recv_init( void *buf , int count , hypre_MPI_Datatype datatype , int dest , int tag , hypre_MPI_Comm comm , hypre_MPI_Request *request );
int hypre_MPI_Irsend( void *buf , int count , hypre_MPI_Datatype datatype , int dest , int tag , hypre_MPI_Comm comm , hypre_MPI_Request *request );
int hypre_MPI_Startall( int count , hypre_MPI_Request *array_of_requests );
int hypre_MPI_Probe( int source , int tag , hypre_MPI_Comm comm , hypre_MPI_Status *status );
int hypre_MPI_Iprobe( int source , int tag , hypre_MPI_Comm comm , int *flag , hypre_MPI_Status *status );
int hypre_MPI_Test( hypre_MPI_Request *request , int *flag , hypre_MPI_Status *status );
int hypre_MPI_Testall( int count , hypre_MPI_Request *array_of_requests , int *flag , hypre_MPI_Status *array_of_statuses );
int hypre_MPI_Wait( hypre_MPI_Request *request , hypre_MPI_Status *status );
int hypre_MPI_Waitall( int count , hypre_MPI_Request *array_of_requests , hypre_MPI_Status *array_of_statuses );
int hypre_MPI_Waitany( int count , hypre_MPI_Request *array_of_requests , int *index , hypre_MPI_Status *status );
int hypre_MPI_Allreduce( void *sendbuf , void *recvbuf , int count , hypre_MPI_Datatype datatype , hypre_MPI_Op op , hypre_MPI_Comm comm );
int hypre_MPI_Request_free( hypre_MPI_Request *request );
int hypre_MPI_Type_contiguous( int count , hypre_MPI_Datatype oldtype , hypre_MPI_Datatype *newtype );
int hypre_MPI_Type_vector( int count , int blocklength , int stride , hypre_MPI_Datatype oldtype , hypre_MPI_Datatype *newtype );
int hypre_MPI_Type_hvector( int count , int blocklength , hypre_MPI_Aint stride , hypre_MPI_Datatype oldtype , hypre_MPI_Datatype *newtype );
int hypre_MPI_Type_struct( int count , int *array_of_blocklengths , hypre_MPI_Aint *array_of_displacements , hypre_MPI_Datatype *array_of_types , hypre_MPI_Datatype *newtype );
int hypre_MPI_Type_commit( hypre_MPI_Datatype *datatype );
int hypre_MPI_Type_free( hypre_MPI_Datatype *datatype );

#ifdef __cplusplus
}
#endif

#endif

#endif
/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
 *********************************************************************EHEADER*/
/******************************************************************************
 *
 * Header file for memory management utilities
 *
 *****************************************************************************/

#ifndef hypre_MEMORY_HEADER
#define hypre_MEMORY_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 * Use "Debug Malloc Library", dmalloc
 *--------------------------------------------------------------------------*/

#ifdef HYPRE_MEMORY_DMALLOC

#define hypre_InitMemoryDebug(id)    hypre_InitMemoryDebugDML(id)
#define hypre_FinalizeMemoryDebug()  hypre_FinalizeMemoryDebugDML()

#define hypre_TAlloc(type, count) \
( (type *)hypre_MAllocDML((unsigned int)(sizeof(type) * (count)),\
                          __FILE__, __LINE__) )

#define hypre_CTAlloc(type, count) \
( (type *)hypre_CAllocDML((unsigned int)(count), (unsigned int)sizeof(type),\
                          __FILE__, __LINE__) )

#define hypre_TReAlloc(ptr, type, count) \
( (type *)hypre_ReAllocDML((char *)ptr,\
                           (unsigned int)(sizeof(type) * (count)),\
                           __FILE__, __LINE__) )

#define hypre_TFree(ptr) \
( hypre_FreeDML((char *)ptr, __FILE__, __LINE__), ptr = NULL )

/*--------------------------------------------------------------------------
 * Use standard memory routines
 *--------------------------------------------------------------------------*/

#else

#define hypre_InitMemoryDebug(id)
#define hypre_FinalizeMemoryDebug()  

#define hypre_TAlloc(type, count) \
( (type *)hypre_MAlloc((unsigned int)(sizeof(type) * (count))) )

#define hypre_CTAlloc(type, count) \
( (type *)hypre_CAlloc((unsigned int)(count), (unsigned int)sizeof(type)) )

#define hypre_TReAlloc(ptr, type, count) \
( (type *)hypre_ReAlloc((char *)ptr, (unsigned int)(sizeof(type) * (count))) )

#define hypre_TFree(ptr) \
( hypre_Free((char *)ptr), ptr = NULL )

#endif


#ifdef HYPRE_USE_PTHREADS

#define hypre_SharedTAlloc(type, count) \
( (type *)hypre_SharedMAlloc((unsigned int)(sizeof(type) * (count))) )


#define hypre_SharedCTAlloc(type, count) \
( (type *)hypre_SharedCAlloc((unsigned int)(count),\
                             (unsigned int)sizeof(type)) )

#define hypre_SharedTReAlloc(ptr, type, count) \
( (type *)hypre_SharedReAlloc((char *)ptr,\
                              (unsigned int)(sizeof(type) * (count))) )

#define hypre_SharedTFree(ptr) \
( hypre_SharedFree((char *)ptr), ptr = NULL )

#else

#define hypre_SharedTAlloc(type, count) hypre_TAlloc(type, (count))
#define hypre_SharedCTAlloc(type, count) hypre_CTAlloc(type, (count))
#define hypre_SharedTReAlloc(type, count) hypre_TReAlloc(type, (count))
#define hypre_SharedTFree(ptr) hypre_TFree(ptr)

#endif

/*--------------------------------------------------------------------------
 * Prototypes
 *--------------------------------------------------------------------------*/

/* memory.c */
int hypre_OutOfMemory( int size );
char *hypre_MAlloc( int size );
char *hypre_CAlloc( int count , int elt_size );
char *hypre_ReAlloc( char *ptr , int size );
void hypre_Free( char *ptr );
char *hypre_SharedMAlloc( int size );
char *hypre_SharedCAlloc( int count , int elt_size );
char *hypre_SharedReAlloc( char *ptr , int size );
void hypre_SharedFree( char *ptr );
double *hypre_IncrementSharedDataPtr( double *ptr , int size );

/* memory_dmalloc.c */
int hypre_InitMemoryDebugDML( int id );
int hypre_FinalizeMemoryDebugDML( void );
char *hypre_MAllocDML( int size , char *file , int line );
char *hypre_CAllocDML( int count , int elt_size , char *file , int line );
char *hypre_ReAllocDML( char *ptr , int size , char *file , int line );
void hypre_FreeDML( char *ptr , char *file , int line );

#ifdef __cplusplus
}
#endif

#endif

/* random.c */
void hypre_SeedRand( int seed );
double hypre_Rand( void );

/*BHEADER**********************************************************************
 * (c) 1998   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
 *********************************************************************EHEADER*/
/******************************************************************************
 *
 *  Fake mpi stubs to generate serial codes without mpi
 *
 *****************************************************************************/
/*just a test comment*/
#ifndef hypre_thread_MPISTUBS
#define hypre_thread_MPISTUBS

#ifdef HYPRE_USE_PTHREADS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HYPRE_USING_THREAD_MPISTUBS

#define MPI_Init           hypre_thread_MPI_Init             
#define MPI_Wtime          hypre_thread_MPI_Wtime            
#define MPI_Wtick          hypre_thread_MPI_Wtick            
#define MPI_Barrier        hypre_thread_MPI_Barrier          
#define MPI_Finalize       hypre_thread_MPI_Finalize         
#define MPI_Comm_group     hypre_thread_MPI_Comm_group       
#define MPI_Comm_dup       hypre_thread_MPI_Comm_dup         
#define MPI_Group_incl     hypre_thread_MPI_Group_incl       
#define MPI_Comm_create    hypre_thread_MPI_Comm_create      
#define MPI_Allgather      hypre_thread_MPI_Allgather        
#define MPI_Allgatherv     hypre_thread_MPI_Allgatherv       
#define MPI_Bcast          hypre_thread_MPI_Bcast            
#define MPI_Send           hypre_thread_MPI_Send             
#define MPI_Recv           hypre_thread_MPI_Recv             

#define MPI_Isend          hypre_thread_MPI_Isend            
#define MPI_Irecv          hypre_thread_MPI_Irecv            
#define MPI_Wait           hypre_thread_MPI_Wait             
#define MPI_Waitall        hypre_thread_MPI_Waitall          
#define MPI_Waitany        hypre_thread_MPI_Waitany          
#define MPI_Comm_size      hypre_thread_MPI_Comm_size        
#define MPI_Comm_rank      hypre_thread_MPI_Comm_rank        
#define MPI_Allreduce      hypre_thread_MPI_Allreduce        
#define MPI_Type_hvector   hypre_thread_MPI_Type_hvector     
#define MPI_Type_struct    hypre_thread_MPI_Type_struct      
#define MPI_Type_free      hypre_thread_MPI_Type_free        
#define MPI_Type_commit    hypre_thread_MPI_Type_commit        

#endif

/*--------------------------------------------------------------------------
 * Prototypes
 *--------------------------------------------------------------------------*/

/* mpistubs.c */
int MPI_Init( int *argc , char ***argv );
double MPI_Wtime( void );
double MPI_Wtick( void );
int MPI_Barrier( MPI_Comm comm );
int MPI_Finalize( void );
int MPI_Abort( MPI_Comm comm , int errorcode );
int MPI_Comm_group( MPI_Comm comm , MPI_Group *group );
int MPI_Comm_dup( MPI_Comm comm , MPI_Comm *newcomm );
int MPI_Group_incl( MPI_Group group , int n , int *ranks , MPI_Group *newgroup );
int MPI_Comm_create( MPI_Comm comm , MPI_Group group , MPI_Comm *newcomm );
int MPI_Get_count( MPI_Status *status , MPI_Datatype datatype , int *count );
int MPI_Alltoall( void *sendbuf , int sendcount , MPI_Datatype sendtype , void *recvbuf , int recvcount , MPI_Datatype recvtype , MPI_Comm comm );
int MPI_Allgather( void *sendbuf , int sendcount , MPI_Datatype sendtype , void *recvbuf , int recvcount , MPI_Datatype recvtype , MPI_Comm comm );
int MPI_Allgatherv( void *sendbuf , int sendcount , MPI_Datatype sendtype , void *recvbuf , int *recvcounts , int *displs , MPI_Datatype recvtype , MPI_Comm comm );
int MPI_Gather( void *sendbuf , int sendcount , MPI_Datatype sendtype , void *recvbuf , int recvcount , MPI_Datatype recvtype , int root , MPI_Comm comm );
int MPI_Scatter( void *sendbuf , int sendcount , MPI_Datatype sendtype , void *recvbuf , int recvcount , MPI_Datatype recvtype , int root , MPI_Comm comm );
int MPI_Bcast( void *buffer , int count , MPI_Datatype datatype , int root , MPI_Comm comm );
int MPI_Send( void *buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm );
int MPI_Recv( void *buf , int count , MPI_Datatype datatype , int source , int tag , MPI_Comm comm , MPI_Status *status );
int MPI_Isend( void *buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm , MPI_Request *request );
int MPI_Irecv( void *buf , int count , MPI_Datatype datatype , int source , int tag , MPI_Comm comm , MPI_Request *request );
int MPI_Wait( MPI_Request *request , MPI_Status *status );
int MPI_Waitall( int count , MPI_Request *array_of_requests , MPI_Status *array_of_statuses );
int MPI_Waitany( int count , MPI_Request *array_of_requests , int *index , MPI_Status *status );
int MPI_Comm_size( MPI_Comm comm , int *size );
int MPI_Comm_rank( MPI_Comm comm , int *rank );
int MPI_Allreduce( void *sendbuf , void *recvbuf , int count , MPI_Datatype datatype , MPI_Op op , MPI_Comm comm );
int MPI_Address( void *location , MPI_Aint *address );
int MPI_Type_contiguous( int count , MPI_Datatype oldtype , MPI_Datatype *newtype );
int MPI_Type_vector( int count , int blocklength , int stride , MPI_Datatype oldtype , MPI_Datatype *newtype );
int MPI_Type_hvector( int count , int blocklength , MPI_Aint stride , MPI_Datatype oldtype , MPI_Datatype *newtype );
int MPI_Type_struct( int count , int *array_of_blocklengths , MPI_Aint *array_of_displacements , MPI_Datatype *array_of_types , MPI_Datatype *newtype );
int MPI_Type_free( MPI_Datatype *datatype );
int MPI_Type_commit( MPI_Datatype *datatype );
int MPI_Request_free( MPI_Request *request );
int MPI_Send_init( void *buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm , MPI_Request *request );
int MPI_Recv_init( void *buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm , MPI_Request *request );
int MPI_Startall( int count , MPI_Request *array_of_requests );
int MPI_Iprobe( int source , int tag , MPI_Comm comm , int *flag , MPI_Status *status );
int MPI_Probe( int source , int tag , MPI_Comm comm , MPI_Status *status );
int MPI_Irsend( void *buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm , MPI_Request *request );

#ifdef __cplusplus
}
#endif

#endif

#endif
/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
*********************************************************************EHEADER*/

#ifndef hypre_THREADING_HEADER
#define hypre_THREADING_HEADER

#if defined(HYPRE_USING_OPENMP) || defined (HYPRE_USING_PGCC_SMP)

int hypre_NumThreads( void );

#else

#define hypre_NumThreads() 1

#endif


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* The pthreads stuff needs to be reworked */

#ifdef HYPRE_USE_PTHREADS

#ifndef MAX_QUEUE
#define MAX_QUEUE 256
#endif

#include <pthread.h>

/* hypre_work_proc_t typedef'd to be a pointer to a function with a void*
   argument and a void return type */
typedef void (*hypre_work_proc_t)(void *);

typedef struct hypre_workqueue_struct {
   pthread_mutex_t lock;
   pthread_cond_t work_wait;
   pthread_cond_t finish_wait;
   hypre_work_proc_t worker_proc_queue[MAX_QUEUE];
   int n_working;
   int n_waiting;
   int n_queue;
   int inp;
   int outp;
   void *argqueue[MAX_QUEUE];
} *hypre_workqueue_t;

void hypre_work_put( hypre_work_proc_t funcptr, void *argptr );
void hypre_work_wait( void );
int HYPRE_InitPthreads( int num_threads );
void HYPRE_DestroyPthreads( void );
void hypre_pthread_worker( int threadid );
int ifetchadd( int *w, pthread_mutex_t *mutex_fetchadd );
int hypre_fetch_and_add( int *w );
void hypre_barrier(pthread_mutex_t *mpi_mtx, int unthreaded);
int hypre_GetThreadID( void );

pthread_t initial_thread;
pthread_t hypre_thread[hypre_MAX_THREADS];
pthread_mutex_t hypre_mutex_boxloops;
pthread_mutex_t talloc_mtx;
pthread_mutex_t worker_mtx;
hypre_workqueue_t hypre_qptr;
pthread_mutex_t mpi_mtx;
pthread_mutex_t time_mtx;
volatile int hypre_thread_release;

#ifdef HYPRE_THREAD_GLOBALS
int hypre_NumThreads = 4;
#else
extern int hypre_NumThreads;
#endif

#endif
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#endif
/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
 *********************************************************************EHEADER*/

/******************************************************************************
 *
 * Header file for doing timing
 *
 *****************************************************************************/

#ifndef HYPRE_TIMING_HEADER
#define HYPRE_TIMING_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 * Prototypes for low-level timing routines
 *--------------------------------------------------------------------------*/

/* timer.c */
double time_getWallclockSeconds( void );
double time_getCPUSeconds( void );
double time_get_wallclock_seconds_( void );
double time_get_cpu_seconds_( void );

/*--------------------------------------------------------------------------
 * With timing off
 *--------------------------------------------------------------------------*/

#ifndef HYPRE_TIMING

#define hypre_InitializeTiming(name) 0
#define hypre_IncFLOPCount(inc)
#define hypre_BeginTiming(i)
#define hypre_EndTiming(i)
#define hypre_PrintTiming(heading, comm)
#define hypre_FinalizeTiming(index)

/*--------------------------------------------------------------------------
 * With timing on
 *--------------------------------------------------------------------------*/

#else

/*-------------------------------------------------------
 * Global timing structure
 *-------------------------------------------------------*/

typedef struct
{
   double  *wall_time;
   double  *cpu_time;
   double  *flops;
   char   **name;
   int     *state;     /* boolean flag to allow for recursive timing */
   int     *num_regs;  /* count of how many times a name is registered */

   int      num_names;
   int      size;

   double   wall_count;
   double   CPU_count;
   double   FLOP_count;

} hypre_TimingType;

#ifdef HYPRE_TIMING_GLOBALS
hypre_TimingType *hypre_global_timing = NULL;
#else
extern hypre_TimingType *hypre_global_timing;
#endif

/*-------------------------------------------------------
 * Accessor functions
 *-------------------------------------------------------*/

#ifndef HYPRE_USE_PTHREADS
#define hypre_TimingWallTime(i) (hypre_global_timing -> wall_time[(i)])
#define hypre_TimingCPUTime(i)  (hypre_global_timing -> cpu_time[(i)])
#define hypre_TimingFLOPS(i)    (hypre_global_timing -> flops[(i)])
#define hypre_TimingName(i)     (hypre_global_timing -> name[(i)])
#define hypre_TimingState(i)    (hypre_global_timing -> state[(i)])
#define hypre_TimingNumRegs(i)  (hypre_global_timing -> num_regs[(i)])
#define hypre_TimingWallCount   (hypre_global_timing -> wall_count)
#define hypre_TimingCPUCount    (hypre_global_timing -> CPU_count)
#define hypre_TimingFLOPCount   (hypre_global_timing -> FLOP_count)
#else
#define hypre_TimingWallTime(i) (hypre_global_timing[threadid].wall_time[(i)])
#define hypre_TimingCPUTime(i)  (hypre_global_timing[threadid].cpu_time[(i)])
#define hypre_TimingFLOPS(i)    (hypre_global_timing[threadid].flops[(i)])
#define hypre_TimingName(i)     (hypre_global_timing[threadid].name[(i)])
#define hypre_TimingState(i)    (hypre_global_timing[threadid].state[(i)])
#define hypre_TimingNumRegs(i)  (hypre_global_timing[threadid].num_regs[(i)])
#define hypre_TimingWallCount   (hypre_global_timing[threadid].wall_count)
#define hypre_TimingCPUCount    (hypre_global_timing[threadid].CPU_count)
#define hypre_TimingFLOPCount   (hypre_global_timing[threadid].FLOP_count)
#define hypre_TimingAllFLOPS    (hypre_global_timing[hypre_NumThreads].FLOP_count)
#endif

/*-------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------*/

/* timing.c */
int hypre_InitializeTiming( char *name );
int hypre_FinalizeTiming( int time_index );
int hypre_IncFLOPCount( int inc );
int hypre_BeginTiming( int time_index );
int hypre_EndTiming( int time_index );
int hypre_ClearTiming( void );
int hypre_PrintTiming( char *heading , MPI_Comm comm );

#endif

#ifdef __cplusplus
}
#endif

#endif
/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 21217 $
 *********************************************************************EHEADER*/

/******************************************************************************
 *
 * Header file link lists
 *
 *****************************************************************************/

#ifndef HYPRE_LINKLIST_HEADER
#define HYPRE_LINKLIST_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIST_HEAD -1
#define LIST_TAIL -2

struct double_linked_list
{
       int                        data;
       struct double_linked_list *next_elt;
       struct double_linked_list *prev_elt;
       int                        head;
       int                        tail;
};

typedef struct double_linked_list hypre_ListElement;
typedef hypre_ListElement  *hypre_LinkList;  

#ifdef __cplusplus
}
#endif

#endif

/* amg_linklist.c */
void dispose_elt( hypre_LinkList element_ptr );
void remove_point( hypre_LinkList *LoL_head_ptr , hypre_LinkList *LoL_tail_ptr , int measure , int index , int *lists , int *where );
hypre_LinkList create_elt( int Item );
void enter_on_lists( hypre_LinkList *LoL_head_ptr , hypre_LinkList *LoL_tail_ptr , int measure , int index , int *lists , int *where );


/* binsearch.c */
int hypre_BinarySearch( int *list , int value , int list_length );


/* qsplit.c */
int hypre_DoubleQuickSplit( double *values , int *indices , int list_length , int NumberKept );


#ifdef __cplusplus
}
#endif

#endif

