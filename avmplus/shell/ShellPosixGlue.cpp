#ifdef AVMFEATURE_ALCHEMY_POSIX
#ifdef __APPLE__
#define _DARWIN_NO_64_BIT_INODE
#endif /* __APPLE__ */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/dirent.h>
#include <sys/uio.h>
#include <dirent.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include "avmshell.h"
#include "ShellPosixGlue.h"

#ifdef VMCFG_ALCHEMY_SDK_BUILD
#include "SetAlchemySDKLocation.c"

#endif
#ifdef __CYGWIN__
#define st_atimespec st_atim
#define st_mtimespec st_mtim
#define st_ctimespec st_ctim
#endif /* __CYGWIN__ */

namespace avmplus {
	static void *domainMemoryPtr(ScriptObject *obj, int offset) {
		DomainEnv *env = obj->core()->codeContext()->domainEnv();
		ByteArrayObject *domainMemory = env->get_globalMemory();
		return &(domainMemory->GetByteArray()[offset]);
	}

	static Atom twoInts(ScriptObject *obj, unsigned int high, unsigned int low) {
		Atom highatom = obj->core()->uintToAtom(high);
		Atom lowatom = obj->core()->uintToAtom(low);
		ArrayObject *ret = obj->toplevel()->arrayClass()->newArray(2);
		ret->setUintProperty(0, highatom);
		ret->setUintProperty(1, lowatom);
		return ret->atom();
	}

	ShellPosixObject::ShellPosixObject(VTable *vtable, ScriptObject *delegate)
		: ScriptObject(vtable, delegate)
	{}

	int ShellPosixObject::fork(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fork();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::read(int A_fd, int A_buf, int A_nbyte, int errnoPtr) {
		void *A_buf_ptr = domainMemoryPtr(this, A_buf);
		int saved_err = errno;
		errno = 0;
		int ret = ::read(A_fd, A_buf_ptr, A_nbyte);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::write(int A_fd, int A_buf, int A_nbyte, int errnoPtr) {
		void *A_buf_ptr = domainMemoryPtr(this, A_buf);
		int saved_err = errno;
		errno = 0;
		int ret = ::write(A_fd, A_buf_ptr, A_nbyte);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::open(int A_path, int A_flags, int A_mode, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::open(A_path_ptr, A_flags, A_mode);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::close(int A_fd, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::close(A_fd);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::wait4(int A_pid, int A_status, int A_options, int A_rusage, int errnoPtr) {
		int *A_status_ptr = (int *) domainMemoryPtr(this, A_status);
		char *A_rusage_ptr = (char *) domainMemoryPtr(this, A_rusage);
		struct rusage A_rusage_struct;
		int saved_err = errno;
		errno = 0;
		int ret = ::wait4(A_pid, A_status_ptr, A_options, &A_rusage_struct);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_rusage_ptr + 0) = (uint32_t) A_rusage_struct.ru_utime.tv_sec;
		*(uint32_t *) (A_rusage_ptr + 4) = (uint32_t) A_rusage_struct.ru_utime.tv_usec;
		*(uint32_t *) (A_rusage_ptr + 8) = (uint32_t) A_rusage_struct.ru_stime.tv_sec;
		*(uint32_t *) (A_rusage_ptr + 12) = (uint32_t) A_rusage_struct.ru_stime.tv_usec;
		return ret;
	}

	int ShellPosixObject::link(int A_path, int A_linkpath, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		char *A_linkpath_ptr = (char *) domainMemoryPtr(this, A_linkpath);
		int saved_err = errno;
		errno = 0;
		int ret = ::link(A_path_ptr, A_linkpath_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::unlink(int A_path, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::unlink(A_path_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::chdir(int A_path, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::chdir(A_path_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::fchdir(int A_fd, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fchdir(A_fd);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::chmod(int A_path, int A_mode, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::chmod(A_path_ptr, A_mode);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::chown(int A_path, int A_uid, int A_gid, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::chown(A_path_ptr, A_uid, A_gid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	Atom ShellPosixObject::lseek(int A_fd, int A_offset_high, int A_offset_low, int A_whence, int errnoPtr) {
		off_t A_offset = A_offset_high;
		A_offset = A_offset << 32;
		A_offset |= A_offset_low;
		int saved_err = errno;
		errno = 0;
		off_t ret = ::lseek(A_fd, A_offset, A_whence);
		unsigned int ret_high = ret >> 32;
		unsigned int ret_low = ret & 0xffffffff;
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return twoInts(this, ret_high, ret_low);
	}

	int ShellPosixObject::getpid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getpid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setuid(int A_uid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setuid(A_uid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getuid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getuid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::geteuid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::geteuid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::access(int A_path, int A_flags, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::access(A_path_ptr, A_flags);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	void ShellPosixObject::sync(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		::sync();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
	}

	int ShellPosixObject::kill(int A_pid, int A_signum, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::kill(A_pid, A_signum);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getppid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getppid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::dup(int A_fd, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::dup(A_fd);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::pipe(int A_fildes, int errnoPtr) {
		int *A_fildes_ptr = (int *) domainMemoryPtr(this, A_fildes);
		int saved_err = errno;
		errno = 0;
		int ret = ::pipe(A_fildes_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getegid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getegid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getgid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getgid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::ioctl(int A_fd, int A_com, int A_data, int errnoPtr) {
		caddr_t A_data_ptr = (caddr_t) domainMemoryPtr(this, A_data);
		int saved_err = errno;
		errno = 0;
		int ret = ::ioctl(A_fd, A_com, A_data_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::revoke(int A_path, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::revoke(A_path_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::symlink(int A_path, int A_link, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		char *A_link_ptr = (char *) domainMemoryPtr(this, A_link);
		int saved_err = errno;
		errno = 0;
		int ret = ::symlink(A_path_ptr, A_link_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::umask(int A_newmask, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::umask(A_newmask);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::chroot(int A_path, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::chroot(A_path_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::msync(int A_addr, int A_len, int A_flags, int errnoPtr) {
		void *A_addr_ptr = domainMemoryPtr(this, A_addr);
		int saved_err = errno;
		errno = 0;
		int ret = ::msync(A_addr_ptr, A_len, A_flags);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::vfork(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::vfork();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getgroups(int A_gidsetsize, int A_gidset, int errnoPtr) {
		gid_t *A_gidset_ptr = (gid_t *) domainMemoryPtr(this, A_gidset);
		int saved_err = errno;
		errno = 0;
		int ret = ::getgroups(A_gidsetsize, A_gidset_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setgroups(int A_gidsetsize, int A_gidset, int errnoPtr) {
		gid_t *A_gidset_ptr = (gid_t *) domainMemoryPtr(this, A_gidset);
		int saved_err = errno;
		errno = 0;
		int ret = ::setgroups(A_gidsetsize, A_gidset_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getpgrp(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getpgrp();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setpgid(int A_pid, int A_pgid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setpgid(A_pid, A_pgid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getdtablesize(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getdtablesize();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::dup2(int A_from, int A_to, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::dup2(A_from, A_to);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::fcntl(int A_fd, int A_cmd, int A_arg, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fcntl(A_fd, A_cmd, A_arg);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::fsync(int A_fd, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fsync(A_fd);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setpriority(int A_which, int A_who, int A_prio, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setpriority(A_which, A_who, A_prio);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::socket(int A_domain, int A_type, int A_protocol, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::socket(A_domain, A_type, A_protocol);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getpriority(int A_which, int A_who, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getpriority(A_which, A_who);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setsockopt(int A_s, int A_level, int A_name, int A_val, int A_valsize, int errnoPtr) {
		void *A_val_ptr = domainMemoryPtr(this, A_val);
		int saved_err = errno;
		errno = 0;
		int ret = ::setsockopt(A_s, A_level, A_name, A_val_ptr, A_valsize);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::listen(int A_s, int A_backlog, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::listen(A_s, A_backlog);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sigsuspend(int A_mask, int errnoPtr) {
		sigset_t *A_mask_ptr = (sigset_t *) domainMemoryPtr(this, A_mask);
		int saved_err = errno;
		errno = 0;
		int ret = ::sigsuspend(A_mask_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getrusage(int A_who, int A_rusage, int errnoPtr) {
		char *A_rusage_ptr = (char *) domainMemoryPtr(this, A_rusage);
		struct rusage A_rusage_struct;
		int saved_err = errno;
		errno = 0;
		int ret = ::getrusage(A_who, &A_rusage_struct);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_rusage_ptr + 0) = (uint32_t) A_rusage_struct.ru_utime.tv_sec;
		*(uint32_t *) (A_rusage_ptr + 4) = (uint32_t) A_rusage_struct.ru_utime.tv_usec;
		*(uint32_t *) (A_rusage_ptr + 8) = (uint32_t) A_rusage_struct.ru_stime.tv_sec;
		*(uint32_t *) (A_rusage_ptr + 12) = (uint32_t) A_rusage_struct.ru_stime.tv_usec;
		return ret;
	}

	int ShellPosixObject::getsockopt(int A_s, int A_level, int A_name, int A_val, int A_avalsize, int errnoPtr) {
		void *A_val_ptr = domainMemoryPtr(this, A_val);
		socklen_t *A_avalsize_ptr = (socklen_t *) domainMemoryPtr(this, A_avalsize);
		int saved_err = errno;
		errno = 0;
		int ret = ::getsockopt(A_s, A_level, A_name, A_val_ptr, A_avalsize_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::readv(int A_fd, int A_iovp, int A_iovcnt, int errnoPtr) {
		char *A_iovp_ptr = (char *) domainMemoryPtr(this, A_iovp);
		struct iovec A_iovp_tmp;
		memmove(&A_iovp_tmp, A_iovp_ptr, sizeof(struct iovec));
		int saved_err = errno;
		errno = 0;
		int i;
		struct iovec tmp_iovec[A_iovcnt];
		char *vecp = (char *)domainMemoryPtr(this, A_iovp);
		for (i = 0; i < A_iovcnt; i++) {
			int nbytes = *(vecp + (i * 8) + 4);
			char *tmp = (char *)malloc(nbytes);
			tmp_iovec[i].iov_base = tmp;
			tmp_iovec[i].iov_len = nbytes;
		}
		int ret = ::readv(A_fd, tmp_iovec, A_iovcnt);
		for (i = 0; i < A_iovcnt; i++) {
			int offset = *((int *) (vecp + (i * 8)));
			void *datap = domainMemoryPtr(this, offset);
			memmove(datap, tmp_iovec[i].iov_base, tmp_iovec[i].iov_len);
			free(tmp_iovec[i].iov_base);
		}
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::writev(int A_fd, int A_iovp, int A_iovcnt, int errnoPtr) {
		char *A_iovp_ptr = (char *) domainMemoryPtr(this, A_iovp);
		struct iovec A_iovp_tmp;
		memmove(&A_iovp_tmp, A_iovp_ptr, sizeof(struct iovec));
		int saved_err = errno;
		errno = 0;
		int i;
		struct iovec tmp_iovec[A_iovcnt];
		char *vecp = (char *)domainMemoryPtr(this, A_iovp);
		for (i = 0; i < A_iovcnt; i++) {
			int nbytes = *(vecp + (i * 8) + 4);
			char *tmp = (char *)malloc(nbytes);
			int offset = *((int *) (vecp + (i * 8)));
			void *datap = domainMemoryPtr(this, offset);
			memmove(tmp, (void *) datap, nbytes);
			tmp_iovec[i].iov_base = tmp;
			tmp_iovec[i].iov_len = nbytes;
		}
		int ret = ::writev(A_fd, tmp_iovec, A_iovcnt);
		for (i = 0; i < A_iovcnt; i++) {
			free(tmp_iovec[i].iov_base);
		}
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::fchown(int A_fd, int A_uid, int A_gid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fchown(A_fd, A_uid, A_gid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::fchmod(int A_fd, int A_mode, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fchmod(A_fd, A_mode);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setreuid(int A_ruid, int A_euid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setreuid(A_ruid, A_euid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setregid(int A_rgid, int A_egid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setregid(A_rgid, A_egid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::rename(int A_from, int A_to, int errnoPtr) {
		char *A_from_ptr = (char *) domainMemoryPtr(this, A_from);
		char *A_to_ptr = (char *) domainMemoryPtr(this, A_to);
		int saved_err = errno;
		errno = 0;
		int ret = ::rename(A_from_ptr, A_to_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::mkfifo(int A_path, int A_mode, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::mkfifo(A_path_ptr, A_mode);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::shutdown(int A_s, int A_how, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::shutdown(A_s, A_how);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::socketpair(int A_domain, int A_type, int A_protocol, int A_rsv, int errnoPtr) {
		int *A_rsv_ptr = (int *) domainMemoryPtr(this, A_rsv);
		int saved_err = errno;
		errno = 0;
		int ret = ::socketpair(A_domain, A_type, A_protocol, A_rsv_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::mkdir(int A_path, int A_mode, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::mkdir(A_path_ptr, A_mode);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::rmdir(int A_path, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::rmdir(A_path_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setsid(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setsid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getdirentries(int A_fd, int A_buf, int A_count, int A_basep, int errnoPtr) {
#ifdef __APPLE__
		char *A_buf_ptr = (char *) domainMemoryPtr(this, A_buf);
		long *A_basep_ptr = (long *) domainMemoryPtr(this, A_basep);
		int saved_err = errno;
		errno = 0;
		int ret = ::getdirentries(A_fd, A_buf_ptr, A_count, A_basep_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
#else
		(void)A_fd;
		(void)A_buf;
		(void)A_count;
		(void)A_basep;
		(void)errnoPtr;
		return -1;
#endif /* __APPLE__ */
	}

	int ShellPosixObject::setgid(int A_gid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setgid(A_gid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::setegid(int A_egid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::setegid(A_egid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::seteuid(int A_euid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::seteuid(A_euid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::stat(int A_path, int A_ub, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		char *A_ub_ptr = (char *) domainMemoryPtr(this, A_ub);
		struct stat A_ub_struct;
		int saved_err = errno;
		errno = 0;
		int ret = ::stat(A_path_ptr, &A_ub_struct);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_ub_ptr + 0) = (uint32_t) A_ub_struct.st_dev;
		*(uint16_t *) (A_ub_ptr + 8) = (uint16_t) A_ub_struct.st_mode;
		*(uint16_t *) (A_ub_ptr + 10) = (uint16_t) A_ub_struct.st_nlink;
		*(uint32_t *) (A_ub_ptr + 12) = (uint32_t) A_ub_struct.st_uid;
		*(uint32_t *) (A_ub_ptr + 16) = (uint32_t) A_ub_struct.st_gid;
		*(uint32_t *) (A_ub_ptr + 20) = (uint32_t) A_ub_struct.st_rdev;
		*(uint64_t *) (A_ub_ptr + 48) = (uint64_t) A_ub_struct.st_size;
		*(uint64_t *) (A_ub_ptr + 56) = (uint64_t) A_ub_struct.st_blocks;
		*(uint32_t *) (A_ub_ptr + 64) = (uint32_t) A_ub_struct.st_blksize;
		*(uint32_t *) (A_ub_ptr + 24) = (uint32_t) A_ub_struct.st_atimespec.tv_sec;
		*(uint32_t *) (A_ub_ptr + 28) = (uint32_t) A_ub_struct.st_atimespec.tv_nsec;
		*(uint32_t *) (A_ub_ptr + 32) = (uint32_t) A_ub_struct.st_mtimespec.tv_sec;
		*(uint32_t *) (A_ub_ptr + 36) = (uint32_t) A_ub_struct.st_mtimespec.tv_nsec;
		*(uint32_t *) (A_ub_ptr + 40) = (uint32_t) A_ub_struct.st_ctimespec.tv_sec;
		*(uint32_t *) (A_ub_ptr + 44) = (uint32_t) A_ub_struct.st_ctimespec.tv_nsec;
		return ret;
	}

	int ShellPosixObject::fstat(int A_fd, int A_sb, int errnoPtr) {
		char *A_sb_ptr = (char *) domainMemoryPtr(this, A_sb);
		struct stat A_sb_struct;
		int saved_err = errno;
		errno = 0;
		int ret = ::fstat(A_fd, &A_sb_struct);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_sb_ptr + 0) = (uint32_t) A_sb_struct.st_dev;
		*(uint16_t *) (A_sb_ptr + 8) = (uint16_t) A_sb_struct.st_mode;
		*(uint16_t *) (A_sb_ptr + 10) = (uint16_t) A_sb_struct.st_nlink;
		*(uint32_t *) (A_sb_ptr + 12) = (uint32_t) A_sb_struct.st_uid;
		*(uint32_t *) (A_sb_ptr + 16) = (uint32_t) A_sb_struct.st_gid;
		*(uint32_t *) (A_sb_ptr + 20) = (uint32_t) A_sb_struct.st_rdev;
		*(uint64_t *) (A_sb_ptr + 48) = (uint64_t) A_sb_struct.st_size;
		*(uint64_t *) (A_sb_ptr + 56) = (uint64_t) A_sb_struct.st_blocks;
		*(uint32_t *) (A_sb_ptr + 64) = (uint32_t) A_sb_struct.st_blksize;
		*(uint32_t *) (A_sb_ptr + 24) = (uint32_t) A_sb_struct.st_atimespec.tv_sec;
		*(uint32_t *) (A_sb_ptr + 28) = (uint32_t) A_sb_struct.st_atimespec.tv_nsec;
		*(uint32_t *) (A_sb_ptr + 32) = (uint32_t) A_sb_struct.st_mtimespec.tv_sec;
		*(uint32_t *) (A_sb_ptr + 36) = (uint32_t) A_sb_struct.st_mtimespec.tv_nsec;
		*(uint32_t *) (A_sb_ptr + 40) = (uint32_t) A_sb_struct.st_ctimespec.tv_sec;
		*(uint32_t *) (A_sb_ptr + 44) = (uint32_t) A_sb_struct.st_ctimespec.tv_nsec;
		return ret;
	}

	int ShellPosixObject::lstat(int A_path, int A_ub, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		char *A_ub_ptr = (char *) domainMemoryPtr(this, A_ub);
		struct stat A_ub_struct;
		int saved_err = errno;
		errno = 0;
		int ret = ::lstat(A_path_ptr, &A_ub_struct);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_ub_ptr + 0) = (uint32_t) A_ub_struct.st_dev;
		*(uint16_t *) (A_ub_ptr + 8) = (uint16_t) A_ub_struct.st_mode;
		*(uint16_t *) (A_ub_ptr + 10) = (uint16_t) A_ub_struct.st_nlink;
		*(uint32_t *) (A_ub_ptr + 12) = (uint32_t) A_ub_struct.st_uid;
		*(uint32_t *) (A_ub_ptr + 16) = (uint32_t) A_ub_struct.st_gid;
		*(uint32_t *) (A_ub_ptr + 20) = (uint32_t) A_ub_struct.st_rdev;
		*(uint64_t *) (A_ub_ptr + 48) = (uint64_t) A_ub_struct.st_size;
		*(uint64_t *) (A_ub_ptr + 56) = (uint64_t) A_ub_struct.st_blocks;
		*(uint32_t *) (A_ub_ptr + 64) = (uint32_t) A_ub_struct.st_blksize;
		*(uint32_t *) (A_ub_ptr + 24) = (uint32_t) A_ub_struct.st_atimespec.tv_sec;
		*(uint32_t *) (A_ub_ptr + 28) = (uint32_t) A_ub_struct.st_atimespec.tv_nsec;
		*(uint32_t *) (A_ub_ptr + 32) = (uint32_t) A_ub_struct.st_mtimespec.tv_sec;
		*(uint32_t *) (A_ub_ptr + 36) = (uint32_t) A_ub_struct.st_mtimespec.tv_nsec;
		*(uint32_t *) (A_ub_ptr + 40) = (uint32_t) A_ub_struct.st_ctimespec.tv_sec;
		*(uint32_t *) (A_ub_ptr + 44) = (uint32_t) A_ub_struct.st_ctimespec.tv_nsec;
		return ret;
	}

	int ShellPosixObject::pathconf(int A_path, int A_name, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::pathconf(A_path_ptr, A_name);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::fpathconf(int A_fd, int A_name, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::fpathconf(A_fd, A_name);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getpgid(int A_pid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getpgid(A_pid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::semget(int A_key, int A_nsems, int A_semflg, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::semget(A_key, A_nsems, A_semflg);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::msgget(int A_key, int A_msgflg, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::msgget(A_key, A_msgflg);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::msgsnd(int A_msqid, int A_msgp, int A_msgsz, int A_msgflg, int errnoPtr) {
		void *A_msgp_ptr = domainMemoryPtr(this, A_msgp);
		int saved_err = errno;
		errno = 0;
		int ret = ::msgsnd(A_msqid, A_msgp_ptr, A_msgsz, A_msgflg);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::msgrcv(int A_msqid, int A_msgp, int A_msgsz, int A_msgtyp, int A_msgflg, int errnoPtr) {
		void *A_msgp_ptr = domainMemoryPtr(this, A_msgp);
		int saved_err = errno;
		errno = 0;
		int ret = ::msgrcv(A_msqid, A_msgp_ptr, A_msgsz, A_msgtyp, A_msgflg);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::shmdt(int A_shmaddr, int errnoPtr) {
		void *A_shmaddr_ptr = domainMemoryPtr(this, A_shmaddr);
		int saved_err = errno;
		errno = 0;
		int ret = ::shmdt(A_shmaddr_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::shmget(int A_key, int A_size, int A_shmflg, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::shmget(A_key, A_size, A_shmflg);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::clock_gettime(int A_clock_id, int A_tp, int errnoPtr) {
		char *A_tp_ptr = (char *) domainMemoryPtr(this, A_tp);
		struct timespec A_tp_struct;
		int saved_err = errno;
		errno = 0;
#ifdef __APPLE__
		(void)A_clock_id;
		int ret = ::gettimeofday((struct timeval *) &A_tp_struct, NULL);
		A_tp_struct.tv_nsec /= 1000;
#else
		int ret = ::clock_gettime(A_clock_id, &A_tp_struct);
#endif /* __APPLE__ */
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_tp_ptr + 0) = (uint32_t) A_tp_struct.tv_sec;
		*(uint32_t *) (A_tp_ptr + 4) = (uint32_t) A_tp_struct.tv_nsec;
		return ret;
	}

	int ShellPosixObject::nanosleep(int A_rqtp, int A_rmtp, int errnoPtr) {
		char *A_rqtp_ptr = (char *) domainMemoryPtr(this, A_rqtp);
		struct timespec A_rqtp_tmp;
		memmove(&A_rqtp_tmp, A_rqtp_ptr, sizeof(struct timespec));
		char *A_rmtp_ptr = (char *) domainMemoryPtr(this, A_rmtp);
		struct timespec A_rmtp_struct;
		int saved_err = errno;
		errno = 0;
		int ret = ::nanosleep(&A_rqtp_tmp, &A_rmtp_struct);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		*(uint32_t *) (A_rmtp_ptr + 0) = (uint32_t) A_rmtp_struct.tv_sec;
		*(uint32_t *) (A_rmtp_ptr + 4) = (uint32_t) A_rmtp_struct.tv_nsec;
		return ret;
	}

	int ShellPosixObject::lchown(int A_path, int A_uid, int A_gid, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::lchown(A_path_ptr, A_uid, A_gid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::getsid(int A_pid, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::getsid(A_pid);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sched_yield(int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::sched_yield();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sched_get_priority_max(int A_policy, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::sched_get_priority_max(A_policy);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sched_get_priority_min(int A_policy, int errnoPtr) {
		int saved_err = errno;
		errno = 0;
		int ret = ::sched_get_priority_min(A_policy);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sigprocmask(int A_how, int A_set, int A_oset, int errnoPtr) {
		sigset_t *A_set_ptr = (sigset_t *) domainMemoryPtr(this, A_set);
		sigset_t *A_oset_ptr = (sigset_t *) domainMemoryPtr(this, A_oset);
		int saved_err = errno;
		errno = 0;
		int ret = ::sigprocmask(A_how, A_set_ptr, A_oset_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sigpending(int A_set, int errnoPtr) {
		sigset_t *A_set_ptr = (sigset_t *) domainMemoryPtr(this, A_set);
		int saved_err = errno;
		errno = 0;
		int ret = ::sigpending(A_set_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::sigwait(int A_set, int A_sig, int errnoPtr) {
		sigset_t *A_set_ptr = (sigset_t *) domainMemoryPtr(this, A_set);
		int *A_sig_ptr = (int *) domainMemoryPtr(this, A_sig);
		int saved_err = errno;
		errno = 0;
		int ret = ::sigwait(A_set_ptr, A_sig_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::shm_unlink(int A_path, int errnoPtr) {
		char *A_path_ptr = (char *) domainMemoryPtr(this, A_path);
		int saved_err = errno;
		errno = 0;
		int ret = ::shm_unlink(A_path_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::pselect(int A_nd, int A_in, int A_ou, int A_ex, int A_ts, int A_sm, int errnoPtr) {
		fd_set *A_in_ptr = (fd_set *) domainMemoryPtr(this, A_in);
		fd_set *A_ou_ptr = (fd_set *) domainMemoryPtr(this, A_ou);
		fd_set *A_ex_ptr = (fd_set *) domainMemoryPtr(this, A_ex);
		char *A_ts_ptr = (char *) domainMemoryPtr(this, A_ts);
		struct timespec A_ts_tmp;
		memmove(&A_ts_tmp, A_ts_ptr, sizeof(struct timespec));
		sigset_t *A_sm_ptr = (sigset_t *) domainMemoryPtr(this, A_sm);
		int saved_err = errno;
		errno = 0;
		int ret = ::pselect(A_nd, A_in_ptr, A_ou_ptr, A_ex_ptr, &A_ts_tmp, A_sm_ptr);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::__getcwd(int A_buf, int A_buflen, int errnoPtr) {
		char *A_buf_ptr = (char *) domainMemoryPtr(this, A_buf);
		int saved_err = errno;
		errno = 0;
		int ret = 0;
		::getcwd(A_buf_ptr, A_buflen);
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
	}

	int ShellPosixObject::issetugid(int errnoPtr) {
#ifdef __APPLE__
		int saved_err = errno;
		errno = 0;
		int ret = ::issetugid();
		if (errno) {
			int *alc_errno = (int *) domainMemoryPtr(this, errnoPtr);
			*alc_errno = errno;
		}
		errno = saved_err;
		return ret;
#else
		(void)errnoPtr;
		return -1;
#endif /* __APPLE__ */
	}

	ShellPosixClass::ShellPosixClass(VTable *vtable)
		: ClassClosure(vtable)
	{}

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
    }
#endif // VMCFG_ALCHEMY_POSIX
