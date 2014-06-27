
/*!
 **************************************************************************************
 * \file
 *    output.h
 * \brief
 *    Picture writing routine headers
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Suehring        <suehring@hhi.de>
 ***************************************************************************************
 */
#ifndef _OUTPUT_H_
#define _OUTPUT_H_

int testEndian();

void write_stored_frame(FrameStore *fs, int p_out);
void direct_output(StorablePicture *p, int p_out);
void init_out_buffer();
void uninit_out_buffer();

#ifdef PAIR_FIELDS_IN_OUTPUT
void flush_pending_output(int p_out);
#endif

#endif //_OUTPUT_H_
