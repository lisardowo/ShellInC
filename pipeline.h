
#pragma once


#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include "builtIn.h"
#include "utils.h"
#include "binariesManager.h"

#define initializerValues -1

int reddirectInChild(bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);
int runPipeline(bool toBackground, char *argv[],char *commands[100][100], int commandCount ,char **historyBuffer, bool redirectedstdout, bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath, char *stdoutAppendPath, char *stderrAppendPath);
int externalInChild(char **current, bool redirectedStdErr, bool appendStdErr, char *stdErrPath, char* stdErrAppendPath);
int runBuiltinChild(char *argv[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);
int runBuiltin(char *argv[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);
int reddirectInChild(bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);