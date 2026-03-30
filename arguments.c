

#include "arguments.h"

static char argvStorage[10000];
char *argv[100];

void argumentExtractor(char *userInput, int argumentCount)
{
  
  char arguments[10000000];
  char joinedArguments[10000000];
  bool quoted = false;
  char quoteChar = '\0';
  int i = 0;
  int storagePosition = 0;

  strncpy(arguments, userInput, sizeof(arguments));
  arguments[sizeof(arguments) - 1] = '\0';
  joinedArguments[0] = '\0';

  char *token = strtok(arguments, " ");
  while(token != NULL && i < argumentCount)
  {

    if(!quoted)
    {
	
      size_t len = strlen(token);
      bool startsQuoted = token[0] == '\'';
      bool closedInToken = (len > 1 && token[len - 1 ] == token[0]);

      if (startsQuoted && !closedInToken)
      {
      
        quoted = true;
        quoteChar = token[0];
        strncpy (joinedArguments, token , sizeof(joinedArguments) - 1);
        joinedArguments[sizeof(joinedArguments) - 1] = '\0';

      }

    else
    {
	  removeQuotes(token);

	  size_t tokenLen = strlen(token);

	  if(storagePosition + (int)tokenLen + 1 >= (int)sizeof(argvStorage))
	  {
		break ;
	  }


      argv[i] = &argvStorage[storagePosition];
      memcpy(argv[i], token, tokenLen + 1);
	  storagePosition += (int)tokenLen + 1;
      i++;

    }

  }
   else
  {
	size_t room = sizeof(joinedArguments) - (strlen(joinedArguments) - 1);
	if (room > 0)
	{
		strncat(joinedArguments, " ", room);
	}

	room = sizeof(joinedArguments) - (strlen(joinedArguments) - 1);
	if (room > 0 )
	{
		strncat(joinedArguments, token, room);

	}

	size_t len = strlen(token);
	if (len > 0 && token[len - 1] == quoteChar)
	{
		quoted = false;
		quoteChar = '\0';

		removeQuotes(joinedArguments);

		size_t joinedArgumentsLen = strlen(joinedArguments);

		if ((storagePosition + (int)joinedArgumentsLen + 1) >= (int)sizeof(argvStorage))
		{
			break;
		}

		argv[i] = &argvStorage[storagePosition];
		memcpy(argv[i], joinedArguments, joinedArgumentsLen + 1);
		storagePosition += (int)joinedArgumentsLen + 1;
		i ++;

		joinedArguments[0] = '\0';

	}

	token = strtok(NULL, " ");

  }
    


  }
  argv[i] = 
}

void argumentCounter(char *userInput, int* argumentCount)
{

  bool activeQuotes = false;
  *argumentCount = 1;
  for(int i = 0 ; userInput[i] != '\0' ; i++)
  {
    if (userInput[i] == '\'')
    {
      activeQuotes = toogleQuotes(activeQuotes);
    }
    if (userInput[i] == ' ' && activeQuotes == false)
    {
      (*argumentCount) ++ ;
    }

  }
  printf("Argc : %d\n", *argumentCount);

}

bool toogleQuotes(bool activeQuotes)
{
    printf("now : %d, changed : %d\n", activeQuotes, !activeQuotes);
    return !activeQuotes;
}

void removeQuotes(char *token)
{
   char *src = token; // Pointer to read
    char *dst = token; // Pointer to write

    while (*src) {
        if (*src != '\"' && *src != '\'') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0'; // Null-terminate the new string
}