
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

void redraw(char *prompt, char *buf)
{
    printf("\r\033[2K%s%s", prompt, buf);
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

void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize)
{
    size_t len = 0;
    int tabCount = 0;
    out[0] = '\0';

    printf("%s", prompt);
    fflush(stdout);

    if(!enableRaw())
    {
        printf("problems");
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

        if (c == '\n')
        {
            out[len] = '\0';
            printf("\n");
            disableRaw();
            return;
        }

        if (c == 127 || c == '\b')
        {
            if (len > 0)
            {
                len --;
                out[len] = '\0';
                redraw(prompt, out);

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
                out[len] = '\0';
                redraw(prompt, out);
            }
            else 
            {
                size_t lcp = lengestCommonPrefix(matches, matchesSize);
                size_t prefixLen = strlen(prefix);
                while (prefixLen < lcp && len + 1 < outSize)
                {
                    out[len ++] = matches[0][prefixLen++];
                }
                out[len] = '\0';
                redraw(prompt, out);

                if (tabCount >= 1)
                {
                    printf("\n");
                    for (size_t i = 0 ; i < matchesSize ; i++)
                    {
                        printf("%s ", matches[i]);
                    }
                    printf("\n");
                    redraw(prompt, out);

                
                }
            }
            free(matches);
            tabCount ++ ;
            continue;
        }
        if (c >= 32 && c <= 126)
        {
            if(len +1 < outSize)
            {
                out[len ++] = c;
                out[len] = '\0'; 
                write(STDOUT_FILENO, &c , 1);
            }
            tabCount = 0;
        }
    }
}