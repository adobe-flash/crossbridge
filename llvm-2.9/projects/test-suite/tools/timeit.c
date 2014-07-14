/*===-- timeit.c - LLVM Test Suite Timing Tool ------------------*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>

int g_posix_mode = 0;

static double sample_wall_time(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (double) t.tv_sec + t.tv_usec * 1.e-6;
}

int execute(char * const argv[]) {
  double start_time = sample_wall_time();
  pid_t pid = fork();

  double real_time, user_time, sys_time;
  struct rusage usage;
  int res, status;

  if (!pid) {
    execvp(argv[0], argv);
    perror("execvp");
    return 127;
  }

  res = waitpid(pid, &status, 0);
  if (res < 0) {
    perror("waitpid");
    return 126;
  }

  real_time = sample_wall_time() - start_time;

  if (getrusage(RUSAGE_CHILDREN, &usage) < 0) {
    perror("getrusage");
    return 125;
  }
  user_time = (double) usage.ru_utime.tv_sec + usage.ru_utime.tv_usec/1000000.0;
  sys_time = (double) usage.ru_stime.tv_sec + usage.ru_stime.tv_usec/1000000.0;

  if (g_posix_mode) {
    fprintf(stderr, "real %12.4f\nuser %12.4f\nsys  %12.4f\n",
            real_time, user_time, sys_time);
  } else {
    fprintf(stderr, "%12.4f real %12.4f user %12.4f sys\n",
            real_time, user_time, sys_time);
  }

  return WEXITSTATUS(status);
}

int main(int argc, char * const argv[]) {
  int i;

  for (i = 1; i != argc; ++i) {
    if (argv[i][0] != '-')
      break;

    switch (argv[i][1]) {
    case 'p':
      g_posix_mode = 1;
      continue;

    default:
      fprintf(stderr, "error: invalid argument '%s'\n", argv[i]);
      return 1;
    }
  }

  return execute(&argv[i]);
}
