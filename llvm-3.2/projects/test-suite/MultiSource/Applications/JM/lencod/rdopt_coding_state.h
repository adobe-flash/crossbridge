
/*!
 ***************************************************************************
 * \file
 *    rdopt_coding_state.h
 *
 * \author
 *    Heiko Schwarz
 *
 * \date
 *    17. April 2001
 *
 * \brief
 *    Headerfile for storing/restoring coding state
 *    (for rd-optimized mode decision)
 **************************************************************************
 */

#ifndef _RD_OPT_CS_H_
#define _RD_OPT_CS_H_

typedef struct {

  // important variables of data partition array
  int                   no_part;
  EncodingEnvironment  *encenv;
  Bitstream            *bitstream;

  // contexts for binary arithmetic coding
  int                   symbol_mode;
  MotionInfoContexts   *mot_ctx;
  TextureInfoContexts  *tex_ctx;

  // bit counter
  int                   bitcounter[MAX_BITCOUNTER_MB];

  // elements of current macroblock
  int                   mvd[2][BLOCK_MULTIPLE][BLOCK_MULTIPLE][2];
  int64                 cbp_bits;
} CSobj;
typedef CSobj* CSptr;


void  delete_coding_state  (CSptr);  //!< delete structure
CSptr create_coding_state  (void);       //!< create structure

void  store_coding_state   (CSptr);  //!< store parameters
void  reset_coding_state   (CSptr);  //!< restore parameters


#endif

