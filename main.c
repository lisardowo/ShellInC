#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

void REPL();
char *argv[100];
char* sanitizeInput(char* userInput);
char userInput[120];
char* getPath(char *command);
char* typeGetCommand(char *userInput);
char command[50];
void executeBin();
char binPath[100000];
void argumentCounter(char *userInput, int* argumentCount);
void argumentExtractor(char *userInput, int argumentCount);
char arguments[100000];

int main()
{
  
  setbuf(stdout, NULL);

  REPL();

  return 0;
 
}
void REPL()
{
  while (true)
  {
    memset(argv,0,sizeof(argv));
    int argumentCount = 0;
    printf("$ ");
    fgets(userInput, 100, stdin);
    sanitizeInput(userInput);

    typeGetCommand(userInput);
    argumentCounter(userInput, &argumentCount);
    argumentExtractor(userInput, argumentCount);

    if(strcmp("exit", argv[0]) == 0)
    {
      break;
    }
    else if(strcmp("echo", argv[0]) == 0)
    {
      printf("%s\n", userInput + 5);
    }
    else if(strcmp("type", argv[0]) == 0)
    {
      
      if(!strcmp("echo", userInput + 5) || !strcmp("exit", userInput + 5) || !strcmp("type", userInput + 5) )
      {
        printf("%s is a shell builtin\n", userInput + 5);
      }
      else
      {
        
        char* path = getPath(command);
        if (path != NULL)
        {
        printf("%s is %s\n",command, path);
        }
        else
        {
          printf("%s: not found\n", command);
        }
      }
      
    }
    else
    {
      char* binPath = getPath(argv[0]);
     // printf("%s\n", bin);
      if (binPath == NULL)
      {
        printf("%s: command not found\n", argv[0]);
      }
      else
      {
      if(fork() == 0)
      {
        
        execv(binPath, argv); //path , command , arguments 
                                         //null indicates end of arguments
                           
      }
      else
      {
        wait(NULL);
      }
      //printf("%s: command not found\n", userInput);
      
    }
    }
  }

}




char* typeGetCommand(char *userInput)
{
  strcpy(command, userInput + 5);
  return command;
}

char* getPath(char *command)
{
  char *path = getenv("PATH");
  char iterablepath[100000];
  strncpy(iterablepath, path, sizeof(iterablepath));
  iterablepath[sizeof(iterablepath) - 1] = '\0';

  
  char *myPtr = strtok(iterablepath, ":");
  bool found = false;
  while(myPtr != NULL) 
  {
    
    snprintf(binPath, sizeof(binPath), "%s/%s", myPtr,command);
    
    if (access(binPath, X_OK) == 0)
    {
      
      found = true;
      return binPath;
      break;
    }
   
    myPtr = strtok(NULL, ":");
  }
    
 /* if(!found)
    {
       //TODO quitar get path
    } */
  
    //break;
  


}

void executeBin()
{

}

char* sanitizeInput(char *userInput)
{

    size_t strLen = strlen(userInput);
    if(strLen > 0 && userInput[strLen - 1] == '\n')
    {
        userInput[strLen - 1] = '\0';
    }
    return userInput;

}

void argumentExtractor(char *userInput, int argumentCount)
{
  
  strncpy(arguments, userInput, sizeof(arguments));
  arguments[sizeof(arguments) - 1] = '\0';
  
  char *token = strtok(arguments, " ");
  int i = 0;
  while(i < argumentCount)
  {
   
    argv[i] = token;
    token = strtok(NULL, " ");
    i++;
  }
  //return arguments;
}

void argumentCounter(char *userInput, int* argumentCount)
{
  *argumentCount = 1;
  for(int i = 0 ; userInput[i] != '\0' ; i++)
  {
    if (userInput[i] == ' ')
    {
      (*argumentCount) ++ ;
    }

  }
  

}
