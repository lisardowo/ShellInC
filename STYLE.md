# Intro

Following document will cover general rules that Im enforcing for PR review.

And yes, this set is not final and Im open to discussion.
If you want to add/remove/change something here please feel free to contact me.

# General rules

## Readability and Simplicity first

Code is intended to be public.
Avoid one-liners from hell and keep code complexity under control.
Try to make code self-explanatory and add comments if needed.
Leave references to standards that you are implementing.

## Variable and function names must clearly define what it's doing

It's ok if it will be long, but it should clearly state what it's doing, without need to dive into code.
This also applies to function/method's code.
Try to avoid one letter variables.

By no means magic numbers shall be used, use defines to state whatever you're doing and from where that number is coming from

### Functions and variables are camelCase
    
```C
extern char **commandTokens;
extern int commandTokensCapacity;
```

### defines are SCREAMING_SNAKE_CASE

Examples:
```C
#define MAX_SEGMENTS 100
#define MAX_ARGS_PER_SEG 100
#define MAX_PIPELINES 100
#define HISTORY_BUFFER_CAPACITY 10000
```

    