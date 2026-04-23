
#include <fcntl.h>
#include "utils.h"

char binPath[100000]; 
#define generalPermissions 0644

void createPrompt(char *prompt, size_t promptSize)
{

  char cwd[4096];

  if (getcwd(cwd, sizeof(cwd)) == NULL)
  {
    snprintf(prompt, promptSize, "$ ");
    return;
  }


  char modfiable[4096];
  strncpy(modfiable, cwd , sizeof(modfiable) - 1);
  modfiable[sizeof(modfiable) - 1] = 0;

  char *parts[512];
  int count = 0;

  char *token = strtok(modfiable, "/");
  while (token != NULL && count < 511)
  {
    parts[count++] = token;
    token = strtok(NULL, "/");
  }

  if (count >= 2)
  {
    snprintf(prompt, promptSize, "%s/%s $ ", parts[count - 2] , parts[count - 1]);
  }
  else if (count == 1)
  {
    snprintf(prompt, promptSize, "%s $ ", parts[0]);
  }
  else
  {
    snprintf(prompt, promptSize, "$ ");
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
  
  if (path == NULL || command == NULL || command[0] == '\0')
  {
    return NULL;
  }

  char modifiablePath[4096];
  strncpy(modifiablePath, path, sizeof(modifiablePath) - 1);
  modifiablePath[sizeof(modifiablePath) - 1] = '\0';

  char possibleBin[4096];
  char *dir = strtok(modifiablePath, ":");

  while(dir != NULL)
  {
    snprintf(possibleBin, sizeof(possibleBin), "%s/%s", dir, command);

    if(access(possibleBin, X_OK) == 0)
    {
      return strdup(possibleBin);
    }
    
    dir = strtok(NULL, ":");

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