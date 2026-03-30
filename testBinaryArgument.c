#include <stdio.h>


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("binary requires one text argument\n");
        return -1;
    }
    
    
    printf("argument is : %s\n", argv[1]);
    
    
    return 0;
}