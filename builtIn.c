
#include "builtIn.h"

int type(char **current, bool redirectedstdout, bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    
    if (current[1] == NULL)
    {
        printf("Usage : type <command>\n") ;
        return 1;
    }
    else if(!strcmp("echo", current[1]) || !strcmp("exit", current[1]) ||
             !strcmp("type", current[1]) || !strcmp("pwd", current[1]) ||
             !strcmp("cd", current[1]) || !strcmp("history", current[1]))
    {

         if(redirectedstdout)
        {
            int fd = getFileDescriptor(stdoutPath, O_TRUNC | O_CREAT | O_WRONLY);
            dprintf(fd,"%s is a shell builtin\n", current[1]);
            return 0;
        }

        if(appendStdOut)
        {
            int fd = getFileDescriptor(stdoutAppendPath, O_APPEND | O_CREAT | O_WRONLY);
            dprintf(fd, "%s is a shell builtin\n", current[1]);
            return 0;
        }

        printf("%s is a shell builtin\n", current[1]);
        return 0;
    }
    else
    {
        if(redirectedstdout)
        {
            int fd = getFileDescriptor(stdoutPath, O_TRUNC | O_CREAT | O_WRONLY);
            char *path = getPath(current[1]);
            if (path != NULL)
            {
                dprintf(fd, "%s is %s\n", current[1], path);
                return  0;
            }
            else
            {
                printf("%s: not found\n", current[1]);
                return 1;
            }
        }


        if(redirectedstderr)
        {
            int fd = getFileDescriptor(stderrPath, O_TRUNC | O_CREAT | O_WRONLY);
            char *path = getPath(current[1]);
            if (path != NULL)
            {
                printf("%s is %s\n", current[1], path);
                return  0;
            }
            else
            {
                dprintf(fd, "%s: not found\n", current[1]);
                return 1;
            }
        }


        if(appendStdOut)
        {
            int fd = getFileDescriptor(stdoutAppendPath, O_APPEND | O_CREAT | O_WRONLY);
            char *path = getPath(current[1]);
            if (path != NULL)
            {
                dprintf(fd, "%s is %s\n", current[1], path);
                return  0;
            }
            else
            {
                printf("%s: not found\n", current[1]);
                return 1;
            }
        }


        if(appendStdErr)
        {
            int fd = getFileDescriptor(stderrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
            char *path = getPath(current[1]);
            if (path != NULL)
            {
                printf("%s is %s\n", current[1], path);
                return  0;
            }
            else
            {
                dprintf(fd, "%s: not found\n", current[1]);
                return 1;
            }
        }

            char *path = getPath(current[1]);
            if (path != NULL)
            {
                printf("%s is %s\n", current[1], path);
                return  0;
            }
            else
            {
                printf("%s: not found\n", current[1]);
                return 1;
            }

    }

    return 1; 
}

int history(char *historyBuffer[], bool redirectedstdout, bool appendStdOut,  char *stdoutPath, char *stdoutAppendPath)
{
    if (redirectedstdout)
    {
        int fd = getFileDescriptor(redirectedstdout , O_CREAT | O_WRONLY | O_TRUNC);
        for(int i = 0 ; historyBuffer[i] != NULL ; i++)
        {
            dprintf(fd, "%d %s\n", i + 1, historyBuffer[i]);
        }
        return 0;
    }
    if (appendStdOut)
    {
        int fd = getFileDescriptor(stdoutAppendPath , O_CREAT | O_WRONLY | O_APPEND);
        for(int i = 0 ; historyBuffer[i] != NULL ; i++)
        {
            dprintf(fd, "%d %s\n", i + 1, historyBuffer[i]);
        }
        return 0;
    }
    
    for(int i = 0 ; historyBuffer[i] != NULL ; i++)
      {
        printf("%d %s\n", i + 1, historyBuffer[i]);
      }
    return 0;
}

int pwd( bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath)
{
    char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)))
      {
        if (redirectedstdout)
        {
            int fd = getFileDescriptor(stdoutAppendPath, O_CREAT | O_TRUNC | O_WRONLY);
            dprintf(fd, "%s\n",cwd);
            return 0;
        }

        if (appendStdOut)
        {
            int fd = getFileDescriptor(stdoutAppendPath, O_CREAT | O_APPEND | O_WRONLY);
            dprintf(fd, "%s\n",cwd);
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

int cd(char **current, bool redirectedstderr, bool appendStdErr, char *stderrPath, char *stderrAppendPath)
{
     if (current[1] == NULL || strcmp ("~", current[1]) == 0 )
      {
        char *homePath = getenv("HOME");
        return (chdir(homePath) ) ? 0 : 1;
      }
      else
      {
        if (chdir(current[1]) != 0)
        {
          if (redirectedstderr)
          {
            int fd = getFileDescriptor(stderrPath, O_CREAT | O_WRONLY | O_TRUNC);
            dprintf("%s: no such file or directory : \"%s\" \n", current[0], current[1]);
            return 1;

          } 

          if (appendStdErr)
          {
            int fd = getFileDescriptor(stderrAppendPath , O_CREAT | O_WRONLY | O_APPEND);
            dprintf(fd, "%s: no such file or directory : \"%s\" \n", current[0], current[1]);
            return 1;
          } 
                    
          printf("%s: no such file or directory : \"%s\" \n", current[0], current[1]);
          return 1;

        }
        else
        {
          return 0;
        }
      }
}

int echo(char **current, bool redirectedstdout, bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    if (redirectedstdout)
    {
     int fd = getFileDescriptor(stdoutPath , O_WRONLY | O_TRUNC | O_CREAT);
     for (int i = 1 ; current[i] != NULL ; i++)
      {
        dprintf(fd,"%s ", current[i]);
      }
      dprintf(fd, "\n");
      return 0;

    }

    if (appendStdOut)
    {
    
        int fd = getFileDescriptor(appendStdOut , O_WRONLY | O_APPEND | O_CREAT);
        for (int i = 1 ; current[i] != NULL ; i++)
        {
            dprintf(fd,"%s ", current[i]);
        }
        dprintf(fd, "\n");
        return 0;
        
    }

    for (int i = 1 ; current[i] != NULL ; i++)
      {
        printf("%s ", current[i]);
      }
      printf("\n");
      return 0;
}