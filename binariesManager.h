
#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include "arguments.h"

extern char binPath[100000];

char* getPath(char *command);
void executeBin(char *redirectPath, bool redirected);