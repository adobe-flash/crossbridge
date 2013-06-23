#line 4 "grammar.g"

#include "gramgram.h"
#include "d.h"

#include "dparse.h"

D_Reduction d_reduction_0_dparser_gram = {1, 0, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_1_dparser_gram = {1, 1, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_2_dparser_gram = {2, 2, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_3_dparser_gram = {2, 3, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_4_dparser_gram = {1, 3, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_5_dparser_gram = {2, 4, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_6_dparser_gram = {2, 5, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_7_dparser_gram = {0, 5, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_8_dparser_gram = {2, 6, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_9_dparser_gram = {0, 6, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
int d_final_reduction_code_7_10_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 11 "grammar.g"
 
	      add_global_code((D_PN(_ps, _offset)->globals), (*(D_PN(_children[0], _offset))).start_loc.s+1, (*(D_PN(_children[0], _offset))).end-1, 
	        (*(D_PN(_children[0], _offset))).start_loc.line); 
              return 0;}

D_Reduction d_reduction_10_dparser_gram = {1, 7, NULL, d_final_reduction_code_7_10_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_7_11_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 15 "grammar.g"

	      (D_PN(_ps, _offset)->globals)->scanner.code = dup_str((*(D_PN(_children[1], _offset))).start_loc.s, (*(D_PN(_children[1], _offset))).end);
	      (D_PN(_ps, _offset)->globals)->scanner.line = (*(D_PN(_children[0], _offset))).start_loc.line;
	      return 0;}

D_Reduction d_reduction_11_dparser_gram = {3, 7, NULL, d_final_reduction_code_7_11_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_7_12_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 19 "grammar.g"

	      if (!d_get_number_of_children(&(*(D_PN(_children[2], _offset)))))
	      	add_declaration((D_PN(_ps, _offset)->globals), (*(D_PN(_children[2], _offset))).start_loc.s, (*(D_PN(_children[2], _offset))).end,  (D_PN(_children[1], _offset)->user).kind, (*(D_PN(_children[2], _offset))).start_loc.line);
              else {
	        int i, n = d_get_number_of_children(&(*(D_PN(_children[2], _offset))));
		for (i = 0; i < n; i++) {
		  D_ParseNode *pn = d_get_child(&(*(D_PN(_children[2], _offset))), i);
		  add_declaration((D_PN(_ps, _offset)->globals), pn->start_loc.s, pn->end,  (D_PN(_children[1], _offset)->user).kind, pn->start_loc.line);
		}
	      }
	      return 0;}

D_Reduction d_reduction_12_dparser_gram = {4, 7, NULL, d_final_reduction_code_7_12_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_13_dparser_gram = {3, 7, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_7_14_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 31 "grammar.g"
 (D_PN(_ps, _offset)->globals)->action_index++;   return 0;}

D_Reduction d_reduction_14_dparser_gram = {1, 7, NULL, d_final_reduction_code_7_14_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_7_15_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 32 "grammar.g"

	      add_pass((D_PN(_ps, _offset)->globals), (*(D_PN(_children[1], _offset))).start_loc.s, (*(D_PN(_children[1], _offset))).end,  (D_PN(_children[2], _offset)->user).kind, (*(D_PN(_children[1], _offset))).start_loc.line);
	      return 0;}

D_Reduction d_reduction_15_dparser_gram = {4, 7, NULL, d_final_reduction_code_7_15_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_16_dparser_gram = {2, 8, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_17_dparser_gram = {1, 8, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_18_dparser_gram = {2, 9, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_19_dparser_gram = {0, 9, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_20_dparser_gram = {2, 10, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_21_dparser_gram = {1, 10, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_22_dparser_gram = {0, 11, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_11_23_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 38 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = (D_PN(_children[0], _offset)->user).kind | (D_PN(_children[1], _offset)->user).kind;   return 0;}

D_Reduction d_reduction_23_dparser_gram = {2, 11, NULL, d_final_reduction_code_11_23_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_12_24_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 41 "grammar.g"
 (D_PN(_ps, _offset)->user).kind |= D_PASS_PRE_ORDER;   return 0;}

D_Reduction d_reduction_24_dparser_gram = {1, 12, NULL, d_final_reduction_code_12_24_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_12_25_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 42 "grammar.g"
 (D_PN(_ps, _offset)->user).kind |= D_PASS_POST_ORDER;   return 0;}

D_Reduction d_reduction_25_dparser_gram = {1, 12, NULL, d_final_reduction_code_12_25_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_12_26_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 43 "grammar.g"
 (D_PN(_ps, _offset)->user).kind |= D_PASS_MANUAL;   return 0;}

D_Reduction d_reduction_26_dparser_gram = {1, 12, NULL, d_final_reduction_code_12_26_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_12_27_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 44 "grammar.g"
 (D_PN(_ps, _offset)->user).kind |= D_PASS_FOR_ALL;   return 0;}

D_Reduction d_reduction_27_dparser_gram = {1, 12, NULL, d_final_reduction_code_12_27_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_12_28_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 45 "grammar.g"
 (D_PN(_ps, _offset)->user).kind |= D_PASS_FOR_UNDEFINED;   return 0;}

D_Reduction d_reduction_28_dparser_gram = {1, 12, NULL, d_final_reduction_code_12_28_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_29_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 48 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_TOKENIZE;   return 0;}

D_Reduction d_reduction_29_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_29_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_30_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 49 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_LONGEST_MATCH;   return 0;}

D_Reduction d_reduction_30_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_30_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_31_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 50 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_WHITESPACE;   return 0;}

D_Reduction d_reduction_31_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_31_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_32_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 51 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_ALL_MATCHES;   return 0;}

D_Reduction d_reduction_32_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_32_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_33_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 52 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_SET_OP_PRIORITY;   return 0;}

D_Reduction d_reduction_33_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_33_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_34_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 53 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_STATES_FOR_ALL_NTERMS;   return 0;}

D_Reduction d_reduction_34_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_34_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_35_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 54 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_STATE_FOR;   return 0;}

D_Reduction d_reduction_35_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_35_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_13_36_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 55 "grammar.g"
 (D_PN(_ps, _offset)->user).kind = DECLARE_SAVE_PARSE_TREE;   return 0;}

D_Reduction d_reduction_36_dparser_gram = {1, 13, NULL, d_final_reduction_code_13_36_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_14_37_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 58 "grammar.g"
 new_token((D_PN(_ps, _offset)->globals), (*(D_PN(_children[0], _offset))).start_loc.s, (*(D_PN(_children[0], _offset))).end);   return 0;}

D_Reduction d_reduction_37_dparser_gram = {1, 14, NULL, d_final_reduction_code_14_37_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_38_dparser_gram = {4, 15, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_40_dparser_gram = {1, 15, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_16_41_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 63 "grammar.g"
 (D_PN(_ps, _offset)->globals)->p->regex = 1;   return 0;}

D_Reduction d_reduction_41_dparser_gram = {1, 16, NULL, d_final_reduction_code_16_41_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_17_42_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 66 "grammar.g"
 (D_PN(_ps, _offset)->globals)->p = new_production((D_PN(_ps, _offset)->globals), dup_str((*(D_PN(_children[0], _offset))).start_loc.s, (*(D_PN(_children[0], _offset))).end));   return 0;}

D_Reduction d_reduction_42_dparser_gram = {1, 17, NULL, d_final_reduction_code_17_42_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_43_dparser_gram = {1, 18, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_45_dparser_gram = {2, 19, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_46_dparser_gram = {2, 20, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_47_dparser_gram = {0, 20, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_48_dparser_gram = {2, 21, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
int d_final_reduction_code_22_49_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 71 "grammar.g"

  vec_add(&(D_PN(_ps, _offset)->globals)->p->rules, (D_PN(_ps, _offset)->globals)->r);
  return 0;}

D_Reduction d_reduction_49_dparser_gram = {4, 22, NULL, d_final_reduction_code_22_49_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_50_dparser_gram = {2, 23, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_51_dparser_gram = {0, 23, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_52_dparser_gram = {1, 24, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_53_dparser_gram = {0, 24, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_54_dparser_gram = {3, 25, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_55_dparser_gram = {2, 26, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_56_dparser_gram = {0, 26, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_57_dparser_gram = {2, 27, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_58_dparser_gram = {0, 27, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_59_dparser_gram = {2, 28, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_60_dparser_gram = {2, 29, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_61_dparser_gram = {0, 29, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
int d_final_reduction_code_30_62_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 75 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r = new_rule((D_PN(_ps, _offset)->globals), (D_PN(_ps, _offset)->globals)->p);   return 0;}

D_Reduction d_reduction_62_dparser_gram = {0, 30, NULL, d_final_reduction_code_30_62_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_31_63_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 78 "grammar.g"
 (D_PN(_ps, _offset)->globals)->e = new_string((D_PN(_ps, _offset)->globals), (*(D_PN(_children[0], _offset))).start_loc.s, (*(D_PN(_children[0], _offset))).end, (D_PN(_ps, _offset)->globals)->r);   return 0;}

D_Reduction d_reduction_63_dparser_gram = {1, 31, NULL, d_final_reduction_code_31_63_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_31_64_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 79 "grammar.g"
 (D_PN(_ps, _offset)->globals)->e = new_string((D_PN(_ps, _offset)->globals), (*(D_PN(_children[0], _offset))).start_loc.s, (*(D_PN(_children[0], _offset))).end, (D_PN(_ps, _offset)->globals)->r);   return 0;}

D_Reduction d_reduction_64_dparser_gram = {1, 31, NULL, d_final_reduction_code_31_64_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_31_65_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 80 "grammar.g"
 (D_PN(_ps, _offset)->globals)->e = new_ident((*(D_PN(_children[0], _offset))).start_loc.s, (*(D_PN(_children[0], _offset))).end, (D_PN(_ps, _offset)->globals)->r);   return 0;}

D_Reduction d_reduction_65_dparser_gram = {1, 31, NULL, d_final_reduction_code_31_65_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_31_66_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 81 "grammar.g"
 (D_PN(_ps, _offset)->globals)->e = new_code((D_PN(_ps, _offset)->globals), (*(D_PN(_children[1], _offset))).start_loc.s, (*(D_PN(_children[1], _offset))).end, (D_PN(_ps, _offset)->globals)->r);   return 0;}

D_Reduction d_reduction_66_dparser_gram = {3, 31, NULL, d_final_reduction_code_31_66_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_31_67_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 82 "grammar.g"

     (D_PN(_ps, _offset)->globals)->e = new_elem_nterm((D_PN(_ps, _offset)->globals)->p, (D_PN(_children[1], _offset)->user).r);
     (D_PN(_ps, _offset)->globals)->p = (D_PN(_children[1], _offset)->user).p;
     (D_PN(_ps, _offset)->globals)->r = (D_PN(_children[1], _offset)->user).r;
     vec_add(&(D_PN(_ps, _offset)->globals)->r->elems, (D_PN(_ps, _offset)->globals)->e);
     return 0;}

D_Reduction d_reduction_67_dparser_gram = {4, 31, NULL, d_final_reduction_code_31_67_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_68_dparser_gram = {2, 32, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_69_dparser_gram = {1, 32, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_70_dparser_gram = {1, 33, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_33_71_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 92 "grammar.g"

     Production *p = new_internal_production((D_PN(_ps, _offset)->globals), NULL);
     Rule *r = new_rule((D_PN(_ps, _offset)->globals), p);
     vec_add(&p->rules, r);
     r->speculative_code.code = dup_str((*(D_PN(_children[0], _offset))).start_loc.s + 1, (*(D_PN(_children[0], _offset))).end - 1);
     r->speculative_code.line = (*(D_PN(_children[0], _offset))).start_loc.line;
     (D_PN(_ps, _offset)->globals)->e = new_elem_nterm(p, (D_PN(_ps, _offset)->globals)->r);
     vec_add(&(D_PN(_ps, _offset)->globals)->r->elems, (D_PN(_ps, _offset)->globals)->e);
     return 0;}

D_Reduction d_reduction_71_dparser_gram = {1, 33, NULL, d_final_reduction_code_33_71_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_33_72_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 101 "grammar.g"

     Production *p = new_internal_production((D_PN(_ps, _offset)->globals), NULL);
     Rule *r = new_rule((D_PN(_ps, _offset)->globals), p);
     vec_add(&p->rules, r);
     r->final_code.code = dup_str((*(D_PN(_children[0], _offset))).start_loc.s + 1, (*(D_PN(_children[0], _offset))).end - 1);
     r->final_code.line = (*(D_PN(_children[0], _offset))).start_loc.line;
     (D_PN(_ps, _offset)->globals)->e = new_elem_nterm(p, (D_PN(_ps, _offset)->globals)->r);
     vec_add(&(D_PN(_ps, _offset)->globals)->r->elems, (D_PN(_ps, _offset)->globals)->e);
     return 0;}

D_Reduction d_reduction_72_dparser_gram = {1, 33, NULL, d_final_reduction_code_33_72_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_34_73_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 112 "grammar.g"

  (D_PN(_ps, _offset)->user).p = (D_PN(_ps, _offset)->globals)->p;
  (D_PN(_ps, _offset)->user).r = (D_PN(_ps, _offset)->globals)->r;
  (D_PN(_ps, _offset)->globals)->p = new_internal_production((D_PN(_ps, _offset)->globals), (D_PN(_ps, _offset)->globals)->p);
  (D_PN(_ps, _offset)->globals)->r = new_rule((D_PN(_ps, _offset)->globals), (D_PN(_ps, _offset)->globals)->p);
  return 0;}

D_Reduction d_reduction_73_dparser_gram = {0, 34, NULL, d_final_reduction_code_34_73_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_35_74_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 120 "grammar.g"
 
  if ((D_PN(_ps, _offset)->globals)->e->kind != ELEM_TERM) 
    d_fail("terminal priority on non-terminal");
  (D_PN(_ps, _offset)->globals)->e->e.term->term_priority = strtol((*(D_PN(_children[1], _offset))).start_loc.s, NULL, 0); 
  return 0;}

D_Reduction d_reduction_74_dparser_gram = {2, 35, NULL, d_final_reduction_code_35_74_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_35_75_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 125 "grammar.g"
 
  if ((D_PN(_ps, _offset)->globals)->e->kind != ELEM_TERM) 
    d_fail("ignore-case (/i) on non-terminal");
  (D_PN(_ps, _offset)->globals)->e->e.term->ignore_case = 1; 
  return 0;}

D_Reduction d_reduction_75_dparser_gram = {1, 35, NULL, d_final_reduction_code_35_75_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_35_76_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 130 "grammar.g"
 conditional_EBNF((D_PN(_ps, _offset)->globals));   return 0;}

D_Reduction d_reduction_76_dparser_gram = {1, 35, NULL, d_final_reduction_code_35_76_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_35_77_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 131 "grammar.g"
 star_EBNF((D_PN(_ps, _offset)->globals));   return 0;}

D_Reduction d_reduction_77_dparser_gram = {1, 35, NULL, d_final_reduction_code_35_77_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_35_78_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 132 "grammar.g"
 plus_EBNF((D_PN(_ps, _offset)->globals));   return 0;}

D_Reduction d_reduction_78_dparser_gram = {1, 35, NULL, d_final_reduction_code_35_78_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_79_dparser_gram = {2, 36, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_80_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 137 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->op_assoc = ASSOC_UNARY_RIGHT;   return 0;}

D_Reduction d_reduction_80_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_80_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_81_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 138 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->op_assoc = ASSOC_UNARY_LEFT;   return 0;}

D_Reduction d_reduction_81_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_81_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_82_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 139 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->op_assoc = ASSOC_BINARY_RIGHT;   return 0;}

D_Reduction d_reduction_82_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_82_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_83_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 140 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->op_assoc = ASSOC_BINARY_LEFT;   return 0;}

D_Reduction d_reduction_83_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_83_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_84_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 141 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->rule_assoc = ASSOC_UNARY_RIGHT;   return 0;}

D_Reduction d_reduction_84_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_84_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_85_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 142 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->rule_assoc = ASSOC_UNARY_LEFT;   return 0;}

D_Reduction d_reduction_85_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_85_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_86_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 143 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->rule_assoc = ASSOC_BINARY_RIGHT;   return 0;}

D_Reduction d_reduction_86_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_86_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_87_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 144 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->rule_assoc = ASSOC_BINARY_LEFT;   return 0;}

D_Reduction d_reduction_87_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_87_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_88_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 145 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->rule_assoc = ASSOC_NARY_RIGHT;   return 0;}

D_Reduction d_reduction_88_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_88_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_37_89_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 146 "grammar.g"
 (D_PN(_ps, _offset)->globals)->r->rule_assoc = ASSOC_NARY_LEFT;   return 0;}

D_Reduction d_reduction_89_dparser_gram = {1, 37, NULL, d_final_reduction_code_37_89_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_38_90_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 149 "grammar.g"
 
  if ((D_PN(_ps, _offset)->globals)->r->op_assoc) (D_PN(_ps, _offset)->globals)->r->op_priority = strtol((*(D_PN(_children[0], _offset))).start_loc.s, NULL, 0); 
  else (D_PN(_ps, _offset)->globals)->r->rule_priority = strtol((*(D_PN(_children[0], _offset))).start_loc.s, NULL, 0); 
  return 0;}

D_Reduction d_reduction_90_dparser_gram = {1, 38, NULL, d_final_reduction_code_38_90_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_91_dparser_gram = {3, 39, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_92_dparser_gram = {2, 40, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_93_dparser_gram = {0, 40, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_94_dparser_gram = {1, 41, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_95_dparser_gram = {0, 41, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_96_dparser_gram = {1, 42, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_97_dparser_gram = {0, 42, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
int d_final_reduction_code_43_98_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 156 "grammar.g"

  (D_PN(_ps, _offset)->globals)->r->speculative_code.code = dup_str((*(D_PN(_children[0], _offset))).start_loc.s + 1, (*(D_PN(_children[0], _offset))).end - 1);
  (D_PN(_ps, _offset)->globals)->r->speculative_code.line = (*(D_PN(_children[0], _offset))).start_loc.line;
  return 0;}

D_Reduction d_reduction_98_dparser_gram = {1, 43, NULL, d_final_reduction_code_43_98_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_44_99_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 161 "grammar.g"

  (D_PN(_ps, _offset)->globals)->r->final_code.code = dup_str((*(D_PN(_children[0], _offset))).start_loc.s + 1, (*(D_PN(_children[0], _offset))).end - 1);
  (D_PN(_ps, _offset)->globals)->r->final_code.line = (*(D_PN(_children[0], _offset))).start_loc.line;
  return 0;}

D_Reduction d_reduction_99_dparser_gram = {1, 44, NULL, d_final_reduction_code_44_99_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
int d_final_reduction_code_45_100_dparser_gram(void *_ps, void **_children, int _n_children, int _offset, D_Parser *_parser) {
#line 166 "grammar.g"

  add_pass_code((D_PN(_ps, _offset)->globals), (D_PN(_ps, _offset)->globals)->r, (*(D_PN(_children[0], _offset))).start_loc.s, (*(D_PN(_children[0], _offset))).end, (*(D_PN(_children[2], _offset))).start_loc.s+1,
    (*(D_PN(_children[2], _offset))).end-1, (*(D_PN(_children[0], _offset))).start_loc.line, (*(D_PN(_children[2], _offset))).start_loc.line);
  return 0;}

D_Reduction d_reduction_100_dparser_gram = {3, 45, NULL, d_final_reduction_code_45_100_dparser_gram, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_101_dparser_gram = {3, 46, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_102_dparser_gram = {2, 47, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_103_dparser_gram = {0, 47, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_104_dparser_gram = {3, 48, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_105_dparser_gram = {2, 49, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_106_dparser_gram = {0, 49, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_107_dparser_gram = {3, 50, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_110_dparser_gram = {1, 50, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_115_dparser_gram = {2, 51, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_116_dparser_gram = {0, 51, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_117_dparser_gram = {2, 52, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_118_dparser_gram = {0, 52, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_119_dparser_gram = {2, 53, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_120_dparser_gram = {0, 53, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
D_Reduction d_reduction_121_dparser_gram = {1, 54, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_122_dparser_gram = {1, 55, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_123_dparser_gram = {1, 56, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_124_dparser_gram = {1, 57, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_125_dparser_gram = {1, 58, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_128_dparser_gram = {1, 59, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_129_dparser_gram = {1, 60, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Reduction d_reduction_130_dparser_gram = {1, 61, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL};
D_Shift d_shift_0_dparser_gram = { 62, 0, 0, 0, 0, NULL };
D_Shift d_shift_1_dparser_gram = { 63, 0, 0, 0, 0, NULL };
D_Shift d_shift_2_dparser_gram = { 64, 0, 0, 0, 0, NULL };
D_Shift d_shift_3_dparser_gram = { 65, 0, 0, 0, 0, NULL };
D_Shift d_shift_4_dparser_gram = { 66, 0, 0, 0, 0, NULL };
D_Shift d_shift_5_dparser_gram = { 67, 0, 0, 0, 0, NULL };
D_Shift d_shift_6_dparser_gram = { 68, 0, 0, 0, 0, NULL };
D_Shift d_shift_7_dparser_gram = { 69, 0, 0, 0, 0, NULL };
D_Shift d_shift_8_dparser_gram = { 70, 0, 0, 0, 0, NULL };
D_Shift d_shift_9_dparser_gram = { 71, 0, 0, 0, 0, NULL };
D_Shift d_shift_10_dparser_gram = { 72, 0, 0, 0, 0, NULL };
D_Shift d_shift_11_dparser_gram = { 73, 0, 0, 0, 0, NULL };
D_Shift d_shift_12_dparser_gram = { 74, 0, 0, 0, 0, NULL };
D_Shift d_shift_13_dparser_gram = { 75, 0, 0, 0, 0, NULL };
D_Shift d_shift_14_dparser_gram = { 76, 0, 0, 0, 0, NULL };
D_Shift d_shift_15_dparser_gram = { 77, 0, 0, 0, 0, NULL };
D_Shift d_shift_16_dparser_gram = { 78, 0, 0, 0, 0, NULL };
D_Shift d_shift_17_dparser_gram = { 79, 0, 0, 0, 0, NULL };
D_Shift d_shift_18_dparser_gram = { 80, 0, 0, 0, 0, NULL };
D_Shift d_shift_19_dparser_gram = { 81, 0, 0, 0, 0, NULL };
D_Shift d_shift_20_dparser_gram = { 82, 0, 0, 0, 0, NULL };
D_Shift d_shift_21_dparser_gram = { 83, 0, 0, 0, 0, NULL };
D_Shift d_shift_22_dparser_gram = { 84, 0, 0, 0, 0, NULL };
D_Shift d_shift_23_dparser_gram = { 85, 0, 0, 0, 0, NULL };
D_Shift d_shift_24_dparser_gram = { 86, 0, 0, 0, 0, NULL };
D_Shift d_shift_25_dparser_gram = { 87, 0, 0, 0, 0, NULL };
D_Shift d_shift_26_dparser_gram = { 88, 0, 0, 0, 0, NULL };
D_Shift d_shift_27_dparser_gram = { 89, 0, 0, 0, 0, NULL };
D_Shift d_shift_28_dparser_gram = { 90, 0, 0, 0, 0, NULL };
D_Shift d_shift_29_dparser_gram = { 91, 0, 0, 0, 0, NULL };
D_Shift d_shift_30_dparser_gram = { 92, 0, 0, 0, 0, NULL };
D_Shift d_shift_31_dparser_gram = { 93, 0, 0, 0, 0, NULL };
D_Shift d_shift_32_dparser_gram = { 94, 0, 0, 0, 0, NULL };
D_Shift d_shift_33_dparser_gram = { 95, 0, 0, 0, 0, NULL };
D_Shift d_shift_34_dparser_gram = { 96, 0, 0, 0, 0, NULL };
D_Shift d_shift_35_dparser_gram = { 97, 0, 0, 0, 0, NULL };
D_Shift d_shift_36_dparser_gram = { 98, 0, 0, 0, 0, NULL };
D_Shift d_shift_37_dparser_gram = { 99, 0, 0, 0, 0, NULL };
D_Shift d_shift_38_dparser_gram = { 100, 0, 0, 0, 0, NULL };
D_Shift d_shift_39_dparser_gram = { 101, 0, 0, 0, 0, NULL };
D_Shift d_shift_40_dparser_gram = { 102, 0, 0, 0, 0, NULL };
D_Shift d_shift_41_dparser_gram = { 103, 0, 0, 0, 0, NULL };
D_Shift d_shift_42_dparser_gram = { 104, 0, 0, 0, 0, NULL };
D_Shift d_shift_43_dparser_gram = { 105, 0, 0, 0, 0, NULL };
D_Shift d_shift_44_dparser_gram = { 106, 0, 0, 0, 0, NULL };
D_Shift d_shift_45_dparser_gram = { 107, 0, 0, 0, 0, NULL };
D_Shift d_shift_46_dparser_gram = { 108, 0, 0, 0, 0, NULL };
D_Shift d_shift_47_dparser_gram = { 109, 0, 0, 0, 0, NULL };
D_Shift d_shift_48_dparser_gram = { 110, 0, 0, 0, 0, NULL };
D_Shift d_shift_49_dparser_gram = { 111, 0, 0, 0, 0, NULL };
D_Shift d_shift_50_dparser_gram = { 112, 0, 0, 0, 0, NULL };
D_Shift d_shift_51_dparser_gram = { 113, 0, 0, 0, 0, NULL };
D_Shift d_shift_52_dparser_gram = { 114, 0, 0, 0, 0, NULL };
D_Shift d_shift_53_dparser_gram = { 115, 0, 0, 0, 0, NULL };
D_Shift d_shift_54_dparser_gram = { 116, 0, 0, 0, 0, NULL };
D_Shift d_shift_55_dparser_gram = { 117, 0, 0, 0, 0, NULL };
D_Shift d_shift_56_dparser_gram = { 118, 0, 0, 0, 0, NULL };
D_Shift d_shift_57_dparser_gram = { 119, 0, 0, 0, 0, NULL };
D_Shift d_shift_58_dparser_gram = { 120, 0, 0, 0, 0, NULL };
D_Shift d_shift_59_dparser_gram = { 121, 0, 0, 0, 0, NULL };
D_Shift d_shift_60_dparser_gram = { 122, 0, 0, 0, 0, NULL };
D_Shift d_shift_61_dparser_gram = { 123, 0, 0, 0, 0, NULL };
D_Shift d_shift_62_dparser_gram = { 124, 0, 0, 0, -1, NULL };
D_Shift d_shift_63_dparser_gram = { 125, 0, 0, 0, 0, NULL };
D_Shift d_shift_64_dparser_gram = { 126, 0, 0, 0, 0, NULL };
D_Shift d_shift_65_dparser_gram = { 127, 0, 0, 0, 0, NULL };

D_Shift *d_shifts_3_dparser_gram[] = {
&d_shift_0_dparser_gram, &d_shift_2_dparser_gram, &d_shift_4_dparser_gram, &d_shift_6_dparser_gram, &d_shift_7_dparser_gram, &d_shift_23_dparser_gram, &d_shift_27_dparser_gram, &d_shift_49_dparser_gram, &d_shift_62_dparser_gram, NULL};

D_Shift *d_accepts_diff_3_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_3_1_dparser_gram[] = {&d_shift_27_dparser_gram,0};
D_Shift **d_accepts_diff_3_dparser_gram[] = {
d_accepts_diff_3_0_dparser_gram,
d_accepts_diff_3_1_dparser_gram
};

unsigned char d_scanner_3_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,
};

unsigned char d_accepts_diff_3_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,5,
0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,6,0,0,0,0,
};

unsigned char d_scanner_3_0_2_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,
};

D_Shift *d_shift_3_2_dparser_gram[] = { &d_shift_23_dparser_gram, NULL};

unsigned char d_scanner_3_3_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
4,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,
};

unsigned char d_scanner_3_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,4,
0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,0,
};

D_Shift *d_shift_3_3_dparser_gram[] = { &d_shift_62_dparser_gram, NULL};

unsigned char d_accepts_diff_3_4_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
};

unsigned char d_accepts_diff_3_4_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
};

D_Shift *d_shift_3_4_dparser_gram[] = { &d_shift_27_dparser_gram, NULL};

D_Shift *d_shift_3_5_dparser_gram[] = { &d_shift_49_dparser_gram, NULL};

unsigned char d_scanner_3_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,8,0,0,9,0,0,0,0,0,0,0,0,0,0,0,
10,0,0,11,12,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_7_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_10_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_11_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_12_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_13_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_14_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_15_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_16_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_17_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_20_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_21_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,27,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_22_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_23_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_3_24_dparser_gram[] = { &d_shift_7_dparser_gram, NULL};

unsigned char d_scanner_3_25_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_26_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_27_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_28_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_29_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,34,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_3_30_dparser_gram[] = { &d_shift_4_dparser_gram, NULL};

unsigned char d_scanner_3_31_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,
};

unsigned char d_scanner_3_32_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_3_33_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,37,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_3_34_dparser_gram[] = { &d_shift_6_dparser_gram, NULL};

D_Shift *d_shift_3_35_dparser_gram[] = { &d_shift_2_dparser_gram, NULL};

D_Shift *d_shift_3_36_dparser_gram[] = { &d_shift_0_dparser_gram, NULL};

D_Shift *d_shifts_4_dparser_gram[] = {
&d_shift_31_dparser_gram, &d_shift_49_dparser_gram, &d_shift_51_dparser_gram, &d_shift_59_dparser_gram, &d_shift_60_dparser_gram, &d_shift_61_dparser_gram, &d_shift_62_dparser_gram, &d_shift_63_dparser_gram, &d_shift_64_dparser_gram, &d_shift_65_dparser_gram, NULL};

D_Shift *d_accepts_diff_4_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_4_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_4_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_4_dparser_gram[] = {
d_accepts_diff_4_0_dparser_gram,
d_accepts_diff_4_1_dparser_gram,
d_accepts_diff_4_2_dparser_gram
};

unsigned char d_scanner_4_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,2,3,2,2,2,2,4,5,0,2,2,2,6,2,2,
7,8,8,8,8,8,8,8,8,8,2,2,2,2,2,2,
};

unsigned char d_scanner_4_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,10,2,0,2,11,
2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,12,2,0,2,0,
};

D_Shift *d_shift_4_1_dparser_gram[] = { &d_shift_59_dparser_gram, NULL};

unsigned char d_scanner_4_2_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,14,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
};

unsigned char d_scanner_4_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,15,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
};

unsigned char d_scanner_4_2_2_dparser_gram[SCANNER_BLOCK_SIZE] = {
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
};

unsigned char d_scanner_4_3_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,17,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
};

unsigned char d_scanner_4_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,18,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
};

unsigned char d_scanner_4_3_2_dparser_gram[SCANNER_BLOCK_SIZE] = {
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
};

D_Shift *d_shift_4_4_dparser_gram[] = { &d_shift_31_dparser_gram, NULL};

unsigned char d_scanner_4_5_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
7,8,8,8,8,8,8,8,8,8,0,0,0,0,0,0,
};

D_Shift *d_shift_4_5_dparser_gram[] = { &d_shift_59_dparser_gram, NULL};

unsigned char d_scanner_4_6_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_4_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,
0,0,0,0,0,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,
0,0,0,0,0,20,0,0,22,0,0,0,0,0,0,0,
};

unsigned char d_accepts_diff_4_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,
};

D_Shift *d_shift_4_6_dparser_gram[] = { &d_shift_65_dparser_gram, NULL};

unsigned char d_scanner_4_7_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
8,8,8,8,8,8,8,8,8,8,0,0,0,0,0,0,
};

unsigned char d_scanner_4_7_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,
0,0,0,0,0,23,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,
0,0,0,0,0,23,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_4_7_dparser_gram[] = { &d_shift_63_dparser_gram, NULL};

unsigned char d_scanner_4_8_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
9,9,9,9,9,9,9,9,9,9,0,0,0,0,0,0,
};

unsigned char d_scanner_4_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,0,0,0,0,9,
0,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,0,0,0,0,0,
};

D_Shift *d_shift_4_9_dparser_gram[] = { &d_shift_51_dparser_gram, NULL};

D_Shift *d_shift_4_10_dparser_gram[] = { &d_shift_59_dparser_gram, NULL};

D_Shift *d_shift_4_13_dparser_gram[] = { &d_shift_61_dparser_gram, NULL};

unsigned char d_scanner_4_14_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
};

unsigned char d_scanner_4_14_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
};

D_Shift *d_shift_4_16_dparser_gram[] = { &d_shift_60_dparser_gram, NULL};

unsigned char d_scanner_4_17_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
};

unsigned char d_scanner_4_17_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
};

unsigned char d_scanner_4_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,
0,0,0,0,0,20,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,
0,0,0,0,0,20,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_4_18_dparser_gram[] = { &d_shift_65_dparser_gram, NULL};

D_Shift *d_shift_4_19_dparser_gram[] = { &d_shift_65_dparser_gram, NULL};

unsigned char d_scanner_4_20_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
26,26,26,26,26,26,26,26,26,26,0,0,0,0,0,0,
};

unsigned char d_scanner_4_20_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,26,26,26,26,26,26,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,26,26,26,26,26,26,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_4_22_dparser_gram[] = { &d_shift_63_dparser_gram, NULL};

unsigned char d_scanner_4_25_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,26,26,26,26,26,26,0,0,0,0,0,27,0,0,0,
0,0,0,0,0,27,0,0,0,0,0,0,0,0,0,0,
0,26,26,26,26,26,26,0,0,0,0,0,27,0,0,0,
0,0,0,0,0,27,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_4_25_dparser_gram[] = { &d_shift_64_dparser_gram, NULL};

D_Shift *d_shift_4_26_dparser_gram[] = { &d_shift_64_dparser_gram, NULL};

D_Shift *d_shifts_5_dparser_gram[] = {
&d_shift_14_dparser_gram, &d_shift_15_dparser_gram, &d_shift_16_dparser_gram, &d_shift_17_dparser_gram, &d_shift_18_dparser_gram, &d_shift_19_dparser_gram, &d_shift_20_dparser_gram, &d_shift_21_dparser_gram, NULL};

D_Shift *d_accepts_diff_5_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_5_dparser_gram[] = {
d_accepts_diff_5_0_dparser_gram
};

unsigned char d_scanner_5_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,2,0,0,0,0,0,0,0,0,0,0,3,0,0,0,
0,0,0,4,5,0,0,6,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,9,0,0,0,10,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_4_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_5_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,13,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_7_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,17,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_10_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_11_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,19,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_12_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,20,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_13_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_14_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_15_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,23,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_16_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_17_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,26,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,27,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_20_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,28,0,0,
0,0,0,29,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_21_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,30,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_22_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_23_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_24_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_25_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_26_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,35,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_27_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_28_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,37,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_29_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_30_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_31_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_32_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_33_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_34_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_35_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_36_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,45,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_37_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_38_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,47,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_39_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_40_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,49,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_41_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,50,0,0,0,0,0,
};

unsigned char d_scanner_5_42_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
51,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_43_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,52,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_44_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_45_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_46_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,55,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_47_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_48_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,57,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_49_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_50_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,59,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_51_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,60,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_52_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,61,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_53_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_54_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_55_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,64,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_56_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,65,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_5_57_dparser_gram[] = { &d_shift_14_dparser_gram, NULL};

unsigned char d_scanner_5_58_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,66,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_59_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,67,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_60_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,68,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_61_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_62_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,70,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_63_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,71,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_5_64_dparser_gram[] = { &d_shift_20_dparser_gram, NULL};

unsigned char d_scanner_5_65_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,72,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_66_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,73,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_67_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,74,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_68_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,75,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_69_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,76,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_70_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,77,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_5_71_dparser_gram[] = { &d_shift_16_dparser_gram, NULL};

D_Shift *d_shift_5_72_dparser_gram[] = { &d_shift_17_dparser_gram, NULL};

unsigned char d_scanner_5_73_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,78,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_74_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,79,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_75_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,80,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_76_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_77_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_78_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,83,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_79_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_80_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_81_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,86,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_5_82_dparser_gram[] = { &d_shift_15_dparser_gram, NULL};

unsigned char d_scanner_5_83_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,87,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_84_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,88,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_5_85_dparser_gram[] = { &d_shift_19_dparser_gram, NULL};

unsigned char d_scanner_5_86_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,89,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_87_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,90,0,0,0,0,0,0,
};

D_Shift *d_shift_5_88_dparser_gram[] = { &d_shift_21_dparser_gram, NULL};

unsigned char d_scanner_5_89_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,91,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_90_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,92,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_91_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_92_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,94,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_93_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,95,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_94_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_95_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,97,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_96_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,98,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_97_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,99,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_5_98_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,100,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_5_99_dparser_gram[] = { &d_shift_18_dparser_gram, NULL};

D_Shift *d_shifts_6_dparser_gram[] = {
&d_shift_62_dparser_gram, NULL};

D_Shift *d_accepts_diff_6_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_6_dparser_gram[] = {
d_accepts_diff_6_0_dparser_gram
};

unsigned char d_scanner_6_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,2,
0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,
};

unsigned char d_scanner_6_1_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,
};

D_Shift *d_accepts_diff_8_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_8_dparser_gram[] = {
d_accepts_diff_8_0_dparser_gram
};

D_Shift *d_shifts_13_dparser_gram[] = {
&d_shift_23_dparser_gram, &d_shift_27_dparser_gram, &d_shift_62_dparser_gram, NULL};

D_Shift *d_accepts_diff_13_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_13_1_dparser_gram[] = {&d_shift_27_dparser_gram,0};
D_Shift **d_accepts_diff_13_dparser_gram[] = {
d_accepts_diff_13_0_dparser_gram,
d_accepts_diff_13_1_dparser_gram
};

unsigned char d_scanner_13_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,
};

unsigned char d_scanner_13_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,4,
0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,
};

unsigned char d_scanner_13_2_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,
};

unsigned char d_scanner_13_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,3,
0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,
};

D_Shift *d_shifts_17_dparser_gram[] = {
&d_shift_22_dparser_gram, &d_shift_26_dparser_gram, NULL};

D_Shift *d_accepts_diff_17_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_17_1_dparser_gram[] = {&d_shift_22_dparser_gram,0};
D_Shift **d_accepts_diff_17_dparser_gram[] = {
d_accepts_diff_17_0_dparser_gram,
d_accepts_diff_17_1_dparser_gram
};

unsigned char d_scanner_17_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,
};

unsigned char d_scanner_17_1_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,
};

unsigned char d_accepts_diff_17_1_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,
};

D_Shift *d_shift_17_1_dparser_gram[] = { &d_shift_22_dparser_gram, NULL};

unsigned char d_scanner_17_2_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,
};

D_Shift *d_shift_17_3_dparser_gram[] = { &d_shift_26_dparser_gram, NULL};

D_Shift *d_shifts_30_dparser_gram[] = {
&d_shift_1_dparser_gram, &d_shift_31_dparser_gram, &d_shift_49_dparser_gram, &d_shift_51_dparser_gram, &d_shift_59_dparser_gram, &d_shift_60_dparser_gram, &d_shift_61_dparser_gram, &d_shift_62_dparser_gram, &d_shift_63_dparser_gram, &d_shift_64_dparser_gram, &d_shift_65_dparser_gram, NULL};

D_Shift *d_accepts_diff_30_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_30_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_30_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_30_dparser_gram[] = {
d_accepts_diff_30_0_dparser_gram,
d_accepts_diff_30_1_dparser_gram,
d_accepts_diff_30_2_dparser_gram
};

unsigned char d_scanner_30_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,10,2,0,2,11,
2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,12,2,13,2,0,
};

unsigned char d_scanner_30_2_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,15,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
};

unsigned char d_scanner_30_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,16,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
};

unsigned char d_scanner_30_2_2_dparser_gram[SCANNER_BLOCK_SIZE] = {
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
};

unsigned char d_scanner_30_3_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,18,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
};

unsigned char d_scanner_30_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,19,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
};

unsigned char d_scanner_30_3_2_dparser_gram[SCANNER_BLOCK_SIZE] = {
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
};

unsigned char d_scanner_30_6_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
20,20,20,20,20,20,20,20,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_30_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,21,0,0,0,
0,0,0,0,0,21,0,0,22,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,21,0,0,0,
0,0,0,0,0,21,0,0,23,0,0,0,0,0,0,0,
};

unsigned char d_scanner_30_7_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,0,
0,0,0,0,0,24,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,0,
0,0,0,0,0,24,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_30_12_dparser_gram[] = { &d_shift_1_dparser_gram, NULL};

unsigned char d_scanner_30_18_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
};

unsigned char d_scanner_30_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
};

unsigned char d_scanner_30_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,21,0,0,0,
0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,21,0,0,0,
0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_30_21_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
27,27,27,27,27,27,27,27,27,27,0,0,0,0,0,0,
};

unsigned char d_scanner_30_21_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,27,27,27,27,27,27,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,27,27,27,27,27,27,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_30_26_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,27,27,27,27,27,27,0,0,0,0,0,28,0,0,0,
0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,
0,27,27,27,27,27,27,0,0,0,0,0,28,0,0,0,
0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shifts_49_dparser_gram[] = {
&d_shift_1_dparser_gram, &d_shift_62_dparser_gram, NULL};

D_Shift *d_accepts_diff_49_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_49_dparser_gram[] = {
d_accepts_diff_49_0_dparser_gram
};

unsigned char d_scanner_49_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,2,
0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,0,0,3,0,0,
};

D_Shift *d_shifts_52_dparser_gram[] = {
&d_shift_9_dparser_gram, &d_shift_10_dparser_gram, &d_shift_11_dparser_gram, &d_shift_12_dparser_gram, &d_shift_13_dparser_gram, NULL};

D_Shift *d_accepts_diff_52_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_52_dparser_gram[] = {
d_accepts_diff_52_0_dparser_gram
};

unsigned char d_scanner_52_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,2,0,0,0,0,0,0,3,0,0,
4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,
0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_4_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_5_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_7_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,12,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_10_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_11_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_12_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_13_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_14_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_15_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_16_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_17_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_20_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,26,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_21_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,27,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_22_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_52_23_dparser_gram[] = { &d_shift_11_dparser_gram, NULL};

unsigned char d_scanner_52_24_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,29,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_52_26_dparser_gram[] = { &d_shift_12_dparser_gram, NULL};

unsigned char d_scanner_52_27_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_28_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_30_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,34,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_31_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_52_32_dparser_gram[] = { &d_shift_9_dparser_gram, NULL};

unsigned char d_scanner_52_33_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_52_34_dparser_gram[] = { &d_shift_10_dparser_gram, NULL};

unsigned char d_scanner_52_35_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_36_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_52_37_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_52_38_dparser_gram[] = { &d_shift_13_dparser_gram, NULL};

D_Shift *d_accepts_diff_53_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_53_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_53_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_53_dparser_gram[] = {
d_accepts_diff_53_0_dparser_gram,
d_accepts_diff_53_1_dparser_gram,
d_accepts_diff_53_2_dparser_gram
};

D_Shift *d_shifts_55_dparser_gram[] = {
&d_shift_0_dparser_gram, &d_shift_2_dparser_gram, &d_shift_4_dparser_gram, &d_shift_6_dparser_gram, &d_shift_7_dparser_gram, &d_shift_49_dparser_gram, NULL};

D_Shift *d_accepts_diff_55_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_55_dparser_gram[] = {
d_accepts_diff_55_0_dparser_gram
};

unsigned char d_scanner_55_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,
};

unsigned char d_scanner_55_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,
};

unsigned char d_scanner_55_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,5,0,0,6,0,0,0,0,0,0,0,0,0,0,0,
7,0,0,8,9,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_4_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_5_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_11_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_12_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_15_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,21,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_16_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,24,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_20_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_22_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,27,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_23_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_24_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,29,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_25_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,30,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_28_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,
};

unsigned char d_scanner_55_29_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,33,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_55_30_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shifts_59_dparser_gram[] = {
&d_shift_31_dparser_gram, &d_shift_32_dparser_gram, &d_shift_49_dparser_gram, &d_shift_51_dparser_gram, &d_shift_59_dparser_gram, &d_shift_60_dparser_gram, &d_shift_61_dparser_gram, &d_shift_62_dparser_gram, &d_shift_63_dparser_gram, &d_shift_64_dparser_gram, &d_shift_65_dparser_gram, NULL};

D_Shift *d_accepts_diff_59_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_59_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_59_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_59_dparser_gram[] = {
d_accepts_diff_59_0_dparser_gram,
d_accepts_diff_59_1_dparser_gram,
d_accepts_diff_59_2_dparser_gram
};

unsigned char d_scanner_59_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,2,3,2,2,2,2,4,5,6,2,2,2,7,2,2,
8,9,9,9,9,9,9,9,9,9,2,2,2,2,2,2,
};

unsigned char d_scanner_59_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
2,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,11,2,0,2,12,
2,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,13,2,0,2,0,
};

D_Shift *d_shift_59_5_dparser_gram[] = { &d_shift_32_dparser_gram, NULL};

unsigned char d_scanner_59_6_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
8,9,9,9,9,9,9,9,9,9,0,0,0,0,0,0,
};

unsigned char d_scanner_59_9_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
10,10,10,10,10,10,10,10,10,10,0,0,0,0,0,0,
};

unsigned char d_scanner_59_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,0,0,0,0,10,
0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,0,0,0,0,0,
};

D_Shift *d_accepts_diff_60_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_60_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_60_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_60_dparser_gram[] = {
d_accepts_diff_60_0_dparser_gram,
d_accepts_diff_60_1_dparser_gram,
d_accepts_diff_60_2_dparser_gram
};

D_Shift *d_shifts_61_dparser_gram[] = {
&d_shift_31_dparser_gram, &d_shift_49_dparser_gram, &d_shift_51_dparser_gram, &d_shift_52_dparser_gram, &d_shift_59_dparser_gram, &d_shift_60_dparser_gram, &d_shift_61_dparser_gram, &d_shift_62_dparser_gram, &d_shift_63_dparser_gram, &d_shift_64_dparser_gram, &d_shift_65_dparser_gram, NULL};

D_Shift *d_accepts_diff_61_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_61_1_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift *d_accepts_diff_61_2_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift **d_accepts_diff_61_dparser_gram[] = {
d_accepts_diff_61_0_dparser_gram,
d_accepts_diff_61_1_dparser_gram,
d_accepts_diff_61_2_dparser_gram
};

unsigned char d_scanner_61_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,10,2,11,2,12,
2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,13,2,0,2,0,
};

unsigned char d_accepts_diff_61_5_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,
};

unsigned char d_accepts_diff_61_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
};

D_Shift *d_shift_61_10_dparser_gram[] = { &d_shift_52_dparser_gram, NULL};

unsigned char d_accepts_diff_61_11_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,2,
0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,
};

D_Shift *d_accepts_diff_64_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_64_dparser_gram[] = {
d_accepts_diff_64_0_dparser_gram
};

D_Shift *d_shifts_72_dparser_gram[] = {
&d_shift_1_dparser_gram, NULL};

D_Shift *d_accepts_diff_72_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_72_dparser_gram[] = {
d_accepts_diff_72_0_dparser_gram
};

unsigned char d_scanner_72_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,
};

D_Shift *d_accepts_diff_73_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_73_dparser_gram[] = {
d_accepts_diff_73_0_dparser_gram
};

D_Shift *d_shifts_77_dparser_gram[] = {
&d_shift_23_dparser_gram, NULL};

D_Shift *d_accepts_diff_77_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_77_dparser_gram[] = {
d_accepts_diff_77_0_dparser_gram
};

D_Shift *d_accepts_diff_80_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_80_dparser_gram[] = {
d_accepts_diff_80_0_dparser_gram
};

D_Shift *d_shifts_92_dparser_gram[] = {
&d_shift_28_dparser_gram, NULL};

D_Shift *d_accepts_diff_92_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_92_dparser_gram[] = {
d_accepts_diff_92_0_dparser_gram
};

unsigned char d_scanner_92_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,
};

D_Shift *d_shift_92_1_dparser_gram[] = { &d_shift_28_dparser_gram, NULL};

D_Shift *d_shifts_95_dparser_gram[] = {
&d_shift_29_dparser_gram, &d_shift_31_dparser_gram, &d_shift_49_dparser_gram, &d_shift_51_dparser_gram, &d_shift_60_dparser_gram, &d_shift_61_dparser_gram, &d_shift_62_dparser_gram, NULL};

D_Shift *d_accepts_diff_95_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_95_dparser_gram[] = {
d_accepts_diff_95_0_dparser_gram
};

unsigned char d_scanner_95_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,2,0,3,0,0,4,5,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_95_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,7,0,0,0,6,
0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,8,0,0,0,0,
};

unsigned char d_scanner_95_1_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,10,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
};

unsigned char d_scanner_95_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,11,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
};

unsigned char d_scanner_95_1_2_dparser_gram[SCANNER_BLOCK_SIZE] = {
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
};

unsigned char d_scanner_95_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,12,0,0,0,0,
};

unsigned char d_scanner_95_3_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
};

unsigned char d_scanner_95_5_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,
};

unsigned char d_scanner_95_5_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,0,0,0,0,6,
0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,
};

unsigned char d_scanner_95_10_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
};

unsigned char d_scanner_95_14_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
};

unsigned char d_scanner_95_14_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
};

unsigned char d_scanner_95_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_95_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_95_20_dparser_gram[] = { &d_shift_29_dparser_gram, NULL};

D_Shift *d_shifts_99_dparser_gram[] = {
&d_shift_38_dparser_gram, &d_shift_39_dparser_gram, &d_shift_40_dparser_gram, &d_shift_41_dparser_gram, &d_shift_42_dparser_gram, &d_shift_43_dparser_gram, &d_shift_44_dparser_gram, &d_shift_45_dparser_gram, &d_shift_46_dparser_gram, &d_shift_47_dparser_gram, &d_shift_51_dparser_gram, NULL};

D_Shift *d_accepts_diff_99_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_99_dparser_gram[] = {
d_accepts_diff_99_0_dparser_gram
};

unsigned char d_scanner_99_0_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,4,0,0,0,0,0,0,0,0,0,5,0,0,0,
0,0,6,0,0,7,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_4_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_5_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_7_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_11_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_13_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_14_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_15_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_16_dparser_gram[] = { &d_shift_47_dparser_gram, NULL};

unsigned char d_scanner_99_17_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_18_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,
};

unsigned char d_scanner_99_19_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,
};

D_Shift *d_shift_99_20_dparser_gram[] = { &d_shift_46_dparser_gram, NULL};

unsigned char d_scanner_99_22_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_23_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,26,0,0,27,
0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_24_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,29,0,0,30,
0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_26_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_27_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,34,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_29_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_30_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_31_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_32_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_33_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,40,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_34_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,41,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_35_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_36_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,43,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_38_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,45,0,0,0,
0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_39_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,47,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_40_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_41_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,49,0,0,0,
0,0,50,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_42_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,51,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_43_dparser_gram[] = { &d_shift_43_dparser_gram, NULL};

unsigned char d_scanner_99_44_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,52,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_45_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,53,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_46_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_47_dparser_gram[] = { &d_shift_45_dparser_gram, NULL};

unsigned char d_scanner_99_48_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,55,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_49_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,56,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_50_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,57,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_51_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,58,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_52_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,59,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_53_dparser_gram[] = { &d_shift_42_dparser_gram, NULL};

unsigned char d_scanner_99_54_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,60,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_55_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,61,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_56_dparser_gram[] = { &d_shift_44_dparser_gram, NULL};

unsigned char d_scanner_99_57_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,62,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_58_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,63,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_59_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,64,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_99_60_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,65,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_61_dparser_gram[] = { &d_shift_39_dparser_gram, NULL};

unsigned char d_scanner_99_62_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,66,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_63_dparser_gram[] = { &d_shift_41_dparser_gram, NULL};

unsigned char d_scanner_99_64_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,67,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_99_65_dparser_gram[] = { &d_shift_38_dparser_gram, NULL};

D_Shift *d_shift_99_66_dparser_gram[] = { &d_shift_40_dparser_gram, NULL};

D_Shift *d_accepts_diff_100_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_100_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_100_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_100_dparser_gram[] = {
d_accepts_diff_100_0_dparser_gram,
d_accepts_diff_100_1_dparser_gram,
d_accepts_diff_100_2_dparser_gram
};

D_Shift *d_shifts_123_dparser_gram[] = {
&d_shift_63_dparser_gram, &d_shift_64_dparser_gram, &d_shift_65_dparser_gram, NULL};

D_Shift *d_accepts_diff_123_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_123_1_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_123_dparser_gram[] = {
d_accepts_diff_123_0_dparser_gram,
d_accepts_diff_123_1_dparser_gram
};

unsigned char d_scanner_123_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,
3,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,
};

unsigned char d_scanner_123_1_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,
};

unsigned char d_scanner_123_2_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_123_2_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,
0,0,0,0,0,6,0,0,7,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,
0,0,0,0,0,6,0,0,8,0,0,0,0,0,0,0,
};

unsigned char d_scanner_123_3_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,
0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,
0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_123_4_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,
0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,
0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_123_6_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,10,10,10,10,10,10,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,10,10,10,10,10,10,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_123_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,10,10,10,10,10,10,0,0,0,0,0,11,0,0,0,
0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,
0,10,10,10,10,10,10,0,0,0,0,0,11,0,0,0,
0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shifts_125_dparser_gram[] = {
&d_shift_49_dparser_gram, NULL};

D_Shift *d_accepts_diff_125_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_125_dparser_gram[] = {
d_accepts_diff_125_0_dparser_gram
};

D_Shift *d_accepts_diff_128_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_128_1_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift *d_accepts_diff_128_2_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_128_dparser_gram[] = {
d_accepts_diff_128_0_dparser_gram,
d_accepts_diff_128_1_dparser_gram,
d_accepts_diff_128_2_dparser_gram
};

D_Shift *d_accepts_diff_131_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_131_1_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift *d_accepts_diff_131_2_dparser_gram[] = {&d_shift_59_dparser_gram,0};
D_Shift **d_accepts_diff_131_dparser_gram[] = {
d_accepts_diff_131_0_dparser_gram,
d_accepts_diff_131_1_dparser_gram,
d_accepts_diff_131_2_dparser_gram
};

D_Shift *d_shifts_132_dparser_gram[] = {
&d_shift_33_dparser_gram, &d_shift_34_dparser_gram, &d_shift_35_dparser_gram, &d_shift_36_dparser_gram, &d_shift_37_dparser_gram, NULL};

D_Shift *d_accepts_diff_132_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_132_dparser_gram[] = {
d_accepts_diff_132_0_dparser_gram
};

unsigned char d_scanner_132_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,2,0,0,0,0,0,3,4,0,0,0,5,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,
};

unsigned char d_scanner_132_1_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_132_2_dparser_gram[] = { &d_shift_36_dparser_gram, NULL};

D_Shift *d_shift_132_3_dparser_gram[] = { &d_shift_37_dparser_gram, NULL};

D_Shift *d_shift_132_5_dparser_gram[] = { &d_shift_35_dparser_gram, NULL};

D_Shift *d_shift_132_7_dparser_gram[] = { &d_shift_34_dparser_gram, NULL};

unsigned char d_scanner_132_8_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

unsigned char d_scanner_132_9_1_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_shift_132_10_dparser_gram[] = { &d_shift_33_dparser_gram, NULL};

D_Shift *d_accepts_diff_133_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_133_dparser_gram[] = {
d_accepts_diff_133_0_dparser_gram
};

D_Shift *d_shifts_141_dparser_gram[] = {
&d_shift_32_dparser_gram, NULL};

D_Shift *d_accepts_diff_141_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_141_dparser_gram[] = {
d_accepts_diff_141_0_dparser_gram
};

unsigned char d_scanner_141_0_0_dparser_gram[SCANNER_BLOCK_SIZE] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

D_Shift *d_accepts_diff_144_0_dparser_gram[] = {0};
D_Shift *d_accepts_diff_144_1_dparser_gram[] = {&d_shift_65_dparser_gram,0};
D_Shift **d_accepts_diff_144_dparser_gram[] = {
d_accepts_diff_144_0_dparser_gram,
d_accepts_diff_144_1_dparser_gram
};

D_Shift *d_accepts_diff_151_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_151_dparser_gram[] = {
d_accepts_diff_151_0_dparser_gram
};

D_Shift *d_shifts_155_dparser_gram[] = {
&d_shift_22_dparser_gram, NULL};

D_Shift *d_accepts_diff_155_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_155_dparser_gram[] = {
d_accepts_diff_155_0_dparser_gram
};

D_Shift *d_accepts_diff_156_0_dparser_gram[] = {0};
D_Shift **d_accepts_diff_156_dparser_gram[] = {
d_accepts_diff_156_0_dparser_gram
};

SB_uint8 d_scanner_3_dparser_gram[37] = {
{ NULL, {d_scanner_3_0_0_dparser_gram, d_scanner_3_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_1_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_2_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_3_3_0_dparser_gram, d_scanner_3_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_4_dparser_gram, {d_scanner_3_3_0_dparser_gram, d_scanner_3_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_10_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_12_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_13_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_14_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_15_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_16_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_17_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_20_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_22_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_23_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_24_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_25_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_27_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_28_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_29_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_30_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_31_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_32_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_33_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_34_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_35_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_36_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_3_dparser_gram[37] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_4_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_4_dparser_gram[27] = {
{ NULL, {d_scanner_4_0_0_dparser_gram, d_scanner_4_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_2_0_dparser_gram, d_scanner_4_2_1_dparser_gram, 
  d_scanner_4_2_2_dparser_gram, d_scanner_4_2_2_dparser_gram}},
{ NULL, {d_scanner_4_3_0_dparser_gram, d_scanner_4_3_1_dparser_gram, 
  d_scanner_4_3_2_dparser_gram, d_scanner_4_3_2_dparser_gram}},
{ d_shift_4_4_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_4_5_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_4_6_0_dparser_gram, d_scanner_4_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_4_7_0_dparser_gram, d_scanner_4_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_4_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_9_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_4_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_2_0_dparser_gram, d_scanner_4_2_1_dparser_gram, 
  d_scanner_4_2_2_dparser_gram, d_scanner_4_2_2_dparser_gram}},
{ d_shift_4_13_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_14_0_dparser_gram, d_scanner_4_14_1_dparser_gram, 
  d_scanner_4_14_1_dparser_gram, d_scanner_4_14_1_dparser_gram}},
{ NULL, {d_scanner_4_3_0_dparser_gram, d_scanner_4_3_1_dparser_gram, 
  d_scanner_4_3_2_dparser_gram, d_scanner_4_3_2_dparser_gram}},
{ d_shift_4_16_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_17_0_dparser_gram, d_scanner_4_17_1_dparser_gram, 
  d_scanner_4_17_1_dparser_gram, d_scanner_4_17_1_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_4_6_0_dparser_gram, d_scanner_4_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_20_0_dparser_gram, d_scanner_4_20_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_20_0_dparser_gram, d_scanner_4_20_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_2_0_dparser_gram, d_scanner_4_2_1_dparser_gram, 
  d_scanner_4_2_2_dparser_gram, d_scanner_4_2_2_dparser_gram}},
{ NULL, {d_scanner_4_3_0_dparser_gram, d_scanner_4_3_1_dparser_gram, 
  d_scanner_4_3_2_dparser_gram, d_scanner_4_3_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_4_20_0_dparser_gram, d_scanner_4_25_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_4_dparser_gram[27] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_4_6_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_4_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_5_dparser_gram[100] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_1_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_2_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_4_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_5_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_10_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_12_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_13_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_14_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_15_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_16_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_17_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_20_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_22_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_23_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_24_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_25_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_27_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_28_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_29_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_30_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_31_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_32_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_33_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_34_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_35_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_36_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_37_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_38_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_39_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_40_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_41_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_42_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_43_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_44_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_45_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_46_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_47_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_48_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_49_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_50_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_51_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_52_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_53_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_54_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_55_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_56_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_57_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_58_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_59_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_60_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_61_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_62_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_63_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_64_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_65_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_66_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_67_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_68_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_69_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_70_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_71_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_72_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_73_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_74_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_75_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_76_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_77_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_78_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_79_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_80_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_81_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_82_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_83_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_84_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_85_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_86_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_87_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_88_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_89_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_90_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_91_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_92_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_93_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_94_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_95_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_96_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_97_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_98_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_5_99_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_5_dparser_gram[100] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_6_dparser_gram[2] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_6_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_6_1_0_dparser_gram, d_scanner_6_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_6_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_13_dparser_gram[4] = {
{ NULL, {d_scanner_13_0_0_dparser_gram, d_scanner_13_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_2_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_13_2_0_dparser_gram, d_scanner_13_2_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_4_dparser_gram, {d_scanner_13_2_0_dparser_gram, d_scanner_13_2_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_13_dparser_gram[4] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_4_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_17_dparser_gram[4] = {
{ NULL, {d_scanner_17_0_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_17_1_dparser_gram, {d_scanner_17_1_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_17_2_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_17_3_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_17_dparser_gram[4] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_17_1_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_30_dparser_gram[28] = {
{ NULL, {d_scanner_4_0_0_dparser_gram, d_scanner_30_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_4_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_4_5_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_30_6_0_dparser_gram, d_scanner_30_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_4_7_0_dparser_gram, d_scanner_30_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_4_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_9_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_4_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_30_12_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ d_shift_4_13_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_17_0_dparser_gram, d_scanner_4_17_1_dparser_gram, 
  d_scanner_4_17_1_dparser_gram, d_scanner_4_17_1_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_16_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_18_0_dparser_gram, d_scanner_30_18_1_dparser_gram, 
  d_scanner_30_18_1_dparser_gram, d_scanner_30_18_1_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_30_6_0_dparser_gram, d_scanner_30_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_21_0_dparser_gram, d_scanner_30_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_21_0_dparser_gram, d_scanner_30_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_30_21_0_dparser_gram, d_scanner_30_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_30_dparser_gram[28] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_4_6_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_4_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_49_dparser_gram[3] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_49_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_6_1_0_dparser_gram, d_scanner_6_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_30_12_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_49_dparser_gram[3] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_52_dparser_gram[39] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_1_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_2_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_4_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_5_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_10_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_12_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_13_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_14_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_15_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_16_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_17_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_20_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_22_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_52_23_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_24_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_52_26_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_27_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_28_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_28_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_30_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_31_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_52_32_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_33_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_52_34_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_35_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_36_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_37_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_52_38_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_52_dparser_gram[39] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_55_dparser_gram[34] = {
{ NULL, {d_scanner_55_0_0_dparser_gram, d_scanner_55_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_1_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_4_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_5_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_10_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_10_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_12_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_12_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_15_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_16_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_17_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_20_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_24_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_22_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_23_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_24_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_25_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_27_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_30_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_28_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_29_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_30_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_34_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_35_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_36_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_55_dparser_gram[34] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_59_dparser_gram[28] = {
{ NULL, {d_scanner_59_0_0_dparser_gram, d_scanner_59_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_4_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_59_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_59_6_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_30_6_0_dparser_gram, d_scanner_30_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_30_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_59_9_0_dparser_gram, d_scanner_59_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_9_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_59_9_0_dparser_gram, d_scanner_59_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ d_shift_4_13_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_17_0_dparser_gram, d_scanner_4_17_1_dparser_gram, 
  d_scanner_4_17_1_dparser_gram, d_scanner_4_17_1_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_16_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_18_0_dparser_gram, d_scanner_30_18_1_dparser_gram, 
  d_scanner_30_18_1_dparser_gram, d_scanner_30_18_1_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_30_6_0_dparser_gram, d_scanner_30_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_21_0_dparser_gram, d_scanner_30_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_21_0_dparser_gram, d_scanner_30_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_30_21_0_dparser_gram, d_scanner_30_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_59_dparser_gram[28] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_4_6_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_4_0_dparser_gram, d_accepts_diff_3_4_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_61_dparser_gram[28] = {
{ NULL, {d_scanner_4_0_0_dparser_gram, d_scanner_61_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_4_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_4_5_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_30_6_0_dparser_gram, d_scanner_30_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_4_7_0_dparser_gram, d_scanner_30_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_4_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_9_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_61_10_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_10_dparser_gram, {d_scanner_4_8_0_dparser_gram, d_scanner_4_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ d_shift_4_13_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_4_17_0_dparser_gram, d_scanner_4_17_1_dparser_gram, 
  d_scanner_4_17_1_dparser_gram, d_scanner_4_17_1_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_16_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_18_0_dparser_gram, d_scanner_30_18_1_dparser_gram, 
  d_scanner_30_18_1_dparser_gram, d_scanner_30_18_1_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_30_6_0_dparser_gram, d_scanner_30_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_21_0_dparser_gram, d_scanner_30_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_21_0_dparser_gram, d_scanner_30_21_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_30_2_0_dparser_gram, d_scanner_30_2_1_dparser_gram, 
  d_scanner_30_2_2_dparser_gram, d_scanner_30_2_2_dparser_gram}},
{ NULL, {d_scanner_30_3_0_dparser_gram, d_scanner_30_3_1_dparser_gram, 
  d_scanner_30_3_2_dparser_gram, d_scanner_30_3_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_30_21_0_dparser_gram, d_scanner_30_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_61_dparser_gram[28] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_61_5_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_61_6_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_61_5_0_dparser_gram, d_accepts_diff_61_11_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_72_dparser_gram[2] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_72_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_30_12_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_72_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_77_dparser_gram[2] = {
{ NULL, {d_scanner_13_0_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_2_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_77_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_92_dparser_gram[2] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_92_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_92_1_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_92_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_95_dparser_gram[21] = {
{ NULL, {d_scanner_95_0_0_dparser_gram, d_scanner_95_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_1_0_dparser_gram, d_scanner_95_1_1_dparser_gram, 
  d_scanner_95_1_2_dparser_gram, d_scanner_95_1_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_95_2_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_3_0_dparser_gram, d_scanner_4_2_1_dparser_gram, 
  d_scanner_4_2_2_dparser_gram, d_scanner_4_2_2_dparser_gram}},
{ d_shift_4_4_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_3_dparser_gram, {d_scanner_95_5_0_dparser_gram, d_scanner_95_5_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_9_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_1_0_dparser_gram, d_scanner_95_1_1_dparser_gram, 
  d_scanner_95_1_2_dparser_gram, d_scanner_95_1_2_dparser_gram}},
{ d_shift_4_13_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_10_0_dparser_gram, d_scanner_4_3_2_dparser_gram, 
  d_scanner_4_3_2_dparser_gram, d_scanner_4_3_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_55_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_3_0_dparser_gram, d_scanner_4_2_1_dparser_gram, 
  d_scanner_4_2_2_dparser_gram, d_scanner_4_2_2_dparser_gram}},
{ d_shift_4_16_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_14_0_dparser_gram, d_scanner_95_14_1_dparser_gram, 
  d_scanner_95_14_1_dparser_gram, d_scanner_95_14_1_dparser_gram}},
{ NULL, {d_scanner_95_1_0_dparser_gram, d_scanner_95_1_1_dparser_gram, 
  d_scanner_95_1_2_dparser_gram, d_scanner_95_1_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_13_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_95_3_0_dparser_gram, d_scanner_4_2_1_dparser_gram, 
  d_scanner_4_2_2_dparser_gram, d_scanner_4_2_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_95_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_95_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_95_20_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_95_dparser_gram[21] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_99_dparser_gram[67] = {
{ NULL, {d_scanner_55_0_0_dparser_gram, d_scanner_99_0_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_1_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_9_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_4_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_5_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_7_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_3_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_11_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_13_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_14_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_15_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_16_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_17_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_18_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_19_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_20_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_16_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_22_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_23_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_24_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_52_28_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_27_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_26_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_29_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_30_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_31_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_32_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_33_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_34_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_35_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_36_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_5_35_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_38_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_39_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_40_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_41_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_42_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_43_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_44_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_45_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_46_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_47_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_48_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_49_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_50_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_51_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_52_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_53_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_54_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_55_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_56_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_57_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_58_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_59_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_60_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_61_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_62_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_63_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_64_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_65_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_99_66_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_99_dparser_gram[67] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_123_dparser_gram[11] = {
{ NULL, {d_scanner_123_0_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_123_1_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_123_2_0_dparser_gram, d_scanner_123_2_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_3_3_0_dparser_gram, d_scanner_123_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_123_2_0_dparser_gram, d_scanner_123_4_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_19_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_59_9_0_dparser_gram, d_scanner_123_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_59_9_0_dparser_gram, d_scanner_123_6_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_22_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_59_9_0_dparser_gram, d_scanner_123_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_4_26_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_123_dparser_gram[11] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_61_6_1_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_125_dparser_gram[2] = {
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_13_0_0_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_3_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_125_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_132_dparser_gram[11] = {
{ NULL, {d_scanner_132_0_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_132_1_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_132_2_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_132_3_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_3_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_132_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_99_4_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_132_7_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_132_8_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ NULL, {d_scanner_3_0_2_dparser_gram, d_scanner_132_9_1_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_132_10_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_132_dparser_gram[11] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_141_dparser_gram[2] = {
{ NULL, {d_scanner_141_0_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_59_5_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_141_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

SB_uint8 d_scanner_155_dparser_gram[2] = {
{ NULL, {d_scanner_17_0_0_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}},
{ d_shift_17_1_dparser_gram, {d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram, 
  d_scanner_3_0_2_dparser_gram, d_scanner_3_0_2_dparser_gram}}
};

SB_trans_uint8 d_transition_155_dparser_gram[2] = {
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}},
{{ d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram, 
  d_accepts_diff_3_0_0_dparser_gram, d_accepts_diff_3_0_0_dparser_gram}}
};

unsigned char d_goto_valid_0_dparser_gram[] = {
0x46, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_0_dparser_gram[] = {&d_reduction_9_dparser_gram};
D_Reduction *d_reductions_2_dparser_gram[] = {&d_reduction_1_dparser_gram};
unsigned char d_goto_valid_3_dparser_gram[] = {
0x98, 0x80, 0x6, 0x0, 0x0, 0x40, 0x0, 0x42, 0x35, 0x0, 0x20, 0x2, 0x0, 0x80, 0x0, 0x10};
unsigned char d_goto_valid_4_dparser_gram[] = {
0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0xc4, 0x3f, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x2, 0xfe};
unsigned char d_goto_valid_5_dparser_gram[] = {
0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char d_goto_valid_6_dparser_gram[] = {
0x0, 0x41, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10};
D_Reduction *d_reductions_7_dparser_gram[] = {&d_reduction_14_dparser_gram};
unsigned char d_goto_valid_8_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10};
D_Reduction *d_reductions_9_dparser_gram[] = {&d_reduction_40_dparser_gram};
D_Reduction *d_reductions_10_dparser_gram[] = {&d_reduction_43_dparser_gram};
unsigned char d_goto_valid_11_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_11_dparser_gram[] = {&d_reduction_103_dparser_gram};
D_Reduction *d_reductions_12_dparser_gram[] = {&d_reduction_124_dparser_gram};
unsigned char d_goto_valid_13_dparser_gram[] = {
0x10, 0x80, 0x6, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x20, 0x2, 0x0, 0x0, 0x0, 0x10};
D_Reduction *d_reductions_13_dparser_gram[] = {&d_reduction_2_dparser_gram};
D_Reduction *d_reductions_14_dparser_gram[] = {&d_reduction_4_dparser_gram};
D_Reduction *d_reductions_15_dparser_gram[] = {&d_reduction_8_dparser_gram};
unsigned char d_goto_valid_16_dparser_gram[] = {
0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_16_dparser_gram[] = {&d_reduction_7_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_16_dparser_gram[] = {{ 0, 55, &d_reduction_5_dparser_gram}};
unsigned char d_goto_valid_17_dparser_gram[] = {
0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x1, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_18_dparser_gram[] = {&d_reduction_42_dparser_gram};
D_Reduction *d_reductions_19_dparser_gram[] = {&d_reduction_10_dparser_gram};
D_Reduction *d_reductions_20_dparser_gram[] = {&d_reduction_43_dparser_gram};
unsigned char d_goto_valid_21_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_21_dparser_gram[] = {&d_reduction_120_dparser_gram};
unsigned char d_goto_valid_22_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_22_dparser_gram[] = {&d_reduction_116_dparser_gram};
unsigned char d_goto_valid_23_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_23_dparser_gram[] = {&d_reduction_118_dparser_gram};
D_Reduction *d_reductions_24_dparser_gram[] = {&d_reduction_121_dparser_gram};
D_Reduction *d_reductions_25_dparser_gram[] = {&d_reduction_122_dparser_gram};
D_Reduction *d_reductions_26_dparser_gram[] = {&d_reduction_123_dparser_gram};
D_Reduction *d_reductions_27_dparser_gram[] = {&d_reduction_128_dparser_gram};
D_Reduction *d_reductions_28_dparser_gram[] = {&d_reduction_129_dparser_gram};
D_Reduction *d_reductions_29_dparser_gram[] = {&d_reduction_130_dparser_gram};
unsigned char d_goto_valid_30_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0xbf, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x2, 0xfe};
D_Reduction *d_reductions_31_dparser_gram[] = {&d_reduction_21_dparser_gram};
D_Reduction *d_reductions_32_dparser_gram[] = {&d_reduction_110_dparser_gram};
D_Reduction *d_reductions_33_dparser_gram[] = {&d_reduction_110_dparser_gram};
D_Reduction *d_reductions_34_dparser_gram[] = {&d_reduction_110_dparser_gram};
D_Reduction *d_reductions_35_dparser_gram[] = {&d_reduction_110_dparser_gram};
D_Reduction *d_reductions_36_dparser_gram[] = {&d_reduction_110_dparser_gram};
D_Reduction *d_reductions_37_dparser_gram[] = {&d_reduction_125_dparser_gram};
D_Reduction *d_reductions_38_dparser_gram[] = {&d_reduction_125_dparser_gram};
D_Reduction *d_reductions_39_dparser_gram[] = {&d_reduction_125_dparser_gram};
D_Reduction *d_reductions_40_dparser_gram[] = {&d_reduction_29_dparser_gram};
D_Reduction *d_reductions_41_dparser_gram[] = {&d_reduction_30_dparser_gram};
D_Reduction *d_reductions_42_dparser_gram[] = {&d_reduction_31_dparser_gram};
D_Reduction *d_reductions_43_dparser_gram[] = {&d_reduction_32_dparser_gram};
D_Reduction *d_reductions_44_dparser_gram[] = {&d_reduction_33_dparser_gram};
D_Reduction *d_reductions_45_dparser_gram[] = {&d_reduction_34_dparser_gram};
D_Reduction *d_reductions_46_dparser_gram[] = {&d_reduction_35_dparser_gram};
D_Reduction *d_reductions_47_dparser_gram[] = {&d_reduction_36_dparser_gram};
unsigned char d_goto_valid_48_dparser_gram[] = {
0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_48_dparser_gram[] = {&d_reduction_19_dparser_gram};
unsigned char d_goto_valid_49_dparser_gram[] = {
0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x82, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10};
D_Reduction *d_reductions_50_dparser_gram[] = {&d_reduction_17_dparser_gram};
D_Reduction *d_reductions_51_dparser_gram[] = {&d_reduction_37_dparser_gram};
unsigned char d_goto_valid_52_dparser_gram[] = {
0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_52_dparser_gram[] = {&d_reduction_22_dparser_gram};
unsigned char d_goto_valid_53_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0xbf, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x2, 0xfe};
D_Reduction *d_reductions_54_dparser_gram[] = {&d_reduction_3_dparser_gram};
unsigned char d_goto_valid_55_dparser_gram[] = {
0x80, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x40, 0x35, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0};
D_Reduction *d_reductions_55_dparser_gram[] = {&d_reduction_5_dparser_gram};
unsigned char d_goto_valid_56_dparser_gram[] = {
0x0, 0x0, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_56_dparser_gram[] = {&d_reduction_62_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_56_dparser_gram[] = {{ 1, 92, &d_reduction_45_dparser_gram}, { 3, 124, &d_reduction_49_dparser_gram}};
D_Reduction *d_reductions_57_dparser_gram[] = {&d_reduction_41_dparser_gram};
unsigned char d_goto_valid_58_dparser_gram[] = {
0x0, 0x0, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_58_dparser_gram[] = {&d_reduction_62_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_58_dparser_gram[] = {{ 1, 92, &d_reduction_45_dparser_gram}, { 3, 124, &d_reduction_49_dparser_gram}};
unsigned char d_goto_valid_59_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0x3f, 0x0, 0x0, 0x0, 0x60, 0x0, 0x80, 0x2, 0xfe};
unsigned char d_goto_valid_60_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0xbf, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x2, 0xfe};
unsigned char d_goto_valid_61_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0x3f, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x6, 0xfe};
D_Reduction *d_reductions_62_dparser_gram[] = {&d_reduction_11_dparser_gram};
D_Reduction *d_reductions_63_dparser_gram[] = {&d_reduction_20_dparser_gram};
unsigned char d_goto_valid_64_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x82, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10};
D_Reduction *d_reductions_65_dparser_gram[] = {&d_reduction_13_dparser_gram};
D_Reduction *d_reductions_66_dparser_gram[] = {&d_reduction_16_dparser_gram};
D_Reduction *d_reductions_67_dparser_gram[] = {&d_reduction_24_dparser_gram};
D_Reduction *d_reductions_68_dparser_gram[] = {&d_reduction_25_dparser_gram};
D_Reduction *d_reductions_69_dparser_gram[] = {&d_reduction_26_dparser_gram};
D_Reduction *d_reductions_70_dparser_gram[] = {&d_reduction_27_dparser_gram};
D_Reduction *d_reductions_71_dparser_gram[] = {&d_reduction_28_dparser_gram};
unsigned char d_goto_valid_72_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char d_goto_valid_73_dparser_gram[] = {
0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_73_dparser_gram[] = {&d_reduction_22_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_73_dparser_gram[] = {{ 0, 90, &d_reduction_23_dparser_gram}};
D_Reduction *d_reductions_74_dparser_gram[] = {&d_reduction_101_dparser_gram};
D_Reduction *d_reductions_75_dparser_gram[] = {&d_reduction_102_dparser_gram};
D_Reduction *d_reductions_76_dparser_gram[] = {&d_reduction_6_dparser_gram};
unsigned char d_goto_valid_77_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char d_goto_valid_78_dparser_gram[] = {
0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_78_dparser_gram[] = {&d_reduction_47_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_78_dparser_gram[] = {{ 0, 92, &d_reduction_45_dparser_gram}};
unsigned char d_goto_valid_79_dparser_gram[] = {
0x0, 0x0, 0x0, 0xb, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_79_dparser_gram[] = {&d_reduction_53_dparser_gram, &d_reduction_58_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_79_dparser_gram[] = {{ 2, 124, &d_reduction_49_dparser_gram}};
unsigned char d_goto_valid_80_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_81_dparser_gram[] = {&d_reduction_107_dparser_gram};
D_Reduction *d_reductions_82_dparser_gram[] = {&d_reduction_119_dparser_gram};
D_Reduction *d_reductions_83_dparser_gram[] = {&d_reduction_107_dparser_gram};
D_Reduction *d_reductions_84_dparser_gram[] = {&d_reduction_115_dparser_gram};
D_Reduction *d_reductions_85_dparser_gram[] = {&d_reduction_107_dparser_gram};
D_Reduction *d_reductions_86_dparser_gram[] = {&d_reduction_117_dparser_gram};
D_Reduction *d_reductions_87_dparser_gram[] = {&d_reduction_12_dparser_gram};
D_Reduction *d_reductions_88_dparser_gram[] = {&d_reduction_18_dparser_gram};
D_Reduction *d_reductions_89_dparser_gram[] = {&d_reduction_15_dparser_gram};
D_Reduction *d_reductions_90_dparser_gram[] = {&d_reduction_23_dparser_gram};
D_Reduction *d_reductions_91_dparser_gram[] = {&d_reduction_38_dparser_gram};
unsigned char d_goto_valid_92_dparser_gram[] = {
0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_92_dparser_gram[] = {&d_reduction_45_dparser_gram};
unsigned char d_goto_valid_93_dparser_gram[] = {
0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_93_dparser_gram[] = {&d_reduction_51_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_93_dparser_gram[] = {{ 1, 124, &d_reduction_49_dparser_gram}};
D_Reduction *d_reductions_94_dparser_gram[] = {&d_reduction_52_dparser_gram};
unsigned char d_goto_valid_95_dparser_gram[] = {
0x0, 0x0, 0x0, 0x90, 0x2, 0x40, 0x81, 0x3, 0x0, 0x0, 0x0, 0x28, 0x0, 0x80, 0x2, 0x1c};
D_Reduction *d_reductions_96_dparser_gram[] = {&d_reduction_38_dparser_gram};
unsigned char d_goto_valid_97_dparser_gram[] = {
0x0, 0x0, 0x40, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_97_dparser_gram[] = {&d_reduction_62_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_97_dparser_gram[] = {{ 0, 111, &d_reduction_48_dparser_gram}, { 3, 124, &d_reduction_49_dparser_gram}};
D_Reduction *d_reductions_98_dparser_gram[] = {&d_reduction_46_dparser_gram};
unsigned char d_goto_valid_99_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0xb0, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, 0x2, 0x0};
D_Reduction *d_reductions_99_dparser_gram[] = {&d_reduction_97_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_99_dparser_gram[] = {{ 0, 124, &d_reduction_49_dparser_gram}, { 2, 151, &d_reduction_91_dparser_gram}};
unsigned char d_goto_valid_100_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0xc4, 0x3f, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x2, 0xfe};
unsigned char d_goto_valid_101_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_101_dparser_gram[] = {&d_reduction_73_dparser_gram};
unsigned char d_goto_valid_102_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_102_dparser_gram[] = {&d_reduction_106_dparser_gram};
D_Reduction *d_reductions_103_dparser_gram[] = {&d_reduction_57_dparser_gram};
unsigned char d_goto_valid_104_dparser_gram[] = {
0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_104_dparser_gram[] = {&d_reduction_56_dparser_gram, &d_reduction_70_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_104_dparser_gram[] = {{ 0, 132, &d_reduction_54_dparser_gram}};
unsigned char d_goto_valid_105_dparser_gram[] = {
0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_105_dparser_gram[] = {&d_reduction_61_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_105_dparser_gram[] = {{ 0, 133, &d_reduction_59_dparser_gram}};
D_Reduction *d_reductions_106_dparser_gram[] = {&d_reduction_72_dparser_gram};
D_Reduction *d_reductions_107_dparser_gram[] = {&d_reduction_71_dparser_gram};
D_Reduction *d_reductions_108_dparser_gram[] = {&d_reduction_63_dparser_gram};
D_Reduction *d_reductions_109_dparser_gram[] = {&d_reduction_64_dparser_gram};
D_Reduction *d_reductions_110_dparser_gram[] = {&d_reduction_65_dparser_gram};
D_Reduction *d_reductions_111_dparser_gram[] = {&d_reduction_48_dparser_gram};
D_Reduction *d_reductions_112_dparser_gram[] = {&d_reduction_80_dparser_gram};
D_Reduction *d_reductions_113_dparser_gram[] = {&d_reduction_81_dparser_gram};
D_Reduction *d_reductions_114_dparser_gram[] = {&d_reduction_82_dparser_gram};
D_Reduction *d_reductions_115_dparser_gram[] = {&d_reduction_83_dparser_gram};
D_Reduction *d_reductions_116_dparser_gram[] = {&d_reduction_84_dparser_gram};
D_Reduction *d_reductions_117_dparser_gram[] = {&d_reduction_85_dparser_gram};
D_Reduction *d_reductions_118_dparser_gram[] = {&d_reduction_86_dparser_gram};
D_Reduction *d_reductions_119_dparser_gram[] = {&d_reduction_87_dparser_gram};
D_Reduction *d_reductions_120_dparser_gram[] = {&d_reduction_88_dparser_gram};
D_Reduction *d_reductions_121_dparser_gram[] = {&d_reduction_89_dparser_gram};
D_Reduction *d_reductions_122_dparser_gram[] = {&d_reduction_50_dparser_gram};
unsigned char d_goto_valid_123_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x3c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0};
D_Reduction *d_reductions_124_dparser_gram[] = {&d_reduction_49_dparser_gram};
unsigned char d_goto_valid_125_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x52, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0};
D_Reduction *d_reductions_125_dparser_gram[] = {&d_reduction_95_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_125_dparser_gram[] = {{ 1, 151, &d_reduction_91_dparser_gram}};
D_Reduction *d_reductions_126_dparser_gram[] = {&d_reduction_96_dparser_gram};
D_Reduction *d_reductions_127_dparser_gram[] = {&d_reduction_98_dparser_gram};
unsigned char d_goto_valid_128_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0xbf, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x2, 0xfe};
D_Reduction *d_reductions_129_dparser_gram[] = {&d_reduction_69_dparser_gram};
unsigned char d_goto_valid_130_dparser_gram[] = {
0x0, 0x0, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_130_dparser_gram[] = {&d_reduction_62_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_130_dparser_gram[] = {{ 1, 92, &d_reduction_45_dparser_gram}, { 3, 124, &d_reduction_49_dparser_gram}};
unsigned char d_goto_valid_131_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4, 0x3f, 0x0, 0x0, 0x0, 0x20, 0x0, 0x80, 0x6, 0xfe};
unsigned char d_goto_valid_132_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xf, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_132_dparser_gram[] = {&d_reduction_54_dparser_gram};
unsigned char d_goto_valid_133_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xf, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_133_dparser_gram[] = {&d_reduction_59_dparser_gram};
D_Reduction *d_reductions_134_dparser_gram[] = {&d_reduction_79_dparser_gram};
D_Reduction *d_reductions_135_dparser_gram[] = {&d_reduction_90_dparser_gram};
unsigned char d_goto_valid_136_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_136_dparser_gram[] = {&d_reduction_93_dparser_gram};
D_RightEpsilonHint d_right_epsilon_hints_136_dparser_gram[] = {{ 0, 151, &d_reduction_91_dparser_gram}};
D_Reduction *d_reductions_137_dparser_gram[] = {&d_reduction_94_dparser_gram};
D_Reduction *d_reductions_138_dparser_gram[] = {&d_reduction_99_dparser_gram};
D_Reduction *d_reductions_139_dparser_gram[] = {&d_reduction_66_dparser_gram};
D_Reduction *d_reductions_140_dparser_gram[] = {&d_reduction_68_dparser_gram};
unsigned char d_goto_valid_141_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 0x0};
D_Reduction *d_reductions_142_dparser_gram[] = {&d_reduction_104_dparser_gram};
D_Reduction *d_reductions_143_dparser_gram[] = {&d_reduction_105_dparser_gram};
unsigned char d_goto_valid_144_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0};
D_Reduction *d_reductions_145_dparser_gram[] = {&d_reduction_75_dparser_gram};
D_Reduction *d_reductions_146_dparser_gram[] = {&d_reduction_76_dparser_gram};
D_Reduction *d_reductions_147_dparser_gram[] = {&d_reduction_77_dparser_gram};
D_Reduction *d_reductions_148_dparser_gram[] = {&d_reduction_78_dparser_gram};
D_Reduction *d_reductions_149_dparser_gram[] = {&d_reduction_55_dparser_gram};
D_Reduction *d_reductions_150_dparser_gram[] = {&d_reduction_60_dparser_gram};
unsigned char d_goto_valid_151_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10};
D_Reduction *d_reductions_151_dparser_gram[] = {&d_reduction_91_dparser_gram};
D_Reduction *d_reductions_152_dparser_gram[] = {&d_reduction_67_dparser_gram};
D_Reduction *d_reductions_153_dparser_gram[] = {&d_reduction_74_dparser_gram};
D_Reduction *d_reductions_154_dparser_gram[] = {&d_reduction_92_dparser_gram};
unsigned char d_goto_valid_155_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char d_goto_valid_156_dparser_gram[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0};
D_Reduction *d_reductions_157_dparser_gram[] = {&d_reduction_100_dparser_gram};
unsigned short d_gotos_dparser_gram[547] = {
2,3,14,15,31,4,16,49,54,50,56,53,60,55,17,51,
18,19,59,61,62,65,78,90,17,79,18,19,92,89,93,81,
97,80,79,88,67,99,100,73,74,131,80,132,32,20,133,134,
33,34,35,36,37,38,39,40,21,152,52,94,95,5,96,6,
153,7,21,8,9,157,41,42,43,44,45,46,47,48,13,52,
142,112,0,79,10,66,57,22,11,80,58,80,137,0,10,138,
13,139,11,68,69,70,71,72,158,23,98,24,0,77,12,0,
0,0,0,25,26,27,13,28,29,30,0,13,150,13,0,0,
0,0,64,0,155,13,33,34,35,36,37,38,39,40,0,63,
156,0,13,76,20,0,0,33,34,35,36,37,38,39,40,0,
75,0,12,0,5,0,6,151,7,12,8,9,83,22,0,0,
33,34,35,36,37,38,39,40,145,146,147,148,149,0,22,23,
0,24,0,0,0,154,38,39,40,25,26,27,13,28,29,30,
23,0,24,13,0,12,0,22,82,135,25,26,27,13,28,29,
30,91,74,145,146,147,148,149,0,23,0,24,0,136,38,39,
40,0,0,25,26,27,13,28,29,30,85,0,0,0,33,34,
35,36,37,38,39,40,0,84,28,29,30,87,0,0,0,33,
34,35,36,37,38,39,40,0,0,0,0,0,0,68,69,70,
71,72,0,0,0,22,0,123,124,0,125,0,0,126,127,0,
28,29,30,128,0,0,22,23,0,24,0,0,0,0,0,0,
0,25,26,27,13,28,29,30,23,104,24,86,105,0,106,0,
0,0,25,26,27,13,28,29,30,0,0,107,0,108,0,0,
129,0,0,0,109,110,111,113,114,115,116,117,118,119,120,121,
122,0,130,0,103,0,33,34,35,36,37,38,39,40,0,0,
0,0,0,0,0,0,0,141,101,0,102,33,34,35,36,37,
38,39,40,0,140,0,0,0,0,0,0,0,12,22,103,0,
0,0,0,0,0,0,0,26,27,13,0,0,0,0,0,23,
0,24,22,0,0,0,0,0,0,25,26,27,13,28,29,30,
0,0,0,0,23,0,24,0,0,0,0,0,0,0,25,26,
27,13,28,29,30,144,0,0,0,33,34,35,36,37,38,39,
40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,23,0,24,143,0,0,0,0,0,0,25,26,27,13,
28,29,30};

D_ErrorRecoveryHint d_error_recovery_hints_3_dparser_gram[] = {
{ 0, 7, "}"},
{ 0, 15, ";"}};
D_ErrorRecoveryHint d_error_recovery_hints_4_dparser_gram[] = {
{ 0, 7, "}"},
{ 0, 50, ")"},
{ 0, 50, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_5_dparser_gram[] = {{ 1, 7, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_11_dparser_gram[] = {{ 1, 46, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_13_dparser_gram[] = {{ 0, 15, ";"}};
D_ErrorRecoveryHint d_error_recovery_hints_17_dparser_gram[] = {{ 1, 15, ";"}};
D_ErrorRecoveryHint d_error_recovery_hints_21_dparser_gram[] = {{ 1, 50, ")"}};
D_ErrorRecoveryHint d_error_recovery_hints_22_dparser_gram[] = {{ 1, 50, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_23_dparser_gram[] = {{ 1, 50, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_48_dparser_gram[] = {{ 2, 7, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_53_dparser_gram[] = {
{ 0, 46, "}"},
{ 0, 50, ")"},
{ 0, 50, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_55_dparser_gram[] = {{ 0, 7, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_56_dparser_gram[] = {{ 2, 15, ";"}};
D_ErrorRecoveryHint d_error_recovery_hints_59_dparser_gram[] = {
{ 0, 50, ")"},
{ 0, 50, "]"},
{ 0, 50, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_62_dparser_gram[] = {{ 3, 7, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_74_dparser_gram[] = {{ 3, 46, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_77_dparser_gram[] = {{ 3, 15, ";"}};
D_ErrorRecoveryHint d_error_recovery_hints_81_dparser_gram[] = {{ 3, 50, ")"}};
D_ErrorRecoveryHint d_error_recovery_hints_83_dparser_gram[] = {{ 3, 50, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_85_dparser_gram[] = {{ 3, 50, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_87_dparser_gram[] = {{ 4, 7, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_91_dparser_gram[] = {{ 4, 15, ";"}};
D_ErrorRecoveryHint d_error_recovery_hints_95_dparser_gram[] = {
{ 0, 31, "}"},
{ 0, 31, ")"},
{ 0, 48, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_99_dparser_gram[] = {{ 0, 48, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_100_dparser_gram[] = {
{ 0, 31, "}"},
{ 0, 50, ")"},
{ 0, 50, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_101_dparser_gram[] = {{ 1, 31, ")"}};
D_ErrorRecoveryHint d_error_recovery_hints_102_dparser_gram[] = {{ 1, 48, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_125_dparser_gram[] = {{ 0, 46, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_130_dparser_gram[] = {{ 2, 31, ")"}};
D_ErrorRecoveryHint d_error_recovery_hints_131_dparser_gram[] = {
{ 0, 48, "]"},
{ 0, 50, ")"},
{ 0, 50, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_139_dparser_gram[] = {{ 3, 31, "}"}};
D_ErrorRecoveryHint d_error_recovery_hints_141_dparser_gram[] = {{ 3, 31, ")"}};
D_ErrorRecoveryHint d_error_recovery_hints_142_dparser_gram[] = {{ 3, 48, "]"}};
D_ErrorRecoveryHint d_error_recovery_hints_152_dparser_gram[] = {{ 4, 31, ")"}};

D_State d_states_dparser_gram[] = {
{ d_goto_valid_0_dparser_gram, 1, { 1, d_reductions_0_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 0, NULL}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 1, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_2_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_3_dparser_gram, 1, { 0, NULL}, { 0, NULL}, { 2, d_error_recovery_hints_3_dparser_gram}, d_shifts_3_dparser_gram, NULL, (void*)d_scanner_3_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_3_dparser_gram, d_accepts_diff_3_dparser_gram, -1},
{ d_goto_valid_4_dparser_gram, 6, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_4_dparser_gram}, d_shifts_4_dparser_gram, NULL, (void*)d_scanner_4_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_4_dparser_gram, d_accepts_diff_4_dparser_gram, -1},
{ d_goto_valid_5_dparser_gram, 6, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_5_dparser_gram}, d_shifts_5_dparser_gram, NULL, (void*)d_scanner_5_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_5_dparser_gram, d_accepts_diff_5_dparser_gram, -1},
{ d_goto_valid_6_dparser_gram, -1, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_5_dparser_gram}, d_shifts_6_dparser_gram, NULL, (void*)d_scanner_6_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_6_dparser_gram, d_accepts_diff_6_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_7_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_8_dparser_gram, 46, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_5_dparser_gram}, d_shifts_6_dparser_gram, NULL, (void*)d_scanner_6_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_6_dparser_gram, d_accepts_diff_6_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_9_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_10_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_11_dparser_gram, 39, { 1, d_reductions_11_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_11_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_12_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_13_dparser_gram, -9, { 1, d_reductions_13_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_13_dparser_gram}, d_shifts_13_dparser_gram, NULL, (void*)d_scanner_13_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_13_dparser_gram, d_accepts_diff_13_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_14_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_15_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_16_dparser_gram, -5, { 1, d_reductions_16_dparser_gram}, { 1, d_right_epsilon_hints_16_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_17_dparser_gram, -2, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_17_dparser_gram}, d_shifts_17_dparser_gram, NULL, (void*)d_scanner_17_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_17_dparser_gram, d_accepts_diff_17_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_18_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_19_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_20_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_21_dparser_gram, 41, { 1, d_reductions_21_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_21_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_22_dparser_gram, 32, { 1, d_reductions_22_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_22_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_23_dparser_gram, 32, { 1, d_reductions_23_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_23_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_24_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_25_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_26_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_27_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_28_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_29_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_30_dparser_gram, -80, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_4_dparser_gram}, d_shifts_30_dparser_gram, NULL, (void*)d_scanner_30_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_30_dparser_gram, d_accepts_diff_30_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_31_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_32_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_33_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_34_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_35_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_36_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_37_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_38_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_39_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_40_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_41_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_42_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_43_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_44_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_45_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_46_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_47_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_48_dparser_gram, -12, { 1, d_reductions_48_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_48_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_49_dparser_gram, -22, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_48_dparser_gram}, d_shifts_49_dparser_gram, NULL, (void*)d_scanner_49_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_49_dparser_gram, d_accepts_diff_49_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_50_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_51_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_52_dparser_gram, -28, { 1, d_reductions_52_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_48_dparser_gram}, d_shifts_52_dparser_gram, NULL, (void*)d_scanner_52_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_52_dparser_gram, d_accepts_diff_52_dparser_gram, -1},
{ d_goto_valid_53_dparser_gram, -97, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_53_dparser_gram}, d_shifts_30_dparser_gram, NULL, (void*)d_scanner_30_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_30_dparser_gram, d_accepts_diff_30_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_54_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_55_dparser_gram, -102, { 1, d_reductions_55_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_55_dparser_gram}, d_shifts_55_dparser_gram, NULL, (void*)d_scanner_55_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_55_dparser_gram, d_accepts_diff_55_dparser_gram, -1},
{ d_goto_valid_56_dparser_gram, -3, { 1, d_reductions_56_dparser_gram}, { 2, d_right_epsilon_hints_56_dparser_gram}, { 1, d_error_recovery_hints_56_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_57_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_58_dparser_gram, -12, { 1, d_reductions_58_dparser_gram}, { 2, d_right_epsilon_hints_58_dparser_gram}, { 1, d_error_recovery_hints_56_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_59_dparser_gram, -122, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_59_dparser_gram}, d_shifts_59_dparser_gram, NULL, (void*)d_scanner_59_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_59_dparser_gram, d_accepts_diff_59_dparser_gram, -1},
{ d_goto_valid_60_dparser_gram, -200, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_59_dparser_gram}, d_shifts_30_dparser_gram, NULL, (void*)d_scanner_30_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_30_dparser_gram, d_accepts_diff_30_dparser_gram, -1},
{ d_goto_valid_61_dparser_gram, -217, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_59_dparser_gram}, d_shifts_61_dparser_gram, NULL, (void*)d_scanner_61_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_61_dparser_gram, d_accepts_diff_61_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_62_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_62_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_63_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_64_dparser_gram, 28, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_62_dparser_gram}, d_shifts_49_dparser_gram, NULL, (void*)d_scanner_49_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_49_dparser_gram, d_accepts_diff_49_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_65_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_62_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_66_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_67_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_68_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_69_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_70_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_71_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_72_dparser_gram, 40, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_62_dparser_gram}, d_shifts_72_dparser_gram, NULL, (void*)d_scanner_72_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_72_dparser_gram, d_accepts_diff_72_dparser_gram, -1},
{ d_goto_valid_73_dparser_gram, -214, { 1, d_reductions_73_dparser_gram}, { 1, d_right_epsilon_hints_73_dparser_gram}, { 0, NULL}, d_shifts_52_dparser_gram, NULL, (void*)d_scanner_52_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_52_dparser_gram, d_accepts_diff_52_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_74_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_74_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_75_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_76_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_77_dparser_gram, 57, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_77_dparser_gram}, d_shifts_77_dparser_gram, NULL, (void*)d_scanner_77_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_77_dparser_gram, d_accepts_diff_77_dparser_gram, -1},
{ d_goto_valid_78_dparser_gram, -10, { 1, d_reductions_78_dparser_gram}, { 1, d_right_epsilon_hints_78_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_79_dparser_gram, -35, { 2, d_reductions_79_dparser_gram}, { 1, d_right_epsilon_hints_79_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_80_dparser_gram, 53, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_77_dparser_gram}, d_shifts_77_dparser_gram, NULL, (void*)d_scanner_77_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_77_dparser_gram, d_accepts_diff_77_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_81_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_81_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_82_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_83_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_83_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_84_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_85_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_85_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_86_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_87_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_87_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_88_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_89_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_87_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_90_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_91_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_91_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_92_dparser_gram, -16, { 1, d_reductions_92_dparser_gram}, { 0, NULL}, { 0, NULL}, d_shifts_92_dparser_gram, NULL, (void*)d_scanner_92_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_92_dparser_gram, d_accepts_diff_92_dparser_gram, -1},
{ d_goto_valid_93_dparser_gram, -15, { 1, d_reductions_93_dparser_gram}, { 1, d_right_epsilon_hints_93_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_94_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_95_dparser_gram, -301, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_95_dparser_gram}, d_shifts_95_dparser_gram, NULL, (void*)d_scanner_95_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_95_dparser_gram, d_accepts_diff_95_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_96_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_91_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_97_dparser_gram, -59, { 1, d_reductions_97_dparser_gram}, { 2, d_right_epsilon_hints_97_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_98_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_99_dparser_gram, -259, { 1, d_reductions_99_dparser_gram}, { 2, d_right_epsilon_hints_99_dparser_gram}, { 1, d_error_recovery_hints_99_dparser_gram}, d_shifts_99_dparser_gram, NULL, (void*)d_scanner_99_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_99_dparser_gram, d_accepts_diff_99_dparser_gram, -1},
{ d_goto_valid_100_dparser_gram, -320, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_100_dparser_gram}, d_shifts_4_dparser_gram, NULL, (void*)d_scanner_4_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_4_dparser_gram, d_accepts_diff_4_dparser_gram, -1},
{ d_goto_valid_101_dparser_gram, -7, { 1, d_reductions_101_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_101_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_102_dparser_gram, 6, { 1, d_reductions_102_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_102_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_103_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_104_dparser_gram, -20, { 2, d_reductions_104_dparser_gram}, { 1, d_right_epsilon_hints_104_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_105_dparser_gram, -18, { 1, d_reductions_105_dparser_gram}, { 1, d_right_epsilon_hints_105_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_106_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_107_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_108_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_109_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_110_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_111_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_112_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_113_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_114_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_115_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_116_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_117_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_118_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_119_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_120_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_121_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_122_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_123_dparser_gram, -179, { 0, NULL}, { 0, NULL}, { 0, NULL}, d_shifts_123_dparser_gram, NULL, (void*)d_scanner_123_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_123_dparser_gram, d_accepts_diff_123_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_124_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_125_dparser_gram, -51, { 1, d_reductions_125_dparser_gram}, { 1, d_right_epsilon_hints_125_dparser_gram}, { 1, d_error_recovery_hints_125_dparser_gram}, d_shifts_125_dparser_gram, NULL, (void*)d_scanner_125_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_125_dparser_gram, d_accepts_diff_125_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_126_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_127_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_128_dparser_gram, -341, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_100_dparser_gram}, d_shifts_30_dparser_gram, NULL, (void*)d_scanner_30_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_30_dparser_gram, d_accepts_diff_30_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_129_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_130_dparser_gram, -61, { 1, d_reductions_130_dparser_gram}, { 2, d_right_epsilon_hints_130_dparser_gram}, { 1, d_error_recovery_hints_130_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_131_dparser_gram, -419, { 0, NULL}, { 0, NULL}, { 3, d_error_recovery_hints_131_dparser_gram}, d_shifts_61_dparser_gram, NULL, (void*)d_scanner_61_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_61_dparser_gram, d_accepts_diff_61_dparser_gram, -1},
{ d_goto_valid_132_dparser_gram, -89, { 1, d_reductions_132_dparser_gram}, { 0, NULL}, { 0, NULL}, d_shifts_132_dparser_gram, NULL, (void*)d_scanner_132_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_132_dparser_gram, d_accepts_diff_132_dparser_gram, -1},
{ d_goto_valid_133_dparser_gram, -132, { 1, d_reductions_133_dparser_gram}, { 0, NULL}, { 0, NULL}, d_shifts_132_dparser_gram, NULL, (void*)d_scanner_132_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_132_dparser_gram, d_accepts_diff_132_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_134_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_135_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_136_dparser_gram, -17, { 1, d_reductions_136_dparser_gram}, { 1, d_right_epsilon_hints_136_dparser_gram}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_137_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_138_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_139_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_139_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_140_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_141_dparser_gram, 30, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_141_dparser_gram}, d_shifts_141_dparser_gram, NULL, (void*)d_scanner_141_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_141_dparser_gram, d_accepts_diff_141_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_142_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_142_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_143_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_144_dparser_gram, -139, { 0, NULL}, { 0, NULL}, { 0, NULL}, d_shifts_123_dparser_gram, NULL, (void*)d_scanner_123_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_123_dparser_gram, d_accepts_diff_123_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_145_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_146_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_147_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_148_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_149_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_150_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_151_dparser_gram, -87, { 1, d_reductions_151_dparser_gram}, { 0, NULL}, { 0, NULL}, d_shifts_6_dparser_gram, NULL, (void*)d_scanner_6_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_6_dparser_gram, d_accepts_diff_6_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_152_dparser_gram}, { 0, NULL}, { 1, d_error_recovery_hints_152_dparser_gram}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_153_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ NULL, -2147483647, { 1, d_reductions_154_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1},
{ d_goto_valid_155_dparser_gram, 15, { 0, NULL}, { 0, NULL}, { 0, NULL}, d_shifts_155_dparser_gram, NULL, (void*)d_scanner_155_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_155_dparser_gram, d_accepts_diff_155_dparser_gram, -1},
{ d_goto_valid_156_dparser_gram, -58, { 0, NULL}, { 0, NULL}, { 1, d_error_recovery_hints_125_dparser_gram}, d_shifts_125_dparser_gram, NULL, (void*)d_scanner_125_dparser_gram, sizeof(unsigned char), 0, D_SCAN_ALL, (void*)d_transition_125_dparser_gram, d_accepts_diff_125_dparser_gram, -1},
{ NULL, -2147483647, { 1, d_reductions_157_dparser_gram}, { 0, NULL}, { 0, NULL}, NULL, NULL, NULL, sizeof(unsigned char), 0, D_SCAN_ALL, NULL, (D_Shift***)NULL, -1}
};

D_Symbol d_symbols_dparser_gram[] = {
{D_SYMBOL_INTERNAL, "0 Start", 7},
{D_SYMBOL_INTERNAL, "1 Start", 7},
{D_SYMBOL_NTERM, "grammar", 7},
{D_SYMBOL_INTERNAL, "grammar.6", 9},
{D_SYMBOL_INTERNAL, "grammar.4", 9},
{D_SYMBOL_INTERNAL, "grammar.4.5", 11},
{D_SYMBOL_INTERNAL, "grammar.3", 9},
{D_SYMBOL_NTERM, "global_code", 11},
{D_SYMBOL_INTERNAL, "global_code.10", 14},
{D_SYMBOL_INTERNAL, "global_code.9", 13},
{D_SYMBOL_INTERNAL, "global_code.8", 13},
{D_SYMBOL_NTERM, "pass_types", 10},
{D_SYMBOL_NTERM, "pass_type", 9},
{D_SYMBOL_NTERM, "declarationtype", 15},
{D_SYMBOL_NTERM, "token_identifier", 16},
{D_SYMBOL_NTERM, "production", 10},
{D_SYMBOL_NTERM, "regex_production", 16},
{D_SYMBOL_NTERM, "production_name", 15},
{D_SYMBOL_INTERNAL, "production_name.18", 18},
{D_SYMBOL_NTERM, "rules", 5},
{D_SYMBOL_INTERNAL, "rules.21", 8},
{D_SYMBOL_INTERNAL, "rules.20", 8},
{D_SYMBOL_NTERM, "rule", 4},
{D_SYMBOL_INTERNAL, "rule.29", 7},
{D_SYMBOL_INTERNAL, "rule.28", 7},
{D_SYMBOL_INTERNAL, "rule.23", 7},
{D_SYMBOL_INTERNAL, "rule.23.27", 10},
{D_SYMBOL_INTERNAL, "rule.23.26", 10},
{D_SYMBOL_INTERNAL, "rule.23.24", 10},
{D_SYMBOL_INTERNAL, "rule.23.24.25", 13},
{D_SYMBOL_NTERM, "new_rule", 8},
{D_SYMBOL_NTERM, "simple_element", 14},
{D_SYMBOL_INTERNAL, "simple_element.32", 17},
{D_SYMBOL_NTERM, "element", 7},
{D_SYMBOL_NTERM, "new_subrule", 11},
{D_SYMBOL_NTERM, "element_modifier", 16},
{D_SYMBOL_NTERM, "rule_modifier", 13},
{D_SYMBOL_NTERM, "rule_assoc", 10},
{D_SYMBOL_NTERM, "rule_priority", 13},
{D_SYMBOL_NTERM, "rule_code", 9},
{D_SYMBOL_INTERNAL, "rule_code.42", 12},
{D_SYMBOL_INTERNAL, "rule_code.41", 12},
{D_SYMBOL_INTERNAL, "rule_code.40", 12},
{D_SYMBOL_NTERM, "speculative_code", 16},
{D_SYMBOL_NTERM, "final_code", 10},
{D_SYMBOL_NTERM, "pass_code", 9},
{D_SYMBOL_NTERM, "curly_code", 10},
{D_SYMBOL_INTERNAL, "curly_code.47", 13},
{D_SYMBOL_NTERM, "bracket_code", 12},
{D_SYMBOL_INTERNAL, "bracket_code.49", 15},
{D_SYMBOL_NTERM, "balanced_code", 13},
{D_SYMBOL_INTERNAL, "balanced_code.53", 16},
{D_SYMBOL_INTERNAL, "balanced_code.52", 16},
{D_SYMBOL_INTERNAL, "balanced_code.51", 16},
{D_SYMBOL_NTERM, "symbols", 7},
{D_SYMBOL_NTERM, "string", 6},
{D_SYMBOL_NTERM, "regex", 5},
{D_SYMBOL_NTERM, "identifier", 10},
{D_SYMBOL_NTERM, "integer", 7},
{D_SYMBOL_NTERM, "decimalint", 10},
{D_SYMBOL_NTERM, "hexint", 6},
{D_SYMBOL_NTERM, "octalint", 8},
{D_SYMBOL_STRING, "${scanner", 9},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_STRING, "${declare", 9},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_STRING, "${token", 7},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_STRING, "${action}", 9},
{D_SYMBOL_STRING, "${pass", 6},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_STRING, "preorder", 8},
{D_SYMBOL_STRING, "postorder", 9},
{D_SYMBOL_STRING, "manual", 6},
{D_SYMBOL_STRING, "for_all", 7},
{D_SYMBOL_STRING, "for_undefined", 13},
{D_SYMBOL_STRING, "tokenize", 8},
{D_SYMBOL_STRING, "longest_match", 13},
{D_SYMBOL_STRING, "whitespace", 10},
{D_SYMBOL_STRING, "all_matches", 11},
{D_SYMBOL_STRING, "set_op_priority_from_rule", 25},
{D_SYMBOL_STRING, "all_subparsers", 14},
{D_SYMBOL_STRING, "subparser", 9},
{D_SYMBOL_STRING, "save_parse_tree", 15},
{D_SYMBOL_STRING, ":", 1},
{D_SYMBOL_STRING, ";", 1},
{D_SYMBOL_STRING, ";", 1},
{D_SYMBOL_STRING, ";", 1},
{D_SYMBOL_STRING, "::=", 3},
{D_SYMBOL_STRING, "_", 1},
{D_SYMBOL_STRING, "|", 1},
{D_SYMBOL_STRING, "${scan", 6},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_STRING, "(", 1},
{D_SYMBOL_STRING, ")", 1},
{D_SYMBOL_STRING, "$term", 5},
{D_SYMBOL_STRING, "/i", 2},
{D_SYMBOL_STRING, "?", 1},
{D_SYMBOL_STRING, "*", 1},
{D_SYMBOL_STRING, "+", 1},
{D_SYMBOL_STRING, "$unary_op_right", 15},
{D_SYMBOL_STRING, "$unary_op_left", 14},
{D_SYMBOL_STRING, "$binary_op_right", 16},
{D_SYMBOL_STRING, "$binary_op_left", 15},
{D_SYMBOL_STRING, "$unary_right", 12},
{D_SYMBOL_STRING, "$unary_left", 11},
{D_SYMBOL_STRING, "$binary_right", 13},
{D_SYMBOL_STRING, "$binary_left", 12},
{D_SYMBOL_STRING, "$right", 6},
{D_SYMBOL_STRING, "$left", 5},
{D_SYMBOL_STRING, ":", 1},
{D_SYMBOL_STRING, "{", 1},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_STRING, "[", 1},
{D_SYMBOL_STRING, "]", 1},
{D_SYMBOL_STRING, "(", 1},
{D_SYMBOL_STRING, ")", 1},
{D_SYMBOL_STRING, "[", 1},
{D_SYMBOL_STRING, "]", 1},
{D_SYMBOL_STRING, "{", 1},
{D_SYMBOL_STRING, "}", 1},
{D_SYMBOL_REGEX, "[!~`@#$%^&*\\\\-_+=|:;\\\\\\\\<,>.?/]", 37},
{D_SYMBOL_REGEX, "'([^'\\\\\\\\]|\\\\\\\\[^])*'", 29},
{D_SYMBOL_REGEX, "\\\"([^\\\"\\\\\\\\]|\\\\\\\\[^])*\\\"", 38},
{D_SYMBOL_REGEX, "[a-zA-Z_][a-zA-Z_0-9]*", 22},
{D_SYMBOL_REGEX, "-?[1-9][0-9]*[uUlL]?", 20},
{D_SYMBOL_REGEX, "-?(0x|0X)[0-9a-fA-F]+[uUlL]?", 28},
{D_SYMBOL_REGEX, "-?0[0-7]*[uUlL]?", 16},
};

D_ParserTables parser_tables_dparser_gram = {
158, d_states_dparser_gram, d_gotos_dparser_gram, 0, 128, d_symbols_dparser_gram, NULL, 0, NULL, 0};
