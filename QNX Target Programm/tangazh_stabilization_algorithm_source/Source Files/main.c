

#include "tangazh_stabilization_algorithm.h"                 
#include "rtwtypes.h"                 
#include "ert_qnx_utils.h"
#include "ext_work.h"                 
#include "ext_qnx_work.h"


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/procfs.h>
#include <sys/states.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/syspage.h>
#include <sys/socket.h>
#include <sys/siginfo.h>
#include <rpc/rpc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <process.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct threadStructTag {
  char* name;
  int_T channelId;
  sem_t* sem;
  pthread_t threadId;
  int_T priority;
  int_T thExecCount;
  int_T timeOutCount;
} threadStructT;

static threadStructT GBLthreadArr[2];


#define START_STOP_SEM_NAME            "startStopSem"
#ifndef STACK_SIZE
#define STACK_SIZE                     16384
#endif

#define GBL_CMD_START_THREAD           1000
#define GBL_START_PRIORITY             20
#define MY_PULSE_CODE                  _PULSE_CODE_MINAVAIL

typedef union {
  struct _pulse pulse;
} my_message_t;

sem_t *startStopSem;

static int CountStrs(const char_T *str)
{
  int count = 0;
  const char_T *strPtr = str;
  while (*strPtr != '\0') {
    // Найти подстроку
    while (isspace(*strPtr))
      strPtr++;
    count++;
    strPtr++;

    while ((!isspace(*strPtr)) && (*strPtr != '\0'))
      strPtr++;
  }

  return(count);
}

static char_T *GetNextStr(char_T *str, char_T **strPtrNext)
{
  int_T done = TRUE;                  
  char_T *strPtr = (NULL);             
  *strPtrNext = (NULL);             

  while (isspace(*str)) {
    str++;
    if (*str == '\0') {
      goto EXIT_POINT;
    }
  }

  strPtr = str++;

  while (!isspace(*str) && (*str != '\0')) {
    str++;
  }

  if (*str != '\0') {
    done = FALSE;
    *str = '\0';
  }

  if (!done) {
    str++;
    while (isspace(*str)) {
      str++;
      if (*str == '\0') {
        break;
      }
    }

    *strPtrNext = (*str == '\0') ? (NULL) : str;
  } else {
    *strPtrNext = (NULL);
  }

 EXIT_POINT:
  return(strPtr);
}          

static void* tBaseRate(void *thPtrVoid)
{
  int_T i;
  threadStructT *thPtr = (threadStructT *) thPtrVoid;
  struct sigevent event;
  struct itimerspec itime;
  timer_t timer_id;
  my_message_t msg;
  int chid;
  int rcvid;
  real_T minorStep;
  real_T lval_nsec;
  event.sigev_notify = SIGEV_PULSE;
  event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0,
    thPtr->channelId,
    _NTO_SIDE_CHANNEL, 0);
  event.sigev_priority = getprio(0);
  event.sigev_code = MY_PULSE_CODE;

  timer_create(CLOCK_REALTIME, &event, &timer_id);
  minorStep = 0.2 * 1000000;
  itime.it_value.tv_sec = (int_T)(minorStep/1000000);
  lval_nsec = fmod((real_T)minorStep,((real_T)1000000.0)) * 1000.0;
  itime.it_value.tv_nsec = (long long)lval_nsec;
  itime.it_interval.tv_nsec = (long long)lval_nsec;

  timer_settime(timer_id, 0, &itime, NULL);
  while (1) {
    if ((rtmGetErrorStatus(tangazh_stabilization_algorithm_M) != (NULL)) || rtmGetStopRequested
        (tangazh_stabilization_algorithm_M)) {
      fprintf(stderr,"Error status: %s; Stop request: %d \n", rtmGetErrorStatus
              (tangazh_stabilization_algorithm_M),rtmGetStopRequested(tangazh_stabilization_algorithm_M));
      sem_post(startStopSem);
      return((int *)1);
    }

    rcvid = MsgReceive(thPtr->channelId, &msg, sizeof(msg), NULL);

    if (thPtr->thExecCount == 0) {
      thPtr->thExecCount++;
      tangazh_stabilization_algorithm_step();

      thPtr->thExecCount--;
    } else {
      thPtr->timeOutCount++;
    }

    rtExtModeCheckEndTrigger();
  }

  return((int *)1);
}


int_T main(int_T priority, char_T *optStr,int_T port)
{
  const char *status;
  int optStrLen = strlen(optStr);
  int argc = 0;
  char_T **argv = (NULL);
  optStr = strdup("-w");
  port = 17725;

  if ((optStr != (NULL)) && (optStrLen > 0)) {
    int i;
    int count;
    char_T *thisStr;
    char_T *nextStr;


    for (i=0; i<optStrLen; i++) {
      optStr[i] = tolower(optStr[i]);
    }

    argc = CountStrs(optStr) + 1;
    argv = (char **)calloc(argc,sizeof(char *));
    if (argv == (NULL)) {
      (void)fprintf(stderr,
                    "Memory allocation error while parsing options string.");
      exit(EXIT_FAILURE);
    }

    argv[0] = "dummyProgramName";
    i= 1;
    nextStr = optStr;
    while ((nextStr != (NULL)) && (thisStr = GetNextStr(nextStr, &nextStr)) !=
           (NULL) && ( i < argc )) {
      argv[i] = thisStr;
      i++;
    }

    rtERTExtModeParseArgs(argc, (const char_T **)argv);
    rtExtModeTornadoSetPortInExtUD(port);
    free(argv);
    argv = (NULL);
  }


  sem_unlink(START_STOP_SEM_NAME);
  startStopSem = sem_open(START_STOP_SEM_NAME, O_CREAT,S_IRWXU,0);


  tangazh_stabilization_algorithm_initialize();
  rtExtModeTornadoStartup(tangazh_stabilization_algorithm_M->extModeInfo,
    2,
                          (boolean_T *)&rtmGetStopRequested(tangazh_stabilization_algorithm_M),
    priority,
    STACK_SIZE,
    startStopSem);
  priority = getprio(0) + 2 + 2;
  printf("mw_scheduler priority is %d\n", priority);
  GBLthreadArr[0].thExecCount = 0;
  GBLthreadArr[0].timeOutCount = 0;
  GBLthreadArr[0].priority = priority;
  GBLthreadArr[0].channelId = ChannelCreate(_NTO_CHF_FIXED_PRIORITY);
  GBLthreadArr[0].sem = sem_open("tBaseRate", O_CREAT,S_IRWXU,0);
  GBLthreadArr[0].threadId = tgtCreateThread("tBaseRate", tBaseRate,
    &GBLthreadArr[0], priority, STACK_SIZE);
  sem_wait(startStopSem);
  rtExtModeTornadoCleanup(2);
  sem_unlink(START_STOP_SEM_NAME);

  return(EXIT_SUCCESS);
}
