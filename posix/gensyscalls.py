# Copyright (c) 2013 Adobe Systems Inc

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import re
import sys
import contextlib

WHITESPACE = re.compile(r'\s+')
whitespace_or_slash = re.compile(r'\s+|\\?')
TYPE_SEP = re.compile(r'\|')
PTR_CHAR = re.compile(r'\*')
WORD = re.compile(r'\w+')
PARENS_OR_SEMI = re.compile(r'[();]')
COMMA = re.compile(r',')

NO_IMPL_TYPES = set(['UNIMPL', 'OBSOL', 'NODEF', 'NOPROTO'])
ACTIONSCRIPT_PKG = 'flash.utils'
ACTIONSCRIPT_INTERFACE_NAME = 'IKernel'
ACTIONSCRIPT_IMPL_NAME = 'ShellPosix'
VM_GLUE_CLASS_NAME = 'ShellPosixClass'
VM_GLUE_OBJ_NAME = 'ShellPosixObject'
VM_GLUE_INCLUDE_GUARD = 'SHELLPOSIX_INCLUDED'
VM_GLUE_FILE_NAME = 'ShellPosixGlue'

PTR_TYPES = set(['caddr_t'])
_64BIT_TYPES = set(['off_t'])

MAC_ONLY = set(['getdirentries', 'issetugid'])

NO_DELEGATE = set(['nanosleep'])

def mac_clock_gettime_impl(out, args, ret):
    struct = args[1]
    out.write('\t\t(void)A_clock_id;\n')
    out.write('\t\tint {0} = ::gettimeofday((struct timeval *) {1}, NULL);\n'
        .format(ret, struct))
    out.write('\t\t{0}.tv_nsec /= 1000;\n'.format(struct[1:]))

def getcwd_impl(out, args, ret):
    out.write('\t\tint {0} = 0;\n'.format(ret))
    out.write('\t\t::getcwd({0}, {1});\n'.format(args[0], args[1]))

def writev_impl(out, args, ret):
    out.write('\t\tint i;\n')
    out.write('\t\tstruct iovec tmp_iovec[{0}];\n'.format(args[2]))
    out.write('\t\tchar *vecp = (char *)domainMemoryPtr(this, {0});\n'
        .format('A_iovp'))
    out.write('\t\tfor (i = 0; i < {0}; i++) {{\n'.format(args[2]))
    out.write('\t\t\tint nbytes = *(vecp + (i * 8) + 4);\n')
    out.write('\t\t\tchar *tmp = (char *)malloc(nbytes);\n')
    out.write('\t\t\tint offset = *((int *) (vecp + (i * 8)));\n')
    out.write('\t\t\tvoid *datap = domainMemoryPtr(this, offset);\n')
    out.write('\t\t\tmemmove(tmp, (void *) datap, nbytes);\n')
    out.write('\t\t\ttmp_iovec[i].iov_base = tmp;\n')
    out.write('\t\t\ttmp_iovec[i].iov_len = nbytes;\n') 
    out.write('\t\t}\n')
    out.write('\t\tint {0} = ::writev({1}, tmp_iovec, {2});\n'
        .format(ret, args[0], args[2]))
    out.write('\t\tfor (i = 0; i < {0}; i++) {{\n'.format(args[2]))
    out.write('\t\t\tfree(tmp_iovec[i].iov_base);\n')
    out.write('\t\t}\n')
    
def readv_impl(out, args, ret):
    out.write('\t\tint i;\n')
    out.write('\t\tstruct iovec tmp_iovec[{0}];\n'.format(args[2]))
    out.write('\t\tchar *vecp = (char *)domainMemoryPtr(this, {0});\n'
        .format('A_iovp'))
    out.write('\t\tfor (i = 0; i < {0}; i++) {{\n'.format(args[2]))
    out.write('\t\t\tint nbytes = *(vecp + (i * 8) + 4);\n')
    out.write('\t\t\tchar *tmp = (char *)malloc(nbytes);\n')
    out.write('\t\t\ttmp_iovec[i].iov_base = tmp;\n')
    out.write('\t\t\ttmp_iovec[i].iov_len = nbytes;\n') 
    out.write('\t\t}\n')
    out.write('\t\tint {0} = ::readv({1}, tmp_iovec, {2});\n'
        .format(ret, args[0], args[2]))
    out.write('\t\tfor (i = 0; i < {0}; i++) {{\n'.format(args[2]))
    out.write('\t\t\tint offset = *((int *) (vecp + (i * 8)));\n')
    out.write('\t\t\tvoid *datap = domainMemoryPtr(this, offset);\n')
    out.write('\t\t\tmemmove(datap, tmp_iovec[i].iov_base, ' +
                        'tmp_iovec[i].iov_len);\n')
    out.write('\t\t\tfree(tmp_iovec[i].iov_base);\n')
    out.write('\t\t}\n')

MAC_SPECIAL_IMPLS = {'clock_gettime': mac_clock_gettime_impl}
SPECIAL_IMPLS = {'__getcwd': getcwd_impl, 
                    'readv': readv_impl,
                    'writev': writev_impl}    

# This list is produced by print_stat_info.c. For each member of the stat
# structure, it gives the member's name, its offset from the beginning of
# the structure, as well as its size (both in bytes). This script uses
# this information to fill out the stat structure before returning back
# to flascc code, which is necessary because the memory layout of 
# struct stat differs considerably across BSD, Darwin, and Cygwin.
# Note that these values correspond to struct stat as defined by flascc's
# SDK, not the native system headers.
STRUCT_STAT_INFO = [('st_dev', 0, 4),
    ('st_mode', 8, 2),
    ('st_nlink', 10, 2),
    ('st_uid', 12, 4),
    ('st_gid', 16, 4),
    ('st_rdev', 20, 4),
    ('st_size', 48, 8),
    ('st_blocks', 56, 8),
    ('st_blksize', 64, 4),
    ('st_atimespec.tv_sec', 24, 4),
    ('st_atimespec.tv_nsec', 28, 4),
    ('st_mtimespec.tv_sec', 32, 4),
    ('st_mtimespec.tv_nsec', 36, 4),
    ('st_ctimespec.tv_sec', 40, 4),
    ('st_ctimespec.tv_nsec', 44, 4),]

STRUCT_TIMESPEC_INFO =[('tv_sec', 0, 4),
    ('tv_nsec', 4, 4),]

STRUCT_RUSAGE_INFO = [('ru_utime.tv_sec', 0, 4),
    ('ru_utime.tv_usec', 4, 4),
    ('ru_stime.tv_sec', 8, 4),
    ('ru_stime.tv_usec', 12, 4),]

STRUCT_IOVEC_INFO =[('iov_base', 0, 4),
    ('iov_len', 4, 4),]

SUPPORTED_STRUCTS = {'struct stat' : STRUCT_STAT_INFO,
                     'struct timespec' : STRUCT_TIMESPEC_INFO,
                     'struct rusage' : STRUCT_RUSAGE_INFO,
                     'struct iovec' : STRUCT_IOVEC_INFO,
                    }

class Argument:
    def parse_from_str(self, argument_decl):
        # make sure that asterisks in the input become individual tokens
        argument_decl = re.sub(PTR_CHAR, ' * ', argument_decl)

        tokens = WHITESPACE.split(argument_decl.strip())
        self.ptrs = tokens.count('*')
        self.name = 'A_' + tokens[-1]
        type_start = 0
        self.const = tokens[0] == 'const'
        if self.const:
            type_start += 1
        self.struct = tokens[type_start] == 'struct'
        if self.struct:
            type_start += 1
        self.union = tokens[type_start] == 'union'
        if self.union:
            type_start += 1
        self.type_name = 'struct ' * self.struct + tokens[type_start]
        assert self.type_name != 'void' or self.ptrs > 0
        assert re.match(r'[0-9A-Za-z_]+', self.type_name);
        self.needs_translate = self.type_name in PTR_TYPES or self.ptrs > 0
        if self.type_name in _64BIT_TYPES:
            self.highbits = self.name + '_high'
            self.lowbits = self.name + '_low'


class Prototype:
    def parse_from_str(self, prototype):
        prototype = prototype.strip()
        type_match = WORD.match(prototype)
        assert type_match
        self.type = prototype[0:type_match.end()]
        self.returns_64bit = self.type in _64BIT_TYPES
        name_start = type_match.end() + 1
        ptr_match = PTR_CHAR.match(prototype, type_match.end() + 1)
        if ptr_match:
            name_start = ptr_match.end() + 1
            self.returns_ptr = True
        else:
            self.returns_ptr = False
        name_match = WORD.match(prototype, name_start)
        assert name_match
        self.name = prototype[name_match.start():name_match.end()]

        args = PARENS_OR_SEMI.sub('', prototype[name_match.end() + 1:])
        arg_list = COMMA.split(args)
        self.args = []

        if len(arg_list) > 1 or arg_list[0] != 'void':
            for arg in arg_list:
                a = Argument()
                a.parse_from_str(arg)
                self.args.append(a)
    
    def get_32_bit_arg_list(self):
        ret = []
        for arg in self.args:
            if hasattr(arg, 'highbits') and hasattr(arg, 'lowbits'):
                ret.append(arg.highbits)
                ret.append(arg.lowbits)
            else:
                ret.append(arg.name)
        return ret

class SysCall:

    def parse_from_str(self, syscall_str):
        pieces = whitespace_or_slash.split(syscall_str)
        self.num = int(pieces[0])
        self.audit = pieces[1]
        self.types = set(TYPE_SEP.split(pieces[2]))

        if len(self.types & NO_IMPL_TYPES) > 0:
            self.name = pieces[3]
            self.impl = False
        else:
            self.impl = True
            assert pieces[3] == '{'

            end_curly = pieces.index('}')
            self.prototype_str = ''
            for token in pieces[4:end_curly]:
                self.prototype_str += token + ' '
        
            self.prototype_str = self.prototype_str.strip()

            self.prototype = Prototype()
            self.prototype.parse_from_str(self.prototype_str)
            self.name = self.prototype.name

            self.has_unsupported_struct = False
            for arg in self.prototype.args:
                if arg.struct and not arg.type_name in SUPPORTED_STRUCTS:
                    self.has_unsupported_struct = True
                    break

            if len(pieces[end_curly + 1]) > 0:
                self.altname = pieces[end_curly + 1]
                self.alttag = pieces[end_curly + 2]
                self.altrtyp = pieces[end_curly + 3]

def read_syscalls(filename):
    syscall_list = []
    with open(filename) as master_file:
        logical_line = ''
        for line in master_file:
            if line[0] == ';' or line[0] == '#' or len(line.strip()) == 0:
                continue
            elif line[-2] == '\\':
                logical_line += line
            else:
                call = SysCall()
                call.parse_from_str(logical_line + line)
                syscall_list.append(call)
                logical_line = ''

    print 'found {0} syscalls'.format(len(syscall_list))
    return syscall_list

def print_types(syscall_list):
    type_freqs = {}
    def increm(type_key):
        if type_key in type_freqs:
            type_freqs[type_key] += 1
        else:
            type_freqs[type_key] = 1
    
    for call in syscall_list:
        if call.impl:
            if call.prototype.type != 'void':
                increm(call.prototype.type)
            for arg in call.prototype.args:
                increm(('struct ' * arg.struct) + ('union ' * arg.union)
                    + ('*' * arg.ptrs) + arg.type_name)

    freqs = type_freqs.items()
    freqs.sort(key=lambda x:x[1], reverse=True)
    for f in freqs:
        print '{0}: {1}'.format (f[0], f[1])

def commas(str_list):
    return ', '.join(str_list)

def gen_actionscript(syscall_list, interface_file, as_impl_file):
    interface_file.write('package {0} {{\n'.format(ACTIONSCRIPT_PKG))

    interface_file.write('\t\t/** This interface exposes all of the FreeBSD kernel system calls. To provide reasonable implementations you will need to look at the BSD documentation and also at the BSD libc source code.*/\n\t')
    interface_file.write('\tpublic interface {0} {{\n'.format(
            ACTIONSCRIPT_INTERFACE_NAME))

    as_impl_file.write('package {0} {{\n'.format(ACTIONSCRIPT_PKG))
    as_impl_file.write('\t[native(cls="{0}", instance="{1}", methods="auto")]\n'
            .format(VM_GLUE_CLASS_NAME, VM_GLUE_OBJ_NAME))
    as_impl_file.write('\tCONFIG::VMCFG_ALCHEMY_POSIX\n\tpublic class {0} implements {1} {{\n'.format(
            ACTIONSCRIPT_IMPL_NAME, ACTIONSCRIPT_INTERFACE_NAME))

    as_impl_file.write('\tpublic native static function getAppPath():String;\n\n')
        
   
    for call in syscall_list:
        arg_list = []
        for arg in call.prototype.get_32_bit_arg_list():
            arg_list.append('{0}:{1}'.format(arg, 'int'))
        arg_list.append('errnoPtr:int')
        args = commas(arg_list)
        if call.prototype.type == 'void':
            ret_type = 'void'
        elif call.prototype.returns_64bit:
            ret_type = 'Object'
        else:
            ret_type = 'int'
        func = 'function {0}({1}):{2};\n'.format(call.name, args, ret_type)

        interface_file.write('\t\t/**\n\t\t*/\n\t\t' + func)
        as_impl_file.write('\t\tpublic native ' + func)

    interface_file.write('\t}\n')
    interface_file.write('}\n')
    as_impl_file.write('\t}\n')
    as_impl_file.write('}\n')


def gen_alc_c_stub(syscall_list, stub_file):
    stub_file.write('#include <sys/types.h>\n')
    stub_file.write('#include <sys/uio.h>\n')
    stub_file.write('#include <sys/stat.h>\n')
    stub_file.write('#include <sys/acl.h>\n')
    stub_file.write('#include <sys/param.h>\n')
    stub_file.write('#include <sys/cpuset.h>\n')
    stub_file.write('#include <sys/thr.h>\n')
    stub_file.write('#include <sys/wait.h>\n')
    stub_file.write('#include <sys/param.h>\n')
    stub_file.write('#include <sys/mount.h>\n')
    stub_file.write('#include <sys/sem.h>\n')
    stub_file.write('#include <sys/dirent.h>\n')
    stub_file.write('#include <sys/msg.h>\n')
    stub_file.write('#include <sys/shm.h>\n')
    stub_file.write('#include <sys/socket.h>\n')
    stub_file.write('#include <sys/resource.h>\n')
    stub_file.write('#include <ufs/ufs/quota.h>\n')
    stub_file.write('#include <unistd.h>\n')
    stub_file.write('#include <signal.h>\n')
    stub_file.write('#include <errno.h>\n')
    stub_file.write('#include <AS3/AS3.h>\n')
    stub_file.write('#include <AS3/AVM2.h>\n')
    stub_file.write('\n')
    stub_file.write('#define ARGSIZE(X) ((sizeof(X) + 3) & ~3)\n')
    # we might delegate this call to the ui thread!
    stub_file.write('typedef struct {\n')
    stub_file.write('\tvoid *proc;\n')
    stub_file.write('\tvoid *args;\n')
    stub_file.write('\tint size;\n')
    stub_file.write('\tstruct __avm2_retdata retdata;\n')
    stub_file.write('} DelegateReq;\n')
    # "apply" thunk to run in ui thread if delegating
    stub_file.write('static void *applyThunk(void *arg)\n')
    stub_file.write('{\n')
    stub_file.write('\tDelegateReq *req = (DelegateReq *)arg;\n')
#    stub_file.write('\t\ttprintf("applying %p\\n", req->proc);\n')
    stub_file.write('\tmemcpy(&req->retdata, __avm2_apply(req->proc, req->args, req->size), sizeof(struct __avm2_retdata));\n')
    stub_file.write('\treturn NULL;\n')
    stub_file.write('}\n')
    # macro to possibly delegate to the ui thread
    stub_file.write('#define MAYBEDELEGATE(FUN, ARGSSIZE) \\\n')
    stub_file.write('\tif(!avm2_is_ui_worker()) {\\\n')
    stub_file.write('\t\tDelegateReq req = { FUN, __avm2_apply_args(), ARGSSIZE };\\\n')
#    stub_file.write('\t\ttprintf("thunking %s/%p\\n", #FUN, FUN);\\\n')
    stub_file.write('\t\tavm2_ui_thunk(applyThunk, &req);\\\n')
    stub_file.write('\t\t__avm2_return(&req.retdata);\\\n')
#    stub_file.write('\t}\\\n')
#    stub_file.write('\telse\\\n')
#    stub_file.write('\t{\\\n')
#    stub_file.write('\t\ttprintf("running %s\\n", #FUN);\\\n')
    stub_file.write('\t}\n')

    for call in syscall_list:
        arg_list = []
        arg_size_list = ['0']
        arg_names = call.prototype.get_32_bit_arg_list()
        for arg in call.prototype.args:
            # TODO implement pointers and others...
            #assert arg.ptrs == 0
            #assert not arg.const
            assert not arg.struct or arg.type_name in SUPPORTED_STRUCTS
            assert not arg.union
            arg_size_list.append('ARGSIZE({0})'.format(arg.type_name + '*' * arg.ptrs))
            arg_list.append('{0}{1} {2}{3}'.format('const ' * arg.const,
                arg.type_name, '*' * arg.ptrs, arg.name))
        
        arg_names.append('errnoPtr')
        
        args = commas(arg_list)
        stub_file.write('{0} {1}__sys_{2}({3}) __attribute__((noinline));\n'.format(
            call.prototype.type, '*' * call.prototype.returns_ptr, 
            call.name, args))
        stub_file.write('{0} {1}__sys_{2}({3}) {{\n'.format(
            call.prototype.type, '*' * call.prototype.returns_ptr, 
            call.name, args))
        is_void = call.prototype.type == 'void' and not call.prototype.returns_ptr
        if call.prototype.returns_64bit:
            stub_file.write('\t{0} ret;\n'.format(call.prototype.type))
        elif not is_void:
            stub_file.write('\tint ret;\n')
        
        stub_file.write('\tint *errnoPtr = &errno;\n')
        if not call.name in NO_DELEGATE:
            stub_file.write('\tMAYBEDELEGATE(__sys_{0}, {1});\n'.format(call.name, ' + '.join(arg_size_list)));

        for arg in call.prototype.args:
            if hasattr(arg, 'highbits'):
                stub_file.write('\tunsigned int {0} = {1} >> 32;\n'.format(
                    arg.highbits, arg.name))
            if hasattr(arg, 'lowbits'):    
                stub_file.write('\tunsigned int {0} = {1} & 0xffffffff;\n'.
                    format(arg.lowbits, arg.name))
            if arg.struct and not arg.const:
                stub_file.write('\tmemset({0}, 0, sizeof({1}));\n'.format(
                    arg.name, arg.type_name));
        
        
        if call.prototype.returns_64bit:
            stub_file.write('\tunsigned int ret_low;\n')
            stub_file.write('\tunsigned int ret_high;\n')
        
        stub_file.write('\tinline_as3 (\n')

        if is_void:
            first_arg = 0;
        elif call.prototype.returns_64bit:
            first_arg = 2;
        else:
            first_arg = 1;
        asm_args = commas(['%' + str(x) for x in range(first_arg, len(
                arg_names) + first_arg)])
        reg_args = commas(['"r"(' + x + ')' for x in arg_names])
        
        if call.prototype.returns_64bit:
            stub_file.write(
                '\t\t"var retObj = CModule.kernel.{0}({1});\\n"\n'.
                format(call.name, asm_args))
            
            stub_file.write('\t\t"%0 = retObj[0];\\n"\n')
            stub_file.write('\t\t"%1 = retObj[1];\\n"\n')
            stub_file.write(
                '\t\t: "=r"(ret_high), "=r"(ret_low) : {0}\n'.format(reg_args))
        else:
            stub_file.write('\t\t"{0}CModule.kernel.{1}({2});\\n"\n'.format(
                '%0 = ' * (not is_void), call.name, asm_args))
            stub_file.write('\t\t: {0} : {1}\n'.format(
                '"=r"(ret)' * (not is_void), reg_args))

        stub_file.write('\t);\n')

        if call.prototype.returns_64bit:
            stub_file.write('\tret = ret_high;\n')
            stub_file.write('\tret = ret << 32;\n')
            stub_file.write('\tret |= ret_low;\n')
        
        stub_file.write('\treturn ret;\n' * (not is_void))
        stub_file.write('}\n')
        
        stub_file.write('__weak_reference3(__sys_%s, %s);\n\n' % (call.name, call.name))
        stub_file.write('__weak_reference3(__sys_%s, _%s);\n\n' % (call.name, call.name))
        stub_file.write('__weak_reference3(__sys_%s, __sys_freebsd6_%s);\n\n' % (call.name, call.name))


def gen_vm_glue(syscall_list, header_file, impl_file):
    header_file.write('#ifdef VMCFG_ALCHEMY_POSIX\n')
    header_file.write('#ifndef {0}\n'.format(VM_GLUE_INCLUDE_GUARD))
    header_file.write('#define {0}\n\n'.format(VM_GLUE_INCLUDE_GUARD))
    header_file.write('namespace avmplus {\n')
    header_file.write('\tclass {0} : public ClassClosure\n'.format(
        VM_GLUE_CLASS_NAME))
    header_file.write('\t{\n')
    header_file.write('\tpublic:\n')

    header_file.write('\t\tStringp getAppPath();\n')

    header_file.write('\t\t{0}(VTable *vtable);\n'.format(VM_GLUE_CLASS_NAME))
    header_file.write('\tprivate:\n')
    header_file.write('\t\tDECLARE_SLOTS_{0};\n'.format(VM_GLUE_CLASS_NAME))
    header_file.write('\t};\n\n')

    header_file.write('\tclass {0} : public ScriptObject\n'.format(
        VM_GLUE_OBJ_NAME))
    header_file.write('\t{\n')
    header_file.write('\tpublic:\n')
    header_file.write('\t\t{0}(VTable *vtable, ScriptObject *delegate);\n'
        .format(VM_GLUE_OBJ_NAME))

    impl_file.write('#ifdef AVMFEATURE_ALCHEMY_POSIX\n')
    impl_file.write('#ifdef __APPLE__\n')
    impl_file.write('#define _DARWIN_NO_64_BIT_INODE\n')
    impl_file.write('#endif /* __APPLE__ */\n')
    impl_file.write('#include <sys/types.h>\n')
    impl_file.write('#include <unistd.h>\n')
    impl_file.write('#include <stdio.h>\n')
    impl_file.write('#include <fcntl.h>\n')
    impl_file.write('#include <time.h>\n')
    impl_file.write('#include <sys/stat.h>\n')
    impl_file.write('#include <sys/param.h>\n')
    impl_file.write('#include <sys/mount.h>\n')
    impl_file.write('#include <sys/ioctl.h>\n')
    impl_file.write('#include <sys/wait.h>\n')
    impl_file.write('#include <sys/sem.h>\n')
    # <sys/dirent.h> is probably included in <dirent.h> anyway
    #impl_file.write('#include <sys/dirent.h>\n')
    impl_file.write('#include <sys/uio.h>\n')
    impl_file.write('#include <dirent.h>\n')
    impl_file.write('#include <sys/msg.h>\n')
    impl_file.write('#include <sys/shm.h>\n')
    impl_file.write('#include <sys/socket.h>\n')

    impl_file.write('#include "avmshell.h"\n')
    impl_file.write('#include "{0}.h"\n\n'.format(VM_GLUE_FILE_NAME))

    #ifdef VMCFG_ALCHEMY_SDK_BUILD
    impl_file.write('#ifdef VMCFG_ALCHEMY_SDK_BUILD\n')
    impl_file.write('#include "SetAlchemySDKLocation.c"\n\n')
    impl_file.write('#endif\n')
    
    # These members of struct stat have different names on Cygwin or Linux
    impl_file.write('#if defined(__CYGWIN__) || defined(__linux__)\n')
    impl_file.write('#define st_atimespec st_atim\n')
    impl_file.write('#define st_mtimespec st_mtim\n')
    impl_file.write('#define st_ctimespec st_ctim\n')
    impl_file.write('#endif /* __CYGWIN__ || __linux__ */\n\n')

    impl_file.write('namespace avmplus {\n')
    impl_file.write('\tstatic void *domainMemoryPtr(ScriptObject *obj, ' + 
        'int offset) {\n')
    impl_file.write('\t\tDomainEnv *env = ' + 
        'obj->core()->codeContext()->domainEnv();\n')
    impl_file.write('\t\tByteArrayObject *domainMemory = ' + 
        'env->get_globalMemory();\n')
    impl_file.write('\t\treturn &(domainMemory->GetByteArray()[offset]);\n')
    impl_file.write('\t}\n\n')

    impl_file.write(
        '\tstatic Atom twoInts(ScriptObject *obj, unsigned int high, unsigned int low) {\n')
    impl_file.write('\t\tAtom highatom = obj->core()->uintToAtom(high);\n')
    impl_file.write('\t\tAtom lowatom = obj->core()->uintToAtom(low);\n')
    impl_file.write('\t\tArrayObject *ret = ' + 
        'obj->toplevel()->arrayClass()->newArray(2);\n')
    impl_file.write('\t\tret->setUintProperty(0, highatom);\n')
    impl_file.write('\t\tret->setUintProperty(1, lowatom);\n')
    impl_file.write('\t\treturn ret->atom();\n')
    impl_file.write('\t}\n\n')

    
    impl_file.write('\t{0}::{0}(VTable *vtable, ScriptObject *delegate)\n'
        .format(VM_GLUE_OBJ_NAME))
    impl_file.write('\t\t: ScriptObject(vtable, delegate)\n')
    impl_file.write('\t{}\n\n')

    for call in syscall_list:
        is_void = call.prototype.type == 'void'
        mac_only = call.name in MAC_ONLY
        mac_special = call.name in MAC_SPECIAL_IMPLS
        if is_void:
            ret_type = 'void'
        elif call.prototype.returns_64bit:
            ret_type = 'Atom'
        else:
            ret_type = 'int'
        args = commas(['int ' + x for x in 
            call.prototype.get_32_bit_arg_list() + ['errnoPtr']])
        header_file.write('\t\tvirtual {0} {1}({2});\n'.format(
            ret_type, call.name, args))
        impl_file.write('\t{0} {1}::{2}({3}) {{\n'.format (
            ret_type, VM_GLUE_OBJ_NAME, call.name, args))
        
        if mac_only:
            impl_file.write('#ifdef __APPLE__\n')

        translate_map = {}
        extract_map = {}
        for arg in call.prototype.args:
            if arg.needs_translate:
                # TODO need to handle pointers to pointers
                assert arg.ptrs <= 1
                assert not ((arg.name + '_ptr') in 
                    [x.name for x in call.prototype.args])
                translate_map[arg.name] = arg.name + '_ptr'
                type_name = arg.type_name
                if arg.type_name == 'void':
                    cast_str = ''
                elif arg.struct:
                    cast_str = '(char *) '
                    type_name = 'char'
                else:
                    cast_str = '({0}{1}) '.format(arg.type_name, 
                        ' *' * arg.ptrs)
                impl_file.write(
                    '\t\t{0} {1}{2} = {3}domainMemoryPtr(this, {4});\n'.
                    format(type_name, '*' * arg.ptrs, 
                    translate_map[arg.name], 
                    cast_str, arg.name))
            
            if hasattr(arg, 'highbits') and hasattr(arg, 'lowbits'):
                impl_file.write('\t\t{0} {1} = {2};\n'.format(
                    arg.type_name, arg.name, arg.highbits))
                impl_file.write('\t\t{0} = {0} << 32;\n'.format(arg.name))
                impl_file.write('\t\t{0} |= {1};\n'.format(
                    arg.name, arg.lowbits))

            # Assume that structs are passed as pointers, never as entire
            # structs on the stack. Also assume that the syscall will write
            # its output into the members of the struct, unless it's const.
            # First, allocate a native struct on the stack. Then pass a pointer
            # to this struct to the real syscall. Finally, copy members out
            # of the native struct to the flascc struct, based on the
            # offsets and sizes of the flascc struct memebers, stored in
            # the SUPPORTED_STRUCTS map.
            if arg.struct:
                if arg.const:
                    tmpstruct = '{0}_tmp'.format(arg.name)
                    impl_file.write('\t\t{0} {1};\n'
                        .format(arg.type_name, tmpstruct))
                    impl_file.write('\t\tmemmove(&{0}, {1}, sizeof({2}));\n'
                        .format(tmpstruct, translate_map[arg.name], 
                                arg.type_name))
                    translate_map[arg.name] = '&' + tmpstruct
                else:
                    extract_map[arg] = translate_map[arg.name]
                    translate_map[arg.name] = '&'+ arg.name + '_struct'
                    impl_file.write('\t\t{0} {1};\n'.format(arg.type_name,
                                    translate_map[arg.name][1:]))

        syscall_args = commas(
            [translate_map.setdefault(x.name, x.name) 
            for x in call.prototype.args])
        
        impl_file.write('\t\tint saved_err = errno;\n')
        impl_file.write('\t\terrno = 0;\n')
       
        if mac_special:
            impl_file.write('#ifdef __APPLE__\n')
            MAC_SPECIAL_IMPLS[call.name](impl_file, 
                [translate_map.setdefault(x.name, x.name) 
                for x in call.prototype.args], 'ret')
            impl_file.write('#else\n')

        if call.prototype.returns_64bit:
            impl_file.write('\t\t{0} ret = ::{1}({2});\n'.format(
                call.prototype.type, call.name, syscall_args))
            impl_file.write('\t\tunsigned int ret_high = ret >> 32;\n')
            impl_file.write('\t\tunsigned int ret_low = ret & 0xffffffff;\n')
            ret_str = 'twoInts(this, ret_high, ret_low)'
        elif call.name in SPECIAL_IMPLS:
            SPECIAL_IMPLS[call.name](impl_file, 
                [translate_map.setdefault(x.name, x.name) 
                for x in call.prototype.args], 'ret')
        else:
            impl_file.write('\t\t{0}::{1}({2});\n'.format(
                'int ret = ' * (not is_void), call.name, syscall_args))
            ret_str = 'ret'
        
        if mac_special:
            impl_file.write('#endif /* __APPLE__ */\n')

        impl_file.write('\t\tif (errno) {\n')
        impl_file.write('\t\t\tint *alc_errno = (int *) ' + 
            'domainMemoryPtr(this, errnoPtr);\n')
        impl_file.write('\t\t\t*alc_errno = errno;\n')
        impl_file.write('\t\t}\n')
        impl_file.write('\t\terrno = saved_err;\n')

        for (arg, dest) in extract_map.items():
            for (mname, moff, msize) in SUPPORTED_STRUCTS[arg.type_name]:
                if arg.const:
                    continue
                if msize == 1:
                    cast_type = 'uint8_t'
                elif msize == 2:
                    cast_type = 'uint16_t'
                elif msize == 4:
                    cast_type = 'uint32_t'
                elif msize == 8:
                    cast_type = 'uint64_t'
                else:
                    assert False

                impl_file.write('\t\t*({0} *) ({1} + {2}) = ({0}) {3}.{4};\n'
                    .format(cast_type, dest, moff, 
                    translate_map[arg.name][1:], #remove leading '&'
                    mname))

        impl_file.write('\t\treturn {0};\n'.format(ret_str) * (not is_void))
        if mac_only:
            impl_file.write('#else\n')
            for arg in call.prototype.args:
                impl_file.write('\t\t(void){0};\n'.format(arg.name))
            impl_file.write('\t\t(void)errnoPtr;\n')
            impl_file.write('\t\treturn -1;\n')
            impl_file.write('#endif /* __APPLE__ */\n')
        impl_file.write('\t}\n\n')

    
    impl_file.write('\t{0}::{0}(VTable *vtable)\n'.format(VM_GLUE_CLASS_NAME))
    impl_file.write('\t\t: ClassClosure(vtable)\n')
    impl_file.write('\t{}\n')

    impl_file.write('''
    Stringp ShellPosixClass::getAppPath()
    {
        #if VMCFG_ALCHEMY_SDK_BUILD
        char path[PATH_MAX];
        unsigned int sz = PATH_MAX;
        GetAppPath(&path[0], &sz);
        return core()->newStringLatin1(&path[0], sz);
        #else
        return core()->kEmptyString;
        #endif
    }
    ''')

    impl_file.write('}\n')
    impl_file.write('#endif // VMCFG_ALCHEMY_POSIX\n')
    
    header_file.write('\tprivate:\n')
    header_file.write('\t\tDECLARE_SLOTS_{0};\n'.format(VM_GLUE_OBJ_NAME))
    header_file.write('\t};\n')
    header_file.write('}\n\n')
    header_file.write('#endif // {0}\n'.format(VM_GLUE_INCLUDE_GUARD))
    header_file.write('#endif // VMCFG_ALCHEMY_POSIX\n')
    

def gen_code(syscall_list):
    # need to generate ShellPosix.as, Posix.as, posix.c, and ShellPosixGlue.ch
    # Posix.as: just a list of calls
    # ShellPosix.as: function defintions
    # posix.c: stubs that call CModule
    # ShellPosixGlue: implementation of native classes, translate pointers
    #   and call real sys calls 
    with contextlib.nested(open(VM_GLUE_FILE_NAME + '.h', 'w'), 
            open(VM_GLUE_FILE_NAME + '.cpp', 'w'),
            open(ACTIONSCRIPT_INTERFACE_NAME + '.as', 'w'),
            open(ACTIONSCRIPT_IMPL_NAME + '.as', 'w'),
            open('posix.c', 'w')) as (header_file, impl_file, interface_file, 
            as_impl_file, stub_file):
        gen_actionscript(syscall_list, interface_file, as_impl_file) 
        gen_alc_c_stub(syscall_list, stub_file)
        gen_vm_glue(syscall_list, header_file, impl_file)

def can_gen(call):
    if not hasattr(call, 'prototype'):
        return False
    
    has_ptr_to_ptr = False
    has_union = False
    for arg in call.prototype.args:
        if arg.ptrs > 1:
            has_ptr_to_ptr = True
        if arg.union:
            has_union = True

    has_name_mismatch = False
    if hasattr(call, 'altname'):
        has_name_mismatch = call.name != call.altname

    return not (call.has_unsupported_struct or has_union or has_ptr_to_ptr or 
        has_name_mismatch)

calls = read_syscalls(sys.argv[1])
#print_types(calls)

acceptable_calls = [x for x in calls if can_gen(x)]

gen_code(acceptable_calls)
print 'generated {0} syscalls'.format(len(acceptable_calls))


