
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

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