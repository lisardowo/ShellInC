
#pragma once

#include <stdlib.h> 
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void writeToFile(char *stdoutPath, char *argv[]);