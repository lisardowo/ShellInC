
#include "utils.h"

extern char prompt[1024];

void createPrompt()
{

    if (getcwd(prompt, sizeof(prompt)) != NULL)
    {
      char *temp[1024];
      char modifiablePrompt[1024];
      strncpy(modifiablePrompt, prompt, sizeof(modifiablePrompt)); 
      modifiablePrompt[sizeof(modifiablePrompt) - 1] = '\0';
      char *modpromptPtr = strtok(modifiablePrompt, "/");
      int i = 0;
      while(modpromptPtr != NULL)
      {

        temp[i] = modpromptPtr;

        i++;
        modpromptPtr = strtok(NULL, "/");
      }
      snprintf(prompt, sizeof(prompt), "%s/%s $ ", temp[i - 2], temp[i - 1]);

    }

}

bool isOperator(char *token)
{
    if (token == NULL)
    {
        return false;
    }

    return strcmp(token, ">") == 0 || strcmp(token, "1>") == 0 || strcmp(token, "2>") == 0 || strcmp(token, ">>") == 0 || strcmp(token, "2>>") == 0 || strcmp(token, "&&") == 0 ;
    

}