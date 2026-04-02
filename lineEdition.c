
#include "lineEdition.h"

static struct termios g_old;

bool enableRaw(void)
{
    if(tcgetattr(STDIN_FILENO, &g_old) == - 1)
    {
        return false;
    }
    struct termios raw = g_old;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    return tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != - 1;
}

void disableRaw(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old);
}

void redraw(char *prompt, char *buf, size_t cursorPos)
{
    printf("\r\033[2K%s%s", prompt, buf);

    int column = strlen(prompt) + cursorPos + 1;

    printf("\033[%dG", column);
    fflush(stdout);
}

void firstToken(char *buf, char *out, size_t outSize)
{
    size_t i = 0 ;
    while (buf[i] != '\0' && buf[i] != ' ' && i + 1 < outSize)
    {
        out[i] = buf[i];
        i ++;
    }
    out[i] = '\0';
}

void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize,int *historyCount, char *historyBuffer[])
{

    int historyIndex = *historyCount;
    size_t cursorPos = 0;
    char tempDraft[outSize];
    strcpy(tempDraft, "");
    size_t len = 0;
    int tabCount = 0;
    out[0] = '\0';

    printf("%s", prompt);
    fflush(stdout);

    if(!enableRaw())
    {
        return;
    }

    while(true)
    {
        char c;

        if (read(STDIN_FILENO, &c, 1) != 1)
        {
            disableRaw();
            return;
        }

        if (c == '\033')
        {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) == 0)
            {
                continue;
            }
            if (read(STDIN_FILENO, &seq[1], 1) == 0)
            {
                continue;
            }

            if (seq[0] == '[')
            {
                if (seq[1] == leftArrowKey)
                {
                    if(cursorPos > 0)
                    {
                        cursorPos --;
                        redraw(prompt, out, cursorPos);
                    }
                }
                if(seq[1] == rightArrowKey)
                {
                    if (cursorPos < len)
                    {
                        cursorPos++;
                        redraw(prompt, out, cursorPos);
                    }
                }
                if (seq[1] == upArrowKey)
                {
                    
                    if (*historyCount <= 0)
                    {
                        continue;
                    }
                    if (historyIndex == *historyCount)
                    {
                        strncpy(tempDraft, out, outSize);
                        tempDraft[outSize - 1] = '\0';
                    }
                    if (historyIndex > 0)
                    {
                        historyIndex--;
                        strncpy(out, historyBuffer[historyIndex], outSize);
                        out[outSize - 1] = '\0';
                        len = strlen(out);
                        cursorPos = len;
                        redraw(prompt, out, cursorPos);
                    }
                }
            
            else if (seq[1] == downArrowKey)
            {
                if (*historyCount == 0)
                {
                    continue;
                }
                if (historyIndex < *historyCount)
                {
                    historyIndex ++;
                    if (historyIndex == *historyCount)
                    {
                        strncpy(out, tempDraft, outSize - 1);
                    }
                    else
                    {
                        strncpy(out, historyBuffer[historyIndex], outSize);
                    }
                    out[outSize - 1] = '\0';
                    len = strlen(out);
                    cursorPos = len;
                    redraw(prompt,out, cursorPos);
                }
            }
        }
            continue;
        }

        if (c == '\n')
        {
            out[len] = '\0';
            printf("\n");
            disableRaw();
            return;
        }

        if (c == 127 || c == '\b')
        {
            if (cursorPos > 0)
            {

                for (size_t i = cursorPos -1 ; i < len ; i++)
                {
                    out[i] = out[i + 1];
                }
                len --;
                cursorPos --;
                redraw(prompt, out, cursorPos);

            }
            tabCount = 0;
            continue;
        }

        if (c == '\t')
        {
            char prefix[256];
            firstToken(out, prefix, sizeof(prefix));

            char **matches = NULL;
            size_t matchesSize = prefixMatches(list,prefix, &matches);

            if (matchesSize == 0)
            {
                write(STDOUT_FILENO, "\a", 1);
            }
            else if (matchesSize == 1)
            {
                size_t v = strlen(prefix);
                char *full = matches[0];
                while (full[v] != '\0' && len + 1 < outSize)
                {
                    out[len ++] = full[v++];
                }
                if(len + 1 < outSize)
                {
                    out[len++] = ' ';
                }
                out[outSize - 1] = '\0';
                len = strlen(out);
                cursorPos = len;
                redraw(prompt, out, cursorPos);
            }
            else 
            {
                size_t lcp = lengestCommonPrefix(matches, matchesSize);
                size_t prefixLen = strlen(prefix);
                while (prefixLen < lcp && len + 1 < outSize)
                {
                    out[len ++] = matches[0][prefixLen++];
                }
                out[outSize - 1] = '\0';
                len = strlen(out);
                cursorPos = len;
                redraw(prompt, out, cursorPos);

                if (tabCount >= 1)
                {
                    printf("\n");
                    for (size_t i = 0 ; i < matchesSize ; i++)
                    {
                        printf("%s ", matches[i]);
                    }
                    printf("\n");
                    redraw(prompt, out, cursorPos);
 
                
                }
            }
            free(matches);
            tabCount ++ ;
            continue;
        }
        if (c >= 32 && c <= 126)
        {
            if(len + 1 < outSize)
            {
                if (cursorPos < len)
                {
                    for(size_t i = len; i > cursorPos ; i--)
                    {
                        out[i] = out[i - 1];
                    }
                }
                out[cursorPos] = c;
                len ++;
                cursorPos++;
                out[len] = '\0'; 
              
                redraw(prompt, out, cursorPos);
            }
            tabCount = 0;
        }
    }
}