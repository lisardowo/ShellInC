
#define PerimissionsCode 00700

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{


    char testBuff[] = "hola";

    int fd = creat(argv[1], 00700);

    int writecode = write(fd, testBuff , strlen(testBuff));
    
    close(fd);
    printf("fd = %d\n", fd);
    printf("was writing? %d\n", writecode);

}