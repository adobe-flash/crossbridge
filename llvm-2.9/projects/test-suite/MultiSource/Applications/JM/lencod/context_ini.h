
/*!
 *************************************************************************************
 * \file context_ini.h
 *
 * \brief
 *    CABAC context initializations
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Detlev Marpe                    <marpe@hhi.de>
 *    - Heiko Schwarz                   <hschwarz@hhi.de>
 **************************************************************************************
 */

#ifndef _CONTEXT_INI_
#define _CONTEXT_INI_


void  create_context_memory (void);
void  free_context_memory   (void);

void  init_contexts  (void);
void  store_contexts (void);

void  update_field_frame_contexts (int);
void  update_rd_picture_contexts  (int);

void  SetCtxModelNumber (void);

#endif

