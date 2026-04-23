#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "definitions.h"
#include "proccesess.h"
#include "pipeline.h"
#include "arguments.h"
#include "utils.h"
#include "selfCompletion.h"
#include "history.h"
#include "commands.h"


#define MAX_SEGMENTS 100
#define MAX_ARGS_PER_SEG 100
#define MAX_PIPELINES 100
#define HISTORY_BUFFER_CAPACITY 10000

typedef enum 
{
  
  NONE,
  PIPE,
  AND,
  OR

} segmentType;

typedef struct 
{
  
  char **historyBuffer;
  int historyCount;
  char prompt[4096];
  availableCommands commandsList;

} shellState;

static void shellStateInit(shellState *state);
static void shellStateDestroy(shellState *state);
static void redirectConfigInit (redirectConfig *redirect);
static bool readAndPrepareInput(shellState *state);
static bool detectBackground();
static bool parseTokensToSegments(char *segments[MAX_SEGMENTS][MAX_ARGS_PER_SEG], segmentType typeOfSegment[MAX_SEGMENTS], redirectConfig *redirect, int *segmentCount);
static int buildPipelines (char *segments[MAX_SEGMENTS][MAX_ARGS_PER_SEG], segmentType typeOfSegment[MAX_SEGMENTS], int segmentCount, char *(*pipelines)[MAX_SEGMENTS][MAX_ARGS_PER_SEG], int pipelineSegment[MAX_PIPELINES], segmentType pipelineConditionals[MAX_PIPELINES]);
static int dispatchBuiltIn(char **current, shellState *state, redirectConfig *redirect, bool toBackground, bool *outShouldExit);
static int executePipelines(char *(*pipelines)[MAX_SEGMENTS][MAX_ARGS_PER_SEG],int pipelineCount,int pipelineSegment[MAX_PIPELINES],
segmentType pipeLineConditionals[MAX_PIPELINES],shellState *state,redirectConfig *r,
bool toBackground,bool *outShouldExit);
static void REPL(shellState *state);

int main()
{
  setbuf(stdout, NULL);
  ignoreSignalsInParent();

  shellState state;
  shellStateInit(&state);
  REPL(&state);
  shellStateDestroy(&state);

  return 0;

}

static void shellStateInit(shellState *state)
{
  
  memset(state, 0, sizeof(*state));
  state->historyBuffer = calloc(HISTORY_BUFFER_CAPACITY, sizeof(char *));
  if (state->historyBuffer == NULL)
  {
    fprintf(stderr, "shell: out of memory\n");
    exit(1);
  }
  fillCommands(&state->commandsList);
  state->historyCount = getHistory(state->historyBuffer);

}
static void shellStateDestroy(shellState *state)
{
  
  commandsFree(&state->commandsList);
  dumpHistory(state->historyBuffer);
  historyBufferFree(state->historyBuffer);
  free(state->historyBuffer);

}
static void redirectConfigInit (redirectConfig *redirect)
{
  redirect->redirectStdout = false;
  redirect->redirectStderr = false;
  redirect->appendStdout = false;
  redirect->appendStderr = false;
  redirect->stdOutPath = NULL;
  redirect->stdErrPath = NULL;
  redirect->stdoutAppendPath = NULL;
  redirect->stderrAppendPath = NULL;
}

static bool readAndPrepareInput(shellState *state)
{
  createPrompt(state->prompt, sizeof(state->prompt));
  readLineTab(state->prompt, &state->commandsList, userInput, sizeof(userInput), &state->historyCount, state->historyBuffer);
  sanitizeInput(userInput);
  if(!expandHistory(userInput, sizeof(userInput), state->historyCount, state->historyBuffer))
  {
    return false;
  }

  addHistory(userInput, &state->historyCount, state->historyBuffer);
  int argumentCount = 0;

  argumentCounter(userInput, &argumentCount);
  argumentExtractor(userInput);
  expandArguments(commandTokens);
  expandGlobs(commandTokens);

  return true;
  
}

static bool detectBackground()
{
  if (commandTokens == NULL || commandTokens[0] == NULL)
  {
    return false;
  }
  int lastIndex = 0;
  while(commandTokens[lastIndex + 1] != NULL)
  {
    lastIndex++;
  }

  if(strcmp(commandTokens[lastIndex], "&") == 0)
  {
    commandTokens[lastIndex] = NULL;
    return true;
  }

  return false;

}
static bool parseTokensToSegments(char *segments[MAX_SEGMENTS][MAX_ARGS_PER_SEG], segmentType typeOfSegment[MAX_SEGMENTS], redirectConfig *redirect, int *segmentCount)
{
  int segment = 0;
  int position = 0;
  bool ok = true;
  for(int i = 0 ; commandTokens[i] != NULL ; i++)
  {
    char *tok = commandTokens[i];
    if (strcmp(tok, ">") == 0 || strcmp(tok, "1>") == 0)
    {
      if (commandTokens[i + 1] == NULL)
      {
        fprintf(stderr, "syntax error: expected file after '>'\n");
        ok = false;
        break;
      }
      redirect->redirectStdout = true;
      redirect->stdOutPath = commandTokens[++i];
      continue;
    }
    if(strcmp(tok, ">>") == 0)
    {
      if (commandTokens[i + 1] == NULL)
      {
        fprintf(stderr, "syntax error: expected file after '>>'\n");
        ok = false;
        break;
      }
      redirect->appendStdout = true;
      redirect->stdoutAppendPath = commandTokens[++i];
      continue;
    }
    if(strcmp(tok, "2>") == 0)
    {
      if (commandTokens[i + 1] == NULL)
      {
        fprintf(stderr, "syntax error: expected file after '2>'\n");
        ok = false;
        break;
      }
      redirect->redirectStderr = true;
      redirect->stdErrPath = commandTokens[++i];
      continue;
    }
    if(strcmp(tok, "2>>") == 0)
    {
      if (commandTokens[i + 1] == NULL)
      {
        fprintf(stderr, "syntax error: expected file after '2>>'\n");
        ok = false;
        break;
      }
      redirect->appendStderr = true;
      redirect->stderrAppendPath = commandTokens[++i];
      continue;
    }
    if(strcmp(tok, "&&") == 0 || strcmp(tok, "||") == 0 || strcmp(tok, "|") == 0)
    {
      if(commandTokens[i + 1] == NULL)
      {
        fprintf(stderr, "syntax error: expected file after %s\n", tok);
        ok = false;
        break;
      }
      if(position == 0)
      {
        fprintf(stderr, "syntax error near unexpected token '%s'\n", tok);
        ok = false;
        break;
      }
      if(segment >= MAX_SEGMENTS - 1)
      {
        fprintf(stderr, "shell: too many pipeline segments (max %d)\n", MAX_SEGMENTS - 1);
        ok = false;
        break;
      }
      segments[segment][position] = NULL;
      if(strcmp(tok, "&&") == 0)
      {
        typeOfSegment[segment] = AND;
      }
      else if (strcmp(tok, "||") == 0)
      {
        typeOfSegment[segment] = OR;
      }
      else
      {
        typeOfSegment[segment] = PIPE;
      }
      segment++;
      position = 0;
      continue;
    }
    if(position >= MAX_ARGS_PER_SEG - 1)
    {
      fprintf(stderr, "shell: too many arguments in segment (max %d)\n", MAX_ARGS_PER_SEG - 1);
      ok = false;
      break;
    }
    segments[segment][position++] = tok;
  }
  segments[segment][position] = NULL;
  typeOfSegment[segment] = NONE;
  *segmentCount = segment + 1;
  return ok;
}
static int buildPipelines (char *segments[MAX_SEGMENTS][MAX_ARGS_PER_SEG], segmentType typeOfSegment[MAX_SEGMENTS], int segmentCount, char *(*pipelines)[MAX_SEGMENTS][MAX_ARGS_PER_SEG], int pipelineSegment[MAX_PIPELINES], segmentType pipelineConditionals[MAX_PIPELINES])
{
  int pipelineCount = 0;
  int commandInScope = 0;
  
  for(int i = 0 ; i < segmentCount ; i++)
  {
    int currentArgument = 0;
    while(segments[i][currentArgument] != NULL)
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
    pipelineConditionals[pipelineCount] = typeOfSegment[i];
    pipelineCount++;
    commandInScope = 0;
  }
  return pipelineCount;
}
static int dispatchBuiltIn(char **current, shellState *state, redirectConfig *redirect, bool toBackground, bool *outShouldExit)
{
  if(strcmp("exit", current[0]) == 0)
  {
    *outShouldExit = true;
    return 0;
  }
  if(strcmp("echo", current[0]) == 0)
  {
    return echo(current, redirect);
  }
  if(strcmp("cd", current[0]) == 0)
  {
    return cd(current, redirect);
  }
  if(strcmp("pwd", current[0]) == 0)
  {
    return pwd(redirect);
  }
  if(strcmp("history", current[0]) == 0)
  {
    return history(current, state->historyBuffer, redirect);
  }
  if(strcmp("type", current[0]) == 0)
  {
    type(current, redirect);
    return 0;
  }
  if(strcmp("jobs", current[0]) == 0)
  {
   jobs(jobList, redirect);
    return 0;
  }
  
  return executeBin(toBackground, redirect, commandTokens);

}
static int executePipelines(char *(*pipelines)[MAX_SEGMENTS][MAX_ARGS_PER_SEG],int pipelineCount,int pipelineSegment[MAX_PIPELINES],
segmentType pipeLineConditionals[MAX_PIPELINES],shellState *state,redirectConfig *redirect,
bool toBackground,bool *outShouldExit)
{
  int lastStatus = 0;

  segmentType prevConditional = NONE;

  for(int v = 0 ; v < pipelineCount; v++)
  {

      if (v > 0)
      {
        if (prevConditional == AND && lastStatus != 0)
        {
          prevConditional = pipeLineConditionals[v];
          continue;
        }
        if(prevConditional == OR && lastStatus == 0)
        {
          prevConditional = pipeLineConditionals[v];
          continue;
        }
      }
    char **current = pipelines[v][0];
    if(current[0] == NULL)
    {
      lastStatus = 1;
      prevConditional = pipeLineConditionals[v];
      continue;
    }
    if(pipelineSegment[v] > 1)
    {
      lastStatus = runPipeline(toBackground, commandTokens,pipelines[v], pipelineSegment[v], state->historyBuffer, redirect);
    }
    else
    {
      lastStatus = dispatchBuiltIn(current, state, redirect, toBackground, outShouldExit);
      if(*outShouldExit)
      {
        return lastStatus;
      }
    }
    prevConditional = pipeLineConditionals[v];
  }
  return lastStatus;
}
static void REPL(shellState *state)
{
  while(true)
  {
    if(!readAndPrepareInput(state))
    {
      continue;
    }
    bool toBackground = detectBackground();
    checkBacktroundJobs();
    if(commandTokens[0] == NULL)
    {
      continue;
    }

    redirectConfig redirect;
    redirectConfigInit(&redirect);

    char *segments[MAX_SEGMENTS][MAX_ARGS_PER_SEG];
    segmentType typeOfSegments[MAX_SEGMENTS];
    int segmentCount = 0;
    
    if(!parseTokensToSegments(segments, typeOfSegments, &redirect, &segmentCount))
    {
      continue;
    }

    char *(*pipelines)[MAX_SEGMENTS][MAX_ARGS_PER_SEG] = calloc(MAX_PIPELINES, sizeof(*pipelines));

    if(pipelines == NULL)
    {
      fprintf(stderr, "shell: out of memory\n");
      exit(1);
    }
    int pipelineSegment[MAX_PIPELINES];
    segmentType pipelineConditionals[MAX_PIPELINES];

    int pipelineCount = buildPipelines(segments, typeOfSegments, segmentCount, pipelines, pipelineSegment, pipelineConditionals);

    bool shouldExit = false;
    executePipelines(pipelines, pipelineCount, pipelineSegment, pipelineConditionals, state, &redirect, toBackground, &shouldExit);

    free(pipelines);

    if(shouldExit)
    {
      break;
    }

  }
}