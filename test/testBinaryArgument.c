#include <stdio.h>


int main(int argc, char* commandTokens[])
{
    if (argc < 2)
    {
        printf("binary requires one text argument\n");
        return -1;
    }
    
    
    printf("argument is : %s\n", commandTokens[1]);
    
    
    return 0;
}