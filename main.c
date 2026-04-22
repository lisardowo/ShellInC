#include <unistd.h>

#define MAX_SEGMENTS 100
#define MAX_ARGS_PER_SEG 100
#define MAX_PIPELINES 100

#include "proccesess.h"
#include "pipeline.h"
#include "arguments.h"
#include "utils.h"
#include "selfCompletion.h"
#include "history.h"
#include "commands.h"

//TODO bug: cuando pegas cosas con comillas, el auto completado detecta las comillas y las cierra automaticamente (Se debe asumir que si lo esta pegando el usuario, es por que ya no necesita autocompletar)
//TODO jump between words when ctrl + arrow are detected


void REPL();
char *historyBuffer[10000]; 
int historyCount;
char prompt[1024];

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

    int pipelineSegment[MAX_PIPELINES]; 
    segmentType pipeLineConditionals[MAX_PIPELINES];

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
    argumentCounter(userInput, &argumentCount);
    argumentExtractor(userInput);

    expandArguments(commandTokens);
    expandGlobs(commandTokens);

    bool toBackgrund = false;

    if (argumentCount > 0)
    {
      int lastIndex = 0;
      while(commandTokens[lastIndex + 1] != NULL )
      {
        lastIndex++;
      }
      if(strcmp(commandTokens[lastIndex], "&") == 0)
      {
        toBackgrund = true;
        commandTokens[lastIndex] = false;
      }
    }

    checkBacktroundJobs();

    char *segments[MAX_SEGMENTS][MAX_ARGS_PER_SEG];
    segmentType typeOfSegment[MAX_SEGMENTS];

    if (commandTokens[0] == NULL)
    {
      continue;
    }

    for(int i = 0 ; commandTokens[i] != NULL ; i++)
    {
      //echo  >   eas 2>  ead
      //tok0 op0  tok1 op1 tok2
      //ls -lh > test.txt
      //tok tok op tok

      if (strcmp(commandTokens[i], ">") == 0 || strcmp(commandTokens[i], "1>") == 0)
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("syntax error: expected file after '>'\n");
          redirectedstdout = false;
          stdoutPath = NULL;
          break;
        }
        else
        {
          redirectedstdout = true;
          stdoutPath = commandTokens[i + 1];

          i++;
          continue;
        }

      }
      else if (strcmp(commandTokens[i], "2>") == 0)
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("syntax error: expected file after '2>'\n");
          redirectedstderr = false;
          stderrPath = NULL;
          break;

        }
        else
        {

          redirectedstderr = true;
          stderrPath = commandTokens[i + 1];
          i++;
          continue;

        }
      }
      else if (strcmp(commandTokens[i], ">>") == 0) // appendstdout
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("syntax error: expected file after '>>'\n");
          appendStdOut = false;
          stdoutAppendPath = NULL;
          break;
        }
        else
        {

          appendStdOut = true;
          stdoutAppendPath = commandTokens[i + 1];
          i++;
          continue;

        }
      }
      else if (strcmp(commandTokens[i], "2>>") == 0) // appendstderr
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("syntax error: expected file after '2>>'\n");
          appendStdErr = false;
          stderrAppendPath = NULL;
          break;
        }
        else
        {

          appendStdErr = true;
          stderrAppendPath = commandTokens[i + 1];
          i++;
          continue;

        }

      }

      else if (strcmp(commandTokens[i], "&&") == 0)
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("Esto deberia dejar escribir en multiples lineas\n"); 
          break;

        }

        if (position == 0 )
        {
          printf("Syntax error near unexpected token '&&'\n");
          break;
        }

        if (segment >= MAX_SEGMENTS - 1)
        {
          fprintf(stderr, "shell: too many pipeline segments (max %d)\n", MAX_SEGMENTS - 1);
          goto next_iteration;
        }

        segments[segment][position] = NULL;
        typeOfSegment[segment] = AND;
        segment ++;
        position = 0;

        continue;

      }

      else if (strcmp(commandTokens[i], "||") == 0)
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("Esto deberia dejar escribir en multiples lineas\n"); 
          break;

        }

        if (position == 0 )
        {
          printf("Syntax error near unexpected token '||'\n");
          break;
        }

        if (segment >= MAX_SEGMENTS - 1)
        {
          fprintf(stderr, "shell: too many pipeline segments (max %d)\n", MAX_SEGMENTS - 1);
          goto next_iteration;
        }

        segments[segment][position] = NULL;
        typeOfSegment[segment] = OR;
        segment ++;
        position = 0;

        continue;

      }

      else if (strcmp(commandTokens[i], "|") == 0)
      {
        if (commandTokens[i + 1] == NULL)
        {
          printf("Esto deberia dejar escribir en multiples lineas\n"); 
          break;

        }

        if (position == 0 )
        {
          printf("Syntax error near unexpected token '|'\n");
          break;
        }

        if (segment >= MAX_SEGMENTS - 1)
        {
          fprintf(stderr, "shell: too many pipeline segments (max %d)\n", MAX_SEGMENTS - 1);
          goto next_iteration;
        }

        segments[segment][position] = NULL;
        typeOfSegment[segment] = PIPE;
        segment ++;
        position = 0;

        continue;

      }

      segments[segment][position++] = commandTokens[i];

      if (segment >= MAX_SEGMENTS - 1)
      {
        fprintf(stderr, "shell: too many pipeline segments (max %d)\n", MAX_SEGMENTS - 1);
        goto next_iteration;
      }
      if (position >= MAX_ARGS_PER_SEG - 1)
      {
        fprintf(stderr, "shell: too many arguments in segment (max %d)\n", MAX_ARGS_PER_SEG - 1);
        goto next_iteration;
      }
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
        lastStatus = runPipeline(toBackgrund,commandTokens, pipelines[v], pipelineSegment[v], historyBuffer , redirectedstdout, redirectedstderr, appendStdOut, appendStdErr, stdoutPath, stderrPath, stdoutAppendPath , stderrAppendPath);
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
    continue;

next_iteration:
    free(pipelines);
  }

  commandsFree(&commandsList);
  dumpHistory(historyBuffer);
  historyBufferFree(historyBuffer);
}