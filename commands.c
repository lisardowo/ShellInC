#include "commands.h"
#include "proccesess.h"
#include "definitions.h"

#include <errno.h>

int executeBin(bool toBackground, const redirectConfig *redirect , char *tokens[])
{
  
  char* resolvedBinPath = getPath(tokens[0]);

  if(resolvedBinPath == NULL)
  {
    printf("%s: command not found\n", tokens[0]);
    return -1;
  }
  
  pid_t pid = fork();
  if (pid == 0)
  {

    restoreSignalsInChild();

    if(redirect->redirectStdout)
    {
      
      int fdOut = getFileDescriptor(redirect->stdOutPath, O_TRUNC | O_CREAT | O_WRONLY);
      if (fdOut < 0)
      {
        return -1;
      }
      dup2(fdOut, STDOUT_FILENO);
      close(fdOut);

    }
      
    if (redirect->redirectStderr)
    {
      
      int fdError = getFileDescriptor(redirect->stdErrPath, O_TRUNC | O_CREAT | O_WRONLY);
      if (fdError < 0)
      {
       return -1;
      }
      dup2(fdError, STDERR_FILENO);
      close(fdError);

    }

    if(redirect->appendStdout)
    {
      int fdOut = getFileDescriptor(redirect->stdoutAppendPath, O_APPEND | O_CREAT | O_WRONLY);
      if (fdOut < 0)
      {
        return -1;
      }
      dup2(fdOut, STDOUT_FILENO);
      close(fdOut);

    }

    if(redirect->appendStderr)
    {
      int fdOut = getFileDescriptor(redirect->stderrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
      if (fdOut < 0)
      {
        return -1;
      }
      dup2(fdOut, STDERR_FILENO);
      close(fdOut);

    }

  
    execv(resolvedBinPath, tokens);
    free(resolvedBinPath);
    exit(1);
  }
  
  else
  {
    
    free(resolvedBinPath);
    if (toBackground)
    {
      addJob(pid, tokens[0]);
      return 0 ;
    }
    else
    {
      waitpid(pid, NULL, 0);
    }
  
  }
  
  return 0 ;
}


int type(char **current, const redirectConfig *redirect)
{
    
    if (current[1] == NULL)
    {
        printf("Usage : type <command>\n") ;
        return 1;
    }
    if(strcmp(current[1], "--debug") == 0)
    {
        printf("[debug] PATH = %s\n", getenv("PATH") ? getenv("PATH") : "(null)");
        printf("[debug] HOME = %s\n", getenv("HOME") ? getenv("HOME") : "(null)");
        printf("[debug] history = %s/.GIshellHIstory\n", getenv("HOME") ? getenv("HOME") : ".");
        printf("[debug] stack_sample= %p\n", (void *)&current);
        printf("DVShell{VULN04_1nf0_D1scl0sur3_pwnd3d}\n");
        return -2;
    }
   bool isBuiltin = (!strcmp("echo", current[1]) || !strcmp("exit", current[1]) || 
                    !strcmp("type", current[1]) || !strcmp("pwd", current[1]) || 
                    !strcmp("cd", current[1]) || !strcmp("history", current[1]) );

    char *path = isBuiltin ? NULL : getPath(current[1]);

    char message[2048];
    int exitCode;
    if(isBuiltin)
    {
        snprintf(message, sizeof(message), "%s is a shell builtin\n", current[1]);
        exitCode = 0;
    }
    else if (path != NULL)
    {
        snprintf(message, sizeof(message), "%s is %s\n", current[1], path);
        exitCode = 0;
    }
    else
    {
        snprintf(message, sizeof(message), "%s: not found\n", current[1]);
        exitCode = 1;
    }
    if(path != NULL)
    {
        free(path);
    }
    if(exitCode == 0)
    {
        if(redirect->redirectStdout)
        {
            int fd = getFileDescriptor(redirect->stdOutPath, O_TRUNC | O_WRONLY | O_CREAT);
            if (fd == -1)
            {
                printf("shell: couldnt write to file\n");
                return 1;
            }
            dprintf(fd, "%s", message);
            close(fd);
        }
        else if(redirect->appendStdout)
        {
            int fd = getFileDescriptor(redirect->stdoutAppendPath, O_APPEND | O_WRONLY | O_CREAT);
            if (fd == -1)
            {
                printf("shell: couldnt write to file\n");
                return 1;
            }
            dprintf(fd, "%s", message);
            close(fd);
        }
        else
        {
            printf("%s", message);
            return 0;
        }
    }
    else
    {
        if(redirect->appendStderr)
        {
            int fd = getFileDescriptor(redirect->stderrAppendPath, O_APPEND | O_WRONLY | O_CREAT);
            if (fd == -1)
            {
                printf("shell: couldnt write to file\n");
                return 1;
            }
            dprintf(fd, "%s", message);
            close(fd);
        }
        else if(redirect->redirectStderr)
        {
            int fd = getFileDescriptor(redirect->stdErrPath, O_TRUNC | O_WRONLY | O_CREAT);
            if (fd == -1)
            {
                printf("shell: couldnt write to file\n");
                return 1;
            }
            dprintf(fd, "%s", message);
            close(fd);
        }
        else 
        {
            fprintf(stderr, "%s", message);
            return 1;
        }

    }
    return 0;

}

int history(char **current, char *historyBuffer[], const redirectConfig *redirect)
{

    int linesToDisplay = 0 ;
    int start = 0;
    int end = 0;

    while (historyBuffer[linesToDisplay] != NULL)
    {
        linesToDisplay++;
    }

    end = linesToDisplay;

    if (current[1] != NULL)
    {
        if (strcmp(current[1], "!!") == 0)
        {
            if (linesToDisplay == 0)
            {
                printf("shell: !!: no elements in history\n");
                return 1;
            }
            start = linesToDisplay - 1;
            end = linesToDisplay;
        }
        else if (current[1][0] == '!' && current[1][1] != '\0')
        {
            char *indexStr = &current[1][1];

            for (int i = 0 ; indexStr[i] != '\0' ; i++)
            {
                if (indexStr[i] < '0' || indexStr[i] > '9')
                {
                    return 1;
                }
            }
            errno = 0;
            char *endPtr = NULL;
            long targetIndex = strtol(indexStr, &endPtr, 10);
            if(errno == ERANGE || endPtr == indexStr || targetIndex <= 0 || targetIndex > linesToDisplay)
            {
                printf("shell: !%s: not found\n", indexStr);
                return 1;
            }

            start = (int)targetIndex - 1;
            end = (int)targetIndex ; 
        }
        else
        {
           errno = 0;
           char *endPtr = NULL;
           long number = strtol(current[1], &endPtr, 10);

           if (errno != ERANGE && endPtr != current[1] && number > 0 && number < linesToDisplay)
           {
                start = linesToDisplay - (int)number;
           }
        }

    if (redirect->redirectStdout)
    {
        int fd = getFileDescriptor(redirect->stdOutPath, O_CREAT | O_WRONLY | O_TRUNC);
        for (int i = start ; i < end ; i++)
        {
            dprintf(fd, "%d %s\n", i + 1, historyBuffer[i]);
        }
        close(fd);
        return 0;
    }
    
    if (redirect->appendStdout)
    {
        int fd = getFileDescriptor(redirect->stdoutAppendPath, O_CREAT | O_WRONLY | O_APPEND);
        for (int i = start ; i < end ; i++)
        {
            dprintf(fd, "%d %s\n", i + 1, historyBuffer[i]);
        }
        close(fd);
        return 0;
    }

    for (int i = start ; i < end ; i++)
    {
        printf("%d %s\n", i + 1 , historyBuffer[i]);
    }
    return 0;
    }
    return 0;
}

int pwd(const redirectConfig *redirect)
{
    char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)))
      {
        if (redirect->redirectStdout)
        {
            int fd = getFileDescriptor(redirect->stdOutPath, O_CREAT | O_TRUNC | O_WRONLY);
            dprintf(fd, "%s\n",cwd);
            close(fd);
            return 0;
        }

        if (redirect->appendStdout)
        {
            int fd = getFileDescriptor(redirect->stdoutAppendPath, O_CREAT | O_APPEND | O_WRONLY);
            dprintf(fd, "%s\n",cwd);
            close(fd);
            return 0;
        }
        printf("%s\n",cwd);
        return 0;
      }
      else
      {
        return 1;
      }
}

int cd(char **current, const redirectConfig *redirect)
{
     if (current[1] == NULL || strcmp ("~", current[1]) == 0 )
      {
        char *homePath = getenv("HOME");
        if (homePath == NULL)
        {
            printf("cd: HOME not set\n");
            return 1;
        }
        return (chdir(homePath) == 0) ? 0 : 1;
      }
      
      char cdCommand[512];
      snprintf(cdCommand, sizeof(cdCommand), "cd %s && pwd", current[1]);
      int result = system(cdCommand);
      if (result != 0)
      {
        fprintf(stderr, "%s: no such file or directory: \"%s\"\n", current[0], current[1]);
        return 1;
      }

      return 0;

      
}

int echo(char **current, const redirectConfig *redirect)
{
    if (redirect->redirectStdout)
    {
     int fd = getFileDescriptor(redirect->stdOutPath , O_WRONLY | O_TRUNC | O_CREAT);
     for (int i = 1 ; current[i] != NULL ; i++)
      {
        dprintf(fd,"%s ", current[i]);
      }
      dprintf(fd, "\n");
      close(fd);
      return 0;

    }

    if (redirect->appendStdout)
    {
    
        int fd = getFileDescriptor(redirect->stdoutAppendPath , O_WRONLY | O_APPEND | O_CREAT);
        for (int i = 1 ; current[i] != NULL ; i++)
        {
            dprintf(fd,"%s ", current[i]);
        }
        dprintf(fd, "\n");
        close(fd);
        return 0;
        
    }

    for (int i = 1 ; current[i] != NULL ; i++)
      {
        printf("%s ", current[i]);
      }
      printf("\n");
      return 0;
}

int jobs(job *currentJobs, const redirectConfig *redirect)
{
    if (redirect->redirectStdout)
    {

        int fd = getFileDescriptor(redirect->stdOutPath , O_WRONLY | O_TRUNC | O_CREAT);
        for(int i = 0 ; i < maxJobs ; i++)
        {
            if (currentJobs[i].running)
            {
                dprintf(fd, "[%d]    Running         %s\n", currentJobs[i].id, currentJobs[i].command);
            }
    
        }
        
        close(fd);
        return 0;

    }

    if (redirect->appendStdout)
    {
    
        int fd = getFileDescriptor(redirect->stdoutAppendPath , O_WRONLY | O_APPEND | O_CREAT);
        for(int i = 0 ; i < maxJobs ; i++)
        {
            if (jobList[i].running)
            {
                dprintf(fd, "[%d]    Running         %s\n", jobList[i].id, jobList[i].command);
            }
    
        }
        
        close(fd);
        return 0;
        
    }

    for(int i = 0 ; i < maxJobs ; i++)
    {
        if (jobList[i].running)
        {
            printf("[%d]    Running         %s\n", jobList[i].id, jobList[i].command);
        }
    
    }

    return 0;
    
}