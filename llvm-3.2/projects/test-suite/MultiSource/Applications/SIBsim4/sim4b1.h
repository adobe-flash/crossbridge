/* $Id: sim4b1.h 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2004 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 */

#ifndef SIM4B1_H
#define SIM4B1_H

extern int encoding[NACHARS];
#define CODE_CNT 5

void free_align(edit_script_list_p_t);
void print_exons(collec_p_t, int);
void SIM4(hash_env_p_t, seq_p_t, collec_p_t);
void init_encoding(void);
void init_hash_env(hash_env_p_t, unsigned int, uchar *, unsigned int);
void free_hash_env(hash_env_p_t);
void bld_table(hash_env_p_t);
void init_col(collec_p_t, unsigned int);
#endif /* SIM4B1_H */
