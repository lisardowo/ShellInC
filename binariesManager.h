
#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include "arguments.h"

extern char binPath[100000];

char* getPath(char *command);
int executeBin(char *stdoutPath,char *stdErrPath, bool redirectedstdout, bool redirectedStdErr, bool appendStdOut, bool appendStdErr, char *argv[]);