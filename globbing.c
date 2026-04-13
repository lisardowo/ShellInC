
#include "globbing.h"

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
