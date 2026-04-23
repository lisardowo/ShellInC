
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
#include "definitions.h"
#include "utils.h"

#define maxJobs 100

extern job jobList[maxJobs];

void ignoreSignalsInParent(void);
void restoreSignalsInChild(void);

int addJob(pid_t pid, char *command);
void removeJob(pid_t pid);
void checkBacktroundJobs();
int reddirectInChild(redirectConfig *redirect);
int runBuiltin(char **current, redirectConfig *redirect, char *historyBuffer[]);
int runBuiltinChild(char **current, redirectConfig *redirect, char *historyBuffer[]);
int runPipeline(bool toBackground, char *commandTokens[],char *commands[100][100], int commandCount, char **historyBuffer, redirectConfig *redirect);
