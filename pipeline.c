#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include "builtIn.h"
#include "utils.h"
#include "binariesManager.h"

int reddirectInChild(bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    if (redirectedStdOut)
    {
        int fd = getFileDescriptor(stdOutPath, O_TRUNC | O_CREAT | O_WRONLY);
        if (fd < 0)
        {
            return 1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if (appendStdOuut)
    {
        int fd = getFileDescriptor(stdoutAppendPath, O_APPEND | O_CREAT | O_WRONLY);
        if (fd < 0 )
        {
            return 1;
        }
        dup2(fd , STDOUT_FILENO);
        close(fd);
    }
    if(redirectedStdErr)
    {
        int fd = getFileDescriptor(stdErrPath, O_TRUNC | O_CREAT | O_WRONLY);
        if (fd < 0 )
        {
            return 1 ;
        }
        dup2(fd, STDERR_FILENO);
    }
    if (appendStdErr)
    {
        int fd = getFileDescriptor(stderrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
        if (fd < 0)
        {
            return 1 ;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    return 0;
} 

int runBuiltin(char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    if (strcmp("echo", current[0]) == 0 )
    {
        return echo(current);
    }
    if (strcmp("cd", current[0]) == 0 )
    {
        return cd(current);
    }
    if (strcmp("pwd", current[0]) == 0 )
    {
        return pwd();
    }
    if (strcmp("history", current[0]) == 0 )
    {
        return history(historyBuffer);
    }
    if (strcmp("type", current[0]) == 0 )
    {
        return type(current, redirectedStdOut, redirectedStdErr, appendStdOuut, appendStdErr, stdOutPath, stdErrPath, stdoutAppendPath, stderrAppendPath);
    }
  return 1 ;
}

int runBuiltinChild(char **current, char **historyBuffer)
{
    if(strcmp("echo", current[0]) == 0)
    {
        return echo(current);
    }
    if(strcmp("pwd", current[0]) == 0)
    {
        return pwd();
    }
    if(strcmp("history", current[0]) == 0)
    {
        return history(historyBuffer);
    }
    if(strcmp("type", current[0]) == 0)
    {
        return type(current, false, false, false, false, NULL, NULL, NULL, NULL);
    }
    if(strcmp("cd", current[0]) == 0 || strcmp("exit", current[0]) == 0)
    {
        printf("%s: not supported inside pipeline\n", current[0]);
        return 1;
    }
    return 1;
}

int externalInChild(char **current, char *stderr)
{
    char *binPath = getPath(current[0]);
    if (binPath == NULL)
    {
        fprintf(stderr, "%s command not found\n", current[0] ); //TODO have a feeling that this snippet gon be bugged
        return 1;
    }
    execv(binPath, current);
    return 0;
}