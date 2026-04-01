#include "redirectOutput.h"

void writeToFile(char *redirectpath, char *argv[])
{
    int pipeFileDescriptor[2];
    if (pipe(pipeFileDescriptor) == -1)
    {
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        close(pipeFileDescriptor[0]);
        close(pipeFileDescriptor[1]);
        return;
    }

    if (pid == 0)
    {
        close(pipeFileDescriptor[0]);
        dup2(pipeFileDescriptor[1], STDOUT_FILENO);
        close(pipeFileDescriptor[1]);
        execvp(argv[0], argv);
        _exit(127);
    }
    else
    {
        close(pipeFileDescriptor[1]);
        int outputFd = open(redirectpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outputFd == -1)
        {
            close(pipeFileDescriptor[0]);
            waitpid(pid, NULL, 0);
            return;
        }

        char chunk[4096];
        ssize_t n;
        while ((n = read(pipeFileDescriptor[0], chunk, sizeof(chunk))) > 0)
        {
            ssize_t written = 0;
            while (written < n)
            {
                ssize_t w = write(outputFd, chunk + written, (size_t)(n - written));
                if (w <= 0)
                {
                    break;
                }
                written += w;
            }
        }

        close(outputFd);
        close(pipeFileDescriptor[0]);
        waitpid(pid, NULL, 0);
    }
}





