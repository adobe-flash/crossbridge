/* translation of file(s)
	"abs.k"
	"main.k"
	"parse.k"
	"error.k"
	"occur.k"
	"util.k"
	"gen.k"
	"gutil.k"
	"pat.k"
 */
/* generated by:
 *  @(#)$Author: criswell $
 */
#ifndef KC_UNPARSE_HEADER
#define KC_UNPARSE_HEADER

namespace kc { }
using namespace kc;
/* included stuff */
//
// The Termprocessor Kimwitu++
//
// Copyright (C) 1991 University of Twente, Dept TIOS.
// Copyright (C) 1998-2003 Humboldt-University of Berlin, Institute of Informatics
// All rights reserved.
//
// Kimwitu++ is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Kimwitu++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Kimwitu++; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/* end included stuff */


namespace kc {

typedef enum {
    base_uview_enum,
    view_error_format_enum,
    view_filename_enum,
    view_error_enum,
    view_error_tID_enum,
    view_check_enum,
    view_check_count_enum,
    view_check_count1_enum,
    view_check_outmostopers_in_phylum_enum,
    view_set_type_enum,
    view_check_is_var_enum,
    view_gen_initializephyla_c_enum,
    view_check_u_enum,
    view_check_r_enum,
    view_check_viewnames_enum,
    view_check_uniq_enum,
    view_check_uniq1_enum,
    view_check_uniq2_enum,
    view_make_patternreps_enum,
    view_printer_outputfileline_enum,
    view_printer_reset_enum,
    view_no_of_printed_string_chars_reset_enum,
    view_open_namespace_enum,
    view_close_namespace_enum,
    view_gen_k_h_enum,
    view_gen_end_k_h_enum,
    view_gen_k_c_enum,
    view_gen_alloc_h_enum,
    view_gen_alloc_c_enum,
    view_gen_deprecated_enum,
    view_gen_enumphyla_h_enum,
    view_gen_enumoperators_h_enum,
    view_gen_operatormap_type_h_enum,
    view_gen_phylummap_c_enum,
    view_gen_operatormap_c_enum,
    view_count_args_enum,
    view_count_attrs_enum,
    view_count_nonstaticmembers_enum,
    view_gen_operatormap_subphyla_enum,
    view_gen_operatormap_attributes_enum,
    view_gen_uniqmap_c_enum,
    view_gen_uniqmap_c_1_enum,
    view_gen_uniqmap_c_2_enum,
    view_gen_nodetypedefs_h_enum,
    view_gen_nodetypes_h_enum,
    view_gen_yaccstacktype_h_enum,
    view_gen_noofoperators_h_enum,
    view_gen_assertmacros_h_enum,
    view_gen_assertmacros_c_enum,
    view_gen_operatordecls_h_enum,
    view_gen_operatorcast_h_enum,
    view_gen_operatordefs_c_enum,
    view_gen_operatordefs_c_0_enum,
    view_gen_operatordefs_c_1_enum,
    view_gen_operatordefs_c_2_enum,
    view_gen_initializephyla_whiletest_c_enum,
    view_gen_initializephyla_init_el_c_enum,
    view_gen_initializephyla_update_loop_c_enum,
    view_gen_operatordefs_nonhash_c_enum,
    view_gen_operatordefs_hash_c_enum,
    view_gen_access_functions_enum,
    view_gen_create_function_enum,
    view_gen_attributeOf_function_enum,
    view_gen_argseqnr_enum,
    view_gen_fnarg_asserts_enum,
    view_gen_fnarg_and_decls_enum,
    view_gen_fnarg_and_decls_predef_enum,
    view_gen_assignments_enum,
    view_gen_assignment_inis_enum,
    view_gen_user_assignment_inis_enum,
    view_gen_assignments_predef_enum,
    view_gen_assignments_predef_ini_enum,
    view_gen_test_enum,
    view_gen_error_decls_h_enum,
    view_gen_error_defs_c_enum,
    view_gen_printdotdecls_h_enum,
    view_gen_printdotdefs_c_enum,
    view_gen_printdotedges_c_enum,
    view_gen_listdecls_h_enum,
    view_gen_listdefs_c_enum,
    view_gen_includes_enum,
    view_do_gen_includes_enum,
    view_gen_csgio_start_h_enum,
    view_gen_csgio_end_h_enum,
    view_gen_csgio_h_enum,
    view_gen_csgio_start_c_enum,
    view_gen_csgio_c_enum,
    view_gen_copy_attributes_c_enum,
    view_gen_copydefs_c_enum,
    view_gen_rewritek_h_enum,
    view_gen_end_rewritek_h_enum,
    view_gen_rewritek_c_enum,
    view_gen_rewritedefs_c_enum,
    view_gen_rewritedefs_default_c_enum,
    view_gen_rewritedefs_other_c_enum,
    view_gen_rewritedefs_rewritearg_c_enum,
    view_gen_rewritedefs_nl_arg_c_enum,
    view_gen_rewritedefs_testarg_c_enum,
    view_gen_rewritedefs_dotestarg_c_enum,
    view_gen_rewritedefs_args_c_enum,
    view_gen_rewritedefs_body_c_enum,
    view_gen_withcases_and_default_enum,
    view_rw_predicates_enum,
    view_wc_predicates_enum,
    view_unp_predicates_enum,
    view_wc_bindings_enum,
    view_unp_bindings_enum,
    view_rw_bindings_enum,
    view_gen_fnk_h_enum,
    view_gen_fnkdecls_c_enum,
    view_gen_fnk_c_enum,
    view_gen_fn_pointer_name_enum,
    view_gen_fnkargs_enum,
    view_gen_fnkdecls_enum,
    view_gen_fns_start_h_enum,
    view_gen_fns_end_h_enum,
    view_gen_fns_start_c_enum,
    view_gen_fns_owninclude_c_enum,
    view_gen_unpk_h_enum,
    view_gen_unparsedecls_h_enum,
    view_gen_rewritedecls_h_enum,
    view_uview_def_enum,
    view_gen_end_unpk_h_enum,
    view_gen_unpk_c_enum,
    view_gen_default_types_unpk_c_enum,
    view_gen_unparsedefs_c_enum,
    view_gen_unparsedefs_default_c_enum,
    view_gen_unparsedefs_other_c_enum,
    view_gen_unpstr_c_enum,
    view_gen_user_predicates_enum,
    view_predicate_bindings_enum,
    view_checklanguagenames_enum,
    view_output_collection_enum,
    view_gen_classdecls1_h_enum,
    view_gen_classdecls2_h_enum,
    view_gen_classdefs_c_enum,
    view_gen_subphylumdefs_c_enum,
    view_gen_cast_enum,
    view_gen_hashtables_h_enum,
    view_gen_hashtables_c_enum,
    view_gen_yxx_union_h_enum,
    view_gen_member_dcl_h_enum,
    view_gen_set_subphylumdefs_c_enum,
    view_gen_viewvars_c_enum,
    view_gen_argseqnr_rec_enum,
    view_gen_opt_const_enum,
    view_uview_class_decl_enum,
    view_uview_class_def_enum,
    view_rview_class_decl_enum,
    view_rview_class_def_enum,
    view_collect_strings_enum,
    view_class_of_op_enum,
    view_class_of_phy_enum,
    last_uview
} uview_enum;

struct impl_uviews {
    const char *name;
    uview_class *view;
};
extern impl_uviews uviews[];
class uview_class {
protected:
    // only used in derivations
    uview_class(uview_enum v): m_view(v) { }
    uview_class(c_uview): m_view(base_uview_enum)
	{ /* do not copy m_view */ }
public:
    const char* name() const
	{ return uviews[m_view].name; }
    operator uview_enum() const
	{ return m_view; }
    bool operator==(const uview_class& other) const
	{ return m_view == other.m_view; }
private:
    uview_enum m_view;
};

class printer_functor_class {
public:
    virtual void operator()(const kc_char*, uview) { }
    virtual ~printer_functor_class() { }
};

class printer_functor_function_wrapper : public printer_functor_class {
public:
    printer_functor_function_wrapper(const printer_function opf =0): m_old_printer(opf) { }
    virtual ~printer_functor_function_wrapper() { }
    virtual void operator()(const kc_char* s, uview v)
	{ if(m_old_printer) m_old_printer(s, v); }
private:
    printer_function m_old_printer;
};

/* Use uviews instead
extern char *kc_view_names[];
*/
struct base_uview_class: uview_class {
    base_uview_class():uview_class(base_uview_enum){}
};
extern base_uview_class base_uview;
class view_error_format_baseclass: public uview_class {
protected:
    view_error_format_baseclass():uview_class(view_error_format_enum){}
};
// class view_error_format_class is defined externally
struct view_filename_class: uview_class {
    view_filename_class():uview_class(view_filename_enum){}
};
extern view_filename_class view_filename;
struct view_error_class: uview_class {
    view_error_class():uview_class(view_error_enum){}
};
extern view_error_class view_error;
struct view_error_tID_class: uview_class {
    view_error_tID_class():uview_class(view_error_tID_enum){}
};
extern view_error_tID_class view_error_tID;
struct view_check_class: uview_class {
    view_check_class():uview_class(view_check_enum){}
};
extern view_check_class view_check;
struct view_check_count_class: uview_class {
    view_check_count_class():uview_class(view_check_count_enum){}
};
extern view_check_count_class view_check_count;
struct view_check_count1_class: uview_class {
    view_check_count1_class():uview_class(view_check_count1_enum){}
};
extern view_check_count1_class view_check_count1;
struct view_check_outmostopers_in_phylum_class: uview_class {
    view_check_outmostopers_in_phylum_class():uview_class(view_check_outmostopers_in_phylum_enum){}
};
extern view_check_outmostopers_in_phylum_class view_check_outmostopers_in_phylum;
struct view_set_type_class: uview_class {
    view_set_type_class():uview_class(view_set_type_enum){}
};
extern view_set_type_class view_set_type;
struct view_check_is_var_class: uview_class {
    view_check_is_var_class():uview_class(view_check_is_var_enum){}
};
extern view_check_is_var_class view_check_is_var;
struct view_gen_initializephyla_c_class: uview_class {
    view_gen_initializephyla_c_class():uview_class(view_gen_initializephyla_c_enum){}
};
extern view_gen_initializephyla_c_class view_gen_initializephyla_c;
struct view_check_u_class: uview_class {
    view_check_u_class():uview_class(view_check_u_enum){}
};
extern view_check_u_class view_check_u;
struct view_check_r_class: uview_class {
    view_check_r_class():uview_class(view_check_r_enum){}
};
extern view_check_r_class view_check_r;
struct view_check_viewnames_class: uview_class {
    view_check_viewnames_class():uview_class(view_check_viewnames_enum){}
};
extern view_check_viewnames_class view_check_viewnames;
struct view_check_uniq_class: uview_class {
    view_check_uniq_class():uview_class(view_check_uniq_enum){}
};
extern view_check_uniq_class view_check_uniq;
struct view_check_uniq1_class: uview_class {
    view_check_uniq1_class():uview_class(view_check_uniq1_enum){}
};
extern view_check_uniq1_class view_check_uniq1;
struct view_check_uniq2_class: uview_class {
    view_check_uniq2_class():uview_class(view_check_uniq2_enum){}
};
extern view_check_uniq2_class view_check_uniq2;
struct view_make_patternreps_class: uview_class {
    view_make_patternreps_class():uview_class(view_make_patternreps_enum){}
};
extern view_make_patternreps_class view_make_patternreps;
struct view_printer_outputfileline_class: uview_class {
    view_printer_outputfileline_class():uview_class(view_printer_outputfileline_enum){}
};
extern view_printer_outputfileline_class view_printer_outputfileline;
struct view_printer_reset_class: uview_class {
    view_printer_reset_class():uview_class(view_printer_reset_enum){}
};
extern view_printer_reset_class view_printer_reset;
struct view_no_of_printed_string_chars_reset_class: uview_class {
    view_no_of_printed_string_chars_reset_class():uview_class(view_no_of_printed_string_chars_reset_enum){}
};
extern view_no_of_printed_string_chars_reset_class view_no_of_printed_string_chars_reset;
struct view_open_namespace_class: uview_class {
    view_open_namespace_class():uview_class(view_open_namespace_enum){}
};
extern view_open_namespace_class view_open_namespace;
struct view_close_namespace_class: uview_class {
    view_close_namespace_class():uview_class(view_close_namespace_enum){}
};
extern view_close_namespace_class view_close_namespace;
struct view_gen_k_h_class: uview_class {
    view_gen_k_h_class():uview_class(view_gen_k_h_enum){}
};
extern view_gen_k_h_class view_gen_k_h;
struct view_gen_end_k_h_class: uview_class {
    view_gen_end_k_h_class():uview_class(view_gen_end_k_h_enum){}
};
extern view_gen_end_k_h_class view_gen_end_k_h;
struct view_gen_k_c_class: uview_class {
    view_gen_k_c_class():uview_class(view_gen_k_c_enum){}
};
extern view_gen_k_c_class view_gen_k_c;
struct view_gen_alloc_h_class: uview_class {
    view_gen_alloc_h_class():uview_class(view_gen_alloc_h_enum){}
};
extern view_gen_alloc_h_class view_gen_alloc_h;
struct view_gen_alloc_c_class: uview_class {
    view_gen_alloc_c_class():uview_class(view_gen_alloc_c_enum){}
};
extern view_gen_alloc_c_class view_gen_alloc_c;
struct view_gen_deprecated_class: uview_class {
    view_gen_deprecated_class():uview_class(view_gen_deprecated_enum){}
};
extern view_gen_deprecated_class view_gen_deprecated;
struct view_gen_enumphyla_h_class: uview_class {
    view_gen_enumphyla_h_class():uview_class(view_gen_enumphyla_h_enum){}
};
extern view_gen_enumphyla_h_class view_gen_enumphyla_h;
struct view_gen_enumoperators_h_class: uview_class {
    view_gen_enumoperators_h_class():uview_class(view_gen_enumoperators_h_enum){}
};
extern view_gen_enumoperators_h_class view_gen_enumoperators_h;
struct view_gen_operatormap_type_h_class: uview_class {
    view_gen_operatormap_type_h_class():uview_class(view_gen_operatormap_type_h_enum){}
};
extern view_gen_operatormap_type_h_class view_gen_operatormap_type_h;
struct view_gen_phylummap_c_class: uview_class {
    view_gen_phylummap_c_class():uview_class(view_gen_phylummap_c_enum){}
};
extern view_gen_phylummap_c_class view_gen_phylummap_c;
struct view_gen_operatormap_c_class: uview_class {
    view_gen_operatormap_c_class():uview_class(view_gen_operatormap_c_enum){}
};
extern view_gen_operatormap_c_class view_gen_operatormap_c;
struct view_count_args_class: uview_class {
    view_count_args_class():uview_class(view_count_args_enum){}
};
extern view_count_args_class view_count_args;
struct view_count_attrs_class: uview_class {
    view_count_attrs_class():uview_class(view_count_attrs_enum){}
};
extern view_count_attrs_class view_count_attrs;
struct view_count_nonstaticmembers_class: uview_class {
    view_count_nonstaticmembers_class():uview_class(view_count_nonstaticmembers_enum){}
};
extern view_count_nonstaticmembers_class view_count_nonstaticmembers;
struct view_gen_operatormap_subphyla_class: uview_class {
    view_gen_operatormap_subphyla_class():uview_class(view_gen_operatormap_subphyla_enum){}
};
extern view_gen_operatormap_subphyla_class view_gen_operatormap_subphyla;
struct view_gen_operatormap_attributes_class: uview_class {
    view_gen_operatormap_attributes_class():uview_class(view_gen_operatormap_attributes_enum){}
};
extern view_gen_operatormap_attributes_class view_gen_operatormap_attributes;
struct view_gen_uniqmap_c_class: uview_class {
    view_gen_uniqmap_c_class():uview_class(view_gen_uniqmap_c_enum){}
};
extern view_gen_uniqmap_c_class view_gen_uniqmap_c;
struct view_gen_uniqmap_c_1_class: uview_class {
    view_gen_uniqmap_c_1_class():uview_class(view_gen_uniqmap_c_1_enum){}
};
extern view_gen_uniqmap_c_1_class view_gen_uniqmap_c_1;
struct view_gen_uniqmap_c_2_class: uview_class {
    view_gen_uniqmap_c_2_class():uview_class(view_gen_uniqmap_c_2_enum){}
};
extern view_gen_uniqmap_c_2_class view_gen_uniqmap_c_2;
struct view_gen_nodetypedefs_h_class: uview_class {
    view_gen_nodetypedefs_h_class():uview_class(view_gen_nodetypedefs_h_enum){}
};
extern view_gen_nodetypedefs_h_class view_gen_nodetypedefs_h;
struct view_gen_nodetypes_h_class: uview_class {
    view_gen_nodetypes_h_class():uview_class(view_gen_nodetypes_h_enum){}
};
extern view_gen_nodetypes_h_class view_gen_nodetypes_h;
struct view_gen_yaccstacktype_h_class: uview_class {
    view_gen_yaccstacktype_h_class():uview_class(view_gen_yaccstacktype_h_enum){}
};
extern view_gen_yaccstacktype_h_class view_gen_yaccstacktype_h;
struct view_gen_noofoperators_h_class: uview_class {
    view_gen_noofoperators_h_class():uview_class(view_gen_noofoperators_h_enum){}
};
extern view_gen_noofoperators_h_class view_gen_noofoperators_h;
struct view_gen_assertmacros_h_class: uview_class {
    view_gen_assertmacros_h_class():uview_class(view_gen_assertmacros_h_enum){}
};
extern view_gen_assertmacros_h_class view_gen_assertmacros_h;
struct view_gen_assertmacros_c_class: uview_class {
    view_gen_assertmacros_c_class():uview_class(view_gen_assertmacros_c_enum){}
};
extern view_gen_assertmacros_c_class view_gen_assertmacros_c;
struct view_gen_operatordecls_h_class: uview_class {
    view_gen_operatordecls_h_class():uview_class(view_gen_operatordecls_h_enum){}
};
extern view_gen_operatordecls_h_class view_gen_operatordecls_h;
struct view_gen_operatorcast_h_class: uview_class {
    view_gen_operatorcast_h_class():uview_class(view_gen_operatorcast_h_enum){}
};
extern view_gen_operatorcast_h_class view_gen_operatorcast_h;
struct view_gen_operatordefs_c_class: uview_class {
    view_gen_operatordefs_c_class():uview_class(view_gen_operatordefs_c_enum){}
};
extern view_gen_operatordefs_c_class view_gen_operatordefs_c;
struct view_gen_operatordefs_c_0_class: uview_class {
    view_gen_operatordefs_c_0_class():uview_class(view_gen_operatordefs_c_0_enum){}
};
extern view_gen_operatordefs_c_0_class view_gen_operatordefs_c_0;
struct view_gen_operatordefs_c_1_class: uview_class {
    view_gen_operatordefs_c_1_class():uview_class(view_gen_operatordefs_c_1_enum){}
};
extern view_gen_operatordefs_c_1_class view_gen_operatordefs_c_1;
struct view_gen_operatordefs_c_2_class: uview_class {
    view_gen_operatordefs_c_2_class():uview_class(view_gen_operatordefs_c_2_enum){}
};
extern view_gen_operatordefs_c_2_class view_gen_operatordefs_c_2;
struct view_gen_initializephyla_whiletest_c_class: uview_class {
    view_gen_initializephyla_whiletest_c_class():uview_class(view_gen_initializephyla_whiletest_c_enum){}
};
extern view_gen_initializephyla_whiletest_c_class view_gen_initializephyla_whiletest_c;
struct view_gen_initializephyla_init_el_c_class: uview_class {
    view_gen_initializephyla_init_el_c_class():uview_class(view_gen_initializephyla_init_el_c_enum){}
};
extern view_gen_initializephyla_init_el_c_class view_gen_initializephyla_init_el_c;
struct view_gen_initializephyla_update_loop_c_class: uview_class {
    view_gen_initializephyla_update_loop_c_class():uview_class(view_gen_initializephyla_update_loop_c_enum){}
};
extern view_gen_initializephyla_update_loop_c_class view_gen_initializephyla_update_loop_c;
struct view_gen_operatordefs_nonhash_c_class: uview_class {
    view_gen_operatordefs_nonhash_c_class():uview_class(view_gen_operatordefs_nonhash_c_enum){}
};
extern view_gen_operatordefs_nonhash_c_class view_gen_operatordefs_nonhash_c;
struct view_gen_operatordefs_hash_c_class: uview_class {
    view_gen_operatordefs_hash_c_class():uview_class(view_gen_operatordefs_hash_c_enum){}
};
extern view_gen_operatordefs_hash_c_class view_gen_operatordefs_hash_c;
struct view_gen_access_functions_class: uview_class {
    view_gen_access_functions_class():uview_class(view_gen_access_functions_enum){}
};
extern view_gen_access_functions_class view_gen_access_functions;
struct view_gen_create_function_class: uview_class {
    view_gen_create_function_class():uview_class(view_gen_create_function_enum){}
};
extern view_gen_create_function_class view_gen_create_function;
struct view_gen_attributeOf_function_class: uview_class {
    view_gen_attributeOf_function_class():uview_class(view_gen_attributeOf_function_enum){}
};
extern view_gen_attributeOf_function_class view_gen_attributeOf_function;
struct view_gen_argseqnr_class: uview_class {
    view_gen_argseqnr_class():uview_class(view_gen_argseqnr_enum){}
};
extern view_gen_argseqnr_class view_gen_argseqnr;
struct view_gen_fnarg_asserts_class: uview_class {
    view_gen_fnarg_asserts_class():uview_class(view_gen_fnarg_asserts_enum){}
};
extern view_gen_fnarg_asserts_class view_gen_fnarg_asserts;
struct view_gen_fnarg_and_decls_class: uview_class {
    view_gen_fnarg_and_decls_class():uview_class(view_gen_fnarg_and_decls_enum){}
};
extern view_gen_fnarg_and_decls_class view_gen_fnarg_and_decls;
struct view_gen_fnarg_and_decls_predef_class: uview_class {
    view_gen_fnarg_and_decls_predef_class():uview_class(view_gen_fnarg_and_decls_predef_enum){}
};
extern view_gen_fnarg_and_decls_predef_class view_gen_fnarg_and_decls_predef;
struct view_gen_assignments_class: uview_class {
    view_gen_assignments_class():uview_class(view_gen_assignments_enum){}
};
extern view_gen_assignments_class view_gen_assignments;
struct view_gen_assignment_inis_class: uview_class {
    view_gen_assignment_inis_class():uview_class(view_gen_assignment_inis_enum){}
};
extern view_gen_assignment_inis_class view_gen_assignment_inis;
struct view_gen_user_assignment_inis_class: uview_class {
    view_gen_user_assignment_inis_class():uview_class(view_gen_user_assignment_inis_enum){}
};
extern view_gen_user_assignment_inis_class view_gen_user_assignment_inis;
struct view_gen_assignments_predef_class: uview_class {
    view_gen_assignments_predef_class():uview_class(view_gen_assignments_predef_enum){}
};
extern view_gen_assignments_predef_class view_gen_assignments_predef;
struct view_gen_assignments_predef_ini_class: uview_class {
    view_gen_assignments_predef_ini_class():uview_class(view_gen_assignments_predef_ini_enum){}
};
extern view_gen_assignments_predef_ini_class view_gen_assignments_predef_ini;
struct view_gen_test_class: uview_class {
    view_gen_test_class():uview_class(view_gen_test_enum){}
};
extern view_gen_test_class view_gen_test;
struct view_gen_error_decls_h_class: uview_class {
    view_gen_error_decls_h_class():uview_class(view_gen_error_decls_h_enum){}
};
extern view_gen_error_decls_h_class view_gen_error_decls_h;
struct view_gen_error_defs_c_class: uview_class {
    view_gen_error_defs_c_class():uview_class(view_gen_error_defs_c_enum){}
};
extern view_gen_error_defs_c_class view_gen_error_defs_c;
struct view_gen_printdotdecls_h_class: uview_class {
    view_gen_printdotdecls_h_class():uview_class(view_gen_printdotdecls_h_enum){}
};
extern view_gen_printdotdecls_h_class view_gen_printdotdecls_h;
struct view_gen_printdotdefs_c_class: uview_class {
    view_gen_printdotdefs_c_class():uview_class(view_gen_printdotdefs_c_enum){}
};
extern view_gen_printdotdefs_c_class view_gen_printdotdefs_c;
struct view_gen_printdotedges_c_class: uview_class {
    view_gen_printdotedges_c_class():uview_class(view_gen_printdotedges_c_enum){}
};
extern view_gen_printdotedges_c_class view_gen_printdotedges_c;
struct view_gen_listdecls_h_class: uview_class {
    view_gen_listdecls_h_class():uview_class(view_gen_listdecls_h_enum){}
};
extern view_gen_listdecls_h_class view_gen_listdecls_h;
struct view_gen_listdefs_c_class: uview_class {
    view_gen_listdefs_c_class():uview_class(view_gen_listdefs_c_enum){}
};
extern view_gen_listdefs_c_class view_gen_listdefs_c;
struct view_gen_includes_class: uview_class {
    view_gen_includes_class():uview_class(view_gen_includes_enum){}
};
extern view_gen_includes_class view_gen_includes;
struct view_do_gen_includes_class: uview_class {
    view_do_gen_includes_class():uview_class(view_do_gen_includes_enum){}
};
extern view_do_gen_includes_class view_do_gen_includes;
struct view_gen_csgio_start_h_class: uview_class {
    view_gen_csgio_start_h_class():uview_class(view_gen_csgio_start_h_enum){}
};
extern view_gen_csgio_start_h_class view_gen_csgio_start_h;
struct view_gen_csgio_end_h_class: uview_class {
    view_gen_csgio_end_h_class():uview_class(view_gen_csgio_end_h_enum){}
};
extern view_gen_csgio_end_h_class view_gen_csgio_end_h;
struct view_gen_csgio_h_class: uview_class {
    view_gen_csgio_h_class():uview_class(view_gen_csgio_h_enum){}
};
extern view_gen_csgio_h_class view_gen_csgio_h;
struct view_gen_csgio_start_c_class: uview_class {
    view_gen_csgio_start_c_class():uview_class(view_gen_csgio_start_c_enum){}
};
extern view_gen_csgio_start_c_class view_gen_csgio_start_c;
struct view_gen_csgio_c_class: uview_class {
    view_gen_csgio_c_class():uview_class(view_gen_csgio_c_enum){}
};
extern view_gen_csgio_c_class view_gen_csgio_c;
struct view_gen_copy_attributes_c_class: uview_class {
    view_gen_copy_attributes_c_class():uview_class(view_gen_copy_attributes_c_enum){}
};
extern view_gen_copy_attributes_c_class view_gen_copy_attributes_c;
struct view_gen_copydefs_c_class: uview_class {
    view_gen_copydefs_c_class():uview_class(view_gen_copydefs_c_enum){}
};
extern view_gen_copydefs_c_class view_gen_copydefs_c;
struct view_gen_rewritek_h_class: uview_class {
    view_gen_rewritek_h_class():uview_class(view_gen_rewritek_h_enum){}
};
extern view_gen_rewritek_h_class view_gen_rewritek_h;
struct view_gen_end_rewritek_h_class: uview_class {
    view_gen_end_rewritek_h_class():uview_class(view_gen_end_rewritek_h_enum){}
};
extern view_gen_end_rewritek_h_class view_gen_end_rewritek_h;
struct view_gen_rewritek_c_class: uview_class {
    view_gen_rewritek_c_class():uview_class(view_gen_rewritek_c_enum){}
};
extern view_gen_rewritek_c_class view_gen_rewritek_c;
struct view_gen_rewritedefs_c_class: uview_class {
    view_gen_rewritedefs_c_class():uview_class(view_gen_rewritedefs_c_enum){}
};
extern view_gen_rewritedefs_c_class view_gen_rewritedefs_c;
struct view_gen_rewritedefs_default_c_class: uview_class {
    view_gen_rewritedefs_default_c_class():uview_class(view_gen_rewritedefs_default_c_enum){}
};
extern view_gen_rewritedefs_default_c_class view_gen_rewritedefs_default_c;
struct view_gen_rewritedefs_other_c_class: uview_class {
    view_gen_rewritedefs_other_c_class():uview_class(view_gen_rewritedefs_other_c_enum){}
};
extern view_gen_rewritedefs_other_c_class view_gen_rewritedefs_other_c;
struct view_gen_rewritedefs_rewritearg_c_class: uview_class {
    view_gen_rewritedefs_rewritearg_c_class():uview_class(view_gen_rewritedefs_rewritearg_c_enum){}
};
extern view_gen_rewritedefs_rewritearg_c_class view_gen_rewritedefs_rewritearg_c;
struct view_gen_rewritedefs_nl_arg_c_class: uview_class {
    view_gen_rewritedefs_nl_arg_c_class():uview_class(view_gen_rewritedefs_nl_arg_c_enum){}
};
extern view_gen_rewritedefs_nl_arg_c_class view_gen_rewritedefs_nl_arg_c;
struct view_gen_rewritedefs_testarg_c_class: uview_class {
    view_gen_rewritedefs_testarg_c_class():uview_class(view_gen_rewritedefs_testarg_c_enum){}
};
extern view_gen_rewritedefs_testarg_c_class view_gen_rewritedefs_testarg_c;
struct view_gen_rewritedefs_dotestarg_c_class: uview_class {
    view_gen_rewritedefs_dotestarg_c_class():uview_class(view_gen_rewritedefs_dotestarg_c_enum){}
};
extern view_gen_rewritedefs_dotestarg_c_class view_gen_rewritedefs_dotestarg_c;
struct view_gen_rewritedefs_args_c_class: uview_class {
    view_gen_rewritedefs_args_c_class():uview_class(view_gen_rewritedefs_args_c_enum){}
};
extern view_gen_rewritedefs_args_c_class view_gen_rewritedefs_args_c;
struct view_gen_rewritedefs_body_c_class: uview_class {
    view_gen_rewritedefs_body_c_class():uview_class(view_gen_rewritedefs_body_c_enum){}
};
extern view_gen_rewritedefs_body_c_class view_gen_rewritedefs_body_c;
struct view_gen_withcases_and_default_class: uview_class {
    view_gen_withcases_and_default_class():uview_class(view_gen_withcases_and_default_enum){}
};
extern view_gen_withcases_and_default_class view_gen_withcases_and_default;
struct view_rw_predicates_class: uview_class {
    view_rw_predicates_class():uview_class(view_rw_predicates_enum){}
};
extern view_rw_predicates_class view_rw_predicates;
struct view_wc_predicates_class: uview_class {
    view_wc_predicates_class():uview_class(view_wc_predicates_enum){}
};
extern view_wc_predicates_class view_wc_predicates;
struct view_unp_predicates_class: uview_class {
    view_unp_predicates_class():uview_class(view_unp_predicates_enum){}
};
extern view_unp_predicates_class view_unp_predicates;
struct view_wc_bindings_class: uview_class {
    view_wc_bindings_class():uview_class(view_wc_bindings_enum){}
};
extern view_wc_bindings_class view_wc_bindings;
struct view_unp_bindings_class: uview_class {
    view_unp_bindings_class():uview_class(view_unp_bindings_enum){}
};
extern view_unp_bindings_class view_unp_bindings;
struct view_rw_bindings_class: uview_class {
    view_rw_bindings_class():uview_class(view_rw_bindings_enum){}
};
extern view_rw_bindings_class view_rw_bindings;
struct view_gen_fnk_h_class: uview_class {
    view_gen_fnk_h_class():uview_class(view_gen_fnk_h_enum){}
};
extern view_gen_fnk_h_class view_gen_fnk_h;
struct view_gen_fnkdecls_c_class: uview_class {
    view_gen_fnkdecls_c_class():uview_class(view_gen_fnkdecls_c_enum){}
};
extern view_gen_fnkdecls_c_class view_gen_fnkdecls_c;
struct view_gen_fnk_c_class: uview_class {
    view_gen_fnk_c_class():uview_class(view_gen_fnk_c_enum){}
};
extern view_gen_fnk_c_class view_gen_fnk_c;
struct view_gen_fn_pointer_name_class: uview_class {
    view_gen_fn_pointer_name_class():uview_class(view_gen_fn_pointer_name_enum){}
};
extern view_gen_fn_pointer_name_class view_gen_fn_pointer_name;
struct view_gen_fnkargs_class: uview_class {
    view_gen_fnkargs_class():uview_class(view_gen_fnkargs_enum){}
};
extern view_gen_fnkargs_class view_gen_fnkargs;
struct view_gen_fnkdecls_class: uview_class {
    view_gen_fnkdecls_class():uview_class(view_gen_fnkdecls_enum){}
};
extern view_gen_fnkdecls_class view_gen_fnkdecls;
struct view_gen_fns_start_h_class: uview_class {
    view_gen_fns_start_h_class():uview_class(view_gen_fns_start_h_enum){}
};
extern view_gen_fns_start_h_class view_gen_fns_start_h;
struct view_gen_fns_end_h_class: uview_class {
    view_gen_fns_end_h_class():uview_class(view_gen_fns_end_h_enum){}
};
extern view_gen_fns_end_h_class view_gen_fns_end_h;
struct view_gen_fns_start_c_class: uview_class {
    view_gen_fns_start_c_class():uview_class(view_gen_fns_start_c_enum){}
};
extern view_gen_fns_start_c_class view_gen_fns_start_c;
struct view_gen_fns_owninclude_c_class: uview_class {
    view_gen_fns_owninclude_c_class():uview_class(view_gen_fns_owninclude_c_enum){}
};
extern view_gen_fns_owninclude_c_class view_gen_fns_owninclude_c;
struct view_gen_unpk_h_class: uview_class {
    view_gen_unpk_h_class():uview_class(view_gen_unpk_h_enum){}
};
extern view_gen_unpk_h_class view_gen_unpk_h;
struct view_gen_unparsedecls_h_class: uview_class {
    view_gen_unparsedecls_h_class():uview_class(view_gen_unparsedecls_h_enum){}
};
extern view_gen_unparsedecls_h_class view_gen_unparsedecls_h;
struct view_gen_rewritedecls_h_class: uview_class {
    view_gen_rewritedecls_h_class():uview_class(view_gen_rewritedecls_h_enum){}
};
extern view_gen_rewritedecls_h_class view_gen_rewritedecls_h;
struct view_uview_def_class: uview_class {
    view_uview_def_class():uview_class(view_uview_def_enum){}
};
extern view_uview_def_class view_uview_def;
struct view_gen_end_unpk_h_class: uview_class {
    view_gen_end_unpk_h_class():uview_class(view_gen_end_unpk_h_enum){}
};
extern view_gen_end_unpk_h_class view_gen_end_unpk_h;
struct view_gen_unpk_c_class: uview_class {
    view_gen_unpk_c_class():uview_class(view_gen_unpk_c_enum){}
};
extern view_gen_unpk_c_class view_gen_unpk_c;
struct view_gen_default_types_unpk_c_class: uview_class {
    view_gen_default_types_unpk_c_class():uview_class(view_gen_default_types_unpk_c_enum){}
};
extern view_gen_default_types_unpk_c_class view_gen_default_types_unpk_c;
struct view_gen_unparsedefs_c_class: uview_class {
    view_gen_unparsedefs_c_class():uview_class(view_gen_unparsedefs_c_enum){}
};
extern view_gen_unparsedefs_c_class view_gen_unparsedefs_c;
class view_gen_unparsedefs_default_c_baseclass: public uview_class {
protected:
    view_gen_unparsedefs_default_c_baseclass():uview_class(view_gen_unparsedefs_default_c_enum){}
};
// class view_gen_unparsedefs_default_c_class is defined externally
struct view_gen_unparsedefs_other_c_class: uview_class {
    view_gen_unparsedefs_other_c_class():uview_class(view_gen_unparsedefs_other_c_enum){}
};
extern view_gen_unparsedefs_other_c_class view_gen_unparsedefs_other_c;
struct view_gen_unpstr_c_class: uview_class {
    view_gen_unpstr_c_class():uview_class(view_gen_unpstr_c_enum){}
};
extern view_gen_unpstr_c_class view_gen_unpstr_c;
struct view_gen_user_predicates_class: uview_class {
    view_gen_user_predicates_class():uview_class(view_gen_user_predicates_enum){}
};
extern view_gen_user_predicates_class view_gen_user_predicates;
struct view_predicate_bindings_class: uview_class {
    view_predicate_bindings_class():uview_class(view_predicate_bindings_enum){}
};
extern view_predicate_bindings_class view_predicate_bindings;
struct view_checklanguagenames_class: uview_class {
    view_checklanguagenames_class():uview_class(view_checklanguagenames_enum){}
};
extern view_checklanguagenames_class view_checklanguagenames;
struct view_output_collection_class: uview_class {
    view_output_collection_class():uview_class(view_output_collection_enum){}
};
extern view_output_collection_class view_output_collection;
struct view_gen_classdecls1_h_class: uview_class {
    view_gen_classdecls1_h_class():uview_class(view_gen_classdecls1_h_enum){}
};
extern view_gen_classdecls1_h_class view_gen_classdecls1_h;
struct view_gen_classdecls2_h_class: uview_class {
    view_gen_classdecls2_h_class():uview_class(view_gen_classdecls2_h_enum){}
};
extern view_gen_classdecls2_h_class view_gen_classdecls2_h;
struct view_gen_classdefs_c_class: uview_class {
    view_gen_classdefs_c_class():uview_class(view_gen_classdefs_c_enum){}
};
extern view_gen_classdefs_c_class view_gen_classdefs_c;
struct view_gen_subphylumdefs_c_class: uview_class {
    view_gen_subphylumdefs_c_class():uview_class(view_gen_subphylumdefs_c_enum){}
};
extern view_gen_subphylumdefs_c_class view_gen_subphylumdefs_c;
struct view_gen_cast_class: uview_class {
    view_gen_cast_class():uview_class(view_gen_cast_enum){}
};
extern view_gen_cast_class view_gen_cast;
struct view_gen_hashtables_h_class: uview_class {
    view_gen_hashtables_h_class():uview_class(view_gen_hashtables_h_enum){}
};
extern view_gen_hashtables_h_class view_gen_hashtables_h;
struct view_gen_hashtables_c_class: uview_class {
    view_gen_hashtables_c_class():uview_class(view_gen_hashtables_c_enum){}
};
extern view_gen_hashtables_c_class view_gen_hashtables_c;
struct view_gen_yxx_union_h_class: uview_class {
    view_gen_yxx_union_h_class():uview_class(view_gen_yxx_union_h_enum){}
};
extern view_gen_yxx_union_h_class view_gen_yxx_union_h;
struct view_gen_member_dcl_h_class: uview_class {
    view_gen_member_dcl_h_class():uview_class(view_gen_member_dcl_h_enum){}
};
extern view_gen_member_dcl_h_class view_gen_member_dcl_h;
struct view_gen_set_subphylumdefs_c_class: uview_class {
    view_gen_set_subphylumdefs_c_class():uview_class(view_gen_set_subphylumdefs_c_enum){}
};
extern view_gen_set_subphylumdefs_c_class view_gen_set_subphylumdefs_c;
struct view_gen_viewvars_c_class: uview_class {
    view_gen_viewvars_c_class():uview_class(view_gen_viewvars_c_enum){}
};
extern view_gen_viewvars_c_class view_gen_viewvars_c;
struct view_gen_argseqnr_rec_class: uview_class {
    view_gen_argseqnr_rec_class():uview_class(view_gen_argseqnr_rec_enum){}
};
extern view_gen_argseqnr_rec_class view_gen_argseqnr_rec;
struct view_gen_opt_const_class: uview_class {
    view_gen_opt_const_class():uview_class(view_gen_opt_const_enum){}
};
extern view_gen_opt_const_class view_gen_opt_const;
struct view_uview_class_decl_class: uview_class {
    view_uview_class_decl_class():uview_class(view_uview_class_decl_enum){}
};
extern view_uview_class_decl_class view_uview_class_decl;
struct view_uview_class_def_class: uview_class {
    view_uview_class_def_class():uview_class(view_uview_class_def_enum){}
};
extern view_uview_class_def_class view_uview_class_def;
struct view_rview_class_decl_class: uview_class {
    view_rview_class_decl_class():uview_class(view_rview_class_decl_enum){}
};
extern view_rview_class_decl_class view_rview_class_decl;
struct view_rview_class_def_class: uview_class {
    view_rview_class_def_class():uview_class(view_rview_class_def_enum){}
};
extern view_rview_class_def_class view_rview_class_def;
struct view_collect_strings_class: uview_class {
    view_collect_strings_class():uview_class(view_collect_strings_enum){}
};
extern view_collect_strings_class view_collect_strings;
struct view_class_of_op_class: uview_class {
    view_class_of_op_class():uview_class(view_class_of_op_enum){}
};
extern view_class_of_op_class view_class_of_op;
struct view_class_of_phy_class: uview_class {
    view_class_of_phy_class():uview_class(view_class_of_phy_enum){}
};
extern view_class_of_phy_class view_class_of_phy;

void unparse(abstract_phylum kc_p, printer_functor kc_printer, uview kc_current_view);
void unparse(void *kc_p, printer_functor kc_printer, uview kc_current_view);
void unparse(int kc_v, printer_functor kc_printer, uview kc_current_view);
void unparse(double kc_v, printer_functor kc_printer, uview kc_current_view);
void unparse(kc_char *kc_v, printer_functor kc_printer, uview kc_current_view);
void unparse(kc_string_t kc_v, printer_functor kc_printer, uview kc_current_view);
#define PRINT(string) kc_printer(string,kc_current_view)
#define UNPARSE(node) node->unparse(kc_printer,kc_current_view)

} // namespace kc
#endif // KC_UNPARSE_HEADER
