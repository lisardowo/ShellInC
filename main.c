#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "proccesess.h"
#include "pipeline.h"
#include "arguments.h"
#include "utils.h"
#include "selfCompletion.h"
#include "history.h"
#include "commands.h"

//TODO bug: cuando pegas cosas con comillas, el auto completado detecta las comillas y las cierra automaticamente (Se debe asumir que si lo esta pegando el usuario, es por que ya no necesita autocompletar)
//TODO 

void createPrompt();
void REPL();
char *historyBuffer[10000]; // TODO What if history to long? -> Buffer overflow, remote code execution?
int historyCount;
char prompt[1024]; //user can create a long ass dir/dirPath to max out the prompt size

typedef enum
{
  NONE,
  PIPE,
  AND,
  OR
}  segmentType;

int main()
{
  
  setbuf(stdout, NULL);
  
  ignoreSignalsInParent();

  REPL();

  return 0;
 
}

void REPL()
{
  availableCommands commandsList;
  fillCommands(&commandsList);
  historyCount = getHistory(historyBuffer);

  while (true)
  {
    createPrompt();
    //set up
    int argumentCount = 0;

    bool redirectedstdout = false;
    bool redirectedstderr = false;
    bool appendStdOut = false;
    bool appendStdErr = false;
    char *stdoutPath = NULL;
    char *stderrPath = NULL;
    char *stdoutAppendPath = NULL;
    char *stderrAppendPath = NULL;

    int segment = 0;
    int position = 0;

    int pipelineSegment[100]; //TODO I remember this has some error raising if way to many segments -> Search for that
    segmentType pipeLineConditionals[100];

    int pipelineCount = 0;
    int commandInScope = 0;

    //actuall terminal stuff
    readLineTab(prompt, &commandsList, userInput, sizeof(userInput), &historyCount, historyBuffer);
    //manage input
    sanitizeInput(userInput);

    if(!expandHistory(userInput, sizeof(userInput), historyCount, historyBuffer))
    {
      continue;
    }

    addHistory(userInput, &historyCount, historyBuffer);
    argumentCounter(userInput, &argumentCount);//TODO Remove counter -> currently not in use
    argumentExtractor(userInput);

    expandArguments(argv);
    expandGlobs(argv);

    bool toBackgrund = false;

    if (argumentCount > 0)
    {
      int lastIndex = 0;
      while(argv[lastIndex + 1] != NULL )
      {
        lastIndex++;
      }
      if(strcmp(argv[lastIndex], "&") == 0)
      {
        toBackgrund = true;
        argv[lastIndex] = false;
      }
    }

    checkBacktroundJobs();

    char *segments[100][100];
    segmentType typeOfSegment[100];

    if (argv[0] == NULL)
    {
      continue;
    }

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
          break;
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
          break;

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
          printf("Esto deberia dejar escribir en multiples lineas\n"); //TODO esto deberia dejar de escribir en multiples lineas
          break;

        }

        if (position == 0 )
        {
          printf("Syntax error near unexpected token '&&'\n");
          break;
        }

        segments[segment][position] = NULL;
        typeOfSegment[segment] = AND;
        segment ++;
        position = 0;

        continue;

      }

      else if (strcmp(argv[i], "||") == 0)
      {
        if (argv[i + 1] == NULL)
        {
          printf("Esto deberia dejar escribir en multiples lineas\n"); //TODO esto deberia dejar de escribir en multiples lineas
          break;

        }

        if (position == 0 )
        {
          printf("Syntax error near unexpected token '||'\n");
          break;
        }

        segments[segment][position] = NULL;
        typeOfSegment[segment] = OR;
        segment ++;
        position = 0;

        continue;

      }

      else if (strcmp(argv[i], "|") == 0)
      {
        if (argv[i + 1] == NULL)
        {
          printf("Esto deberia dejar escribir en multiples lineas\n"); //TODO esto deberia dejar de escribir en multiples lineas
          break;

        }

        if (position == 0 )
        {
          printf("Syntax error near unexpected token '|'\n");
          break;
        }

        segments[segment][position] = NULL;
        typeOfSegment[segment] = PIPE;
        segment ++;
        position = 0;

        continue;

      }

      segments[segment][position++] = argv[i];
    }

    segments[segment][position] = NULL;
    typeOfSegment[segment] = NONE;

    int segmentCount = segment + 1;
    char *(*pipelines)[100][100] = calloc(100, sizeof(*pipelines));
    if( pipelines == NULL)
    {
      printf("shell: Out of memory\n");
      exit(1);
    }

    for (int i = 0 ; i < segmentCount ; i++)
    {

      int currentArgument = 0;
      while (segments[i][currentArgument] != NULL)
      {
        pipelines[pipelineCount][commandInScope][currentArgument] = segments[i][currentArgument];
        currentArgument++;
      }
      pipelines[pipelineCount][commandInScope][currentArgument] = NULL;
      commandInScope++;

      if(typeOfSegment[i] == PIPE)
      {
        continue;
      }
      pipelineSegment[pipelineCount] = commandInScope;
      pipeLineConditionals[pipelineCount] = typeOfSegment[i];
      pipelineCount++;
      commandInScope = 0;
    }

    int lastStatus = 0;
    segmentType prevConditional = NONE;
    bool shouldExit = false;

    for (int v = 0 ; v < pipelineCount ; v++)
    {

      if (v > 0)
      {
        if (prevConditional == AND && lastStatus != 0)
        {
          prevConditional = pipeLineConditionals[v];
          continue;
        }
        if (prevConditional == OR && lastStatus == 0)
        {
          prevConditional = pipeLineConditionals[v];
          continue;
        }
      }

      if (pipelineSegment[v] > 1)
      {
        char **current = pipelines[v][0];
        if(current[0] == NULL)
        {
          lastStatus  = 1;
          continue;
        }
        lastStatus = runPipeline(toBackgrund,argv, pipelines[v], pipelineSegment[v], historyBuffer , redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, stdoutPath, stderrPath, stdoutAppendPath , stderrAppendPath);
      }
      else
      {
        char **current = pipelines[v][0];

        if(current[0] == NULL)
        {
          lastStatus = 1;
          continue;
        }

        if(strcmp("exit", current[0]) == 0)
        {
          shouldExit = true;
          break;
        }

        else if(strcmp("echo", current[0]) == 0 )
        {
          lastStatus = echo(current, redirectedstdout, appendStdOut , stdoutPath, stdoutAppendPath);
        }

        else if(strcmp("cd", current[0]) == 0)
        {
          lastStatus = cd(current, redirectedstdout, appendStdOut , stdoutPath, stdoutAppendPath);
        }

        else if(strcmp("pwd", current[0]) == 0 )
        {
          lastStatus = pwd(redirectedstdout, appendStdOut , stdoutPath, stdoutAppendPath);
        }

        else if(strcmp("history", current[0]) == 0 )
        {
          lastStatus = history(current, historyBuffer, redirectedstdout , appendStdOut, stdoutPath, stdoutAppendPath);
        }

        else if(strcmp("type", current[0]) == 0 )
        {
          lastStatus = type(current, redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, stdoutPath, stderrPath, stdoutAppendPath, stderrAppendPath);
        }

        else if(strcmp("jobs", current[0]) == 0 )
        {
          lastStatus = jobs(jobList, redirectedstdout, appendStdOut, stdoutPath, stdoutAppendPath);
        }

        else
        {
          lastStatus = executeBin(toBackgrund, stdoutPath, stderrPath, stdoutAppendPath, stderrAppendPath, redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, current);
        }
      }

      prevConditional = pipeLineConditionals[v];
    }

    free(pipelines);

    if (shouldExit)
    {
      break;
    }
  }

  commandsFree(&commandsList);
  dumpHistory(historyBuffer);
  historyBufferFree(historyBuffer);
}