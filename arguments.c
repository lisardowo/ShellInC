#include "arguments.h"

char userInput[10000];

static char argvStorage[10000];
char *argv[1000];

static char storage[1000];
static int storage_position = 0;

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

  for (int i = 0 ; ; i++)
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


static char *newArgv[1000];
static char globStorage[20000];
static int globStoragePosition = 0;

static void resetGlobStorage()
{
    globStoragePosition = 0;
}

void expandGlobs(char *argv[])
{
    int newArgc = 0;
    resetGlobStorage();

    for (int i = 0 ; argv[i] != NULL ; i++)
    {
        if(strchr(argv[i], '*') != NULL || strchr(argv[i], '?') != NULL)
        {
            glob_t globResult;

            int result = glob(argv[i], GLOB_NOCHECK | GLOB_TILDE, NULL , &globResult);

            if (result == 0)
            {
                for (size_t j = 0; j < globResult.gl_pathc ; j++)
                {
                    int len = strlen(globResult.gl_pathv[j]);

                    if ((globStoragePosition + len) + 1 < (int)sizeof(globStorage) && newArgc < 99)
                    {
                        strcpy(&globStorage[globStoragePosition], globResult.gl_pathv[j]);
                        newArgv[newArgc++] = &globStorage[globStoragePosition];
                        globStoragePosition += len + 1;
                    }
                }
            }
            globfree(&globResult);
        }
        else
            {

                newArgv[newArgc++] = argv[i];
           
            }
        }
            newArgv[newArgc] = NULL;

            for (int i = 0 ; i <= newArgc ; i++)
            {
                argv[i] = newArgv[i];
            }

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


void removeQuotes(char *token)
{
    char activeQuote = '\0';
    char *src = token;
    char *dst = token;
    while (*src != '\0')
    {
        if ((*src == '\'' || *src == '"'))
        {
            if (activeQuote == '\0')
            {
                activeQuote = *src;
                src ++;
                continue;
            }
            if (activeQuote == *src)
            {
                activeQuote = '\0';
                src++;
                continue;
            }
        }

        *dst++ = *src++;
    }

    *dst = '\0';
    
}

void spacesInQuotes(char *userInput)
{
	char activeQuote = '\0';

	for (int i = 0 ; userInput[i] != '\0' ; i++)
	{
		if (userInput[i] == '\'' || userInput[i] == '\"')
		{
			if (activeQuote == '\0')
            {
                activeQuote = userInput[i];
            }
            else if( activeQuote == userInput[i])
            {
                activeQuote = '\0';
            }
            
		}

		if(userInput[i] == ' ' && activeQuote != '\0')
		{
			userInput[i] = 31;
		}
	}
}

void restoreSpaces(char *userInput)
{
	for (int i = 0 ; userInput[i] != '\0' ; i++)
	{

		if((unsigned char)userInput[i] == 31)
		{
			userInput[i] = ' ';
		}
	}
}


static void resetStorage_position() 
{
    storage_position = 0;
}

void expandArguments(char *argv[])
{
    resetStorage_position();

    for(int i = 0 ; argv[i] != NULL; i++)
    {
        if (argv[i] == NULL)
        {
            continue;
        }

        char *originalToken = argv[i];
        char tempBuffer[1000];
        int temp_position = 0;
        bool isExpandible = false;

        for (int v = 0 ; originalToken[v] != '\0' ; v++)
        {
            if(originalToken[v] == '$' && originalToken[v+1] != '\0' && originalToken[v + 1] != ' ')
            {
            
                isExpandible = true;
                v++;

                char varName[100];
                int varPosition = 0;

                while ((originalToken[v] != '\0' && originalToken[v] >= 'a' && originalToken[v] <= 'z') || (originalToken[v] >= 'A' && originalToken[v] <= 'Z') || (originalToken[v] >= '0' && originalToken[v] <= '9') || (originalToken[v] == '_'))
                {
                    if(varPosition < (int)sizeof(varName) - 1)
                    {
                        varName[varPosition++] = originalToken[v];
                    }
                    v++;
                }
                varName[varPosition] = '\0';
                v--;
                
                char *envValue = getenv(varName);
                if(envValue != NULL)
                {
                    for(int k = 0 ; envValue[k] != '\0' ; k++)
                    {
                        if (temp_position < (int)sizeof(tempBuffer) - 1)
                        {
                            tempBuffer[temp_position++] = envValue[k]; 
                        }
                    }
                }
            }

            else
            {
                if(temp_position < (int)sizeof(tempBuffer) - 1)   
                {
                    tempBuffer[temp_position++] = originalToken[v];
                }
            }   
        }
                tempBuffer[temp_position++] = '\0';

                if (isExpandible)
                {
                    int len = strlen(tempBuffer);
                    if((storage_position + len) + 1 < (int)sizeof(storage))
                    {
                        strcpy(&storage[storage_position], tempBuffer);
                        argv[i] = &storage[storage_position];
                        storage_position += (len + 1);
                    }
                }

            }
}




