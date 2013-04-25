/* LLVM LOCAL begin (ENTIRE FILE!)  */
#ifdef ENABLE_LLVM


/* LLVM_TARGET_INTRINSIC_PREFIX - Specify what prefix this target uses for its
 * intrinsics.
 */
#define LLVM_TARGET_INTRINSIC_PREFIX "alpha"

/* LLVM_TARGET_NAME - This specifies the name of the target, which correlates to
 * the llvm::InitializeXXXTarget() function.
 */
#define LLVM_TARGET_NAME Alpha

#define TARGET_ALPHA

/* LLVM_TARGET_INTRINSIC_LOWER - To handle builtins, we want to expand the
 * invocation into normal LLVM code.  If the target can handle the builtin, this
 * macro should call the target TreeToLLVM::TargetIntrinsicLower method and
 *  return true.This macro is invoked from a method in the TreeToLLVM class.
 */
#define LLVM_TARGET_INTRINSIC_LOWER(EXP, BUILTIN_CODE, DESTLOC, RESULT,       \
                                    DESTTY, OPS)                              \
        TargetIntrinsicLower(EXP, BUILTIN_CODE, DESTLOC, RESULT, DESTTY, OPS);

#ifdef LLVM_ABI_H

extern bool llvm_alpha_should_return_scalar_as_shadow(const Type* Ty);

/* check if i128 should be a shadow return */
#define LLVM_SHOULD_RETURN_SCALAR_AS_SHADOW(X) \
  llvm_alpha_should_return_scalar_as_shadow(X)

#endif /* LLVM_ABI_H */

#endif /* ENABLE_LLVM */
/* LLVM LOCAL end (ENTIRE FILE!)  */

