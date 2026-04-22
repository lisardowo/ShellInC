

#include "history.h"
#include <errno.h>

#define historyBufferMaxSize 9999
#define ownerOnlyPermissions 0600

static void getHistoryFilePath(char *pathBuffer, size_t size);

void addHistory(char *command, int *historyCount, char *historyBuffer[])
{
   if (command[0] == '\0')
   {
        return;
   }

   if (*historyCount > 0 && historyBuffer[*historyCount - 1] != NULL &&
       strcmp(historyBuffer[*historyCount - 1], command) == 0)
   {
        return;
   }
   
   
   
   if(*historyCount < historyBufferMaxSize)
   {

        historyBuffer[*historyCount] = strdup(command);
        (*historyCount)++;

   }

   else
   {
        
        free(historyBuffer[0]);
        memmove(&historyBuffer[0], &historyBuffer[1], (historyBufferMaxSize - 1) * sizeof(char *));
        historyBuffer[historyBufferMaxSize - 1] = strdup(command);

    }

}

void dumpHistory(char *historyBuffer[])
{

    char historyPath[4096];
    getHistoryFilePath(historyPath, sizeof(historyPath));

    int fd = open(historyPath, O_CREAT | O_TRUNC | O_WRONLY | O_NOFOLLOW, ownerOnlyPermissions);

    if(fd == -1)
    {
        if(errno == ELOOP)
        {
            fprintf(stderr, "shell: warning: history file is a symlink, refusing to write\n");
        }
        return;
    }

    for(int i = 0 ; historyBuffer[i] != NULL ; i++)
    {
        dprintf(fd, "%s\n", historyBuffer[i]);
    }
    close(fd);
}

static void getHistoryFilePath(char *pathBuffer, size_t size)
{

    char *home = getenv("HOME");

    if (home == NULL)
    {
        snprintf(pathBuffer, size, ".GIshellHistory");
        return;
    }

    size_t homeLen = strlen(home);
    if (homeLen == 0 || homeLen > 4096)
    {
        fprintf(stderr, "shell: warning: HOME is invalid, using local history file\n");
        snprintf(pathBuffer, size, ".GIshellHistory");
        return;
    }

    snprintf(pathBuffer, size, "%s/.GIshellHistory", home);
}

int getHistory(char *historyBuffer[])//try deleting histCount as argument and declare it locally
{
  
  int historyCount = 0;

    char historyPath[4096];
    getHistoryFilePath(historyPath, sizeof(historyPath));

    int historyFd = open(historyPath, O_RDONLY | O_NOFOLLOW);

  if(historyFd == -1)
  {
        historyBuffer[0] = NULL;
        return 0;
  }

  char chunk[1024];
  char line[4096];
  size_t lineLen = 0 ;
  size_t bytesRead;

  while ((bytesRead = read(historyFd, chunk, sizeof(chunk))) > 0)
  {
    for (size_t i = 0 ; i < bytesRead ; i++)
    {
        if(chunk[i] == '\n')
        {
            line[lineLen] = '\0';
            if(lineLen > 0)
            {
                if(historyCount >= historyBufferMaxSize)
                {
                    close(historyFd);
                    historyBuffer[historyBufferMaxSize - 1] = NULL;
                    return historyCount;
                }
                historyBuffer[historyCount] = strdup(line);
                if(historyBuffer[historyCount] != NULL)
                {
                    (historyCount)++;
                }
            }
            lineLen = 0 ;
        }
        else if (lineLen < sizeof(line) - 1)
        {
            line[lineLen++] = chunk[i];
        }
    }
  }
        if(lineLen > 0 && historyCount < historyBufferMaxSize)
        {
            line[lineLen] = '\0';
            historyBuffer[historyCount] = strdup(line);
            if (historyBuffer[historyCount] != NULL)
            {
                (historyCount)++;
            }
        }
        historyBuffer[historyCount] = NULL;
        close(historyFd);
        
        return historyCount;

}

bool expandHistory(char userInput[], size_t userInputSize, int historyCount, char *historyBuffer[])
{
    char tempBuffer[10000];
    int tempPosition = 0;
    bool expanded = false;

    int commandStart = 0;

    while(userInput[commandStart] == ' ' || userInput[commandStart] == '\t')
    {
        commandStart++;
    }
    if(strncmp(&userInput[commandStart], "history", 7) == 0 && (userInput[commandStart + 7] == '\0' || userInput[commandStart + 7] == ' ' || userInput[commandStart + 7] == '\t'))
    {
        return true;
        
    }

    //char *currentToken = userInput;

    for (int i = 0 ; userInput[i] != '\0' ; i++)
    {
        if(userInput[i] == '!' && userInput[i + 1] != '\0' && userInput[i + 1] != ' ')
        {
            char *expansion = NULL;
            int skipChars = 0;

            if (userInput[i + 1] == '!' && (userInput[i + 2] == '\0' || userInput[i + 2] == ' '))
            {
                if (historyCount > 0)
                {
                    expansion = historyBuffer[historyCount - 1];
                    skipChars = 1 ;
                }
                else
                {
                    printf("shell: !!: no elements in hisroty\n");
                    return false;
                }
            }
          
            else if(userInput[i + 1] >= '1' && userInput[i + 1] <= '9' )
            {
                int v = i + 1;
                char numStr[20];
                int numPos = 0;

                while(userInput[v] >= '0' && userInput[v] <= '9' && numPos < (int)sizeof(numStr) - 1)
                {
                    numStr[numPos++] = userInput[v++];
                }
                numStr[numPos] = '\0';

                errno = 0;
                char *endPtr = NULL;
                long targetIndex = strtol(numStr, &endPtr, 10);

                if (errno == ERANGE || endPtr == numStr || targetIndex <= 0 || targetIndex > historyCount)
                {
                    printf("shell: !%s: not found\n", numStr);
                    return false;
                }

                expansion = historyBuffer[(int)targetIndex - 1];
                skipChars = numPos;
            }
            if (expansion != NULL)
            {
                for (int k = 0; expansion[k] ; k++)
                {
                    if (tempPosition < ((int)sizeof(tempBuffer) - 1))
                    {
                        tempBuffer[tempPosition++] = expansion[k];
                    }
                }
                expanded = true;
                i += skipChars;
            }
            else
            {
                if (tempPosition < ((int)sizeof(tempBuffer) - 1))
                {
                    tempBuffer[tempPosition++] = userInput[i];
                }
            }
        }
      
        else
        {
            if (tempPosition < (int)sizeof(tempBuffer) - 1)
            {
                tempBuffer[tempPosition++] = userInput[i];
            }
        }
    }

    tempBuffer[tempPosition] = '\0';

    if(expanded)
    {
        snprintf(userInput, userInputSize, "%s", tempBuffer);
        printf("%s\n", userInput);
    }

    return true;

}

