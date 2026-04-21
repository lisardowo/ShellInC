
#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "utils.h"
#include <errno.h>


void addHistory(char *command, int *historyCount, char *historyBuffer[]);
void dumpHistory(char *historyBuffer[]);
int getHistory(char *historyBuffer[]);
bool expandHistory(char userInput[], size_t userInputSize, int historyCount, char *historyBuffer[]);
