#include "arguments.h"

#define temporalBuffer 4096

char userInput[10000];
char **commandTokens = NULL;
int commandTokensCapacity = 0;

void resetCommandTokens()
{
    if (commandTokens != NULL)
    {
        for ( int i = 0 ; commandTokens[i] != NULL ; i++)
        {
            free(commandTokens[i]);
        }
        free(commandTokens);
        commandTokens = NULL;
    }
    commandTokensCapacity = 0;
}

void argumentExtractor(char *userInput)
{
    resetCommandTokens();
    int argIndex = 0;
    commandTokensCapacity = 10;
    commandTokens = calloc(commandTokensCapacity, sizeof(char *));
    char token[temporalBuffer];
    int tokenLen = 0;
    bool inSingle = false;
    bool inDouble = false;
    bool escaped = false;

    for (int i = 0 ;; i++)
    {
        char c = userInput[i];
        bool endOfInput = (c == '\0');
        bool split = (!endOfInput && userInput[i] == ' ' && !inSingle && !inDouble && !escaped);

        if(endOfInput || split)
        {
            if (escaped)
            {
                if(tokenLen < (int)sizeof(token) - 1)
                {
                    token[tokenLen++] = '\\';
                }
                escaped = false;
            }
            if(tokenLen > 0 || inDouble || inSingle)
            {
                token[tokenLen] = '\0';
                if (argIndex + 1 >= commandTokensCapacity)
                {
                    commandTokensCapacity *= 2;
                    commandTokens = realloc(commandTokens, commandTokensCapacity * sizeof(char *));
                }
                commandTokens[argIndex++] = strdup(token);
                tokenLen = 0;
            }
            if(endOfInput)
            {
                break;
            }
            continue;
        }

        if(escaped)
        {
            if(inDouble)
            {
                if(c == '"' || c == '\\' || c == '$' || c == '`' || c == '\n')
                {
                    if (tokenLen < (int)sizeof(token) - 1)
                    {
                        token[tokenLen++] = c;
                    }
                }
                else
                {
                    if(tokenLen < (int)sizeof(token) - 1)
                    {
                        token[tokenLen++] = '\\';
                    }
                    if(tokenLen < (int)sizeof(token) - 1)
                    {
                        token[tokenLen++] = c;
                    }
                }
            }
            else if (inSingle)
            {
                if (tokenLen < (int)sizeof(token) - 1)
                {
                    token[tokenLen++] = '\\';
                }
                if(tokenLen < (int)sizeof(token) - 1)
                {
                    token[tokenLen++] = c;
                }
            }
            else
            {
                if(tokenLen < (int)sizeof(token) - 1)
                {
                    token[tokenLen++] = c;
                }
            }

            escaped = false;
            continue;
        }

        if (c == '\\')
        {
            if(inSingle)
            {
                if (tokenLen < (int)sizeof(token) - 1)
                {
                    token[tokenLen++] = c;
                }
            }
            else
            {
                escaped = true;
            }
            continue;
        }
        if (c == '"' && !inSingle)
        {
            inDouble = !inDouble;
            continue;
        }
        if(c == '\'' && !inDouble)
        {
            inSingle = !inSingle;
            continue;
        }
        if(tokenLen < (int)sizeof(token) - 1)
        {
            token[tokenLen++] = c;
        }
    }
    commandTokens[argIndex] = NULL;
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


static char *newcommandTokens[1000];

static int globStoragePosition = 0;

static void resetGlobStorage()
{
    globStoragePosition = 0;
}

void expandGlobs(char *commandTokens[])
{
    int newArgc = 0;
    resetGlobStorage();

    for (int i = 0 ; commandTokens[i] != NULL ; i++)
    {
        if(strchr(commandTokens[i], '*') != NULL || strchr(commandTokens[i], '?') != NULL)
        {
            glob_t globResult;

            int result = glob(commandTokens[i], GLOB_NOCHECK | GLOB_TILDE, NULL , &globResult);

            if (result == 0)
            {
                for (size_t j = 0; j < globResult.gl_pathc ; j++)
                {
                  char *copy = strdup(globResult.gl_pathv[j]);
                  if(copy != NULL)
                  {
                    newcommandTokens[newArgc++] = copy;
                  }
                }
            }
            globfree(&globResult);
        }
        else
            {

                newcommandTokens[newArgc++] = commandTokens[i];
           
            }
        }
            newcommandTokens[newArgc] = NULL;
            int originalArgc = 0;
            while(commandTokens[originalArgc] != NULL)
            {
                originalArgc++;
            }

            for(int i = 0 ; i < originalArgc ; i++)
            {
                if (commandTokens[i] != NULL && (strchr(commandTokens[i], '*') != NULL || strchr(commandTokens[i], '?') != NULL))
                {
                    free(commandTokens[i]);
                    commandTokens[i] = NULL;
                }
            }

            for (int i = 0 ; i <= newArgc ; i++)
            {
                commandTokens[i] = newcommandTokens[i];
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

void expandArguments(char *commandTokens[])
{

    for(int i = 0 ; commandTokens[i] != NULL; i++)
    {
        if (commandTokens[i] == NULL)
        {
            continue;
        }

        char *originalToken = commandTokens[i];
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
                    char *expandend = strdup(tempBuffer);
                    if (expandend != NULL)
                    {
                        free(commandTokens[i]);
                        commandTokens[i] = expandend;
                    }
                }

            }
}




