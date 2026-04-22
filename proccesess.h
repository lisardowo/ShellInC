
#pragma once

#include "utils.h"
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stddef.h>
#include <fcntl.h>
#include "job.h"
#include "utils.h"

#define maxJobs 100

extern job jobList[maxJobs];

int addJob(pid_t pid, char *command);
void removeJob(pid_t pid);
void checkBacktroundJobs();
void ignoreSignalsInParent();
void restoreSignalsInChild();
int reddirectInChild(bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);
int runPipeline(bool toBackground, char *commandTokens[],char *commands[100][100], int commandCount ,char **historyBuffer, bool redirectedstdout, bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath, char *stdoutAppendPath, char *stderrAppendPath);
int externalInChild(char **current, bool redirectedStdErr, bool appendStdErr, char *stdErrPath, char* stdErrAppendPath);
int runBuiltinChild(char *commandTokens[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);
int runBuiltin(char *commandTokens[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);
