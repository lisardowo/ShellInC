
#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <termios.h>

#define upArrowKey   'A'
#define downArrowKey 'B'
#define rightArrowKey 'C'
#define leftArrowKey  'D'
//#define ctrlLeft    ';5D'
//#define ctrlRight  ';5C' 

size_t fileMatches(char *prefix, char ***matches);

typedef struct 
{
    char **items;
    size_t count; 
    size_t cap;
}   availableCommands;


bool startCommandsList(availableCommands *list);
void commandsFree(availableCommands *list);
bool commandListGrow(availableCommands *list);
bool commandListAdd (availableCommands *list, char *command);
int compareCommands(const void *a,const void *b);
void commandListSanitize(availableCommands *list);
bool getBuiltIns(availableCommands *list);
bool getBins(availableCommands *list);
bool fillCommands(availableCommands *list);
size_t lengestCommonPrefix(char **matches, size_t count);
size_t prefixMatches (availableCommands *list, char *prefix, char ***matches);
bool startWith(char *word, char *prefix);
void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize, int *historyCount, char* historyBuffer[]);
void firstToken(char *buf, char *out, size_t outSize);
void redraw(char *prompt, char *buf, size_t cursorPosition);
void disableRaw(void);
bool enableRaw(void);
