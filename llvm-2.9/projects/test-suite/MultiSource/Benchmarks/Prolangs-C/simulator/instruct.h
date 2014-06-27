/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== intruction.h ============================ */
/* Contains all the procedures that know how to handle FORMAT3/4 instructions*/
/* Name of the procedure is the same as the SIC/XE instruction name but with */
/* a '_P' added to the end. See p. 410-411 of "System Software: An           */
/* Introduction to Systems Programming" by Leland L. Beck for detailed       */
/* Description to the function of each command.                              */

/* ---------------------------------- ADD_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
extern void ADD_P();

/* --------------------------------- AND_P --------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* Bitwise AND of 2 words.                                                   */
extern void AND_P();

/* --------------------------------- COMP_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void COMP_P();

/* ---------------------------------- DIV_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments.                                                            */
extern void DIV_P();

/* ----------------------------------- J_P --------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void J_P();

/* ---------------------------------- JEQ_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void JEQ_P();

/* -------------------------------- JGT_P ---------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void JGT_P();

/* ------------------------------- JLT_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void JLT_P();

/* --------------------------------- JSUB_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void JSUB_P();

/* ------------------------------- LDA_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void SUB_P();

/* ------------------------------- LDA_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDA_P();

/* ------------------------------- LDB_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDB_P();

/* ------------------------------- LDCH_P ---------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDCH_P();

/* ------------------------------- LDL_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDL_P();

/* ------------------------------- LDS_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDS_P();

/* ------------------------------- LDT_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDT_P();

/* ------------------------------- LDX_P ----------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void LDX_P();

/* ---------------------------------- MUL_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
extern void MUL_P();

/* --------------------------------- OR_P ---------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* BITwise OR of 2 words.                                                    */
extern void OR_P();

/* ----------------------------------- RD_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void RD_P();

/* ---------------------------------- RSUB_P ------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void RSUB_P();

/* ---------------------------------- STA_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STA_P();

/* ---------------------------------- STB_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STB_P();

/* ---------------------------------- STCH_P ------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STCH_P();

/* ---------------------------------- STI_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STI_P();

/* ---------------------------------- STL_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STL_P();

/* ---------------------------------- STS_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STS_P();

/* --------------------------------- STSW_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STSW_P();

/* ---------------------------------- STT_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STT_P();

/* ---------------------------------- STX_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void STX_P();

/* ---------------------------------- SUB_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
extern void SUB_P();

/* ---------------------------------- TD_P --------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void TD_P();

/* ---------------------------------- TIX_P -------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void TIX_P();

/* ---------------------------------- WD_P --------------------------------- */
/* 1 paramater:                                                              */
/*   1) BOOLEAN PRINT_EFFECT;    Output effect of this command for debugging */
extern void WD_P();
