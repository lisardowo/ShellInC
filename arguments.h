
#pragma once

#include <glob.h>
#include <string.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define     normal          1
#define     singleQuote     2
#define     doubleQuote     3


char* sanitizeInput(char* userInput);

extern char userInput[10000];

extern char *argv[1000];
void argumentCounter(char *userInput, int* argumentCount);
void argumentExtractor(char *userInput, int argumentCount);
void expandGlobs(char *argv[]);
bool toogleState(bool state);
bool toogleQuotes(bool activeQuotes);
void removeQuotes(char *token);
void restoreSpaces(char *userInput);
void spacesInQuotes(char *userInput);
void expandArguments(char *argv[]);
