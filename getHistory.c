
#include "getHistory.h"

void addHistory(char *command, int historyCount, char *historyBuffer[])
{
    if(command[0] == '\0')
    {
        return;
    }

    if (historyCount > 0 && strcmp(historyBuffer[historyCount - 1 ], command) == 0)
    {
        return;
    }

    if(historyCount < 10000)
    {
        printf("added\n");
        historyBuffer[historyCount] = strdup(command);
        historyCount ++;

    }
}