#include "timestatus.h"
#include "util.h"
#include <time.h>

#if defined(CLOCKS_PER_SEC)
/* ANSI/ISO systems */
# define TS_CLOCKS_PER_SEC CLOCKS_PER_SEC
#elif defined(CLK_TCK)
/* Non-standard systems */
# define TS_CLOCKS_PER_SEC CLK_TCK
#elif defined(HZ)
/* Older BSD systems */
# define TS_CLOCKS_PER_SEC HZ
#else
# error no suitable value for TS_CLOCKS_PER_SEC
#endif

/*********************************************************/
/* ts_real_time: real time elapsed in seconds            */
/*********************************************************/
FLOAT ts_real_time(long frame) {

  static time_t initial_time;
  time_t current_time;

  time(&current_time);

  if (frame==0) {
    initial_time = current_time;
  }

  return (FLOAT) difftime(current_time, initial_time);
}

/*********************************************************/
/* ts_process_time: process time elapsed in seconds      */
/*********************************************************/
FLOAT ts_process_time(long frame) {
  static clock_t initial_time;
  clock_t current_time;

#if ( defined(_MSC_VER) || defined(__BORLANDC__) ) 

  { static HANDLE hProcess;
    FILETIME Ignored1, Ignored2, KernelTime, UserTime;

    if ( frame==0 ) {
      hProcess = GetCurrentProcess();
    }
        
    /* GetProcessTimes() always fails under Win9x */
    if (GetProcessTimes(hProcess, &Ignored1, &Ignored2, &KernelTime, &UserTime)) {
      LARGE_INTEGER Kernel = { KernelTime.dwLowDateTime, KernelTime.dwHighDateTime };
      LARGE_INTEGER User = { UserTime.dwLowDateTime, UserTime.dwHighDateTime };

      current_time = (clock_t)((FLOAT)(Kernel.QuadPart + User.QuadPart) * TS_CLOCKS_PER_SEC / 10000000);
    } else {
      current_time = clock();
	}
  }
#else
  current_time = clock();
#endif

  if (frame==0) {
    initial_time = current_time;
  }

  return (FLOAT)(current_time - initial_time) / TS_CLOCKS_PER_SEC;
}

#undef TS_CLOCKS_PER_SEC

typedef struct ts_times {
  FLOAT so_far;
  FLOAT estimated;
  FLOAT speed;
  FLOAT eta;
} ts_times;

/*********************************************************/
/* ts_calc_times: calculate time info (eta, speed, etc.) */
/*********************************************************/
void ts_calc_times(ts_times *time, int samp_rate, long frame, long frames,int framesize)
{
  if (frame > 0) {
    time->estimated = time->so_far * frames / frame;
    if (samp_rate * time->estimated > 0) {
      time->speed = frames * framesize / (samp_rate * time->estimated);
    } else {
      time->speed = 0;
    }
    time->eta = time->estimated - time->so_far;
  } else {
    time->estimated = 0;
	time->speed = 0;
	time->eta = 0;
  }
}

/*********************************************************/
/* timestatus: display encoding process time information */
/*********************************************************/
void timestatus(int samp_rate,long frameNum,long totalframes,int framesize)
{
  ts_times real_time, process_time;
  int percent;

  real_time.so_far = ts_real_time(frameNum);
  process_time.so_far = ts_process_time(frameNum);

  if (frameNum == 0) {
    fprintf(stderr, "    Frame          |  CPU/estimated  |  time/estimated | play/CPU |   ETA\n");
    return;
  }  

  ts_calc_times(&real_time, samp_rate, frameNum, totalframes, framesize);
  ts_calc_times(&process_time, samp_rate, frameNum, totalframes, framesize);

  if (totalframes > 1) {
    percent = (int)(100.0 * frameNum / (totalframes - 1));
  } else {
    percent = 100;
  }

#  define TS_TIME_DECOMPOSE(time) \
    (int)((long)(time+.5) / 3600), \
    (int)((long)((time+.5) / 60) % 60), \
    (int)((long)(time+.5) % 60)

  fprintf(stderr,
    "\r%6ld/%6ld(%3d%%)|%2d:%02d:%02d/%2d:%02d:%02d|%2d:%02d:%02d/%2d:%02d:%02d|%10.4f|%2d:%02d:%02d ",
    frameNum,
    totalframes - 1,
    percent,
    TS_TIME_DECOMPOSE(process_time.so_far),
    TS_TIME_DECOMPOSE(process_time.estimated),
    TS_TIME_DECOMPOSE(real_time.so_far),
	TS_TIME_DECOMPOSE(real_time.estimated),
    process_time.speed,
    TS_TIME_DECOMPOSE(real_time.eta)
  );

  fflush(stderr);
}

