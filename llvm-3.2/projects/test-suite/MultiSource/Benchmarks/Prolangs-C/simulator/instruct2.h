/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== intruction2.h ============================ */
/* Contains all the procedures that know how to handle FORMAT2 instructions  */
/* Name of the procedure is the same as the SIC/XE instruction name but with */
/* a '_P' added to the end. See p. 410-411 of "System Software: An           */
/* Introduction to Systems Programming" by Leland L. Beck for detailed       */
/* Description to the function of each command.                              */

/* ----------------------------------- ADDR_P ------------------------------ */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
extern void ADDR_P();

/* ----------------------------------- CLEAR_P ---------------------------- */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void CLEAR_P();

/* ---------------------------------- COMPR_P ------------------------------ */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void COMPR_P();

/* ---------------------------------- DIVR_P ------------------------------ */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments.                                                            */
extern void DIVR_P();

/* ---------------------------------- MULR_P ------------------------------ */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
extern void MULR_P();

/* ----------------------------------- RMO_P ------------------------------- */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void RMO_P();

/* --------------------------------- SHIFTL_P ------------------------------ */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void SHIFTL_P();

/* --------------------------------- SHIFTR_P ------------------------------ */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void SHIFTR_P();

/* ---------------------------------- SUBR_P ------------------------------- */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
extern void SUBR_P();

/* ----------------------------------- SVC_P ------------------------------- */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void SVC_P();

/* ---------------------------------- TIXR_P ------------------------------- */
/* 1 parameter:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void TIXR_P();
