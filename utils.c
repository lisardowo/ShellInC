
#include <fcntl.h>
#include "utils.h"

char binPath[100000];
extern char prompt[1024];

void createPrompt()
{

    if (getcwd(prompt, sizeof(prompt)) != NULL)
    {
      if(strcmp(prompt, "/") == 0)
      {
        snprintf(prompt, sizeof(prompt), "/");
        return;
      }
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
      if (i >= 2)
      {
        snprintf(prompt, sizeof(prompt), "%s/%s $ ", temp[i - 2], temp[i - 1]);
      }
      else if (i == 1)
      {
        snprintf(prompt, sizeof(prompt), "%s $", temp[0]);
      }

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
  int fileDescriptor = open(descriptorTarget, flags, 0644);

  return fileDescriptor;
}


char* getPath(char *command)
{
  char *path = getenv("PATH");
  if (path == NULL)
  {
    return NULL;
  }
  char modifiablePath[1000];
  strncpy(modifiablePath, path, sizeof(modifiablePath));
  modifiablePath[sizeof(modifiablePath) - 1] = '\0';
  char *myPtr = strtok(modifiablePath, ":");

  while(myPtr != NULL) 
  {
    
    snprintf(binPath, sizeof(binPath), "%s/%s", myPtr, command);
    
    if (access(binPath, X_OK) == 0)
    {

      return binPath;

    }
   
    myPtr = strtok(NULL, ":");
  }
  return NULL;
}