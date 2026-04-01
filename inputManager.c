#include "inputManager.h"

char userInput[10000];

char* sanitizeInput(char *userInput)
{

    size_t strLen = strlen(userInput);
    if(strLen > 0 && userInput[strLen - 1] == '\n')
    {
        userInput[strLen - 1] = '\0';
    }
    return userInput;

}



