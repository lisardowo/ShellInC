#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include "utils.h"
#include "jobsManager.h"

int type(char **current, bool redirectedstdout ,bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath , char *stdoutAppendPath, char *stderrAppendPath );
int history(char **current, char *historyBuffer[], bool redirectedstdout, bool appendStdOut,  char *stdoutPath, char *stdoutAppendPath);
int echo(char **current, bool redirectedstdout,  bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);
int cd(char **current, bool redirectedstderr, bool appendStdErr, char *stderrPath, char *stderrAppendPath);
int pwd( bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);
int jobs(job *jobList, bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);