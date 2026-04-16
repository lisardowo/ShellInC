
#include "binariesManager.h"


int executeBin(bool toBackground, char *stdoutPath,char *stdErrPath,char *stdOutAppendPath, char *stdErrAppendPath, bool redirectedstdout, bool redirectedStdErr, bool appendStdOut, bool appendStdErr, char *tokens[])
{
  
  char* binPath = getPath(argv[0]);

  if(binPath == NULL)
  {
    printf("%s: command not found\n", argv[0]);
    return -1;
  }
  
  pid_t pid = fork();
  if (pid == 0)
  {

    restoreSignalsInChild();

    if(redirectedstdout)
    {
      
      int fdOut = getFileDescriptor(stdoutPath, O_TRUNC | O_CREAT | O_WRONLY);
      if (fdOut < 0)
      {
        return -1;
      }
      dup2(fdOut, STDOUT_FILENO);
      close(fdOut);

    }
      
    if (redirectedStdErr)
    {
      
      int fdError = getFileDescriptor(stdErrPath, O_TRUNC | O_CREAT | O_WRONLY);
      if (fdError < 0)
      {
       return -1;
      }
      dup2(fdError, STDERR_FILENO);
      close(fdError);

    }

    if(appendStdOut)
    {
      int fdOut = getFileDescriptor(stdOutAppendPath, O_APPEND | O_CREAT | O_WRONLY);
      if (fdOut < 0)
      {
        return -1;
      }
      dup2(fdOut, STDOUT_FILENO);
      close(fdOut);

    }

    if(appendStdErr)
    {
      int fdOut = getFileDescriptor(stdErrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
      if (fdOut < 0)
      {
        return -1;
      }
      dup2(fdOut, STDERR_FILENO);
      close(fdOut);

    }

  
    execv(binPath, tokens);
    exit(1);
  }
  else
  {
    if (toBackground)
    {
      addJob(pid, argv[0]);
      return 0 ;
    }
    else
    {
      waitpid(pid, NULL, 0);
    }
  }
  return 0 ;
}
