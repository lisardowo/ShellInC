
#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include "arguments.h"
#include "binariesManager.h"
#include "utils.h"
#include "signalsManager.h"
#include "jobsManager.h"

extern char binPath[100000];

char* getPath(char *command);
int executeBin(bool toBackground, char *stdoutPath,char *stdErrPath,char *stdOutAppendPath, char *stdErrAppendPath, bool redirectedstdout, bool redirectedStdErr, bool appendStdOut, bool appendStdErr, char *tokens[]);