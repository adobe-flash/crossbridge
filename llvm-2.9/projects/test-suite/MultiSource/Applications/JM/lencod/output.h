
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

int testEndian(void);

void flush_direct_output(int p_out);
void direct_output_paff(StorablePicture *p, int p_out);
void write_out_picture(StorablePicture *p, int p_out);
void write_stored_frame(FrameStore *fs, int p_out);
void direct_output(StorablePicture *p, int p_out);
void direct_output_paff(StorablePicture *p, int p_out);
void init_out_buffer(void);
void uninit_out_buffer(void);

#endif //_OUTPUT_H_
