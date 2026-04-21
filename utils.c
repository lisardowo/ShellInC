
#include <fcntl.h>
#include "utils.h"

char binPath[100000]; //TODO what if someone on purpose creates a long ass binary ?
extern char prompt[1024]; //same problem of "main" prompt
#define generalPermissions 0644

void createPrompt()
{

    char tempcwd[1024];

    if (getcwd(prompt, sizeof(tempcwd)) != NULL)
    {
      if(strcmp(tempcwd, "/") == 0)
      {
        snprintf(prompt, sizeof(tempcwd), "~ / $ ");
        return;
       }

      char *temp[1024]; //TODO using a fixed size of cwd and prompting creates the ability to RCE -> I read that dirs name are capped to 256 but PATHS can be up to 4096
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
      if (i >= 2)
      {
        snprintf(prompt, sizeof(prompt), "%s/%s $ ", temp[i - 2], temp[i - 1]);
      }
      else if (i == 1)
      {
        snprintf(prompt, sizeof(prompt), "%s $ ", temp[0]);
      }
      else
      {
        snprintf(prompt, sizeof(prompt), "$ ");
      }

    }
    else
    {
      snprintf(prompt, sizeof(prompt), "$ ");
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

int getFileDescriptor(const char *descriptorTarget, int flags)
{
  int fileDescriptor = open(descriptorTarget, flags, generalPermissions);
  return fileDescriptor;
}


char* getPath(char *command)
{
  char *path = getenv("PATH");
  
  if (path == NULL)
  {
    return NULL;
  }

  char modifiablePath[4096];
  if (strlen(path) >= sizeof(modifiablePath))
  {
    printf("shell: warning: PATH too long");
  }
  strncpy(modifiablePath, path, sizeof(modifiablePath) - 1);
  modifiablePath[sizeof(modifiablePath) - 1] = '\0';

  char *myPtr = strtok(modifiablePath, ":");

  while(myPtr != NULL)
  {
    snprintf(binPath, sizeof(binPath), "%s/%s", myPtr, command);

    if(access(binPath, X_OK) == 0)
    {
      return binPath;
    }

    myPtr = strtok(NULL, ":");
  }
  return NULL;
}

void historyBufferFree(char *historyBuffer[])
{
  for(int i = 0 ; historyBuffer[i] != NULL ; i++ )
  {
    free(historyBuffer[i]);
    historyBuffer[i] = NULL;
  }
}