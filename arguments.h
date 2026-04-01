#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inputManager.h"

#define     normal          1
#define     singleQuote     2
#define     doubleQuote     3


extern char *argv[100];
void argumentCounter(char *userInput, int* argumentCount);
void argumentExtractor(char *userInput, int argumentCount);
bool toogleState(bool state);
