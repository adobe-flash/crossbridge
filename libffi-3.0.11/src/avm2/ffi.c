/* -----------------------------------------------------------------------
   ffi.c - Copyright (c) 2011 Anthony Green
           Copyright (c) 1996, 2003-2004, 2007-2008 Red Hat, Inc.
           Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.

   AVM2 Foreign Function Interface

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   ``Software''), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------- */

#include <ffi.h>
#include <ffi_common.h>

#include <stdlib.h>
#include <AS3/AVM2.h>

#define ISPOW2(X) (!((X) & ((X)-1)))

/* Perform machine dependent cif processing */
ffi_status ffi_prep_cif_machdep(ffi_cif *cif)
{
  /* Set the return type flag */
  switch (cif->rtype->type)
    {
    case FFI_TYPE_VOID:
    case FFI_TYPE_FLOAT:
    case FFI_TYPE_DOUBLE:
      cif->flags = (unsigned) cif->rtype->type;
      break;

    case FFI_TYPE_SINT64:
    case FFI_TYPE_UINT64:
      cif->flags = (unsigned) FFI_TYPE_SINT64;
      break;

    case FFI_TYPE_STRUCT:
      if (cif->rtype->size <= 4 && ISPOW2(cif->rtype->size))
	/* A Composite Type not larger than 4 bytes is returned in eax.  */
	cif->flags = (unsigned)FFI_TYPE_INT;
      else if (cif->rtype->size <= 8 && ISPOW2(cif->rtype->size))
	/* A Composite Type not larger than 8 bytes is returned in eax/edx.  */
	cif->flags = (unsigned)FFI_TYPE_UINT64;
      else
	/* A Composite Type larger than 4 bytes, or whose size cannot
	   be determined statically ... is stored in memory at an
	   address passed as arg0.  */
	cif->flags = (unsigned)FFI_TYPE_STRUCT;
      break;

    default:
      cif->flags = FFI_TYPE_INT;
      break;
    }

  cif->nfixedargs = cif->nargs;
  return FFI_OK;
}

/* Perform machine dependent cif processing for variadic calls */
ffi_status ffi_prep_cif_machdep_var(ffi_cif *cif,
				    unsigned int nfixedargs,
				    unsigned int ntotalargs)
{
  ffi_status status;

  if((status = ffi_prep_cif_machdep(cif)) == FFI_OK)
    cif->nfixedargs = nfixedargs;
  return status;
}

void ffi_call(ffi_cif *cif, void (*fn)(void), void *rvalue, void **avalue)
{
  unsigned arg;
  extended_cif ecif;
  int small_struct = ((cif->flags == FFI_TYPE_INT  || cif->flags == FFI_TYPE_UINT64)
		      && cif->rtype->type == FFI_TYPE_STRUCT);

  ecif.cif = cif;
  ecif.avalue = avalue;

  unsigned long long temp;
  
  /* If the return value is a struct and we don't have a return	*/
  /* value address then we need to make one		        */

  if ((rvalue == NULL) && 
      (cif->flags == FFI_TYPE_STRUCT))
      ecif.rvalue = alloca(cif->rtype->size);
  else if (small_struct)
    ecif.rvalue = &temp;
  else
    ecif.rvalue = rvalue;

  switch (cif->abi) 
    {
    case FFI_CDECL: // just verifying here
      break;

    default:
      FFI_ASSERT(0);
      break;
    }

  __asm __volatile__("var args:Vector.<int> = new Vector.<int>");
  if(cif->flags == FFI_TYPE_STRUCT)
    __asm __volatile__("args.push(%0)" :: "r"(ecif.rvalue));
  for(arg = 0; arg < cif->nargs; arg++)
  {
    ffi_type *type = cif->arg_types[arg];

    switch(type->type)
    {
      case FFI_TYPE_UINT8:
        __asm __volatile__("args.push(%0)" :: "r"(*(UINT8*)avalue[arg]));
        break;
      case FFI_TYPE_SINT8:
        __asm __volatile__("args.push(%0)" :: "r"((int)*(SINT8*)avalue[arg]));
        break;
      case FFI_TYPE_UINT16:
        __asm __volatile__("args.push(%0)" :: "r"(*(UINT16*)avalue[arg]));
        break;
      case FFI_TYPE_SINT16:
        __asm __volatile__("args.push(%0)" :: "r"((int)*(SINT16*)avalue[arg]));
        break;
      case FFI_TYPE_FLOAT:
        if(arg < cif->nfixedargs)
          __asm __volatile__("args.push(%0)" :: "r"(*(int*)avalue[arg]));
        else // varargs? promote to double
        {
          double d = *(float*)avalue[arg];
          __asm __volatile__("args.push(%0, %1)" :: "r"(((int*)&d)[0]), "r"(((int*)&d)[1]));
        }
        break;
      default:
      {
        unsigned size = type->size;
        unsigned *data = (unsigned *)avalue[arg];
        while(size)
        {
          unsigned val;
          switch(size)
          {
          case 0:
            FFI_ASSERT(0);
            break;
          case 1:
            val = *(UINT8*)data;
            size = 0;
            break;
          case 2:
            val = *(UINT16*)data;
            size = 0;
            break;
          case 3:
            val = *(UINT16*)data | ((UINT8*)data)[2] << 16;
            size = 0;
            break;
          default:
            val = *data++;
            size -= 4;
            break;
          }
          __asm __volatile__("args.push(%0)" :: "r"(val));
        }
        break;
      }
    }
  }
  if(!rvalue || cif->flags == FFI_TYPE_VOID || cif->flags == FFI_TYPE_STRUCT)
    __asm __volatile__("CModule.callI(%0, args)" :: "r"(fn));
  else
    switch(cif->flags)
    {
      default:
        FFI_ASSERT(0);
        break;
      case FFI_TYPE_UINT64:
      case FFI_TYPE_SINT64:
      {
        unsigned low, high;
        __asm __volatile__("%0 = CModule.callI(%2, args); %1 = edx" : "=r"(low), "=r"(high) : "r"(fn));
        ((unsigned *)ecif.rvalue)[0] = low;
        ((unsigned *)ecif.rvalue)[1] = high;
        break;
      }
      case FFI_TYPE_DOUBLE:
      {
        double d;
        __asm __volatile__("%0 = CModule.callN(%1, args)" : "=r"(d) : "r"(fn));
        *(double *)ecif.rvalue = d;
        break;
      }
      case FFI_TYPE_FLOAT:
      {
        float f;
        __asm __volatile__("%0 = CModule.callN(%1, args)" : "=r"(f) : "r"(fn));
        *(float *)ecif.rvalue = f;
        break;
      }
      case FFI_TYPE_INT:
      {
        unsigned u;
        __asm __volatile__("%0 = CModule.callI(%1, args)" : "=r"(u) : "r"(fn));
        *(unsigned *)ecif.rvalue = u;
        break;
      }
    }
  
  if (small_struct && rvalue)
    memcpy (rvalue, &temp, cif->rtype->size);
}

static void closure_inner (void *trampFunPtr, void *args, ffi_closure *closure)
{
  ffi_cif *cif = closure->cif;
  ffi_type **arg_types = cif->arg_types;
  void **avalue = alloca(cif->nargs * sizeof(void *));
  union
  {
    int i;
    float f;
    double d;
    long long ll;
  } rvalu;
  int i;
  void *rvalue;

  if(cif->flags == FFI_TYPE_STRUCT)
  {
    rvalue = *(void **)args;
    args = 1 + (void **)args;
  }
  else
    rvalue = &rvalu;

  for(i = 0; i < cif->nargs; i++)
  {
    avalue[i] = args;
    args = ((arg_types[i]->size + sizeof(void *) - 1) / sizeof(void *)) + (void **)args;
  }
  (closure->fun) (cif, rvalue, avalue, closure->user_data);

  int rtype = cif->rtype->type;
  if(rtype == FFI_TYPE_STRUCT)
  {
      if (cif->rtype->size <= 4 && ISPOW2(cif->rtype->size))
        /* A Composite Type not larger than 4 bytes is returned in eax.  */
        rtype = FFI_TYPE_INT;
      else if (cif->rtype->size <= 8 && ISPOW2(cif->rtype->size))
        /* A Composite Type not larger than 8 bytes is returned in eax/edx.  */
        rtype = FFI_TYPE_UINT64;
      else            
        /* A Composite Type larger than 4 bytes, or whose size cannot
           be determined statically ... is stored in memory at an
           address passed as arg0.  */
        rtype = FFI_TYPE_VOID;
  }

  switch (rtype)
    {
    case FFI_TYPE_VOID:
      return;
    case FFI_TYPE_FLOAT:
      __asm __volatile__ ("st0 = %0" : : "r"(rvalu.f));
      return;
    case FFI_TYPE_DOUBLE:
      __asm __volatile__ ("st0 = %0" : : "r"(rvalu.d));
     return;
    case FFI_TYPE_SINT64:
    case FFI_TYPE_UINT64:
      __asm __volatile__ ("eax = %0; edx = %1" : : "r"((int)rvalu.ll), "r"((int)(rvalu.ll >> 32)));
      return;
    default:
      __asm __volatile__ ("eax = %0" : : "r"(rvalu.i));
      return;
    }
}

void *ffi_closure_alloc (size_t size, void **code)
{
  ffi_closure *closure = malloc(size);

  *code = closure->trampoline_table_entry = avm2_tramp_alloc(closure_inner, 0, closure, NULL);
  return closure;
}

void ffi_closure_free (void *data)
{
  ffi_closure *closure = data;
  avm2_tramp_free(closure->trampoline_table_entry);
  free(closure);
}

ffi_status
ffi_prep_closure_loc (ffi_closure* closure,
                      ffi_cif* cif,
                      void (*fun)(ffi_cif*,void*,void**,void*),
                      void *user_data,
                      void *codeloc)
{
  if (cif->abi != FFI_CDECL)
    return FFI_BAD_ABI;

  closure->cif  = cif;
  closure->user_data = user_data;
  closure->fun  = fun;

  return FFI_OK;
}

