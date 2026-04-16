
#include "jobsManager.h"

job jobList[maxJobs];
static int nextJobId = 1;

void initJob()
{
    for (int i = 0 ; i < maxJobs ; i++)
    {
        jobList[i].running = false;
    }
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

