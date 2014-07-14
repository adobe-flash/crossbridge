/*
 * Copyright © 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file linker.cpp
 * GLSL linker implementation
 *
 * Given a set of shaders that are to be linked to generate a final program,
 * there are three distinct stages.
 *
 * In the first stage shaders are partitioned into groups based on the shader
 * type.  All shaders of a particular type (e.g., vertex shaders) are linked
 * together.
 *
 *   - Undefined references in each shader are resolve to definitions in
 *     another shader.
 *   - Types and qualifiers of uniforms, outputs, and global variables defined
 *     in multiple shaders with the same name are verified to be the same.
 *   - Initializers for uniforms and global variables defined
 *     in multiple shaders with the same name are verified to be the same.
 *
 * The result, in the terminology of the GLSL spec, is a set of shader
 * executables for each processing unit.
 *
 * After the first stage is complete, a series of semantic checks are performed
 * on each of the shader executables.
 *
 *   - Each shader executable must define a \c main function.
 *   - Each vertex shader executable must write to \c gl_Position.
 *   - Each fragment shader executable must write to either \c gl_FragData or
 *     \c gl_FragColor.
 *
 * In the final stage individual shader executables are linked to create a
 * complete exectuable.
 *
 *   - Types of uniforms defined in multiple shader stages with the same name
 *     are verified to be the same.
 *   - Initializers for uniforms defined in multiple shader stages with the
 *     same name are verified to be the same.
 *   - Types and qualifiers of outputs defined in one stage are verified to
 *     be the same as the types and qualifiers of inputs defined with the same
 *     name in a later stage.
 *
 * \author Ian Romanick <ian.d.romanick@intel.com>
 */

#include "main/core.h"
#include "glsl_symbol_table.h"
#include "ir.h"
#include "program.h"
#include "program/hash_table.h"
#include "linker.h"
#include "ir_optimization.h"

extern "C" {
#include "main/shaderobj.h"
}

/**
 * Visitor that determines whether or not a variable is ever written.
 */
class find_assignment_visitor : public ir_hierarchical_visitor {
public:
   find_assignment_visitor(const char *name)
      : name(name), found(false)
   {
      /* empty */
   }

   virtual ir_visitor_status visit_enter(ir_assignment *ir)
   {
      ir_variable *const var = ir->lhs->variable_referenced();

      if (strcmp(name, var->name) == 0) {
	 found = true;
	 return visit_stop;
      }

      return visit_continue_with_parent;
   }

   virtual ir_visitor_status visit_enter(ir_call *ir)
   {
      exec_list_iterator sig_iter = ir->get_callee()->parameters.iterator();
      foreach_iter(exec_list_iterator, iter, *ir) {
	 ir_rvalue *param_rval = (ir_rvalue *)iter.get();
	 ir_variable *sig_param = (ir_variable *)sig_iter.get();

	 if (sig_param->mode == ir_var_out ||
	     sig_param->mode == ir_var_inout) {
	    ir_variable *var = param_rval->variable_referenced();
	    if (var && strcmp(name, var->name) == 0) {
	       found = true;
	       return visit_stop;
	    }
	 }
	 sig_iter.next();
      }

      return visit_continue_with_parent;
   }

   bool variable_found()
   {
      return found;
   }

private:
   const char *name;       /**< Find writes to a variable with this name. */
   bool found;             /**< Was a write to the variable found? */
};


/**
 * Visitor that determines whether or not a variable is ever read.
 */
class find_deref_visitor : public ir_hierarchical_visitor {
public:
   find_deref_visitor(const char *name)
      : name(name), found(false)
   {
      /* empty */
   }

   virtual ir_visitor_status visit(ir_dereference_variable *ir)
   {
      if (strcmp(this->name, ir->var->name) == 0) {
	 this->found = true;
	 return visit_stop;
      }

      return visit_continue;
   }

   bool variable_found() const
   {
      return this->found;
   }

private:
   const char *name;       /**< Find writes to a variable with this name. */
   bool found;             /**< Was a write to the variable found? */
};


void
linker_error(gl_shader_program *prog, const char *fmt, ...)
{
   va_list ap;

   ralloc_strcat(&prog->InfoLog, "error: ");
   va_start(ap, fmt);
   ralloc_vasprintf_append(&prog->InfoLog, fmt, ap);
   va_end(ap);

   prog->LinkStatus = false;
}


void
linker_warning(gl_shader_program *prog, const char *fmt, ...)
{
   va_list ap;

   ralloc_strcat(&prog->InfoLog, "error: ");
   va_start(ap, fmt);
   ralloc_vasprintf_append(&prog->InfoLog, fmt, ap);
   va_end(ap);

}


void
invalidate_variable_locations(gl_shader *sh, enum ir_variable_mode mode,
			      int generic_base)
{
   foreach_list(node, sh->ir) {
      ir_variable *const var = ((ir_instruction *) node)->as_variable();

      if ((var == NULL) || (var->mode != (unsigned) mode))
	 continue;

      /* Only assign locations for generic attributes / varyings / etc.
       */
      if ((var->location >= generic_base) && !var->explicit_location)
	  var->location = -1;
   }
}


/**
 * Determine the number of attribute slots required for a particular type
 *
 * This code is here because it implements the language rules of a specific
 * GLSL version.  Since it's a property of the language and not a property of
 * types in general, it doesn't really belong in glsl_type.
 */
unsigned
count_attribute_slots(const glsl_type *t)
{
   /* From page 31 (page 37 of the PDF) of the GLSL 1.50 spec:
    *
    *     "A scalar input counts the same amount against this limit as a vec4,
    *     so applications may want to consider packing groups of four
    *     unrelated float inputs together into a vector to better utilize the
    *     capabilities of the underlying hardware. A matrix input will use up
    *     multiple locations.  The number of locations used will equal the
    *     number of columns in the matrix."
    *
    * The spec does not explicitly say how arrays are counted.  However, it
    * should be safe to assume the total number of slots consumed by an array
    * is the number of entries in the array multiplied by the number of slots
    * consumed by a single element of the array.
    */

   if (t->is_array())
      return t->array_size() * count_attribute_slots(t->element_type());

   if (t->is_matrix())
      return t->matrix_columns;

   return 1;
}


/**
 * Verify that a vertex shader executable meets all semantic requirements
 *
 * \param shader  Vertex shader executable to be verified
 */
bool
validate_vertex_shader_executable(struct gl_shader_program *prog,
				  struct gl_shader *shader)
{
   if (shader == NULL)
      return true;

   find_assignment_visitor find("gl_Position");
   find.run(shader->ir);
   if (!find.variable_found()) {
      linker_error(prog, "vertex shader does not write to `gl_Position'\n");
      return false;
   }

   return true;
}


/**
 * Verify that a fragment shader executable meets all semantic requirements
 *
 * \param shader  Fragment shader executable to be verified
 */
bool
validate_fragment_shader_executable(struct gl_shader_program *prog,
				    struct gl_shader *shader)
{
   if (shader == NULL)
      return true;

   find_assignment_visitor frag_color("gl_FragColor");
   find_assignment_visitor frag_data("gl_FragData");

   frag_color.run(shader->ir);
   frag_data.run(shader->ir);

   if (frag_color.variable_found() && frag_data.variable_found()) {
      linker_error(prog,  "fragment shader writes to both "
		   "`gl_FragColor' and `gl_FragData'\n");
      return false;
   }

   return true;
}


/**
 * Generate a string describing the mode of a variable
 */
static const char *
mode_string(const ir_variable *var)
{
   switch (var->mode) {
   case ir_var_auto:
      return (var->read_only) ? "global constant" : "global variable";

   case ir_var_uniform: return "uniform";
   case ir_var_in:      return "shader input";
   case ir_var_out:     return "shader output";
   case ir_var_inout:   return "shader inout";

   case ir_var_const_in:
   case ir_var_temporary:
   default:
      assert(!"Should not get here.");
      return "invalid variable";
   }
}


/**
 * Perform validation of global variables used across multiple shaders
 */
bool
cross_validate_globals(struct gl_shader_program *prog,
		       struct gl_shader **shader_list,
		       unsigned num_shaders,
		       bool uniforms_only)
{
   /* Examine all of the uniforms in all of the shaders and cross validate
    * them.
    */
   glsl_symbol_table variables;
   for (unsigned i = 0; i < num_shaders; i++) {
      if (shader_list[i] == NULL)
	 continue;

      foreach_list(node, shader_list[i]->ir) {
	 ir_variable *const var = ((ir_instruction *) node)->as_variable();

	 if (var == NULL)
	    continue;

	 if (uniforms_only && (var->mode != ir_var_uniform))
	    continue;

	 /* Don't cross validate temporaries that are at global scope.  These
	  * will eventually get pulled into the shaders 'main'.
	  */
	 if (var->mode == ir_var_temporary)
	    continue;

	 /* If a global with this name has already been seen, verify that the
	  * new instance has the same type.  In addition, if the globals have
	  * initializers, the values of the initializers must be the same.
	  */
	 ir_variable *const existing = variables.get_variable(var->name);
	 if (existing != NULL) {
	    if (var->type != existing->type) {
	       /* Consider the types to be "the same" if both types are arrays
		* of the same type and one of the arrays is implicitly sized.
		* In addition, set the type of the linked variable to the
		* explicitly sized array.
		*/
	       if (var->type->is_array()
		   && existing->type->is_array()
		   && (var->type->fields.array == existing->type->fields.array)
		   && ((var->type->length == 0)
		       || (existing->type->length == 0))) {
		  if (var->type->length != 0) {
		     existing->type = var->type;
		  }
	       } else {
		  linker_error(prog, "%s `%s' declared as type "
			       "`%s' and type `%s'\n",
			       mode_string(var),
			       var->name, var->type->name,
			       existing->type->name);
		  return false;
	       }
	    }

	    if (var->explicit_location) {
	       if (existing->explicit_location
		   && (var->location != existing->location)) {
		     linker_error(prog, "explicit locations for %s "
				  "`%s' have differing values\n",
				  mode_string(var), var->name);
		     return false;
	       }

	       existing->location = var->location;
	       existing->explicit_location = true;
	    }

        /* Validate layout qualifiers for gl_FragDepth.
         *
         * From the AMD_conservative_depth spec:
         *    "If gl_FragDepth is redeclared in any fragment shader in
         *    a program, it must be redeclared in all fragment shaders in that
         *    program that have static assignments to gl_FragDepth. All
         *    redeclarations of gl_FragDepth in all fragment shaders in
         *    a single program must have the same set of qualifiers."
         */
        if (strcmp(var->name, "gl_FragDepth") == 0) {
           bool layout_declared = var->depth_layout != ir_depth_layout_none;
           bool layout_differs = var->depth_layout != existing->depth_layout;
           if (layout_declared && layout_differs) {
              linker_error(prog,
                 "All redeclarations of gl_FragDepth in all fragment shaders "
                 "in a single program must have the same set of qualifiers.");
           }
           if (var->used && layout_differs) {
              linker_error(prog,
                    "If gl_FragDepth is redeclared with a layout qualifier in"
                    "any fragment shader, it must be redeclared with the same"
                    "layout qualifier in all fragment shaders that have"
                    "assignments to gl_FragDepth");
           }
        }

	    /* FINISHME: Handle non-constant initializers.
	     */
	    if (var->constant_value != NULL) {
	       if (existing->constant_value != NULL) {
		  if (!var->constant_value->has_value(existing->constant_value)) {
		     linker_error(prog, "initializers for %s "
				  "`%s' have differing values\n",
				  mode_string(var), var->name);
		     return false;
		  }
	       } else
		  /* If the first-seen instance of a particular uniform did not
		   * have an initializer but a later instance does, copy the
		   * initializer to the version stored in the symbol table.
		   */
		  /* FINISHME: This is wrong.  The constant_value field should
		   * FINISHME: not be modified!  Imagine a case where a shader
		   * FINISHME: without an initializer is linked in two different
		   * FINISHME: programs with shaders that have differing
		   * FINISHME: initializers.  Linking with the first will
		   * FINISHME: modify the shader, and linking with the second
		   * FINISHME: will fail.
		   */
		  existing->constant_value =
		     var->constant_value->clone(ralloc_parent(existing), NULL);
	    }

	    if (existing->invariant != var->invariant) {
	       linker_error(prog, "declarations for %s `%s' have "
			    "mismatching invariant qualifiers\n",
			    mode_string(var), var->name);
	       return false;
	    }
            if (existing->centroid != var->centroid) {
               linker_error(prog, "declarations for %s `%s' have "
			    "mismatching centroid qualifiers\n",
			    mode_string(var), var->name);
               return false;
            }
	 } else
	    variables.add_variable(var);
      }
   }

   return true;
}


/**
 * Perform validation of uniforms used across multiple shader stages
 */
bool
cross_validate_uniforms(struct gl_shader_program *prog)
{
   return cross_validate_globals(prog, prog->_LinkedShaders,
				 MESA_SHADER_TYPES, true);
}


/**
 * Validate that outputs from one stage match inputs of another
 */
bool
cross_validate_outputs_to_inputs(struct gl_shader_program *prog,
				 gl_shader *producer, gl_shader *consumer)
{
   glsl_symbol_table parameters;
   /* FINISHME: Figure these out dynamically. */
   const char *const producer_stage = "vertex";
   const char *const consumer_stage = "fragment";

   /* Find all shader outputs in the "producer" stage.
    */
   foreach_list(node, producer->ir) {
      ir_variable *const var = ((ir_instruction *) node)->as_variable();

      /* FINISHME: For geometry shaders, this should also look for inout
       * FINISHME: variables.
       */
      if ((var == NULL) || (var->mode != ir_var_out))
	 continue;

      parameters.add_variable(var);
   }


   /* Find all shader inputs in the "consumer" stage.  Any variables that have
    * matching outputs already in the symbol table must have the same type and
    * qualifiers.
    */
   foreach_list(node, consumer->ir) {
      ir_variable *const input = ((ir_instruction *) node)->as_variable();

      /* FINISHME: For geometry shaders, this should also look for inout
       * FINISHME: variables.
       */
      if ((input == NULL) || (input->mode != ir_var_in))
	 continue;

      ir_variable *const output = parameters.get_variable(input->name);
      if (output != NULL) {
	 /* Check that the types match between stages.
	  */
	 if (input->type != output->type) {
	    /* There is a bit of a special case for gl_TexCoord.  This
	     * built-in is unsized by default.  Applications that variable
	     * access it must redeclare it with a size.  There is some
	     * language in the GLSL spec that implies the fragment shader
	     * and vertex shader do not have to agree on this size.  Other
	     * driver behave this way, and one or two applications seem to
	     * rely on it.
	     *
	     * Neither declaration needs to be modified here because the array
	     * sizes are fixed later when update_array_sizes is called.
	     *
	     * From page 48 (page 54 of the PDF) of the GLSL 1.10 spec:
	     *
	     *     "Unlike user-defined varying variables, the built-in
	     *     varying variables don't have a strict one-to-one
	     *     correspondence between the vertex language and the
	     *     fragment language."
	     */
	    if (!output->type->is_array()
		|| (strncmp("gl_", output->name, 3) != 0)) {
	       linker_error(prog,
			    "%s shader output `%s' declared as type `%s', "
			    "but %s shader input declared as type `%s'\n",
			    producer_stage, output->name,
			    output->type->name,
			    consumer_stage, input->type->name);
	       return false;
	    }
	 }

	 /* Check that all of the qualifiers match between stages.
	  */
	 if (input->centroid != output->centroid) {
	    linker_error(prog,
			 "%s shader output `%s' %s centroid qualifier, "
			 "but %s shader input %s centroid qualifier\n",
			 producer_stage,
			 output->name,
			 (output->centroid) ? "has" : "lacks",
			 consumer_stage,
			 (input->centroid) ? "has" : "lacks");
	    return false;
	 }

	 if (input->invariant != output->invariant) {
	    linker_error(prog,
			 "%s shader output `%s' %s invariant qualifier, "
			 "but %s shader input %s invariant qualifier\n",
			 producer_stage,
			 output->name,
			 (output->invariant) ? "has" : "lacks",
			 consumer_stage,
			 (input->invariant) ? "has" : "lacks");
	    return false;
	 }

	 if (input->interpolation != output->interpolation) {
	    linker_error(prog,
			 "%s shader output `%s' specifies %s "
			 "interpolation qualifier, "
			 "but %s shader input specifies %s "
			 "interpolation qualifier\n",
			 producer_stage,
			 output->name,
			 output->interpolation_string(),
			 consumer_stage,
			 input->interpolation_string());
	    return false;
	 }
      }
   }

   return true;
}


/**
 * Populates a shaders symbol table with all global declarations
 */
static void
populate_symbol_table(gl_shader *sh)
{
   sh->symbols = new(sh) glsl_symbol_table;

   foreach_list(node, sh->ir) {
      ir_instruction *const inst = (ir_instruction *) node;
      ir_variable *var;
      ir_function *func;

      if ((func = inst->as_function()) != NULL) {
	 sh->symbols->add_function(func);
      } else if ((var = inst->as_variable()) != NULL) {
	 sh->symbols->add_variable(var);
      }
   }
}


/**
 * Remap variables referenced in an instruction tree
 *
 * This is used when instruction trees are cloned from one shader and placed in
 * another.  These trees will contain references to \c ir_variable nodes that
 * do not exist in the target shader.  This function finds these \c ir_variable
 * references and replaces the references with matching variables in the target
 * shader.
 *
 * If there is no matching variable in the target shader, a clone of the
 * \c ir_variable is made and added to the target shader.  The new variable is
 * added to \b both the instruction stream and the symbol table.
 *
 * \param inst         IR tree that is to be processed.
 * \param symbols      Symbol table containing global scope symbols in the
 *                     linked shader.
 * \param instructions Instruction stream where new variable declarations
 *                     should be added.
 */
void
remap_variables(ir_instruction *inst, struct gl_shader *target,
		hash_table *temps)
{
   class remap_visitor : public ir_hierarchical_visitor {
   public:
	 remap_visitor(struct gl_shader *target,
		    hash_table *temps)
      {
	 this->target = target;
	 this->symbols = target->symbols;
	 this->instructions = target->ir;
	 this->temps = temps;
      }

      virtual ir_visitor_status visit(ir_dereference_variable *ir)
      {
	 if (ir->var->mode == ir_var_temporary) {
	    ir_variable *var = (ir_variable *) hash_table_find(temps, ir->var);

	    assert(var != NULL);
	    ir->var = var;
	    return visit_continue;
	 }

	 ir_variable *const existing =
	    this->symbols->get_variable(ir->var->name);
	 if (existing != NULL)
	    ir->var = existing;
	 else {
	    ir_variable *copy = ir->var->clone(this->target, NULL);

	    this->symbols->add_variable(copy);
	    this->instructions->push_head(copy);
	    ir->var = copy;
	 }

	 return visit_continue;
      }

   private:
      struct gl_shader *target;
      glsl_symbol_table *symbols;
      exec_list *instructions;
      hash_table *temps;
   };

   remap_visitor v(target, temps);

   inst->accept(&v);
}


/**
 * Move non-declarations from one instruction stream to another
 *
 * The intended usage pattern of this function is to pass the pointer to the
 * head sentinel of a list (i.e., a pointer to the list cast to an \c exec_node
 * pointer) for \c last and \c false for \c make_copies on the first
 * call.  Successive calls pass the return value of the previous call for
 * \c last and \c true for \c make_copies.
 *
 * \param instructions Source instruction stream
 * \param last         Instruction after which new instructions should be
 *                     inserted in the target instruction stream
 * \param make_copies  Flag selecting whether instructions in \c instructions
 *                     should be copied (via \c ir_instruction::clone) into the
 *                     target list or moved.
 *
 * \return
 * The new "last" instruction in the target instruction stream.  This pointer
 * is suitable for use as the \c last parameter of a later call to this
 * function.
 */
exec_node *
move_non_declarations(exec_list *instructions, exec_node *last,
		      bool make_copies, gl_shader *target)
{
   hash_table *temps = NULL;

   if (make_copies)
      temps = hash_table_ctor(0, hash_table_pointer_hash,
			      hash_table_pointer_compare);

   foreach_list_safe(node, instructions) {
      ir_instruction *inst = (ir_instruction *) node;

      if (inst->as_function())
	 continue;

      ir_variable *var = inst->as_variable();
      if ((var != NULL) && (var->mode != ir_var_temporary))
	 continue;

      assert(inst->as_assignment()
	     || ((var != NULL) && (var->mode == ir_var_temporary)));

      if (make_copies) {
	 inst = inst->clone(target, NULL);

	 if (var != NULL)
	    hash_table_insert(temps, inst, var);
	 else
	    remap_variables(inst, target, temps);
      } else {
	 inst->remove();
      }

      last->insert_after(inst);
      last = inst;
   }

   if (make_copies)
      hash_table_dtor(temps);

   return last;
}

/**
 * Get the function signature for main from a shader
 */
static ir_function_signature *
get_main_function_signature(gl_shader *sh)
{
   ir_function *const f = sh->symbols->get_function("main");
   if (f != NULL) {
      exec_list void_parameters;

      /* Look for the 'void main()' signature and ensure that it's defined.
       * This keeps the linker from accidentally pick a shader that just
       * contains a prototype for main.
       *
       * We don't have to check for multiple definitions of main (in multiple
       * shaders) because that would have already been caught above.
       */
      ir_function_signature *sig = f->matching_signature(&void_parameters);
      if ((sig != NULL) && sig->is_defined) {
	 return sig;
      }
   }

   return NULL;
}


/**
 * Combine a group of shaders for a single stage to generate a linked shader
 *
 * \note
 * If this function is supplied a single shader, it is cloned, and the new
 * shader is returned.
 */
static struct gl_shader *
link_intrastage_shaders(void *mem_ctx,
			struct gl_context *ctx,
			struct gl_shader_program *prog,
			struct gl_shader **shader_list,
			unsigned num_shaders)
{
   /* Check that global variables defined in multiple shaders are consistent.
    */
   if (!cross_validate_globals(prog, shader_list, num_shaders, false))
      return NULL;

   /* Check that there is only a single definition of each function signature
    * across all shaders.
    */
   for (unsigned i = 0; i < (num_shaders - 1); i++) {
      foreach_list(node, shader_list[i]->ir) {
	 ir_function *const f = ((ir_instruction *) node)->as_function();

	 if (f == NULL)
	    continue;

	 for (unsigned j = i + 1; j < num_shaders; j++) {
	    ir_function *const other =
	       shader_list[j]->symbols->get_function(f->name);

	    /* If the other shader has no function (and therefore no function
	     * signatures) with the same name, skip to the next shader.
	     */
	    if (other == NULL)
	       continue;

	    foreach_iter (exec_list_iterator, iter, *f) {
	       ir_function_signature *sig =
		  (ir_function_signature *) iter.get();

	       if (!sig->is_defined || sig->is_builtin)
		  continue;

	       ir_function_signature *other_sig =
		  other->exact_matching_signature(& sig->parameters);

	       if ((other_sig != NULL) && other_sig->is_defined
		   && !other_sig->is_builtin) {
		  linker_error(prog, "function `%s' is multiply defined",
			       f->name);
		  return NULL;
	       }
	    }
	 }
      }
   }

   /* Find the shader that defines main, and make a clone of it.
    *
    * Starting with the clone, search for undefined references.  If one is
    * found, find the shader that defines it.  Clone the reference and add
    * it to the shader.  Repeat until there are no undefined references or
    * until a reference cannot be resolved.
    */
   gl_shader *main = NULL;
   for (unsigned i = 0; i < num_shaders; i++) {
      if (get_main_function_signature(shader_list[i]) != NULL) {
	 main = shader_list[i];
	 break;
      }
   }

   if (main == NULL) {
      linker_error(prog, "%s shader lacks `main'\n",
		   (shader_list[0]->Type == GL_VERTEX_SHADER)
		   ? "vertex" : "fragment");
      return NULL;
   }

   gl_shader *linked = ctx->Driver.NewShader(NULL, 0, main->Type);
   linked->ir = new(linked) exec_list;
   clone_ir_list(mem_ctx, linked->ir, main->ir);

   populate_symbol_table(linked);

   /* The a pointer to the main function in the final linked shader (i.e., the
    * copy of the original shader that contained the main function).
    */
   ir_function_signature *const main_sig = get_main_function_signature(linked);

   /* Move any instructions other than variable declarations or function
    * declarations into main.
    */
   exec_node *insertion_point =
      move_non_declarations(linked->ir, (exec_node *) &main_sig->body, false,
			    linked);

   for (unsigned i = 0; i < num_shaders; i++) {
      if (shader_list[i] == main)
	 continue;

      insertion_point = move_non_declarations(shader_list[i]->ir,
					      insertion_point, true, linked);
   }

   /* Resolve initializers for global variables in the linked shader.
    */
   unsigned num_linking_shaders = num_shaders;
   for (unsigned i = 0; i < num_shaders; i++)
      num_linking_shaders += shader_list[i]->num_builtins_to_link;

   gl_shader **linking_shaders =
      (gl_shader **) calloc(num_linking_shaders, sizeof(gl_shader *));

   memcpy(linking_shaders, shader_list,
	  sizeof(linking_shaders[0]) * num_shaders);

   unsigned idx = num_shaders;
   for (unsigned i = 0; i < num_shaders; i++) {
      memcpy(&linking_shaders[idx], shader_list[i]->builtins_to_link,
	     sizeof(linking_shaders[0]) * shader_list[i]->num_builtins_to_link);
      idx += shader_list[i]->num_builtins_to_link;
   }

   assert(idx == num_linking_shaders);

   if (!link_function_calls(prog, linked, linking_shaders,
			    num_linking_shaders)) {
      ctx->Driver.DeleteShader(ctx, linked);
      linked = NULL;
   }

   free(linking_shaders);

#ifdef DEBUG
   /* At this point linked should contain all of the linked IR, so
    * validate it to make sure nothing went wrong.
    */
   if (linked)
      validate_ir_tree(linked->ir);
#endif

   /* Make a pass over all variable declarations to ensure that arrays with
    * unspecified sizes have a size specified.  The size is inferred from the
    * max_array_access field.
    */
   if (linked != NULL) {
      class array_sizing_visitor : public ir_hierarchical_visitor {
      public:
	 virtual ir_visitor_status visit(ir_variable *var)
	 {
	    if (var->type->is_array() && (var->type->length == 0)) {
	       const glsl_type *type =
		  glsl_type::get_array_instance(var->type->fields.array,
						var->max_array_access + 1);

	       assert(type != NULL);
	       var->type = type;
	    }

	    return visit_continue;
	 }
      } v;

      v.run(linked->ir);
   }

   return linked;
}


struct uniform_node {
   exec_node link;
   struct gl_uniform *u;
   unsigned slots;
};

/**
 * Update the sizes of linked shader uniform arrays to the maximum
 * array index used.
 *
 * From page 81 (page 95 of the PDF) of the OpenGL 2.1 spec:
 *
 *     If one or more elements of an array are active,
 *     GetActiveUniform will return the name of the array in name,
 *     subject to the restrictions listed above. The type of the array
 *     is returned in type. The size parameter contains the highest
 *     array element index used, plus one. The compiler or linker
 *     determines the highest index used.  There will be only one
 *     active uniform reported by the GL per uniform array.

 */
static void
update_array_sizes(struct gl_shader_program *prog)
{
   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++) {
	 if (prog->_LinkedShaders[i] == NULL)
	    continue;

      foreach_list(node, prog->_LinkedShaders[i]->ir) {
	 ir_variable *const var = ((ir_instruction *) node)->as_variable();

	 if ((var == NULL) || (var->mode != ir_var_uniform &&
			       var->mode != ir_var_in &&
			       var->mode != ir_var_out) ||
	     !var->type->is_array())
	    continue;

	 unsigned int size = var->max_array_access;
	 for (unsigned j = 0; j < MESA_SHADER_TYPES; j++) {
	       if (prog->_LinkedShaders[j] == NULL)
		  continue;

	    foreach_list(node2, prog->_LinkedShaders[j]->ir) {
	       ir_variable *other_var = ((ir_instruction *) node2)->as_variable();
	       if (!other_var)
		  continue;

	       if (strcmp(var->name, other_var->name) == 0 &&
		   other_var->max_array_access > size) {
		  size = other_var->max_array_access;
	       }
	    }
	 }

	 if (size + 1 != var->type->fields.array->length) {
	    /* If this is a built-in uniform (i.e., it's backed by some
	     * fixed-function state), adjust the number of state slots to
	     * match the new array size.  The number of slots per array entry
	     * is not known.  It seems safe to assume that the total number of
	     * slots is an integer multiple of the number of array elements.
	     * Determine the number of slots per array element by dividing by
	     * the old (total) size.
	     */
	    if (var->num_state_slots > 0) {
	       var->num_state_slots = (size + 1)
		  * (var->num_state_slots / var->type->length);
	    }

	    var->type = glsl_type::get_array_instance(var->type->fields.array,
						      size + 1);
	    /* FINISHME: We should update the types of array
	     * dereferences of this variable now.
	     */
	 }
      }
   }
}

static void
add_uniform(void *mem_ctx, exec_list *uniforms, struct hash_table *ht,
	    const char *name, const glsl_type *type, GLenum shader_type,
	    unsigned *next_shader_pos, unsigned *total_uniforms)
{
   if (type->is_record()) {
      for (unsigned int i = 0; i < type->length; i++) {
	 const glsl_type *field_type = type->fields.structure[i].type;
	 char *field_name = ralloc_asprintf(mem_ctx, "%s.%s", name,
					    type->fields.structure[i].name);

	 add_uniform(mem_ctx, uniforms, ht, field_name, field_type,
		     shader_type, next_shader_pos, total_uniforms);
      }
   } else {
      uniform_node *n = (uniform_node *) hash_table_find(ht, name);
      unsigned int vec4_slots;
      const glsl_type *array_elem_type = NULL;

      if (type->is_array()) {
	 array_elem_type = type->fields.array;
	 /* Array of structures. */
	 if (array_elem_type->is_record()) {
	    for (unsigned int i = 0; i < type->length; i++) {
	       char *elem_name = ralloc_asprintf(mem_ctx, "%s[%d]", name, i);
	       add_uniform(mem_ctx, uniforms, ht, elem_name, array_elem_type,
			   shader_type, next_shader_pos, total_uniforms);
	    }
	    return;
	 }
      }

      /* Fix the storage size of samplers at 1 vec4 each. Be sure to pad out
       * vectors to vec4 slots.
       */
      if (type->is_array()) {
	 if (array_elem_type->is_sampler())
	    vec4_slots = type->length;
	 else
	    vec4_slots = type->length * array_elem_type->matrix_columns;
      } else if (type->is_sampler()) {
	 vec4_slots = 1;
      } else {
	 vec4_slots = type->matrix_columns;
      }

      if (n == NULL) {
	 n = (uniform_node *) calloc(1, sizeof(struct uniform_node));
	 n->u = (gl_uniform *) calloc(1, sizeof(struct gl_uniform));
	 n->slots = vec4_slots;

	 n->u->Name = strdup(name);
	 n->u->Type = type;
	 n->u->VertPos = -1;
	 n->u->FragPos = -1;
	 n->u->GeomPos = -1;
	 (*total_uniforms)++;

	 hash_table_insert(ht, n, name);
	 uniforms->push_tail(& n->link);
      }

      switch (shader_type) {
      case GL_VERTEX_SHADER:
	 n->u->VertPos = *next_shader_pos;
	 break;
      case GL_FRAGMENT_SHADER:
	 n->u->FragPos = *next_shader_pos;
	 break;
      case GL_GEOMETRY_SHADER:
	 n->u->GeomPos = *next_shader_pos;
	 break;
      }

      (*next_shader_pos) += vec4_slots;
   }
}

void
assign_uniform_locations(struct gl_shader_program *prog)
{
   /* */
   exec_list uniforms;
   unsigned total_uniforms = 0;
   hash_table *ht = hash_table_ctor(32, hash_table_string_hash,
				    hash_table_string_compare);
   void *mem_ctx = ralloc_context(NULL);

   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++) {
      if (prog->_LinkedShaders[i] == NULL)
	 continue;

      unsigned next_position = 0;

      foreach_list(node, prog->_LinkedShaders[i]->ir) {
	 ir_variable *const var = ((ir_instruction *) node)->as_variable();

	 if ((var == NULL) || (var->mode != ir_var_uniform))
	    continue;

	 if (strncmp(var->name, "gl_", 3) == 0) {
	    /* At the moment, we don't allocate uniform locations for
	     * builtin uniforms.  It's permitted by spec, and we'll
	     * likely switch to doing that at some point, but not yet.
	     */
	    continue;
	 }

	 var->location = next_position;
	 add_uniform(mem_ctx, &uniforms, ht, var->name, var->type,
		     prog->_LinkedShaders[i]->Type,
		     &next_position, &total_uniforms);
      }
   }

   ralloc_free(mem_ctx);

   gl_uniform_list *ul = (gl_uniform_list *)
      calloc(1, sizeof(gl_uniform_list));

   ul->Size = total_uniforms;
   ul->NumUniforms = total_uniforms;
   ul->Uniforms = (gl_uniform *) calloc(total_uniforms, sizeof(gl_uniform));

   unsigned idx = 0;
   uniform_node *next;
   for (uniform_node *node = (uniform_node *) uniforms.head
	   ; node->link.next != NULL
	   ; node = next) {
      next = (uniform_node *) node->link.next;

      node->link.remove();
      memcpy(&ul->Uniforms[idx], node->u, sizeof(gl_uniform));
      idx++;

      free(node->u);
      free(node);
   }

   hash_table_dtor(ht);

   prog->Uniforms = ul;
}


/**
 * Find a contiguous set of available bits in a bitmask.
 *
 * \param used_mask     Bits representing used (1) and unused (0) locations
 * \param needed_count  Number of contiguous bits needed.
 *
 * \return
 * Base location of the available bits on success or -1 on failure.
 */
int
find_available_slots(unsigned used_mask, unsigned needed_count)
{
   unsigned needed_mask = (1 << needed_count) - 1;
   const int max_bit_to_test = (8 * sizeof(used_mask)) - needed_count;

   /* The comparison to 32 is redundant, but without it GCC emits "warning:
    * cannot optimize possibly infinite loops" for the loop below.
    */
   if ((needed_count == 0) || (max_bit_to_test < 0) || (max_bit_to_test > 32))
      return -1;

   for (int i = 0; i <= max_bit_to_test; i++) {
      if ((needed_mask & ~used_mask) == needed_mask)
	 return i;

      needed_mask <<= 1;
   }

   return -1;
}


/**
 * Assign locations for either VS inputs for FS outputs
 *
 * \param prog          Shader program whose variables need locations assigned
 * \param target_index  Selector for the program target to receive location
 *                      assignmnets.  Must be either \c MESA_SHADER_VERTEX or
 *                      \c MESA_SHADER_FRAGMENT.
 * \param max_index     Maximum number of generic locations.  This corresponds
 *                      to either the maximum number of draw buffers or the
 *                      maximum number of generic attributes.
 *
 * \return
 * If locations are successfully assigned, true is returned.  Otherwise an
 * error is emitted to the shader link log and false is returned.
 *
 * \bug
 * Locations set via \c glBindFragDataLocation are not currently supported.
 * Only locations assigned automatically by the linker, explicitly set by a
 * layout qualifier, or explicitly set by a built-in variable (e.g., \c
 * gl_FragColor) are supported for fragment shaders.
 */
bool
assign_attribute_or_color_locations(gl_shader_program *prog,
				    unsigned target_index,
				    unsigned max_index)
{
   /* Mark invalid locations as being used.
    */
   unsigned used_locations = (max_index >= 32)
      ? ~0 : ~((1 << max_index) - 1);

   assert((target_index == MESA_SHADER_VERTEX)
	  || (target_index == MESA_SHADER_FRAGMENT));

   gl_shader *const sh = prog->_LinkedShaders[target_index];
   if (sh == NULL)
      return true;

   /* Operate in a total of four passes.
    *
    * 1. Invalidate the location assignments for all vertex shader inputs.
    *
    * 2. Assign locations for inputs that have user-defined (via
    *    glBindVertexAttribLocation) locations.
    *
    * 3. Sort the attributes without assigned locations by number of slots
    *    required in decreasing order.  Fragmentation caused by attribute
    *    locations assigned by the application may prevent large attributes
    *    from having enough contiguous space.
    *
    * 4. Assign locations to any inputs without assigned locations.
    */

   const int generic_base = (target_index == MESA_SHADER_VERTEX)
      ? (int) VERT_ATTRIB_GENERIC0 : (int) FRAG_RESULT_DATA0;

   const enum ir_variable_mode direction =
      (target_index == MESA_SHADER_VERTEX) ? ir_var_in : ir_var_out;


   invalidate_variable_locations(sh, direction, generic_base);

   if ((target_index == MESA_SHADER_VERTEX) && (prog->Attributes != NULL)) {
      for (unsigned i = 0; i < prog->Attributes->NumParameters; i++) {
	 ir_variable *const var =
	    sh->symbols->get_variable(prog->Attributes->Parameters[i].Name);

	 /* Note: attributes that occupy multiple slots, such as arrays or
	  * matrices, may appear in the attrib array multiple times.
	  */
	 if ((var == NULL) || (var->location != -1))
	    continue;

	 /* From page 61 of the OpenGL 4.0 spec:
	  *
	  *     "LinkProgram will fail if the attribute bindings assigned by
	  *     BindAttribLocation do not leave not enough space to assign a
	  *     location for an active matrix attribute or an active attribute
	  *     array, both of which require multiple contiguous generic
	  *     attributes."
	  *
	  * Previous versions of the spec contain similar language but omit the
	  * bit about attribute arrays.
	  *
	  * Page 61 of the OpenGL 4.0 spec also says:
	  *
	  *     "It is possible for an application to bind more than one
	  *     attribute name to the same location. This is referred to as
	  *     aliasing. This will only work if only one of the aliased
	  *     attributes is active in the executable program, or if no path
	  *     through the shader consumes more than one attribute of a set
	  *     of attributes aliased to the same location. A link error can
	  *     occur if the linker determines that every path through the
	  *     shader consumes multiple aliased attributes, but
	  *     implementations are not required to generate an error in this
	  *     case."
	  *
	  * These two paragraphs are either somewhat contradictory, or I don't
	  * fully understand one or both of them.
	  */
	 /* FINISHME: The code as currently written does not support attribute
	  * FINISHME: location aliasing (see comment above).
	  */
	 const int attr = prog->Attributes->Parameters[i].StateIndexes[0];
	 const unsigned slots = count_attribute_slots(var->type);

	 /* Mask representing the contiguous slots that will be used by this
	  * attribute.
	  */
	 const unsigned use_mask = (1 << slots) - 1;

	 /* Generate a link error if the set of bits requested for this
	  * attribute overlaps any previously allocated bits.
	  */
	 if ((~(use_mask << attr) & used_locations) != used_locations) {
	    linker_error(prog,
			 "insufficient contiguous attribute locations "
			 "available for vertex shader input `%s'",
			 var->name);
	    return false;
	 }

	 var->location = VERT_ATTRIB_GENERIC0 + attr;
	 used_locations |= (use_mask << attr);
      }
   }

   /* Temporary storage for the set of attributes that need locations assigned.
    */
   struct temp_attr {
      unsigned slots;
      ir_variable *var;

      /* Used below in the call to qsort. */
      static int compare(const void *a, const void *b)
      {
	 const temp_attr *const l = (const temp_attr *) a;
	 const temp_attr *const r = (const temp_attr *) b;

	 /* Reversed because we want a descending order sort below. */
	 return r->slots - l->slots;
      }
   } to_assign[16];

   unsigned num_attr = 0;

   foreach_list(node, sh->ir) {
      ir_variable *const var = ((ir_instruction *) node)->as_variable();

      if ((var == NULL) || (var->mode != (unsigned) direction))
	 continue;

      if (var->explicit_location) {
	 const unsigned slots = count_attribute_slots(var->type);
	 const unsigned use_mask = (1 << slots) - 1;
	 const int attr = var->location - generic_base;

	 if ((var->location >= (int)(max_index + generic_base))
	     || (var->location < 0)) {
	    linker_error(prog,
			 "invalid explicit location %d specified for `%s'\n",
			 (var->location < 0) ? var->location : attr,
			 var->name);
	    return false;
	 } else if (var->location >= generic_base) {
	    used_locations |= (use_mask << attr);
	 }
      }

      /* The location was explicitly assigned, nothing to do here.
       */
      if (var->location != -1)
	 continue;

      to_assign[num_attr].slots = count_attribute_slots(var->type);
      to_assign[num_attr].var = var;
      num_attr++;
   }

   /* If all of the attributes were assigned locations by the application (or
    * are built-in attributes with fixed locations), return early.  This should
    * be the common case.
    */
   if (num_attr == 0)
      return true;

   qsort(to_assign, num_attr, sizeof(to_assign[0]), temp_attr::compare);

   if (target_index == MESA_SHADER_VERTEX) {
      /* VERT_ATTRIB_GENERIC0 is a pseudo-alias for VERT_ATTRIB_POS.  It can
       * only be explicitly assigned by via glBindAttribLocation.  Mark it as
       * reserved to prevent it from being automatically allocated below.
       */
      find_deref_visitor find("gl_Vertex");
      find.run(sh->ir);
      if (find.variable_found())
	 used_locations |= (1 << 0);
   }

   for (unsigned i = 0; i < num_attr; i++) {
      /* Mask representing the contiguous slots that will be used by this
       * attribute.
       */
      const unsigned use_mask = (1 << to_assign[i].slots) - 1;

      int location = find_available_slots(used_locations, to_assign[i].slots);

      if (location < 0) {
	 const char *const string = (target_index == MESA_SHADER_VERTEX)
	    ? "vertex shader input" : "fragment shader output";

	 linker_error(prog,
		      "insufficient contiguous attribute locations "
		      "available for %s `%s'",
		      string, to_assign[i].var->name);
	 return false;
      }

      to_assign[i].var->location = generic_base + location;
      used_locations |= (use_mask << location);
   }

   return true;
}


/**
 * Demote shader inputs and outputs that are not used in other stages
 */
void
demote_shader_inputs_and_outputs(gl_shader *sh, enum ir_variable_mode mode)
{
   foreach_list(node, sh->ir) {
      ir_variable *const var = ((ir_instruction *) node)->as_variable();

      if ((var == NULL) || (var->mode != int(mode)))
	 continue;

      /* A shader 'in' or 'out' variable is only really an input or output if
       * its value is used by other shader stages.  This will cause the variable
       * to have a location assigned.
       */
      if (var->location == -1) {
	 var->mode = ir_var_auto;
      }
   }
}


bool
assign_varying_locations(struct gl_context *ctx,
			 struct gl_shader_program *prog,
			 gl_shader *producer, gl_shader *consumer)
{
   /* FINISHME: Set dynamically when geometry shader support is added. */
   unsigned output_index = VERT_RESULT_VAR0;
   unsigned input_index = FRAG_ATTRIB_VAR0;

   /* Operate in a total of three passes.
    *
    * 1. Assign locations for any matching inputs and outputs.
    *
    * 2. Mark output variables in the producer that do not have locations as
    *    not being outputs.  This lets the optimizer eliminate them.
    *
    * 3. Mark input variables in the consumer that do not have locations as
    *    not being inputs.  This lets the optimizer eliminate them.
    */

   invalidate_variable_locations(producer, ir_var_out, VERT_RESULT_VAR0);
   invalidate_variable_locations(consumer, ir_var_in, FRAG_ATTRIB_VAR0);

   foreach_list(node, producer->ir) {
      ir_variable *const output_var = ((ir_instruction *) node)->as_variable();

      if ((output_var == NULL) || (output_var->mode != ir_var_out)
	  || (output_var->location != -1))
	 continue;

      ir_variable *const input_var =
	 consumer->symbols->get_variable(output_var->name);

      if ((input_var == NULL) || (input_var->mode != ir_var_in))
	 continue;

      assert(input_var->location == -1);

      output_var->location = output_index;
      input_var->location = input_index;

      /* FINISHME: Support for "varying" records in GLSL 1.50. */
      assert(!output_var->type->is_record());

      if (output_var->type->is_array()) {
	 const unsigned slots = output_var->type->length
	    * output_var->type->fields.array->matrix_columns;

	 output_index += slots;
	 input_index += slots;
      } else {
	 const unsigned slots = output_var->type->matrix_columns;

	 output_index += slots;
	 input_index += slots;
      }
   }

   unsigned varying_vectors = 0;

   foreach_list(node, consumer->ir) {
      ir_variable *const var = ((ir_instruction *) node)->as_variable();

      if ((var == NULL) || (var->mode != ir_var_in))
	 continue;

      if (var->location == -1) {
	 if (prog->Version <= 120) {
	    /* On page 25 (page 31 of the PDF) of the GLSL 1.20 spec:
	     *
	     *     Only those varying variables used (i.e. read) in
	     *     the fragment shader executable must be written to
	     *     by the vertex shader executable; declaring
	     *     superfluous varying variables in a vertex shader is
	     *     permissible.
	     *
	     * We interpret this text as meaning that the VS must
	     * write the variable for the FS to read it.  See
	     * "glsl1-varying read but not written" in piglit.
	     */

	    linker_error(prog, "fragment shader varying %s not written "
			 "by vertex shader\n.", var->name);
	 }

	 /* An 'in' variable is only really a shader input if its
	  * value is written by the previous stage.
	  */
	 var->mode = ir_var_auto;
      } else {
	 /* The packing rules are used for vertex shader inputs are also used
	  * for fragment shader inputs.
	  */
	 varying_vectors += count_attribute_slots(var->type);
      }
   }

   if (ctx->API == API_OPENGLES2 || prog->Version == 100) {
      if (varying_vectors > ctx->Const.MaxVarying) {
	 linker_error(prog, "shader uses too many varying vectors "
		      "(%u > %u)\n",
		      varying_vectors, ctx->Const.MaxVarying);
	 return false;
      }
   } else {
      const unsigned float_components = varying_vectors * 4;
      if (float_components > ctx->Const.MaxVarying * 4) {
	 linker_error(prog, "shader uses too many varying components "
		      "(%u > %u)\n",
		      float_components, ctx->Const.MaxVarying * 4);
	 return false;
      }
   }

   return true;
}


void
link_shaders(struct gl_context *ctx, struct gl_shader_program *prog)
{
   void *mem_ctx = ralloc_context(NULL); // temporary linker context

   prog->LinkStatus = false;
   prog->Validated = false;
   prog->_Used = false;

   if (prog->InfoLog != NULL)
      ralloc_free(prog->InfoLog);

   prog->InfoLog = ralloc_strdup(NULL, "");

   /* Separate the shaders into groups based on their type.
    */
   struct gl_shader **vert_shader_list;
   unsigned num_vert_shaders = 0;
   struct gl_shader **frag_shader_list;
   unsigned num_frag_shaders = 0;

   vert_shader_list = (struct gl_shader **)
      calloc(2 * prog->NumShaders, sizeof(struct gl_shader *));
   frag_shader_list =  &vert_shader_list[prog->NumShaders];

   unsigned min_version = UINT_MAX;
   unsigned max_version = 0;
   for (unsigned i = 0; i < prog->NumShaders; i++) {
      min_version = MIN2(min_version, prog->Shaders[i]->Version);
      max_version = MAX2(max_version, prog->Shaders[i]->Version);

      switch (prog->Shaders[i]->Type) {
      case GL_VERTEX_SHADER:
	 vert_shader_list[num_vert_shaders] = prog->Shaders[i];
	 num_vert_shaders++;
	 break;
      case GL_FRAGMENT_SHADER:
	 frag_shader_list[num_frag_shaders] = prog->Shaders[i];
	 num_frag_shaders++;
	 break;
      case GL_GEOMETRY_SHADER:
	 /* FINISHME: Support geometry shaders. */
	 assert(prog->Shaders[i]->Type != GL_GEOMETRY_SHADER);
	 break;
      }
   }

   /* Previous to GLSL version 1.30, different compilation units could mix and
    * match shading language versions.  With GLSL 1.30 and later, the versions
    * of all shaders must match.
    */
   assert(min_version >= 100);
   assert(max_version <= 130);
   if ((max_version >= 130 || min_version == 100)
       && min_version != max_version) {
      linker_error(prog, "all shaders must use same shading "
		   "language version\n");
      goto done;
   }

   prog->Version = max_version;

   for (unsigned int i = 0; i < MESA_SHADER_TYPES; i++) {
      if (prog->_LinkedShaders[i] != NULL)
	 ctx->Driver.DeleteShader(ctx, prog->_LinkedShaders[i]);

      prog->_LinkedShaders[i] = NULL;
   }

   /* Link all shaders for a particular stage and validate the result.
    */
   if (num_vert_shaders > 0) {
      gl_shader *const sh =
	 link_intrastage_shaders(mem_ctx, ctx, prog, vert_shader_list,
				 num_vert_shaders);

      if (sh == NULL)
	 goto done;

      if (!validate_vertex_shader_executable(prog, sh))
	 goto done;

      _mesa_reference_shader(ctx, &prog->_LinkedShaders[MESA_SHADER_VERTEX],
			     sh);
   }

   if (num_frag_shaders > 0) {
      gl_shader *const sh =
	 link_intrastage_shaders(mem_ctx, ctx, prog, frag_shader_list,
				 num_frag_shaders);

      if (sh == NULL)
	 goto done;

      if (!validate_fragment_shader_executable(prog, sh))
	 goto done;

      _mesa_reference_shader(ctx, &prog->_LinkedShaders[MESA_SHADER_FRAGMENT],
			     sh);
   }

   /* Here begins the inter-stage linking phase.  Some initial validation is
    * performed, then locations are assigned for uniforms, attributes, and
    * varyings.
    */
   if (cross_validate_uniforms(prog)) {
      unsigned prev;

      for (prev = 0; prev < MESA_SHADER_TYPES; prev++) {
	 if (prog->_LinkedShaders[prev] != NULL)
	    break;
      }

      /* Validate the inputs of each stage with the output of the preceding
       * stage.
       */
      for (unsigned i = prev + 1; i < MESA_SHADER_TYPES; i++) {
	 if (prog->_LinkedShaders[i] == NULL)
	    continue;

	 if (!cross_validate_outputs_to_inputs(prog,
					       prog->_LinkedShaders[prev],
					       prog->_LinkedShaders[i]))
	    goto done;

	 prev = i;
      }

      prog->LinkStatus = true;
   }

   /* Do common optimization before assigning storage for attributes,
    * uniforms, and varyings.  Later optimization could possibly make
    * some of that unused.
    */
   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++) {
      if (prog->_LinkedShaders[i] == NULL)
	 continue;

      detect_recursion_linked(prog, prog->_LinkedShaders[i]->ir);
      if (!prog->LinkStatus)
	 goto done;

      while (do_common_optimization(prog->_LinkedShaders[i]->ir, true, 32))
	 ;
   }

   update_array_sizes(prog);

   assign_uniform_locations(prog);

   /* FINISHME: The value of the max_attribute_index parameter is
    * FINISHME: implementation dependent based on the value of
    * FINISHME: GL_MAX_VERTEX_ATTRIBS.  GL_MAX_VERTEX_ATTRIBS must be
    * FINISHME: at least 16, so hardcode 16 for now.
    */
   if (!assign_attribute_or_color_locations(prog, MESA_SHADER_VERTEX, 16)) {
      goto done;
   }

   if (!assign_attribute_or_color_locations(prog, MESA_SHADER_FRAGMENT, ctx->Const.MaxDrawBuffers)) {
      goto done;
   }

   unsigned prev;
   for (prev = 0; prev < MESA_SHADER_TYPES; prev++) {
      if (prog->_LinkedShaders[prev] != NULL)
	 break;
   }

   for (unsigned i = prev + 1; i < MESA_SHADER_TYPES; i++) {
      if (prog->_LinkedShaders[i] == NULL)
	 continue;

      if (!assign_varying_locations(ctx, prog,
				    prog->_LinkedShaders[prev],
				    prog->_LinkedShaders[i])) {
	 goto done;
      }

      prev = i;
   }

   if (prog->_LinkedShaders[MESA_SHADER_VERTEX] != NULL) {
      demote_shader_inputs_and_outputs(prog->_LinkedShaders[MESA_SHADER_VERTEX],
				       ir_var_out);
   }

   if (prog->_LinkedShaders[MESA_SHADER_GEOMETRY] != NULL) {
      gl_shader *const sh = prog->_LinkedShaders[MESA_SHADER_GEOMETRY];

      demote_shader_inputs_and_outputs(sh, ir_var_in);
      demote_shader_inputs_and_outputs(sh, ir_var_inout);
      demote_shader_inputs_and_outputs(sh, ir_var_out);
   }

   if (prog->_LinkedShaders[MESA_SHADER_FRAGMENT] != NULL) {
      gl_shader *const sh = prog->_LinkedShaders[MESA_SHADER_FRAGMENT];

      demote_shader_inputs_and_outputs(sh, ir_var_in);
   }

   /* OpenGL ES requires that a vertex shader and a fragment shader both be
    * present in a linked program.  By checking for use of shading language
    * version 1.00, we also catch the GL_ARB_ES2_compatibility case.
    */
   if (ctx->API == API_OPENGLES2 || prog->Version == 100) {
      if (prog->_LinkedShaders[MESA_SHADER_VERTEX] == NULL) {
	 linker_error(prog, "program lacks a vertex shader\n");
      } else if (prog->_LinkedShaders[MESA_SHADER_FRAGMENT] == NULL) {
	 linker_error(prog, "program lacks a fragment shader\n");
      }
   }

   /* FINISHME: Assign fragment shader output locations. */

done:
   free(vert_shader_list);

   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++) {
      if (prog->_LinkedShaders[i] == NULL)
	 continue;

      /* Retain any live IR, but trash the rest. */
      reparent_ir(prog->_LinkedShaders[i]->ir, prog->_LinkedShaders[i]->ir);
   }

   ralloc_free(mem_ctx);
}
