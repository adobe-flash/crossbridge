/*!
 ***************************************************************************
 * \file
 *    me_distortion.h
 *
 * \author
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *    Athanasios Leontaris           <aleon@dolby.com>
 *
 * \date
 *    11. August 2006
 *
 * \brief
 *    Headerfile for motion estimation distortion
 **************************************************************************
 */

#ifndef _ME_DISTORTION_H_
#define _ME_DISTORTION_H_

extern imgpel *(*get_line[2]) (imgpel****, int, int);
extern imgpel *(*get_line1[2]) (imgpel****, int, int);
extern imgpel *(*get_line2[2]) (imgpel****, int, int);

extern imgpel *(*get_crline[2]) (imgpel****, int, int);
extern imgpel *(*get_crline1[2]) (imgpel****, int, int);
extern imgpel *(*get_crline2[2]) (imgpel****, int, int);

extern int ref_access_method;
extern int bipred1_access_method;
extern int bipred2_access_method;

extern SubImageContainer ref_pic_sub;
extern SubImageContainer ref_pic1_sub;
extern SubImageContainer ref_pic2_sub;

extern short weight1, weight2, offsetBi;
extern int weight_luma, weight_cr[2], offset_luma, offset_cr[2];
extern short weight1_cr[2], weight2_cr[2], offsetBi_cr[2];
extern short img_width, img_height;
extern int test8x8transform;
extern int ChromaMEEnable;

extern int HadamardSAD4x4(int* diff);
extern int HadamardSAD8x8(int* diff);

extern int computeSAD(imgpel* , int, int, int, int, int);
extern int computeSADWP(imgpel* , int, int, int, int, int);
extern int computeBiPredSAD1(imgpel* , int, int, int, int, int, int, int);
extern int computeBiPredSAD2(imgpel* , int, int, int, int, int, int, int);
extern int computeSATD(imgpel* , int, int, int, int, int);
extern int computeSATDWP(imgpel* , int, int, int, int, int);
extern int computeBiPredSATD1(imgpel* , int, int, int, int, int, int, int);
extern int computeBiPredSATD2(imgpel* , int, int, int, int, int, int, int);
extern int computeSSE(imgpel* , int, int, int, int, int);
extern int computeSSEWP(imgpel* , int, int, int, int, int);
extern int computeBiPredSSE1(imgpel* , int, int, int, int, int, int, int);
extern int computeBiPredSSE2(imgpel* , int, int, int, int, int, int, int);
// Function
extern int (*computeUniPred[6])(imgpel* , int , int , int , int , int );
extern int (*computeBiPred) (imgpel* , int , int , int , int , int, int , int);
extern int (*computeBiPred1[3])(imgpel* , int , int , int , int , int, int , int);
extern int (*computeBiPred2[3])(imgpel* , int , int , int , int , int, int , int);

#endif
