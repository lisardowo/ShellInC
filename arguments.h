#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inputManager.h"

extern char *argv[100];
void argumentCounter(char *userInput, int* argumentCount);
void argumentExtractor(char *userInput, int argumentCount);
bool toogleQuotes(bool activeQuotes);
void removeQuotes(char *token);
