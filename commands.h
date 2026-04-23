
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "definitions.h"
#include "arguments.h"
#include "utils.h"
#include "definitions.h"

extern char binPath[MAX_LINUX_SIZE];


int executeBin(bool toBackground, const redirectConfig *redirect , char *tokens[]);
int history(char **current, char *historyBuffer[], const redirectConfig *redirect);
int pwd(const redirectConfig *redirect);
int cd(char **current, const redirectConfig *redirect);
int echo(char **current, const redirectConfig *redirect);
int jobs(job *jobList, const redirectConfig *redirect);
int type(char **current, const redirectConfig *redirect);
