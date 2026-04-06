#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lineEdition.h"
#include "binariesManager.h"
#include "arguments.h"
#include "inputManager.h"
#include "getHistory.h"
#include "utils.h"

void createPrompt();
void REPL();
char *historyBuffer[10000];
int historyCount;
char prompt[1024];

int main()
{
  
  setbuf(stdout, NULL);
  
  REPL();

  return 0;
 
}

void REPL()
{
  createPrompt();
  availableCommands commandsList;
  fillCommands(&commandsList);
  getHistory(&historyCount, historyBuffer);

  while (true)
  {


    //set up
    memset(argv,0,sizeof(argv));
    int argumentCount = 0;
    bool redirectedstdout = false;
    bool redirectedstderr = false;
    bool appendStdOut = false;
    bool appendStdErr = false;
    bool andOperator = false;
    char *stdoutPath = NULL;
    char *stderrPath = NULL;
    char *stdoutAppendPath = NULL;
    char *stderrAppendPath = NULL;
    //actuall terminal stuff
    readLineTab(prompt, &commandsList, userInput, sizeof(userInput), &historyCount, historyBuffer);
    //manage input
    sanitizeInput(userInput);
    addHistory(userInput, &historyCount, historyBuffer);
    argumentCounter(userInput, &argumentCount);
    argumentExtractor(userInput, argumentCount);

    char *segments[64][100];


    
    if (argv[0] == NULL)
    {
      continue;
    }

char *commandToken[1000];
int commandCount = 0;
int andSegment = 0;
int andPosition = 0;
//TODO from 64 to 83 are probablly gon be erased
  for(int i = 0 ; argv[i] != NULL ; i++)
  {
    //echo  >   eas 2>  ead
    //tok0 op0  tok1 op1 tok2
    //ls -lh > test.txt
    //tok tok op tok
   
    if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], "1>") == 0)
      {
        if (argv[i + 1] == NULL)
        {
          printf("syntax error: expected file after '>'\n");
          redirectedstdout = false;
          stdoutPath = NULL;
          break ;          
        }
        else
        {

          redirectedstdout = true;
          stdoutPath = argv[i + 1];

          i++;
          continue;
        }
         
      }
      else if (strcmp(argv[i], "2>") == 0)
      {
        if (argv[i + 1] == NULL)
        {
          
          printf("syntax error: expected file after '2>'\n");
          redirectedstderr = false;
          stderrPath = NULL;
          break ;          
       
        }
        else
        {
        
          redirectedstderr = true;
          stderrPath = argv[i + 1];
          i++;
          continue;

        }
      }
      else if (strcmp(argv[i], ">>") == 0) // appendstdout
      {
        if (argv[i + 1] == NULL)
        {
          printf("syntax error: expected file after '>>'\n");
          appendStdOut = false;
          stdoutAppendPath = NULL;   
          break;    
        }
        else
        { 

          appendStdOut = true;
          stdoutAppendPath = argv[i + 1];
          i++;
          continue;

        }
      }
      else if (strcmp(argv[i], "2>>") == 0) // appendstderr
      {
        if (argv[i + 1] == NULL)
        {
          printf("syntax error: expected file after '2>>'\n");
          appendStdErr = false;
          stderrAppendPath = NULL;
          break;
        }
        else
        {
          
          appendStdErr = true;
          stderrAppendPath = argv[i + 1];
          i++;
          continue;
        
        }
        
      }
      

       else if (strcmp(argv[i], "&&") == 0)
      {
        if (argv[i + 1] == NULL)
        {
          printf("Esto deberia dejar escribir en multiples lineas\n");
          break;
                 
        }

        if (andPosition == 0 )
        {
          printf("Syntax error near unexpected token '&&'\n");
          break;
        }

        segments[andSegment][andPosition] = NULL;
        andSegment ++;
        andPosition = 0;
        
        continue;

      }
  segments[andSegment][andPosition++] = argv[i];
  commandToken[commandCount++] = argv[i];
}

//for (int i = 0 ; commandToken[i] != NULL ; i++ ) printf("%s", commandToken[i]);;
segments[andSegment][andPosition] = NULL;
int segmentCount = andSegment + 1;
commandToken[commandCount] = NULL ;


    if(strcmp("exit", argv[0]) == 0)
    {

      dumpHistory(historyBuffer);
      break;

    }
    else if((strcmp("echo", argv[0]) == 0) && !redirectedstdout && !appendStdOut)
    {
      for(int i = 1 ; argv[i] != NULL ; i++)
      {
      printf("%s ", argv[i]);
      }
      printf("\n");
    }
    else if(strcmp("cd", argv[0]) == 0)
    {

      if(argv[1] == NULL || strcmp("~", argv[1]) == 0)
      {
        
        char *home = getenv("HOME");
        chdir(home);
        
      }
      else if ((chdir(argv[1])) != 0)
      {
        printf("%s: %s: No such file or directory\n", commandToken[0], commandToken[1]);
      }
    }
    else if((strcmp("pwd", argv[0]) == 0) && !redirectedstdout && !appendStdOut)
    {
      char cwd[1024];
      if(getcwd(cwd, sizeof(cwd)))
      {
        printf("%s\n",cwd);
      }
    }
    else if((strcmp("history", argv[0]) == 0) && !redirectedstdout && !appendStdOut)
    {
      
      for (int i = 0 ; historyBuffer[i] != NULL ; i++)
      {
        printf("%d  %s\n", i + 1, historyBuffer[i]);
      } 

    }
    else if((strcmp("type", argv[0]) == 0) && !redirectedstdout && !appendStdOut)
    { 

      if (argv[1] == NULL)
      {
        printf("Usage : type <command>");
      }

      if(!strcmp("echo", argv[1]) || !strcmp("exit", argv[1]) || !strcmp("type", argv[1]) || !strcmp("pwd", argv[1]) || !strcmp("cd", argv[1]) || !strcmp("history", argv[1])) // not operator may seem odd but strcmp returns 0 if true, for if to properly works needs a 1 if true (reason of not)
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

      executeBin(stdoutPath, stderrPath, stdoutAppendPath, stderrAppendPath, redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, commandToken);
    
    }
    }
    commandsFree(&commandsList);
  }


