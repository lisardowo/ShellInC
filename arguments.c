

#include "arguments.h"
#include "quotationParser.h"

static char argvStorage[10000];
char *argv[100];

void argumentExtractor(char *userInput, int argumentCount)
{

	char token[1000];
  int tokenLen = 0 ;
  int argIndex = 0;
  int storagePos = 0;

  bool inSingle = false;
  bool inDouble = false;
  bool escaped = false;

  int maxArgs = 99;

  if (argumentCount > 0 && argumentCount < 100)
  {
    maxArgs = argumentCount;
  }

  for (int i = 0 ; ; i++)// TODO condition not specified ->trying with until null term
  {
      bool endOfInput = (userInput[i] == '\0');
      bool splitInPosition = (!endOfInput && userInput[i] == ' ' && !inSingle && !inDouble && !escaped);


      if(endOfInput || splitInPosition)
      {
          if (escaped)
          {
              if(tokenLen < ((int)sizeof(token) - 1))
              {
                  token[tokenLen ++] = '\\';
              }
              escaped = false;
          } 
          if (tokenLen > 0 && argIndex < maxArgs)
          {
            token[tokenLen] = '\0';

            if (storagePos + tokenLen + 1 >= (int)sizeof(argvStorage))
            {
              break;
            }

            argv[argIndex] = &argvStorage[storagePos];
            memcpy(argv[argIndex], token, tokenLen + 1);
            storagePos += tokenLen + 1;
            argIndex++;
            tokenLen = 0;
          }

          if(endOfInput)
          {
            break;
          } 
         
          continue;
      }
      if (escaped)
      {
        if (inDouble)
        {
          if (userInput[i] == '\"' || userInput[i] == '\\' || userInput[i] == '$' || userInput[i] == ' ' || userInput[i] == '\n')
          {
              if(tokenLen < ((int)sizeof(token) - 1))
              {
                token[tokenLen++] = userInput[i];
              }
          }
          else 
          {
             if (tokenLen < ((int)sizeof(token) - 1))
             {
                token[tokenLen++] = '\\';
             }
             if (tokenLen < ((int)sizeof(token) - 1))
             {
                token[tokenLen++] = userInput[i];
             }
          }
        }
        else 
        {
          if(tokenLen < ((int)sizeof(token) - 1))
          {
              token[tokenLen ++] = userInput[i];
          }
        }

        escaped = false;
        continue;
      }

      if (userInput[i] == '\\')
      {
          if (inSingle)
          {
            if (tokenLen < (int)sizeof(token) - 1)
            {
                token[tokenLen ++] = '\\';
            }
          }
          else
          {
            escaped = true;
          }
          continue;
      }
      if (userInput[i] == '\"' && !inSingle)
      {
          inDouble = toogleState(inDouble);
          continue;
      }

      if (userInput[i] == '\'' && !inDouble)
      {
          inSingle = toogleState(inSingle);
          continue;
      }

      if (tokenLen < (int)sizeof(token) - 1)
      {
          token[tokenLen ++] = userInput[i];
      }

  }
  argv[argIndex] = NULL; // for vulnerable version, change this null for a fixed last element indicator string
}


void argumentCounter(char *userInput, int* argumentCount)
{
  bool inSingle = false;
  bool inDouble = false; 
  bool escaped = false;
  bool inToken = false;

  *argumentCount = 0;

  for (int i = 0 ; ; i++)
  {
      if(userInput[i] == '\0')
      {
        break;
      }

      if(escaped)
      {
          escaped = false;
          if(!inToken)
          {
            inToken = true;
            (*argumentCount) ++ ;
          }
          continue;
      }
      if (userInput[i] == '\\' && !inSingle)
      {
         escaped = true;
    if (!inToken)
    {
        inToken = true;
        (*argumentCount)++;
    }
    continue;
      }
      if (userInput[i] == '\"' && !inSingle)
      {
          inDouble = toogleState(inDouble);
          if (!inToken)
          {
            inToken = true;
            (*argumentCount) ++ ;

          }
          continue;
      }
      if (userInput[i] == '\'' && !inDouble)
      {
          inSingle = toogleState(inSingle);
          if (!inToken)
          {
              inToken = true;
              (*argumentCount) ++ ;
          }
          continue;
      }
      if (userInput[i] == ' ' && !inSingle && !inDouble)
      {
          inToken = false;
          continue;
      }
      if (!inToken)
      {
          inToken = true;
          (*argumentCount) ++;
      }
  }
}

bool toogleState(bool state)
{
	return !state;
}
