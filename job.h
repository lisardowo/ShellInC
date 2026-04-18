
#pragma once

#include "sys/types.h"

typedef struct 
{
    int id;
    pid_t pid;
    char command[1024];
    bool running;
} job;
