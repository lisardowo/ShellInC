
#include "fileCompletion.h"

size_t fileMatches(char *prefix, char ***matches)
{
    char dirPath[1024];
    char filePrefix[256];

    char *lastSlash = strrchr(prefix, '/');

    if (lastSlash != NULL)
    {
        int dirLen = lastSlash - prefix + 1;
        strncpy(dirPath, prefix, dirLen);
        dirPath[dirLen] = '\0';
        strcpy(filePrefix, lastSlash + 1);
    }
    else
    {
        strcpy(dirPath, ".");
        strcpy(filePrefix, prefix);
    }

    DIR *dir = opendir(dirPath);
    if (!dir)
    {
        *matches = NULL;
        return 0;
    }
    char *tempMatches[1000];
    size_t count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        if (strncmp(entry->d_name, filePrefix, strlen(filePrefix)) == 0)
        {
            char fullPath[2048];
            if (lastSlash != NULL)
            {
                snprintf(fullPath, sizeof(fullPath), "%s%s", dirPath, entry->d_name);
            }
            else
            {
                strcpy(fullPath, entry->d_name);
            }
            struct stat st;
            char statPath[2048];

            snprintf(statPath, sizeof(statPath), "%s/%s", strcmp(dirPath, ".") == 0 ? ".": dirPath, entry->d_name);
            if (stat(statPath, &st) == 0 && S_ISDIR(st.st_mode))
            {
                strcat(fullPath, "/");
            }
            if (count < 1000)
            {
                tempMatches[count++] = strdup(fullPath);
            }
        }   
    }

    closedir(dir);

    if (count == 0)
    {
        *matches = NULL;
        return 0;
    }

    char **finalMatches = malloc(count * sizeof(char *));
    for (size_t i = 0 ; i < count ; i++)
    {
        finalMatches[i] = tempMatches[i];
    }

    *matches = finalMatches;
    return count;
}
