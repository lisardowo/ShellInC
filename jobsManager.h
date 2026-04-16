
#pragma once

#include "utils.h"
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#define maxJobs 100

typedef struct 
{
    int id;
    pid_t pid;
    char command[1024];
    bool running;
} job;

void initJob();
int addJob(pid_t pid, char *command);
void removeJob(pid_t pid);
void checkBacktroundJobs();
int builtIns();