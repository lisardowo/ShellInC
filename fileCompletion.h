#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

size_t fileMatches(char *prefix, char ***matches);