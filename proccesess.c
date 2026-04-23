#include "proccesess.h"
#include "commands.h"

#define MAX_PIPELINE_PROCS 100
job jobList[maxJobs];
static int nextJobId = 1;


void ignoreSignalsInParent()
{

    struct sigaction ignoreAction;

    ignoreAction.sa_handler = SIG_IGN;
    ignoreAction.sa_flags = 0;
    sigemptyset(&ignoreAction.sa_mask);

    sigaction(SIGINT, &ignoreAction, NULL);
    sigaction(SIGQUIT, &ignoreAction, NULL);
    sigaction(SIGTSTP, &ignoreAction, NULL);

}

void restoreSignalsInChild()
{

    struct sigaction defaultAction;
    
    defaultAction.sa_handler = SIG_DFL;
    defaultAction.sa_flags = 0;
    sigemptyset(&defaultAction.sa_mask);
    
    sigaction(SIGINT, &defaultAction, NULL);
    sigaction(SIGQUIT, &defaultAction, NULL);
    sigaction(SIGTSTP, &defaultAction, NULL);

}


int addJob(pid_t pid, char *command)
{
    for (int i = 0 ; i < maxJobs ; i++)
    {
        if(!jobList[i].running)
        {
            
            jobList[i].id = nextJobId++;
            jobList[i].pid = pid;
            strncpy(jobList[i].command, command , sizeof(jobList[i].command) - 1);
            jobList[i].command[sizeof(jobList[i].command) - 1] = '\0';
            jobList[i].running = true;

            printf("[%d] %d\n", jobList[i].id, jobList[i].pid);
            return jobList[i].id;

        }
    }

    printf("shell: too many jobs\n");
    return -1 ;

}

void removeJob(pid_t pid)
{
    for (int i = 0 ; i < maxJobs ; i++)
    {
        if(jobList[i].running && jobList[i].pid == pid)
        {
            jobList[i].running = false;
            bool emptyList = true;

            for (int j = 0 ; j < maxJobs ; j++)
            {
                if(jobList[j].running)
                {
                    emptyList = false;
                    break;
                }
            }
            if (emptyList)
            {
                nextJobId = 1;
            }

            break;

        }
    }
}

void checkBacktroundJobs()
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0 ; i < maxJobs ; i++)
        {
            if (jobList[i].running && jobList[i].pid == pid)
            {
            
                printf("[%d]+ Done                  %s\n", jobList[i].id, jobList[i].command);
                removeJob(pid);
                break;

            }
        }
    }
}


int reddirectInChild(const redirectConfig *redirect)
{
    if (redirect->redirectStdout)
    {
        int fd = getFileDescriptor(redirect->stdOutPath, O_TRUNC | O_CREAT | O_WRONLY);
        if (fd < 0)
        {
            return 1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if (redirect->appendStdout)
    {
        int fd = getFileDescriptor(redirect->stdoutAppendPath, O_APPEND | O_CREAT | O_WRONLY);
        if (fd < 0 )
        {
            return 1;
        }
        dup2(fd , STDOUT_FILENO);
        close(fd);
    }
    if(redirect->redirectStderr)
    {
        int fd = getFileDescriptor(redirect->stdErrPath, O_TRUNC | O_CREAT | O_WRONLY);
        if (fd < 0 )
        {
            return 1 ;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    if (redirect->appendStderr)
    {
        int fd = getFileDescriptor(redirect->stderrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
        if (fd < 0)
        {
            return 1 ;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    return 0;
} 

int runBuiltin(char **current, redirectConfig *redirect, char *historyBuffer[])
{
    if (strcmp("echo", current[0]) == 0 )
    {
        return echo(current, redirect );
    }
    if (strcmp("cd", current[0]) == 0 )
    {
        return cd(current, redirect);
    }
    if (strcmp("pwd", current[0]) == 0 )
    {
        return pwd(redirect);
    }
    if (strcmp("history", current[0]) == 0 )
    {
        return history(current, historyBuffer,redirect);
    }
    if (strcmp("type", current[0]) == 0 )
    {
        return type(current, redirect);
    }
  return 1 ;
}

int runBuiltinChild(char **current, redirectConfig *redirect, char *historyBuffer[])
{
    if(strcmp("echo", current[0]) == 0)
    {
        return echo(current, redirect);
    }
    if(strcmp("pwd", current[0]) == 0)
    {
        return pwd(redirect);
    }
    if(strcmp("history", current[0]) == 0)
    {
        return history(current, historyBuffer, redirect);
    }
    if(strcmp("type", current[0]) == 0)
    {
        return type(current, redirect);
    }
    if(strcmp("cd", current[0]) == 0 || strcmp("exit", current[0]) == 0)
    {
        printf("%s: not supported inside pipeline\n", current[0]);
        return 1;
    }
    return 1;
}

int externalInChild(char **current, redirectConfig *redirect)
{
    char *binPath = getPath(current[0]);
    if (binPath == NULL)
    {

        if(redirect->redirectStderr)
        {
            int fd = getFileDescriptor(redirect->stdErrPath, O_TRUNC | O_CREAT | O_WRONLY);
            dprintf(fd, "%s command not found\n", current[0] ); 
            close(fd);

        }

        if(redirect->appendStderr)
        {
            int fd = getFileDescriptor(redirect->stderrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
            dprintf(fd, "%s command not found\n", current[0] );
            close(fd);

        }
        else
        {
        dprintf(STDERR_FILENO, "%s command not found\n", current[0] ); 
        }
        return 127;
    }
    execv(binPath, current);
    free(binPath);
    return 126;
}

int runPipeline(bool toBackground, char *commandTokens[],char *commands[100][100], int commandCount, char **historyBuffer, redirectConfig *redirect)
{
    (void)commandTokens;
    int prevPipeReadEnd = -1;
    pid_t pids[MAX_PIPELINE_PROCS];
    int pidCount = 0;

    for (int i = 0; i < commandCount; i++)
    {
        int fds[2] = {-1, -1};
        bool isLastCommand = (i == commandCount - 1);

        if (!isLastCommand)
        {
            if (pipe(fds) == -1)
            {
                return 1;
            }
        }

        if (pidCount >= MAX_PIPELINE_PROCS)
        {
            fprintf(stderr, "shell: too many processes in pipeline (max %d)\n", MAX_PIPELINE_PROCS);
            if (prevPipeReadEnd != -1 )
            {
                close(prevPipeReadEnd);
                break;
            }
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            return 1;
        }

        if (pid == 0)
        {

            restoreSignalsInChild();
        
            if (prevPipeReadEnd != -1)
            {
                dup2(prevPipeReadEnd, STDIN_FILENO);
                close(prevPipeReadEnd);
            }

            if (!isLastCommand)
            {
               
                close(fds[0]); 
                dup2(fds[1], STDOUT_FILENO);
                close(fds[1]);
            }
            else
            {
            
                if (reddirectInChild(redirect) != 0)
                {
                    _exit(1); 
                }
            }
            
            int status = runBuiltinChild(commands[i], redirect, historyBuffer);
            if (status == 1)
            { 
                status = externalInChild(commands[i], redirect);
            }
            _exit(status); 
        }
        else
        {
        
            pids[pidCount++] = pid;

            if (prevPipeReadEnd != -1)
            {
                close(prevPipeReadEnd);
            }
    
            if (!isLastCommand)
            {
                close(fds[1]); 
                prevPipeReadEnd = fds[0];
            }
        }

    }

  
    if(toBackground)
    {
        addJob(pids[pidCount - 1], "pipeline");
        return 0 ;
    }

    int lastStatus = 0;
    for (int i = 0; i < pidCount; i++)
    {
        int wstatus;
        waitpid(pids[i], &wstatus, 0);
        
        if (i == pidCount - 1)
        {
            if (WIFEXITED(wstatus))
            {
                lastStatus = WEXITSTATUS(wstatus);
            }
            else
            {
                lastStatus = 1; 
            }
        }
    }

    return lastStatus;
}