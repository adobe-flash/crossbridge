

#ifndef OBSEQUI_UTILS_H
#define OBSEQUI_UTILS_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  //Need this so that __USE_GNU is defined
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//#################################################################
// Define a couple of types which I like to use.
//#################################################################

typedef  u_int8_t   u8bit;
typedef  u_int16_t  u16bit;
typedef  u_int32_t  u32bit;
typedef  u_int64_t  u64bit;

typedef  int16_t    s16bit;
typedef  int32_t    s32bit;


//#################################################################
// Define a number of useful constants and macros.
//#################################################################

//=================================================================
// I think that DEFINED should always be defined.
// Also make sure NOT_DEFINED is not.
//=================================================================
#define DEFINED
#undef  NOT_DEFINED

//=================================================================
// Determine the max value of two numbers.
//=================================================================
#define MAX_TWO(a,b) (((a) > (b)) ? (a) : (b))

//=================================================================
// Determine the min value of two numbers.
//=================================================================
#define MIN_TWO(a,b) (((a) > (b)) ? (b) : (a))

//=================================================================
// Constants and Macros for dealing with bit masks.
//=================================================================
#define ALL_BITS         0xFFFFFFFF
#define NTH_BIT(i)       ( 1 << (i) )
#define FIRST_N_BITS(i)  (~(ALL_BITS << (i)))
#define LAST_N_BITS(i)   (~(ALL_BITS >> (i)))

//=================================================================
// Determine how many elements have been alloced for ptr.
//  - this probably only works if using glibc.
//=================================================================
#define ALLOCED(ptr,type) (malloc_usable_size(ptr)/sizeof(type))


//########################################################
// The following functions print a message to both stderr and
//   to the file ".fatal_error". These messages can be helpful
//   for debugging since they contain both the file name and line number
//   of where this function was called. (They all return void.)
//########################################################

//========================================================
// Besides printing a message this function also causes the
//   program to exit with the value 'err_num'.
//========================================================
#define fatal_error(err_num, format, rest...) \
         (_fatal_error((err_num) ? (err_num) : 256, format, ## rest ))

//========================================================
// This function justs prints the message.
//========================================================
#define warning(format, rest...) \
         (_fatal_error(0, format, ## rest ))

//========================================================
// This function prints the message "Check Point."
//   Exclusively used for debugging.
//========================================================
#define check_point() \
         (_fatal_error(0, "Check Point.\n"))


#define _fatal_error(err_num, format, rest...) \
         (_fatal_error_aux(__FILE__, __LINE__, (err_num), format, ## rest ))


//########################################################
// These are dynamic sprintf function, they malloc space as needed.
//########################################################

//========================================================
// stores the string created from FORMAT, and ..., in *str.
//
// allocates space as needed.
// returns the number of characters written to *str.
//========================================================
extern s32bit
Asprintf(char **str, s32bit *len, s32bit offset, const char *format, ... )
     __attribute__ ((format (printf, 4, 5)));

//========================================================
// returns the number of characters written to *(str+offset)
//========================================================
extern s32bit
asprintf_my(char **str, s32bit offset, const char *format, ... )
     __attribute__ ((format (printf, 3, 4)));



//########################################################
// This function does absolutely nothing.
//########################################################
extern void
null_command();


//########################################################
// Return a string with the printed value of a 64 bit number.
// Note: It returns a static buffer so don't free it, and
//       always copy the value before you call it again.
//########################################################
extern const char*
u64bit_to_string(u64bit val);


//########################################################
// Internals.
//########################################################
extern void
_fatal_error_aux(const char *file, const s32bit line,
                 const s32bit err_num, const char *format, ... )
     __attribute__ ((format (printf, 4, 5)));
     
#endif
