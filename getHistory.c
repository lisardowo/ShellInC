
#include "getHistory.h"
#include "utils.h"

void addHistory(char *command, int *historyCount, char *historyBuffer[])
{
    if(command[0] == '\0')
    {
        return;
    }

    if (*historyCount > 0 && strcmp(historyBuffer[*historyCount - 1 ], command) == 0)
    {
        return;
    }

    if(*historyCount < 10000)
    {
        
        historyBuffer[*historyCount] = strdup(command);
        (*historyCount) ++;

    }
}

void dumpHistory(char *historyBuffer[])
{
    
    int historyFd = getFileDescriptor("historyFile.txt", O_CREAT | O_TRUNC | O_WRONLY);

    for(int i = 0 ; historyBuffer[i] != NULL ; i++)
    {
        dprintf(historyFd, "%s\n", historyBuffer[i]);
    }

}

void getHistory(int *historyCount, char *historyBuffer[])
{
    int historyFD = open("historyFile.txt", O_RDONLY);
    if(historyFD == -1)
    {
        historyBuffer[0] = NULL;
        return;
    }

    char chunk[1024];
    char line[4096];
    size_t lineLen = 0;
    size_t bytesRead;

    while ((bytesRead = read(historyFD, chunk, sizeof(chunk))) > 0)
    {
        for (size_t i = 0 ; i < bytesRead ; i++)
        {
            if(chunk[i] == '\n')
            {
                line[lineLen] = '\0';
                if(lineLen > 0 )
                {
                    historyBuffer[*historyCount] = strdup(line);
                    if (historyBuffer[*historyCount] != NULL)
                    {
                        (*historyCount)++;
                    }
                }
                lineLen = 0;
            }
            else if(lineLen < sizeof(line) - 1)
            {
                line[lineLen++] = chunk[i]; 
            }

        }
    }
    if (lineLen > 0)
    {
        line[lineLen] = '\0';
        historyBuffer[*historyCount] = strdup(line);
        if (historyBuffer[*historyCount] != NULL)
        {
            (*historyCount) ++;
        }
    }

    historyBuffer[*historyCount] = NULL;
    
    
}

bool expandHistory(char *userInput[], int historyCount, char *historyBuffer[])
{
     for (int v = 0 ; userInput[v] != NULL ; v++)
    {
        if(userInput[v] == '!')
        {
            printf("yup\n");
        }
    }

    for (int i = 0 ; userInput[i] != NULL ; i++){
    char tempBuffer[10000];
    int tempPosition = 0;
    bool isExpandible = false;
    char *currentToken = historyBuffer[i];

   
}
}

