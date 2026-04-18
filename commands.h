
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "job.h"
#include "arguments.h"
#include "utils.h"
#include "job.h"

extern char binPath[100000];

int type(char **current, bool redirectedstdout ,bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath , char *stdoutAppendPath, char *stderrAppendPath );
int history(char **current, char *historyBuffer[], bool redirectedstdout, bool appendStdOut,  char *stdoutPath, char *stdoutAppendPath);
int echo(char **current, bool redirectedstdout,  bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);
int cd(char **current, bool redirectedstderr, bool appendStdErr, char *stderrPath, char *stderrAppendPath);
int pwd( bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);
int jobs(job *jobList, bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);
char* getPath(char *command);
int executeBin(bool toBackground, char *stdoutPath,char *stdErrPath,char *stdOutAppendPath, char *stdErrAppendPath, bool redirectedstdout, bool redirectedStdErr, bool appendStdOut, bool appendStdErr, char *tokens[]);