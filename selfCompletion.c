
#include "selfCompletion.h"




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
    list->cap = 0 ; 
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
    char *builtins[] = {"exit", "echo", "cd", "pwd", "type", "history", "jobs"};
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

bool enableRaw(void)
{
    if(tcgetattr(STDIN_FILENO, &g_old) == - 1)
    {
        return false;
    }
    struct termios raw = g_old;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    return tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != - 1;
}

void disableRaw(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old);
}

void redraw(char *prompt, char *buf, size_t cursorPos)
{
    printf("\r\033[2K%s%s", prompt, buf);

    int column = strlen(prompt) + cursorPos + 1;

    printf("\033[%dG", column);
    fflush(stdout);
}

void getLastToken(char *buf, size_t cursorPos, char *out, size_t outSize)
{

    int start = cursorPos - 1;
    while (start >= 0 && buf[start] != ' ')
    {
        start --;
    }
    start++;

    size_t i = 0;
    while(start < (int)cursorPos && i + 1 < outSize)
    {
        out[i++] = buf[start++];
    }
    out[i] = '\0';

}

bool isFirstToken(char *buf, size_t cursorPos)
{
    int start = cursorPos - 1;
    while (start >= 0)
    {
        if(buf[start] == ' ')
        {
            return false;
        }
        start--;
    }
    return true;
}

void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize,int *historyCount, char *historyBuffer[])
{

    int historyIndex = *historyCount;
    size_t cursorPos = 0;
    char tempDraft[outSize];
    strcpy(tempDraft, "");
    size_t len = 0;
    int tabCount = 0;
    out[0] = '\0';

    printf("%s", prompt);
    fflush(stdout);

    if(!enableRaw())
    {
        return;
    }

    while(true)
    {
        char c;

        if (read(STDIN_FILENO, &c, 1) != 1)
        {
            disableRaw();
            return;
        }

        if (c == '\033')
        {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) == 0)
            {
                continue;
            }
            if (read(STDIN_FILENO, &seq[1], 1) == 0)
            {
                continue;
            }

            if (seq[0] == '[')
            {
                if (seq[1] == leftArrowKey)
                {
                    if(cursorPos > 0)
                    {
                        cursorPos --;
                        redraw(prompt, out, cursorPos);
                    }
                }
                if(seq[1] == rightArrowKey)
                {
                    if (cursorPos < len)
                    {
                        cursorPos++;
                        redraw(prompt, out, cursorPos);
                    }
                }
                if (seq[1] == upArrowKey)
                {
                    
                    if (*historyCount <= 0)
                    {
                        continue;
                    }
                    if (historyIndex == *historyCount)
                    {
                        strncpy(tempDraft, out, outSize);
                        tempDraft[outSize - 1] = '\0';
                    }
                    if (historyIndex > 0)
                    {
                        historyIndex--;
                        strncpy(out, historyBuffer[historyIndex], outSize);
                        out[outSize - 1] = '\0';
                        len = strlen(out);
                        cursorPos = len;
                        redraw(prompt, out, cursorPos);
                    }
                }
            
            else if (seq[1] == downArrowKey)
            {
                if (*historyCount == 0)
                {
                    continue;
                }
                if (historyIndex < *historyCount)
                {
                    historyIndex ++;
                    if (historyIndex == *historyCount)
                    {
                        strncpy(out, tempDraft, outSize - 1);
                    }
                    else
                    {
                        strncpy(out, historyBuffer[historyIndex], outSize);
                    }
                    out[outSize - 1] = '\0';
                    len = strlen(out);
                    cursorPos = len;
                    redraw(prompt,out, cursorPos);
                }
            }
        }
            continue;
        }

        if (c == '\n')
        {
            out[len] = '\0';
            printf("\n");
            disableRaw();
            return;
        }

        if (c == 127 || c == '\b')
        {
            if (cursorPos > 0)
            {

                for (size_t i = cursorPos -1 ; i < len ; i++)
                {
                    out[i] = out[i + 1];
                }
                len --;
                cursorPos --;
                redraw(prompt, out, cursorPos);

            }
            tabCount = 0;
            continue;
        }

        if (c == '\t')
        {
            char currentWord[256];
            getLastToken(out, cursorPos, currentWord, sizeof(currentWord));

            char **matches = NULL;
            size_t matchesSize = 0;

            if (isFirstToken(out, cursorPos))
            {
                matchesSize = prefixMatches(list, currentWord, &matches);
            }
            else
            {
                matchesSize = fileMatches(currentWord, &matches);
            }

            if (matchesSize == 0)
            {
                write(STDOUT_FILENO, "\a", 1);
            }
            else if (matchesSize == 1)
            {
                size_t v = strlen(currentWord);
                char *full = matches[0];
                while (full[v] != '\0' && (len + 1 < outSize))
                {
                    out[len++] = full[v++];
                }

                if (len + 1 < outSize && full[strlen(full) - 1] != '/')
                {
                    out[len++] = ' ';
                }

                out[len] = '\0';
                len = strlen(out);
                cursorPos = len;
                redraw(prompt, out, cursorPos);
            }
            else
            {
                size_t lcp = lengestCommonPrefix(matches, matchesSize);
                size_t prefixLen = strlen(currentWord);
                while (prefixLen < lcp && (len + 1 < outSize))
                {
                    out[len++] = matches[0][prefixLen++];
                }

                out[len] = '\0';
                len = strlen(out);
                cursorPos = len;
                redraw(prompt, out, cursorPos);

                if(tabCount >= 1)
                {
                    printf("\n");
                    for(size_t i = 0 ; i < matchesSize ; i++)
                    {
                        printf("%s ", matches[i]);
                    }
                    printf("\n");
                    redraw(prompt, out, cursorPos);
                }
            }

            if(matches != NULL)
            {
                for ( size_t i = 0 ; i < matchesSize ; i++)
                {
                    free(matches[i]);
                }
                free(matches);
            }
            tabCount++;
            continue;
        }
        if (c >= 32 && c <= 126)
        {
            if (c == '\'' || c == '\"')
            {
                if (len + 2 < outSize)
                {
                    for (size_t i = len + 2 ; i > cursorPos + 1 ; i--)
                    {
                        out[i] = out[i - 2];
                    }
                    
                    
                    out[cursorPos] = c;
                    out[cursorPos + 1] = c;
                    len += 2;
                    cursorPos += 1;
                    out[len] = '\0';

                    redraw(prompt, out, cursorPos);
                    
                }
                tabCount = 0;
                continue;
            }
            if(len + 1 < outSize)
            {
                if (cursorPos < len)
                {
                    for(size_t i = len; i > cursorPos ; i--)
                    {
                        out[i] = out[i - 1];
                    }
                }
                out[cursorPos] = c;
                len ++;
                cursorPos++;
                out[len] = '\0'; 
              
                redraw(prompt, out, cursorPos);
            }
            tabCount = 0;
        }
    }
}