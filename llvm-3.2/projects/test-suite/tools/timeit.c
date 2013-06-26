/*===-- timeit.c - LLVM Test Suite Timing Tool ------------------*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>

/* Enumeration for our exit status codes. */
enum ExitCode {
  /* \brief Indicates a failure monitoring the target. */
  EXITCODE_MONITORING_FAILURE = 66,

  /* \brief Indicates a failure in exec() which usually means an invalid program
   * name. */
  EXITCODE_EXEC_FAILURE = 67,
  EXITCODE_EXEC_NOENTRY = 127,
  EXITCODE_EXEC_NOPERMISSION = 126,

  /* \brief Indicates that we were unexpectedly signalled(). */
  EXITCODE_SIGNALLED = 68,

  /* \brief Indicates the child was signalled. */
  EXITCODE_CHILD_SIGNALLED = 69
};

/* \brief Record our own program name, for error messages. */
static const char *g_program_name = 0;

/* \brief Record the child command name, for error messages. */
static const char *g_target_program = 0;

/* \brief If given, report output in POSIX mode format. */
static int g_posix_mode = 0;

/* \brief If non-zero, execute the program with a timeout of the given number
 * of seconds.
 */
static int g_timeout_in_seconds = 0;

/* \brief If non-zero, the PID of the process being monitored. */
static pid_t g_monitored_pid = 0;

/* \brief If non-zero, the path to attempt to chdir() to before executing the
 * target. */
static const char *g_target_exec_directory = 0;

/* \brief If non-zero, the path to write the summary information to (exit status
 * and timing). */
static const char *g_summary_file = 0;

/* \brief If non-zero, the path to redirect the target standard input to. */
static const char *g_target_redirect_input = 0;

/* \brief If non-zero, the path to redirect the target standard output and
 * standard error to. */
static const char *g_target_redirect_output = 0;

/* @name Resource Limit Variables */
/* @{ */

/* \brief If non-sentinel, the CPU time limit to set for the target. */
static rlim_t g_target_cpu_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the stack size limit to set for the target. */
static rlim_t g_target_stack_size_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the data size limit to set for the target. */
static rlim_t g_target_data_size_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the RSS size limit to set for the target. */
static rlim_t g_target_rss_size_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the file size limit to set for the target. */
static rlim_t g_target_file_size_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the core limit to set for the target. */
static rlim_t g_target_core_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the file count limit to set for the target. */
static rlim_t g_target_file_count_limit = ~(rlim_t) 0;

/* \brief If non-sentinel, the subprocess count limit to set for the target. */
static rlim_t g_target_subprocess_count_limit = ~(rlim_t) 0;

/* @} */

static double sample_wall_time(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (double) t.tv_sec + t.tv_usec * 1.e-6;
}

static void terminate_handler(int signal) {
  /* If we are monitoring a process, kill its process group and assume we will
   * complete normally.
   */
  if (g_monitored_pid) {
    fprintf(stderr, ("%s: error: received signal %d. "
                     "killing monitored process(es): %s\n"),
            g_program_name, signal, g_target_program);

    /* Kill the process group of monitored_pid. */
    kill(-g_monitored_pid, SIGKILL);
    return;
  }

  fprintf(stderr, "%s: error: received signal %d. exiting.\n",
          g_program_name, signal);
  /* Otherwise, we received a signal we should treat as for ourselves, and exit
   * quickly. */
  _exit(EXITCODE_SIGNALLED);
}

static void timeout_handler(int signal) {
  fprintf(stderr, "%s: TIMING OUT PROCESS: %s\n", g_program_name,
          g_target_program);
  /* We should always be monitoring a process when we receive an alarm. Kill its
   * process group and assume we will terminate normally.
   */
  kill(-g_monitored_pid, SIGKILL);
}

int monitor_child_process(pid_t pid, double start_time) {
  double real_time, user_time, sys_time;
  struct rusage usage;
  int res, status;

  /* Record the PID we are monitoring, for use in the signal handlers. */
  g_monitored_pid = pid;

  /* If we are running with a timeout, set up an alarm now. */
  if (g_timeout_in_seconds) {
    sigset_t masked;
    sigemptyset(&masked);
    sigaddset(&masked, SIGALRM);

    alarm(g_timeout_in_seconds);
  }

  /* Wait for the process to terminate. */
  do {
    res = waitpid(pid, &status, 0);
  } while (res < 0 && errno == EINTR);
  if (res < 0) {
    perror("waitpid");
    return EXITCODE_MONITORING_FAILURE;
  }

  /* Record the real elapsed time as soon as we can. */
  real_time = sample_wall_time() - start_time;

  /* Just in case, kill the child process group. */
  kill(-pid, SIGKILL);

  /* Collect the other resource data on the children. */
  if (getrusage(RUSAGE_CHILDREN, &usage) < 0) {
    perror("getrusage");
    return EXITCODE_MONITORING_FAILURE;
  }
  user_time = (double) usage.ru_utime.tv_sec + usage.ru_utime.tv_usec/1000000.0;
  sys_time = (double) usage.ru_stime.tv_sec + usage.ru_stime.tv_usec/1000000.0;

  /* If the process was signalled, report a more interesting status. */
  int exit_status;
  if (WIFSIGNALED(status)) {
    fprintf(stderr, "%s: error: child terminated by signal %d\n",
            g_program_name, WTERMSIG(status));

    /* Propagate the signalled status to the caller. */
    exit_status = 128 + WTERMSIG(status);
  } else if (WIFEXITED(status)) {
    exit_status = WEXITSTATUS(status);
  } else {
    /* This should never happen, but if it does assume some kind of failure. */
    exit_status = EXITCODE_MONITORING_FAILURE;
  }

  // If we are not using a summary file, report the information as /usr/bin/time
  // would.
  if (!g_summary_file) {
    if (g_posix_mode) {
      fprintf(stderr, "real %12.4f\nuser %12.4f\nsys  %12.4f\n",
              real_time, user_time, sys_time);
    } else {
      fprintf(stderr, "%12.4f real %12.4f user %12.4f sys\n",
              real_time, user_time, sys_time);
    }
  } else {
    /* Otherwise, write the summary data in a simple parsable format. */
    FILE *fp = fopen(g_summary_file, "w");
    if (!fp) {
      perror("fopen");
      return EXITCODE_MONITORING_FAILURE;
    }

    fprintf(fp, "exit %d\n", exit_status);
    fprintf(fp, "%-10s %.4f\n", "real", real_time);
    fprintf(fp, "%-10s %.4f\n", "user", user_time);
    fprintf(fp, "%-10s %.4f\n", "sys", sys_time);
    fclose(fp);
  }

  return exit_status;
}

#define set_resource_limit(resource, value) \
  set_resource_limit_actual(#resource, resource, value)
static void set_resource_limit_actual(const char *resource_name, int resource,
                                      rlim_t value) {
  /* Get the current limit. */
  struct rlimit current;
  getrlimit(resource, &current);

  /* Set the limits to as close as requested, assuming we are not super-user. */
  struct rlimit requested;
  requested.rlim_cur = requested.rlim_max = \
    (value < current.rlim_max) ? value : current.rlim_max;
  if (setrlimit(resource, &requested) < 0) {
    fprintf(stderr, "%s: warning: unable to set limit for %s (to {%lu, %lu})\n",
            g_program_name, resource_name, (unsigned long) requested.rlim_cur,
            (unsigned long) requested.rlim_max);
  }
}

static int execute_target_process(char * const argv[]) {
  /* Create a new process group for pid, and the process tree it may spawn. We
   * do this, because later on we might want to kill pid _and_ all processes
   * spawned by it and its descendants.
   */
  setpgid(0, 0);

  /* Redirect the standard input, if requested. */
  if (g_target_redirect_input) {
    FILE *fp = fopen(g_target_redirect_input, "r");
    if (!fp) {
      perror("fopen");
      return EXITCODE_MONITORING_FAILURE;
    }

    int fd = fileno(fp);
    if (dup2(fd, 0) < 0) {
      perror("dup2");
      return EXITCODE_MONITORING_FAILURE;
    }

    fclose(fp);
  }

  /* Redirect the standard output, if requested. */
  if (g_target_redirect_output) {
    FILE *fp = fopen(g_target_redirect_output, "w");
    if (!fp) {
      perror("fopen");
      return EXITCODE_MONITORING_FAILURE;
    }

    int fd = fileno(fp);
    if (dup2(fd, 1) < 0 || dup2(fd, 2) < 0) {
      perror("dup2");
      return EXITCODE_MONITORING_FAILURE;
    }

    fclose(fp);
  }

  /* Honor any requested resource limits. */
  if (g_target_cpu_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_CPU, g_target_cpu_limit);
  }
  if (g_target_stack_size_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_STACK, g_target_stack_size_limit);
  }
  if (g_target_data_size_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_DATA, g_target_data_size_limit);
  }
  if (g_target_rss_size_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_RSS, g_target_rss_size_limit);
  }
  if (g_target_file_size_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_FSIZE, g_target_file_size_limit);
  }
  if (g_target_core_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_CORE, g_target_core_limit);
  }
  if (g_target_file_count_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_NOFILE, g_target_file_count_limit);
  }
  if (g_target_subprocess_count_limit != ~(rlim_t) 0) {
    set_resource_limit(RLIMIT_NPROC, g_target_subprocess_count_limit);
  }
  
  /* Honor the desired target execute directory. */
  if (g_target_exec_directory) {
    if (chdir(g_target_exec_directory) < 0) {
      perror("chdir");
      return EXITCODE_MONITORING_FAILURE;
    }
  }

  execvp(argv[0], argv);
  perror("execv");

  if (errno == ENOENT) {
    return EXITCODE_EXEC_NOENTRY;
  } else if (errno == EACCES) {
    return EXITCODE_EXEC_NOPERMISSION;
  }

  return EXITCODE_EXEC_FAILURE;
}

int execute(char * const argv[]) {
  double start_time;
  pid_t pid;

  /* Set up signal handlers so we can terminal the monitored process(es) on
   * SIGINT or SIGTERM. */
  signal(SIGINT, terminate_handler);
  signal(SIGTERM, terminate_handler);

  /* Set up a signal handler to terminate the process on timeout. */
  signal(SIGALRM, timeout_handler);

  start_time = sample_wall_time();

  /* Fork the child process. */
  pid = fork();
  if (pid < 0) {
    perror("fork");
    return EXITCODE_MONITORING_FAILURE;
  }

  /* If we are in the context of the child process, spawn it. */
  if (pid == 0) {
    /* Setup and execute the target process. This never returns except on
     * failure. */
    return execute_target_process(argv);
  }

  /* Otherwise, we are in the context of the monitoring process. */
  return monitor_child_process(pid, start_time);
}

static int streq(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}

static void usage(int is_error) {
#define WRAPPED "\n                       "
  fprintf(stderr, "usage: %s [options] command ... arguments ...\n",
          g_program_name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  %-20s %s", "-h, --help",
          "Show this help text.\n");
  fprintf(stderr, "  %-20s %s", "-p, --posix",
          "Report time in /usr/bin/time POSIX format.\n");
  fprintf(stderr, "  %-20s %s", "-t, --timeout <N>",
          "Execute the subprocess with a timeout of N seconds.\n");
  fprintf(stderr, "  %-20s %s", "-c, --chdir <PATH>",
          "Execute the subprocess in the given working directory.\n");
  fprintf(stderr, "  %-20s %s", "--summary <PATH>",
          "Write monitored process summary (exit code and time) to PATH.\n");
  fprintf(stderr, "  %-20s %s", "--redirect-output <PATH>",
          WRAPPED "Redirect stdout and stderr for the target to PATH.\n");
  fprintf(stderr, "  %-20s %s", "--redirect-input <PATH>",
          WRAPPED "Redirect stdin for the target to PATH.\n");
  fprintf(stderr, "  %-20s %s", "--limit-cpu <N>",
          WRAPPED "Limit the target to N seconds of CPU time.\n");
  fprintf(stderr, "  %-20s %s", "--limit-stack-size <N>",
          WRAPPED "Limit the target to N bytes of stack space.\n");
  fprintf(stderr, "  %-20s %s", "--limit-data-size <N>",
          WRAPPED "Limit the target to N bytes of data.\n");
  fprintf(stderr, "  %-20s %s", "--limit-rss-size <N>",
          WRAPPED "Limit the target to N bytes of resident memory.\n");
  fprintf(stderr, "  %-20s %s", "--limit-file-size <N>",
          WRAPPED "Limit the target to creating files no more than N bytes.\n");
  fprintf(stderr, "  %-20s %s", "--limit-core <N>",
          WRAPPED "Limit the size for which core files will be generated.\n");
  fprintf(stderr, "  %-20s %s", "--limit-file-count <N>",
          (WRAPPED
           "Limit the maximum number of open files the target can have.\n"));
  fprintf(stderr, "  %-20s %s", "--limit-subprocess-count <N>",
          (WRAPPED
           "Limit the maximum number of simultaneous processes "
           "the target can use.\n"));
  _exit(is_error);
}

int main(int argc, char * const argv[]) {
  int i;

  g_program_name = argv[0];
  for (i = 1; i != argc; ++i) {
    const char *arg = argv[i];

    if (arg[0] != '-')
      break;

    if (streq(arg, "-h") || streq(arg, "--help")) {
      usage(/*is_error=*/0);
    }

    if (streq(arg, "-p") || streq(arg, "--posix")) {
      g_posix_mode = 1;
      continue;
    }

    if (streq(arg, "-t") || streq(arg, "--timeout")) {
      if (i + 1 == argc) {
        fprintf(stderr, "error: %s argument requires an option\n", arg);
        usage(/*is_error=*/1);
      }
      g_timeout_in_seconds = atoi(argv[++i]);
      continue;
    }

    if (streq(arg, "--summary")) {
      if (i + 1 == argc) {
        fprintf(stderr, "error: %s argument requires an option\n", arg);
        usage(/*is_error=*/1);
      }
      g_summary_file = argv[++i];
      continue;
    }

    if (streq(arg, "--redirect-input")) {
      if (i + 1 == argc) {
        fprintf(stderr, "error: %s argument requires an option\n", arg);
        usage(/*is_error=*/1);
      }
      g_target_redirect_input = argv[++i];
      continue;
    }

    if (streq(arg, "--redirect-output")) {
      if (i + 1 == argc) {
        fprintf(stderr, "error: %s argument requires an option\n", arg);
        usage(/*is_error=*/1);
      }
      g_target_redirect_output = argv[++i];
      continue;
    }

    if (streq(arg, "-c") || streq(arg, "--chdir")) {
      if (i + 1 == argc) {
        fprintf(stderr, "error: %s argument requires an option\n", arg);
        usage(/*is_error=*/1);
      }
      g_target_exec_directory = argv[++i];
      continue;
    }

    if (strncmp(arg, "--limit-", 8) == 0) {
      rlim_t value;

      if (i + 1 == argc) {
        fprintf(stderr, "error: %s argument requires an option\n", arg);
        usage(/*is_error=*/1);
      }

      value = atoi(argv[++i]);
      if (streq(arg, "--limit-cpu")) {
        g_target_cpu_limit = value;
      } else if (streq(arg, "--limit-stack-size")) {
        g_target_stack_size_limit = value;
      } else if (streq(arg, "--limit-data-size")) {
        g_target_data_size_limit = value;
      } else if (streq(arg, "--limit-rss-size")) {
        g_target_rss_size_limit = value;
      } else if (streq(arg, "--limit-file-size")) {
        g_target_file_size_limit = value;
      } else if (streq(arg, "--limit-core")) {
        g_target_core_limit = value;
      } else if (streq(arg, "--limit-file-count")) {
        g_target_file_count_limit = value;
      } else if (streq(arg, "--limit-subprocess-count")) {
        g_target_subprocess_count_limit = value;
      } else {
        fprintf(stderr, "error: invalid limit argument '%s'\n", arg);
        usage(/*is_error=*/1);
      }
      continue;
    }

    fprintf(stderr, "error: invalid argument '%s'\n", arg);
    usage(/*is_error=*/1);
  }

  if (i == argc) {
    fprintf(stderr, "error: no command (or arguments) was given\n");
    usage(/*is_error=*/1);
  }

  g_target_program = argv[i];
  return execute(&argv[i]);
}
