
/*!
 *************************************************************************************
 * \file explicit_gop.h
 *
 * \brief
 *    Functions for explicit gop and hierarchy support
 *
 * \author
 *     Main contributors (see contributors.h for copyright, address and affiliation details)
 *     - Alexis Michael Tourapis          <alexismt@ieee.org>
 *************************************************************************************
 */

#ifndef _EXPLICIT_GOP_H_
#define _EXPLICIT_GOP_H_

// GOP Hierarchy
void init_gop_structure(void);
void interpret_gop_structure(void);
void create_hierarchy(void);
void clear_gop_structure(void);
void encode_enhancement_layer(void);

#endif
