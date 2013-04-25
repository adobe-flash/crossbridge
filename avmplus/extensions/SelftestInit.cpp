/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is [Open Source Virtual Machine.].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Adobe AS3 Team
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
// Initialization code for generated selftest code
#include "avmshell.h"
namespace avmplus {
#ifdef VMCFG_SELFTEST
namespace ST_avmplus_basics {
extern void create_avmplus_basics(AvmCore* core);
}
namespace ST_avmplus_builtins {
extern void create_avmplus_builtins(AvmCore* core);
}
#if defined AVMPLUS_PEEPHOLE_OPTIMIZER
namespace ST_avmplus_peephole {
extern void create_avmplus_peephole(AvmCore* core);
}
#endif
namespace ST_avmplus_vector_accessors {
extern void create_avmplus_vector_accessors(AvmCore* core);
}
#if defined AVMPLUS_WIN32
#if !defined VMCFG_ARM
namespace ST_mmgc_bugzilla_543560 {
extern void create_mmgc_bugzilla_543560(AvmCore* core);
}
#endif
#endif
namespace ST_mmgc_bugzilla_575631 {
extern void create_mmgc_bugzilla_575631(AvmCore* core);
}
#if defined AVMPLUS_64BIT && defined DEBUG
namespace ST_mmgc_bugzilla_580603 {
extern void create_mmgc_bugzilla_580603(AvmCore* core);
}
#endif
#if defined DEBUGGER
namespace ST_mmgc_bugzilla_603411 {
extern void create_mmgc_bugzilla_603411(AvmCore* core);
}
#endif
namespace ST_mmgc_bugzilla_637993 {
extern void create_mmgc_bugzilla_637993(AvmCore* core);
}
namespace ST_mmgc_basics {
extern void create_mmgc_basics(AvmCore* core);
}
namespace ST_mmgc_dependent {
extern void create_mmgc_dependent(AvmCore* core);
}
namespace ST_mmgc_exact {
extern void create_mmgc_exact(AvmCore* core);
}
namespace ST_mmgc_externalalloc {
extern void create_mmgc_externalalloc(AvmCore* core);
}
namespace ST_mmgc_finalize_uninit {
extern void create_mmgc_finalize_uninit(AvmCore* core);
}
namespace ST_mmgc_fixedmalloc_findbeginning {
extern void create_mmgc_fixedmalloc_findbeginning(AvmCore* core);
}
namespace ST_mmgc_gcheap {
extern void create_mmgc_gcheap(AvmCore* core);
}
namespace ST_mmgc_gcoption {
extern void create_mmgc_gcoption(AvmCore* core);
}
namespace ST_mmgc_mmfx_array {
extern void create_mmgc_mmfx_array(AvmCore* core);
}
#if defined VMCFG_WORKERTHREADS
namespace ST_mmgc_threads {
extern void create_mmgc_threads(AvmCore* core);
}
#endif
namespace ST_mmgc_weakref {
extern void create_mmgc_weakref(AvmCore* core);
}
namespace ST_nanojit_codealloc {
extern void create_nanojit_codealloc(AvmCore* core);
}
namespace ST_vmbase_concurrency {
extern void create_vmbase_concurrency(AvmCore* core);
}
#if defined VMCFG_SAFEPOINTS && defined BUG_754918
namespace ST_vmbase_safepoints {
extern void create_vmbase_safepoints(AvmCore* core);
}
#endif
namespace ST_vmpi_threads {
extern void create_vmpi_threads(AvmCore* core);
}
#if !defined DEBUGGER
namespace ST_workers_Buffers {
extern void create_workers_Buffers(AvmCore* core);
}
#endif
#if !defined DEBUGGER
namespace ST_workers_NoSyncSingleItemBuffer {
extern void create_workers_NoSyncSingleItemBuffer(AvmCore* core);
}
#endif
namespace ST_workers_Promise {
extern void create_workers_Promise(AvmCore* core);
}
void SelftestRunner::createGeneratedSelftestClasses() {
ST_avmplus_basics::create_avmplus_basics(core);
ST_avmplus_builtins::create_avmplus_builtins(core);
#if defined AVMPLUS_PEEPHOLE_OPTIMIZER
ST_avmplus_peephole::create_avmplus_peephole(core);
#endif
ST_avmplus_vector_accessors::create_avmplus_vector_accessors(core);
#if defined AVMPLUS_WIN32
#if !defined VMCFG_ARM
ST_mmgc_bugzilla_543560::create_mmgc_bugzilla_543560(core);
#endif
#endif
ST_mmgc_bugzilla_575631::create_mmgc_bugzilla_575631(core);
#if defined AVMPLUS_64BIT && defined DEBUG
ST_mmgc_bugzilla_580603::create_mmgc_bugzilla_580603(core);
#endif
#if defined DEBUGGER
ST_mmgc_bugzilla_603411::create_mmgc_bugzilla_603411(core);
#endif
ST_mmgc_bugzilla_637993::create_mmgc_bugzilla_637993(core);
ST_mmgc_basics::create_mmgc_basics(core);
ST_mmgc_dependent::create_mmgc_dependent(core);
ST_mmgc_exact::create_mmgc_exact(core);
ST_mmgc_externalalloc::create_mmgc_externalalloc(core);
ST_mmgc_finalize_uninit::create_mmgc_finalize_uninit(core);
ST_mmgc_fixedmalloc_findbeginning::create_mmgc_fixedmalloc_findbeginning(core);
ST_mmgc_gcheap::create_mmgc_gcheap(core);
ST_mmgc_gcoption::create_mmgc_gcoption(core);
ST_mmgc_mmfx_array::create_mmgc_mmfx_array(core);
#if defined VMCFG_WORKERTHREADS
ST_mmgc_threads::create_mmgc_threads(core);
#endif
ST_mmgc_weakref::create_mmgc_weakref(core);
ST_nanojit_codealloc::create_nanojit_codealloc(core);
ST_vmbase_concurrency::create_vmbase_concurrency(core);
#if defined VMCFG_SAFEPOINTS && defined BUG_754918
ST_vmbase_safepoints::create_vmbase_safepoints(core);
#endif
ST_vmpi_threads::create_vmpi_threads(core);
#if !defined DEBUGGER
ST_workers_Buffers::create_workers_Buffers(core);
#endif
#if !defined DEBUGGER
ST_workers_NoSyncSingleItemBuffer::create_workers_NoSyncSingleItemBuffer(core);
#endif
ST_workers_Promise::create_workers_Promise(core);
}
#endif // VMCFG_SELFTEST
}
