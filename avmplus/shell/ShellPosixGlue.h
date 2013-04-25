#ifdef VMCFG_ALCHEMY_POSIX
#ifndef SHELLPOSIX_INCLUDED
#define SHELLPOSIX_INCLUDED

namespace avmplus {
	class ShellPosixClass : public ClassClosure
	{
	public:
		Stringp getAppPath();
		ShellPosixClass(VTable *vtable);
	private:
		DECLARE_SLOTS_ShellPosixClass;
	};

	class ShellPosixObject : public ScriptObject
	{
	public:
		ShellPosixObject(VTable *vtable, ScriptObject *delegate);
		virtual int fork(int errnoPtr);
		virtual int read(int A_fd, int A_buf, int A_nbyte, int errnoPtr);
		virtual int write(int A_fd, int A_buf, int A_nbyte, int errnoPtr);
		virtual int open(int A_path, int A_flags, int A_mode, int errnoPtr);
		virtual int close(int A_fd, int errnoPtr);
		virtual int wait4(int A_pid, int A_status, int A_options, int A_rusage, int errnoPtr);
		virtual int link(int A_path, int A_linkpath, int errnoPtr);
		virtual int unlink(int A_path, int errnoPtr);
		virtual int chdir(int A_path, int errnoPtr);
		virtual int fchdir(int A_fd, int errnoPtr);
		virtual int chmod(int A_path, int A_mode, int errnoPtr);
		virtual int chown(int A_path, int A_uid, int A_gid, int errnoPtr);
		virtual Atom lseek(int A_fd, int A_offset_high, int A_offset_low, int A_whence, int errnoPtr);
		virtual int getpid(int errnoPtr);
		virtual int setuid(int A_uid, int errnoPtr);
		virtual int getuid(int errnoPtr);
		virtual int geteuid(int errnoPtr);
		virtual int access(int A_path, int A_flags, int errnoPtr);
		virtual void sync(int errnoPtr);
		virtual int kill(int A_pid, int A_signum, int errnoPtr);
		virtual int getppid(int errnoPtr);
		virtual int dup(int A_fd, int errnoPtr);
		virtual int pipe(int A_fildes, int errnoPtr);
		virtual int getegid(int errnoPtr);
		virtual int getgid(int errnoPtr);
		virtual int ioctl(int A_fd, int A_com, int A_data, int errnoPtr);
		virtual int revoke(int A_path, int errnoPtr);
		virtual int symlink(int A_path, int A_link, int errnoPtr);
		virtual int umask(int A_newmask, int errnoPtr);
		virtual int chroot(int A_path, int errnoPtr);
		virtual int msync(int A_addr, int A_len, int A_flags, int errnoPtr);
		virtual int vfork(int errnoPtr);
		virtual int getgroups(int A_gidsetsize, int A_gidset, int errnoPtr);
		virtual int setgroups(int A_gidsetsize, int A_gidset, int errnoPtr);
		virtual int getpgrp(int errnoPtr);
		virtual int setpgid(int A_pid, int A_pgid, int errnoPtr);
		virtual int getdtablesize(int errnoPtr);
		virtual int dup2(int A_from, int A_to, int errnoPtr);
		virtual int fcntl(int A_fd, int A_cmd, int A_arg, int errnoPtr);
		virtual int fsync(int A_fd, int errnoPtr);
		virtual int setpriority(int A_which, int A_who, int A_prio, int errnoPtr);
		virtual int socket(int A_domain, int A_type, int A_protocol, int errnoPtr);
		virtual int getpriority(int A_which, int A_who, int errnoPtr);
		virtual int setsockopt(int A_s, int A_level, int A_name, int A_val, int A_valsize, int errnoPtr);
		virtual int listen(int A_s, int A_backlog, int errnoPtr);
		virtual int sigsuspend(int A_mask, int errnoPtr);
		virtual int getrusage(int A_who, int A_rusage, int errnoPtr);
		virtual int getsockopt(int A_s, int A_level, int A_name, int A_val, int A_avalsize, int errnoPtr);
		virtual int readv(int A_fd, int A_iovp, int A_iovcnt, int errnoPtr);
		virtual int writev(int A_fd, int A_iovp, int A_iovcnt, int errnoPtr);
		virtual int fchown(int A_fd, int A_uid, int A_gid, int errnoPtr);
		virtual int fchmod(int A_fd, int A_mode, int errnoPtr);
		virtual int setreuid(int A_ruid, int A_euid, int errnoPtr);
		virtual int setregid(int A_rgid, int A_egid, int errnoPtr);
		virtual int rename(int A_from, int A_to, int errnoPtr);
		virtual int mkfifo(int A_path, int A_mode, int errnoPtr);
		virtual int shutdown(int A_s, int A_how, int errnoPtr);
		virtual int socketpair(int A_domain, int A_type, int A_protocol, int A_rsv, int errnoPtr);
		virtual int mkdir(int A_path, int A_mode, int errnoPtr);
		virtual int rmdir(int A_path, int errnoPtr);
		virtual int setsid(int errnoPtr);
		virtual int getdirentries(int A_fd, int A_buf, int A_count, int A_basep, int errnoPtr);
		virtual int setgid(int A_gid, int errnoPtr);
		virtual int setegid(int A_egid, int errnoPtr);
		virtual int seteuid(int A_euid, int errnoPtr);
		virtual int stat(int A_path, int A_ub, int errnoPtr);
		virtual int fstat(int A_fd, int A_sb, int errnoPtr);
		virtual int lstat(int A_path, int A_ub, int errnoPtr);
		virtual int pathconf(int A_path, int A_name, int errnoPtr);
		virtual int fpathconf(int A_fd, int A_name, int errnoPtr);
		virtual int getpgid(int A_pid, int errnoPtr);
		virtual int semget(int A_key, int A_nsems, int A_semflg, int errnoPtr);
		virtual int msgget(int A_key, int A_msgflg, int errnoPtr);
		virtual int msgsnd(int A_msqid, int A_msgp, int A_msgsz, int A_msgflg, int errnoPtr);
		virtual int msgrcv(int A_msqid, int A_msgp, int A_msgsz, int A_msgtyp, int A_msgflg, int errnoPtr);
		virtual int shmdt(int A_shmaddr, int errnoPtr);
		virtual int shmget(int A_key, int A_size, int A_shmflg, int errnoPtr);
		virtual int clock_gettime(int A_clock_id, int A_tp, int errnoPtr);
		virtual int nanosleep(int A_rqtp, int A_rmtp, int errnoPtr);
		virtual int lchown(int A_path, int A_uid, int A_gid, int errnoPtr);
		virtual int getsid(int A_pid, int errnoPtr);
		virtual int sched_yield(int errnoPtr);
		virtual int sched_get_priority_max(int A_policy, int errnoPtr);
		virtual int sched_get_priority_min(int A_policy, int errnoPtr);
		virtual int sigprocmask(int A_how, int A_set, int A_oset, int errnoPtr);
		virtual int sigpending(int A_set, int errnoPtr);
		virtual int sigwait(int A_set, int A_sig, int errnoPtr);
		virtual int shm_unlink(int A_path, int errnoPtr);
		virtual int pselect(int A_nd, int A_in, int A_ou, int A_ex, int A_ts, int A_sm, int errnoPtr);
	private:
		DECLARE_SLOTS_ShellPosixObject;
	};
}

#endif // SHELLPOSIX_INCLUDED
#endif // VMCFG_ALCHEMY_POSIX
