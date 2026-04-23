
#pragma once

#include "sys/types.h"

#define MAX_LINUX_SIZE 4096


typedef struct 
{
    int id;
    pid_t pid;
    char command[1024];
    bool running;
} job;

typedef struct 
{
 
 bool redirectStdout;
 bool  redirectStderr;
 bool  appendStdout;
 bool appendStderr;
 char *stdOutPath;
 char *stdErrPath;
 char *stdoutAppendPath;
 char *stderrAppendPath;

} redirectConfig;

