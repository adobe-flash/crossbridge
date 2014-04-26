/* APPLE LOCAL file CW asm blocks */
#ifndef CONFIG_ASM_H
#define CONFIG_ASM_H

#include "cpplib.h"

/* We use a small state machine to inform the lexer when to start
   returning tokens marking the beginning of each asm line.  */
enum iasm_states {
  /* Normal code.  */
  iasm_none,
  /* '{' of asm block seen, decls may appear.  */
  iasm_decls,
  /* No more decls, in asm block proper, '}' not seen yet.  */
  iasm_asm
};

/* Nonzero means that CodeWarrior-style inline assembler is to be parsed.  */

extern int flag_iasm_blocks;

extern enum iasm_states iasm_state;
extern bool iasm_in_decl;
extern bool inside_iasm_block;
extern bool iasm_kill_regs;
extern bool iasm_in_operands;
extern tree iasm_do_id (tree);
/* LLVM LOCAL */
extern int iasm_label_counter;
/* Maximum number of arguments.  */
#define IASM_MAX_ARG 11

#ifndef TARGET_IASM_EXTRA_INFO
#define TARGET_IASM_EXTRA_INFO
#endif

struct iasm_md_Extra_info {
  /* Number of operands to the ASM_expr.  Note, this can be different
     from the number of operands to the instruction, in cases like:

        mov 0(foo,bar,4), $42

	where foo and bar are C expressions.  */
  int num;

  struct {
    /* Constraints for operand to the ASM_EXPR.  */
    const char *constraint;
    tree var;
    unsigned int argnum;
    bool must_be_reg;
    bool was_output;
  } dat[IASM_MAX_ARG];

  int num_rewrites;
  struct {
    int dat_index;
    char *arg_p;
  } rewrite[IASM_MAX_ARG];

  bool no_label_map;
  const char *modifier;

  TARGET_IASM_EXTRA_INFO
};
typedef struct iasm_md_Extra_info iasm_md_extra_info;

void iasm_print_operand (char *buf, tree arg, unsigned argnum, tree *uses,
			 bool must_be_reg, bool must_not_be_reg, iasm_md_extra_info *e);

extern void iasm_stmt (tree, tree, int);
extern tree iasm_build_register_offset (tree, tree);
extern tree iasm_label (tree, bool);
extern tree prepend_char_identifier (tree, char);
extern tree iasm_reg_name (tree);
extern void iasm_entry (int, tree);
extern int iasm_typename_or_reserved (tree);
extern tree iasm_c_build_component_ref (tree, tree);
extern tree iasm_get_identifier (tree, const char *);
extern tree iasm_build_bracket (tree, tree);
extern bool iasm_is_prefix (tree);
extern void iasm_skip_to_eol (void);
extern bool iasm_memory_clobber (const char *);
extern void iasm_force_constraint (const char *c, iasm_md_extra_info *e);
extern tree iasm_ptr_conv (tree type, tree exp);
extern void iasm_get_register_var (tree var, const char *modifier, char *buf,
				   unsigned argnum, bool must_be_reg, iasm_md_extra_info *e);
extern bool iasm_is_pseudo (const char *);
extern tree iasm_addr (tree);
extern void iasm_end_block (void);
#endif
