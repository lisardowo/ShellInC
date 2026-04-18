#include "proccesess.h"
#include "commands.h"

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
    sigaction(SIGINT, &defaultAction, NULL);
    sigaction(SIGINT, &defaultAction, NULL);

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
            bool emptyList = false ;

            for (int j = 0 ; j < maxJobs ; j++)
            {
                if(jobList[i].running)
                {
                    emptyList = false;
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
        close(fd);
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

int runBuiltin(char *argv[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    if (strcmp("echo", current[0]) == 0 )
    {
        return echo(current, redirectedStdOut,  appendStdOuut, stdOutPath, stdoutAppendPath);
    }
    if (strcmp("cd", current[0]) == 0 )
    {
        return cd(current, redirectedStdErr,  appendStdErr, stdErrPath, stderrAppendPath);
    }
    if (strcmp("pwd", current[0]) == 0 )
    {
        return pwd(redirectedStdOut,  appendStdOuut, stdOutPath, stdoutAppendPath);
    }
    if (strcmp("history", current[0]) == 0 )
    {
        return history(argv, historyBuffer, redirectedStdOut,  appendStdOuut, stdOutPath, stdoutAppendPath);
    }
    if (strcmp("type", current[0]) == 0 )
    {
        return type(current, redirectedStdOut, redirectedStdErr, appendStdOuut, appendStdErr, stdOutPath, stdErrPath, stdoutAppendPath, stderrAppendPath);
    }
  return 1 ;
}

int runBuiltinChild(char *argv[],char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    if(strcmp("echo", current[0]) == 0)
    {
        return echo(current, redirectedStdOut,  appendStdOuut, stdOutPath, stdoutAppendPath);
    }
    if(strcmp("pwd", current[0]) == 0)
    {
        return pwd(redirectedStdOut,  appendStdOuut, stdOutPath, stdoutAppendPath);
    }
    if(strcmp("history", current[0]) == 0)
    {
        return history(argv, historyBuffer, redirectedStdOut,  appendStdOuut, stdOutPath, stdoutAppendPath);
    }
    if(strcmp("type", current[0]) == 0)
    {
        return type(current, redirectedStdOut , redirectedStdErr, appendStdOuut, appendStdErr, stdOutPath, stdErrPath, stdoutAppendPath, stderrAppendPath);
    }
    if(strcmp("cd", current[0]) == 0 || strcmp("exit", current[0]) == 0)
    {
        printf("%s: not supported inside pipeline\n", current[0]);
        return 1;
    }
    return 1;
}

int externalInChild(char **current, bool redirectedStdErr, bool appendStdErr, char *stdErrPath, char* stdErrAppendPath)
{
    char *binPath = getPath(current[0]);
    if (binPath == NULL)
    {

        if(redirectedStdErr)
        {
            int fd = getFileDescriptor(stdErrPath, O_TRUNC | O_CREAT | O_WRONLY);
            dprintf(fd, "%s command not found\n", current[0] ); 
            close(fd);

        }

        if(appendStdErr)
        {
            int fd = getFileDescriptor(stdErrAppendPath, O_APPEND | O_CREAT | O_WRONLY);
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
    return 126;
}

int runPipeline(bool toBackground, char *argv[],char *commands[100][100], int commandCount, char **historyBuffer, bool redirectedstdout, bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath, char *stdoutAppendPath, char *stderrAppendPath)
{
    int prev_pipe_read_end = -1;
    pid_t pids[100];
    int pidCount = 0;

    for (int i = 0; i < commandCount; i++)
    {
        int fds[2] = {-1, -1};
        bool is_last_command = (i == commandCount - 1);

        if (!is_last_command)
        {
            if (pipe(fds) == -1)
            {
                return 1;
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
        
            if (prev_pipe_read_end != -1)
            {
                dup2(prev_pipe_read_end, STDIN_FILENO);
                close(prev_pipe_read_end);
            }

            if (!is_last_command)
            {
               
                close(fds[0]); 
                dup2(fds[1], STDOUT_FILENO);
                close(fds[1]);
            }
            else
            {
            
                if (reddirectInChild(redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, stdoutPath, stderrPath, stdoutAppendPath, stderrAppendPath) != 0)
                {
                    _exit(1); 
                }
            }
            
            int status = runBuiltinChild(argv, commands[i], historyBuffer, redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, stdoutPath, stderrPath, stdoutAppendPath, stderrAppendPath);
            if (status == 1)
            { 
                status = externalInChild(commands[i], redirectedstderr, appendStdErr, stderrPath, stderrAppendPath);
            }
            _exit(status); 
        }
        else
        {
        
            pids[pidCount++] = pid;

            if (prev_pipe_read_end != -1)
            {
                close(prev_pipe_read_end);
            }
    
            if (!is_last_command)
            {
                close(fds[1]); 
                prev_pipe_read_end = fds[0];
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