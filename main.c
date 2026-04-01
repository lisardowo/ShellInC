#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "binariesManager.h"
#include "arguments.h"
#include "inputManager.h"

void REPL();





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
    //set up
    memset(argv,0,sizeof(argv));
    int argumentCount = 0;
    bool redirected = false;
    char *redirectPath = NULL;
    //actuall terminal stuff
    printf("$ ");
    //manage input
    fgets(userInput, 100, stdin);
    sanitizeInput(userInput);
    //count input
    argumentCounter(userInput, &argumentCount);
    argumentExtractor(userInput, argumentCount);
    
    for (int i = 0 ; argv[i] != NULL ; i++)
    {
      if (strcmp(argv[i], ">") == 0)
      {
        if (argv[i + 1] == NULL)
        {
          printf("syntax error: expected file after '>'\n");
          redirected = false;
          redirectPath = NULL;
          break ;          
        }
        redirected = true;
        redirectPath = argv[i + 1];
        argv[i] = NULL;
        
        break;
      }
    }

    if(strcmp("exit", argv[0]) == 0)
    {
      break;
    }
    else if((strcmp("echo", argv[0]) == 0) && !redirected)
    {
      for(int i = 1 ; argv[i] != NULL ; i++)
      {
      printf("%s ", argv[i]);
      }
      printf("\n");
    }
    else if(strcmp("cd", argv[0]) == 0)
    {
      if(strcmp("~", argv[1]) == 0)
      {
        
        char *home = getenv("HOME");
        chdir(home);
        
      }
      else if ((chdir(argv[1])) != 0)
      {
        printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
      }
    }
    else if((strcmp("pwd", argv[0]) == 0) && !redirected)
    {
      char cwd[1024];
      if(getcwd(cwd, sizeof(cwd)))
      {
        printf("%s\n",cwd);
      }
    }
    else if((strcmp("type", argv[0]) == 0) && !redirected)
    {
      
      if(!strcmp("echo", argv[1]) || !strcmp("exit", argv[1]) || !strcmp("type", argv[1]) || !strcmp("pwd", argv[1]) || !strcmp("cd", argv[1])) // not operator may seem odd but strcmp returns 0 if true, for if to properly works needs a 1 if true (reason of not)
      {
        printf("%s is a shell builtin\n", argv[1]);
      }
      else
      {
        
        char* path = getPath(argv[1]);
        if (path != NULL)
        {
          printf("%s is %s\n",argv[1], path);
        }
        else
        {
          printf("%s: not found\n", argv[1]);
        }
      }
      
    }
    else
    { 
      executeBin(redirectPath, redirected);
    }
    }
  }









