		/** This interface exposes all of the FreeBSD kernel system calls. To provide reasonable implementations you will need to look at the BSD documentation and also at the BSD libc source code.*/
		public interface IKernel {
		/**
		*/
		function fork(errnoPtr:int):int;
		/**
		*/
		function read(A_fd:int, A_buf:int, A_nbyte:int, errnoPtr:int):int;
		/**
		*/
		function write(A_fd:int, A_buf:int, A_nbyte:int, errnoPtr:int):int;
		/**
		*/
		function open(A_path:int, A_flags:int, A_mode:int, errnoPtr:int):int;
		/**
		*/
		function close(A_fd:int, errnoPtr:int):int;
		/**
		*/
		function wait4(A_pid:int, A_status:int, A_options:int, A_rusage:int, errnoPtr:int):int;
		/**
		*/
		function link(A_path:int, A_linkpath:int, errnoPtr:int):int;
		/**
		*/
		function unlink(A_path:int, errnoPtr:int):int;
		/**
		*/
		function chdir(A_path:int, errnoPtr:int):int;
		/**
		*/
		function fchdir(A_fd:int, errnoPtr:int):int;
		/**
		*/
		function chmod(A_path:int, A_mode:int, errnoPtr:int):int;
		/**
		*/
		function chown(A_path:int, A_uid:int, A_gid:int, errnoPtr:int):int;
		/**
		*/
		function lseek(A_fd:int, A_offset_high:int, A_offset_low:int, A_whence:int, errnoPtr:int):Object;
		/**
		*/
		function getpid(errnoPtr:int):int;
		/**
		*/
		function setuid(A_uid:int, errnoPtr:int):int;
		/**
		*/
		function getuid(errnoPtr:int):int;
		/**
		*/
		function geteuid(errnoPtr:int):int;
		/**
		*/
		function access(A_path:int, A_flags:int, errnoPtr:int):int;
		/**
		*/
		function sync(errnoPtr:int):void;
		/**
		*/
		function kill(A_pid:int, A_signum:int, errnoPtr:int):int;
		/**
		*/
		function getppid(errnoPtr:int):int;
		/**
		*/
		function dup(A_fd:int, errnoPtr:int):int;
		/**
		*/
		function pipe(A_fildes:int, errnoPtr:int):int;
		/**
		*/
		function getegid(errnoPtr:int):int;
		/**
		*/
		function getgid(errnoPtr:int):int;
		/**
		*/
		function ioctl(A_fd:int, A_com:int, A_data:int, errnoPtr:int):int;
		/**
		*/
		function revoke(A_path:int, errnoPtr:int):int;
		/**
		*/
		function symlink(A_path:int, A_link:int, errnoPtr:int):int;
		/**
		*/
		function umask(A_newmask:int, errnoPtr:int):int;
		/**
		*/
		function chroot(A_path:int, errnoPtr:int):int;
		/**
		*/
		function msync(A_addr:int, A_len:int, A_flags:int, errnoPtr:int):int;
		/**
		*/
		function vfork(errnoPtr:int):int;
		/**
		*/
		function getgroups(A_gidsetsize:int, A_gidset:int, errnoPtr:int):int;
		/**
		*/
		function setgroups(A_gidsetsize:int, A_gidset:int, errnoPtr:int):int;
		/**
		*/
		function getpgrp(errnoPtr:int):int;
		/**
		*/
		function setpgid(A_pid:int, A_pgid:int, errnoPtr:int):int;
		/**
		*/
		function getdtablesize(errnoPtr:int):int;
		/**
		*/
		function dup2(A_from:int, A_to:int, errnoPtr:int):int;
		/**
		*/
		function fcntl(A_fd:int, A_cmd:int, A_arg:int, errnoPtr:int):int;
		/**
		*/
		function fsync(A_fd:int, errnoPtr:int):int;
		/**
		*/
		function setpriority(A_which:int, A_who:int, A_prio:int, errnoPtr:int):int;
		/**
		*/
		function socket(A_domain:int, A_type:int, A_protocol:int, errnoPtr:int):int;
		/**
		*/
		function getpriority(A_which:int, A_who:int, errnoPtr:int):int;
		/**
		*/
		function setsockopt(A_s:int, A_level:int, A_name:int, A_val:int, A_valsize:int, errnoPtr:int):int;
		/**
		*/
		function listen(A_s:int, A_backlog:int, errnoPtr:int):int;
		/**
		*/
		function sigsuspend(A_mask:int, errnoPtr:int):int;
		/**
		*/
		function getrusage(A_who:int, A_rusage:int, errnoPtr:int):int;
		/**
		*/
		function getsockopt(A_s:int, A_level:int, A_name:int, A_val:int, A_avalsize:int, errnoPtr:int):int;
		/**
		*/
		function readv(A_fd:int, A_iovp:int, A_iovcnt:int, errnoPtr:int):int;
		/**
		*/
		function writev(A_fd:int, A_iovp:int, A_iovcnt:int, errnoPtr:int):int;
		/**
		*/
		function fchown(A_fd:int, A_uid:int, A_gid:int, errnoPtr:int):int;
		/**
		*/
		function fchmod(A_fd:int, A_mode:int, errnoPtr:int):int;
		/**
		*/
		function setreuid(A_ruid:int, A_euid:int, errnoPtr:int):int;
		/**
		*/
		function setregid(A_rgid:int, A_egid:int, errnoPtr:int):int;
		/**
		*/
		function rename(A_from:int, A_to:int, errnoPtr:int):int;
		/**
		*/
		function mkfifo(A_path:int, A_mode:int, errnoPtr:int):int;
		/**
		*/
		function shutdown(A_s:int, A_how:int, errnoPtr:int):int;
		/**
		*/
		function socketpair(A_domain:int, A_type:int, A_protocol:int, A_rsv:int, errnoPtr:int):int;
		/**
		*/
		function mkdir(A_path:int, A_mode:int, errnoPtr:int):int;
		/**
		*/
		function rmdir(A_path:int, errnoPtr:int):int;
		/**
		*/
		function setsid(errnoPtr:int):int;
		/**
		*/
		function getdirentries(A_fd:int, A_buf:int, A_count:int, A_basep:int, errnoPtr:int):int;
		/**
		*/
		function setgid(A_gid:int, errnoPtr:int):int;
		/**
		*/
		function setegid(A_egid:int, errnoPtr:int):int;
		/**
		*/
		function seteuid(A_euid:int, errnoPtr:int):int;
		/**
		*/
		function stat(A_path:int, A_ub:int, errnoPtr:int):int;
		/**
		*/
		function fstat(A_fd:int, A_sb:int, errnoPtr:int):int;
		/**
		*/
		function lstat(A_path:int, A_ub:int, errnoPtr:int):int;
		/**
		*/
		function pathconf(A_path:int, A_name:int, errnoPtr:int):int;
		/**
		*/
		function fpathconf(A_fd:int, A_name:int, errnoPtr:int):int;
		/**
		*/
		function getpgid(A_pid:int, errnoPtr:int):int;
		/**
		*/
		function semget(A_key:int, A_nsems:int, A_semflg:int, errnoPtr:int):int;
		/**
		*/
		function msgget(A_key:int, A_msgflg:int, errnoPtr:int):int;
		/**
		*/
		function msgsnd(A_msqid:int, A_msgp:int, A_msgsz:int, A_msgflg:int, errnoPtr:int):int;
		/**
		*/
		function msgrcv(A_msqid:int, A_msgp:int, A_msgsz:int, A_msgtyp:int, A_msgflg:int, errnoPtr:int):int;
		/**
		*/
		function shmdt(A_shmaddr:int, errnoPtr:int):int;
		/**
		*/
		function shmget(A_key:int, A_size:int, A_shmflg:int, errnoPtr:int):int;
		/**
		*/
		function clock_gettime(A_clock_id:int, A_tp:int, errnoPtr:int):int;
		/**
		*/
		function nanosleep(A_rqtp:int, A_rmtp:int, errnoPtr:int):int;
		/**
		*/
		function lchown(A_path:int, A_uid:int, A_gid:int, errnoPtr:int):int;
		/**
		*/
		function getsid(A_pid:int, errnoPtr:int):int;
		/**
		*/
		function sched_yield(errnoPtr:int):int;
		/**
		*/
		function sched_get_priority_max(A_policy:int, errnoPtr:int):int;
		/**
		*/
		function sched_get_priority_min(A_policy:int, errnoPtr:int):int;
		/**
		*/
		function sigprocmask(A_how:int, A_set:int, A_oset:int, errnoPtr:int):int;
		/**
		*/
		function sigpending(A_set:int, errnoPtr:int):int;
		/**
		*/
		function sigwait(A_set:int, A_sig:int, errnoPtr:int):int;
		/**
		*/
		function shm_unlink(A_path:int, errnoPtr:int):int;
		/**
		*/
		function pselect(A_nd:int, A_in:int, A_ou:int, A_ex:int, A_ts:int, A_sm:int, errnoPtr:int):int;
		/**
		*/
		function __getcwd(A_buf:int, A_buflen:int, errnoPtr:int):int;
		/**
		*/
		function issetugid(errnoPtr:int):int;
	}
