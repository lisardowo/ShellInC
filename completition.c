
#include "completition.h"



bool startCommandsList(availableCommands *list)
{
    list->count = 0;
    list->cap = 64;
    list->items = malloc(list->cap * sizeof(char *));

    return list ->items != NULL;
}

void commandsFree(availableCommands *list)
{
    if(!list || !list->items)
    {
        return ; 
    }
    for (size_t i = 0 ; i < list->count ; i++)
    {
        free(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0 ;
    list->cap = 0 ; //TODO it feels like this will bring problems later ..
}

bool commandListGrow(availableCommands *list)
{
    size_t newCap = list->cap * 2;
    char **temp = realloc(list->items, newCap * sizeof(char *));
    if (!temp)
    {
        return false;
    }
    list->items = temp;
    list->cap = newCap;
    return true;

}

bool commandListAdd (availableCommands *list, char *command)
{
    if (!command || command[0] == '\0')
    {
        return true;
    }
    if (list -> count == list->cap && !commandListGrow(list))
    {
        return false;
    }

    list->items[list->count] = strdup(command);
    
    if (!list ->items[list->count])
    {
        return false;
    }

    list -> count++;
    return true;

}

int compareCommands(const void *a,const void *b)
{
    const char *comA = *(const char * const *)a;
    const char *comB = *(const char * const *)b;
    return strcmp(comA, comB);

}

void commandListSanitize(availableCommands *list)
{
    if (list ->count == 0)
    {
        return;
    }

    qsort(list->items, list->count, sizeof(char *), compareCommands);

    size_t comparator = 1;

    for (size_t index = 1 ; index < list->count ; index++)
    {
        if (strcmp(list->items[index], list->items[comparator - 1]) != 0)
        {
            list->items[comparator++] = list->items[index];
        }
        else
        {
            free(list->items[index]);
        }
    }
    list->count = comparator;
}

bool getBuiltIns(availableCommands *list)
{
    char *builtins[] = {"exit", "echo", "cd", "pwd", "type"};
    size_t n = sizeof(builtins) / sizeof(builtins[0]);

    for (size_t i = 0 ; i < n ; i++)
    
    {
        if (!commandListAdd(list, builtins[i]))
        {
            return false;
        }
    }
    return true;
}

bool getBins(availableCommands *list)
{
    char *path = getenv("PATH");
    
    char *modifiabPath = strdup(path);

    char *ptr = NULL ;
    char *dir = strtok_r(modifiabPath, ":", &ptr);
    while (dir)
    {
        DIR *directory = opendir(dir);
        if (directory)
        {
            struct dirent *entry;
            while ((entry = readdir(directory)) != NULL )
            {
                if(entry->d_name[0] == '.')
                {
                    continue;
                }

                char fullPath[1000];

                snprintf (fullPath , sizeof(fullPath), "%s/%s", dir, entry->d_name);

                if(access(fullPath, X_OK) == 0)
                {
                    if(!commandListAdd(list, entry->d_name))
                    {
                        closedir(directory);
                        free(modifiabPath);
                        return false;
                    }
                }
            }
            closedir(directory);    
        }
        dir = strtok_r(NULL, ":", &ptr);
    }
    free (modifiabPath);
    return true;
}

bool fillCommands(availableCommands *list)
{
    if (!startCommandsList(list)) return false;
    if (!getBuiltIns(list)) { commandsFree(list); return false; }
    if (!getBins(list)) { commandsFree(list); return false; }
    commandListSanitize(list);
    return true;
}


bool startWith(char *word, char *prefix)
{
    size_t v = 0;
    while (prefix[v] != '\0')
    {
        if (word[v] == '\0' || word[v] != prefix[v])
        {
            return false;

        }
        v++;
    }
    return true;
}

size_t prefixMatches (availableCommands *list, char *prefix, char ***matches)
{
    if (!list || !prefix || !matches)
    {
        return 0;
    }

    char **temp = malloc(list->count * sizeof(char *));

    size_t v = 0;
    for (size_t i = 0 ; i < list->count ; i++)
    {
        if (startWith(list->items[i], prefix))
        {
            temp[v++] = strdup(list->items[i]);
        }
    }

    if (v == 0)
    {
        free(temp);
        *matches = NULL;
        return 0;
    }

    *matches = temp;
    return v;

}

size_t lengestCommonPrefix(char **matches, size_t count)
{
    if (!matches || count == 0)
    {
        return 0;
    }
    size_t v = 0 ;
    while (matches[0][v] != '\0')
    {
        for (size_t i = 1 ; i < count ; i++)
        {
            if(matches[i][v] != matches[0][v])
            {
                return v;
            }
        }
        v++;
    }
    return v;
}