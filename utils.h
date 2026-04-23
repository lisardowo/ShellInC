#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

bool isOperator(char *token);
void createPrompt(char *prompt, size_t promptSize);
int getFileDescriptor(const char* descriptorTarget, int flags);
char* getPath(char *command);
void historyBufferFree(char *historyBuffer[]);