
#pragma once

#include "completition.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize);
void firstToken(char *buf, char *out, size_t outSize);
void redraw(char *prompt, char *buf);
void disableRaw(void);
bool enableRaw(void);