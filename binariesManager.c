#include "binariesManager.h"
#include "redirectOutput.h"

char binPath[100000];

char* getPath(char *command)
{
  char *path = getenv("PATH");
  char modifiablePath[10000];
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

void executeBin(char *redirectPath, bool redirected)
{
  if(!redirected)
  {
    char* binPath = getPath(argv[0]);
     
    if (binPath == NULL)
    {
        printf("%s: command not found\n", argv[0]);
    }
    else
    {
      if(fork() == 0)
      {

        execv(binPath, argv); //path , command , arguments 
                              //null indicates end of arguments                  
      }

      else
      {
        wait(NULL);
      }
    }
  }
    else
    {
      writeToFile(redirectPath, argv);
    }
}
