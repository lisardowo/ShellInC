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

void executeBin(char *stdoutPath,char *stdErrPath, bool redirectedstdout, bool redirectedStdErr, char *argv[])
{

  char* binPath = getPath(argv[0]);

  if (fork() == 0)
  {
    if(redirectedstdout)
    {
      int fdOut = creat(stdoutPath, 0644);
      if (fdOut < 0)
      {
        _exit(1);
      }
      dup2(fdOut, STDOUT_FILENO);
      close(fdOut);

    }
  
    if (redirectedStdErr)
    {
      int fdError = creat(stdErrPath, 0644);
      if (fdError < 0)
      {
        _exit(1);
      }
      dup2(fdError, STDERR_FILENO);
      close(fdError);
    }
  
    execv(binPath, argv);
    _exit(127);
  }
  else
  {
    wait(NULL);
  }
}
