# Ghost in the shell

Completely functional linux shell written from scratch, no extern libraries used. Project created with the purpose of understand better operative systems and masterize my C abilities, this project implement system calls, dynamic memory, parsing and file descriptors to assure an stable experience across devices.

When the project is completed, another branch will be opened in which a highly vulnerable shell would be in development following the idea behind "Damn Vulnerable Web Services" and other cibersecurity projects. 

This readme file documents the functional and secure version, the purpose behind such extensive documentation is either look for knoledge gaps when trying to break the vulnerable version or look how original version is thought in order to apply patches to sanitize vulnerable version.

This is not the best work but at least works.

## Index

1. [Purpose and Architecture](#1-purpose-and-architecture)
2. [Dependencies](#2-dependencies)
3. [Public API (headers)](#3-public-api-headers)
4. [Functional Behavior Summary](#4-functional-behavior-summary)
5. [Extending behavior](#5-extending-behavior)
6. [Current Known Gaps](#6-current-known-gaps)
7. [Installation and Quick Test](#7-installation-and-quick-test)

## Project Origin

Ghost in the shell is born out of curiosity about how OS internally work, impulsed by my recent interest in cibersecurity and systems explotation, the program is also thought to have a vulnerable verstryinion that can be easily exploited in systems as metasploitable or similar, granting another way to practice security system by re-parching the vulnerable shell to a most stable version with less risk

## 1. Purpose and Architecture

Main pipeline:

1. Prompting the user.
2. Receive input .
3. Tokenize the input via parsing (similar to how argc and commandTokens work in a main function) and sanitize.
4. search for commandTokens[0] (command) either in PATH or builtin.
5. Execute command passing the other elements in commandTokens as arguments.

Modules:

- `arguments.*`: Parsing and extraction of the arguments that are written in prompt.
- `binariesManager.*`: Module in charge to find and execute binaries.
- `completition.*`: Logic behind autocomplete of input when pressing TAB.
- `inputManager.*`: Implementation of the prompt functions, directly related to completition.
- `lineEdition.*`: Implementation of the completition module, recieving the autocompleted commands and replacing the in terminal input, highly depends in "input manager" and "completition" modules.
- `quotationParser.*`: Parser that detects and interpret input inside quotation.
- `getHistory.*` : module that manages the history logic
- `expansion.*` : module that expands special tokens
- `fileCompletion.*` : module that handles self completion logic
- `globbing.*` : module that expands tokens 
- `jobsManager.*` : module that controls backgrounding of jobs
- `pipeline.*` : module that controls pipeling 
- `signalsManager.*` : module that handles keyboard signals in runtime
- `utils.*` : module with utility, general purpose functions
## 2. Dependencies


### 2.1 C standard library dependencies

- `stdio.h`
- `string.h`
- `stdint.h`
- `unistd.h`
- `fnctl.h`
- `stdbool.h`
- `stdlib.h`

### 2.3 Internal module dependencies

- `main.c` depends on: `lineEdition.h`, `binariesManager.h`,`arguments.h`,`inputManager.h`,`completition.h`
- `lineEdition.c` depends on: `lineEdition.h`, `completition.h`
- `binariesManager.c` depends on: `binariesManager.h`, `arguments.h`
- `arguments.c` depends on: `arguments.h`, `quotationParser.h`
- `inputManager.c` depends on: `inputManager.h`
- `Completition.c` depends on: `completition.h`
- `quotationParser.c` depends on: `quotationParser.h`
- `utils.c` depends on: `utils.h`
- `getHistory.c` depends on: `quotationParser.h`
- `expansion.c` depends on `expansion.h`
- `fileCompletion.c` depends on `fileCompletion.h`
- `globbing.c` depends on `globbing.h`
- `jobsManager.c` depends on `jobsManager.h`, `utils.h`
- `pipeline.c` depends on `binariesManager.h`,`signalsManager.h`,`builtIn.h`,`utils.h`
- `signalsManager.c` depends on `signalsManager.h`



## 3. Public API (headers)

This section lists all functions declared in headers (expected API surface).

### 3.1 `arguments.h`

- `void argumentCounter(char *userInput, int* argumentCount);`
  - Counts the arguments given in the prompt based in the spaces, if part of the string is quoted then counts as just one argument.
- `void argumentExtractor(char *userInput, int argumentCount);`
  - based in argument count, tokenize the arguments and stores them to a argument vector variable
- `bool toogleState(bool state);`
  - utility function for is quoted logic (used in a primitive version) no longer in use for most code base


### 3.2 `binariesManager.h`

- `char* getPath(char *command);`
  - if a command(commandTokens[0]) is not a built-in function then program looks for it in PATH variable and if found returns the dir of the binary .
- `void executeBin(char *stdoutPath,char *stdErrPath, bool redirectedstdout, bool redirectedStdErr, bool appendStdOut, bool appendStdErr, char *commandTokens[]);`
  - First six parameters are "flags" that controls if the output of given  binary has to be redirected (">" operator) or will display in stdout, last parameter is the full (already tokenized) input (commandTokens). Function forks the procces and executes the binary which addres is given by getPath(), when done child fork collapses to his father, father keeps waiting to that collapse to continue

### 3.3 `completition.h`

- `typedef struct{} available commands;`
  - Struct that store all availableCommands system-wide for the autocomplete function, elements of the object are : 
  * char **items; -> array of strings that are all the commands found
  * size_t count; -> how many commands are in the struct
  * size_t cap; -> Represents the maximum count of commands (used mainly to realloc memory for the structure in order to hold all the commands that can be found)

- `bool startCommandsList(availableCommands *list);`
  - Creates an instance of the struct and initialize its elements to 0/NULL.
- `void commandsFree(availableCommands *list);`
  - Set tha values of the struc to 0/NULL then frees the memory efectively deleting previouse instance
- `bool commandListGrow(availableCommands *list);`
  - If commands surpase the limit of list->cap, function dupes cap size via realloc
- `bool commandListAdd (availableCommands *list, char *command);`
  - utility function that adds provided command to the list
- `int compareCommands(const void *a,const void *b);`
  - utility function for qsort, used in Sanitize process
- `void commandListSanitize(availableCommands *list);`
  - Adding to the current list is made in a quick, messy way. Sanitize sorts commands alphabetically and looks for duplicates, if found duplicates are deleted
- `bool getBuiltIns(availableCommands *list);`
  - function that provides the built-ins for the list to be filled.
- `bool getBins(availableCommands *list);`
  - function that provides the binariesPath for the list to be filled.
- `bool fillCommands(availableCommands *list);`
  - utility function that takes as input the filled but unorder list and sanitize it via commandListSanitize().
- `size_t lengestCommonPrefix(char **matches, size_t count);`
  - detect the longest common prefix, used for autocompletition logic
- `size_t prefixMatches (availableCommands *list, char *prefix, char ***matches);`
  - Count how many commands has the same prefix and store them in a matches array
- `bool startWith(char *word, char *prefix);`
  - utility function that checks if the two parameters have the same letter and until what point they are equal

### 3.4 `inputManager.h`

- `char* sanitizeInput(char* userInput);`
  - replaces new line with null terminator of the provided input


### 3.5 `lineEdition.h`

- `void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize);`
  - Uses the other functions to read dinamically and exchange buffers when autocompleted
- `void firstToken(char *buf, char *out, size_t outSize);`
  - gets only the first token in the buffer.
- `void redraw(char *prompt, char *buf);`
  - redraws the whole propmt line to the user.
- `void disableRaw(void);`
  - disable raw mode and goes back to std configuration.
- `bool enableRaw(void);`
  - changes terminal to raw mode, capturing keys instantenously.

### 3.6 `quotationParser.h`

- `void removeQuotes(char *token);`
  - remove the quotes in a argument to use it properly
- `void restoreSpaces(char *userInput);`
  - restore the spaces replacing the arbtary value back to a space.
- `void spacesInQuotes(char *userInput);`
  - replace quoted spaces for an arbitary value in arguments so it can be counted as one .

### 3.7 `pipeline.h`

- `int reddirectInChild(bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);`
  -  setup the output redirection of pipelined commands 
- `int runPipeline(bool toBackground, char *commandTokens[],char *commands[100][100], int commandCount ,char **historyBuffer, bool redirectedstdout, bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath, char *stdoutAppendPath, char *stderrAppendPath);`
  - Executes the piped commands 
- `int externalInChild(char **current, bool redirectedStdErr, bool appendStdErr, char *stdErrPath, char* stdErrAppendPath);`
  - run external commands in the pipeline
- `int runBuiltinChild(char *commandTokens[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);`
  - run built in commands in the pipeline
- `int runBuiltin(char *commandTokens[], char **current, char **historyBuffer,bool redirectedStdOut, bool redirectedStdErr, bool appendStdOuut, bool appendStdErr,char *stdOutPath, char *stdErrPath, char *stdoutAppendPath, char *stderrAppendPath);`
  - run built in commands

### 3.8 `signalsManager.h`

- `void ignoreSignalsInParent();`
  - ignore keyboard signals for the program itself (to not quit with ctrl + C)
- `void restoreSignalsInParent();`
  - restore the ignored signal and passes it to the program to interrupt running process

### 3.10 `utils.h`
- `bool isOperator(char *token);`
  - utility function used to set flags for operators along the code base 
- `void createPrompt();`
  - utility function that creates the prompt that will be rendered by drawLine function
- `int getFileDescriptor(const char *descriptorTarget,  int flags);`
  - utility function that gets the File descriptor for every function that needs it, may delete in the future due security uses 
- `char* getPath(char *command);`
  - utility function that gets the enviroment variable of PATH

### 3.11 `jobsManager.h`

- `typedef struct {} job;`
  - struct that represents the backgrounded job
  * int id;-> id of the job in the list (locally set when filling the list)
  * pid_t pid; -> process id given by the kernel
  * char *command; -> string of the command backgrounded
  * bool running; -> state of the command
- `extern job jobList[maxJobs];`
  - array of jobStructures that represents the backgrounded jobs
- `int addJob(pid_t pid, char *command)`
  - fills the backgrounded job and adds it to jobList
- `void removeJob(pid_t pid)`
  - Takes the pid of the process to remove from the list of backgrounded Jobs when its done
- `void checkBacktroundJobs()`
  - checksBackgroundJobs and if one of them are Done, shows it and removes it from the list

### 3.12 `globbing.h`
- `void expandGlobs(char *commandTokens[]);`
  - expand glob tokens t something the shell will understand
### 3.13 `getHistory.h`
- `void addHistory(char *command, int *historyCount, char *historyBuffer[]);`
  - add used command to history buffer
- `void dumpHistory(char *historyBuffer[]);`
  - Dumps history buffer to historyFile.txt, if historyFile.txt dont exist, will create it
- `void getHistory(int *historyCount, char *historyBuffer[]);`
  - loads history from historyFile to historyBuffer
- `bool expandHistory(char userInput[], size_t userInputSize, int historyCount, char *historyBuffer[]);`
  - utility function that expands !! arguments of history for correct interpretation

### 3.14 `fileCompletion.h`
- `size_t fileMatches(char *prefix, char ***matches);`
  - looks for the dirs and files in the system, returns how many matched the current evaluated prefix
### 3.15 `expansion.h`
- `void expandArguments(char *commandTokens[]);`
  - takes the arguments and if found a $ expands it to something the shell can understand
### 3.16 `builtin.h`
- `int type(char **current, bool redirectedstdout ,bool redirectedstderr, bool appendStdOut, bool appendStdErr, char *stdoutPath, char *stderrPath , char *stdoutAppendPath, char *stderrAppendPath );`
  - says if a command is builtin or system binary
- `int history(char **current, char *historyBuffer[], bool redirectedstdout, bool appendStdOut,  char *stdoutPath, char *stdoutAppendPath);`
  - shows the command history of the shell
- `int echo(char **current, bool redirectedstdout,  bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);`
  - prints the arguments passed after echo
- `int cd(char **current, bool redirectedstderr, bool appendStdErr, char *stderrPath, char *stderrAppendPath);`
  - navigate trought the system file
- `int pwd( bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);`
  - prints current directory
- `int jobs(job *jobList, bool redirectedstdout, bool appendStdOut, char *stdoutPath,  char *stdoutAppendPath);`
  - prints the list of background jobs

## 4. Functional Behavior Summary

- prompt : shell shows the prompt waiting for input, input is in raw mode which allows self completion
- sanitization: input is sanitized deleting new lines
- parsing: the program parses through the input creating tokens for each space and detects "|", "&" and ">" tokens
- expansion: relative arguments (such as ?.c) are expanded so the shell understands
- redirecting and pipelining: 
    * detect special operators (|, >, &)
    * separates the commands in segments
    * run the commands with modifiers if found
- execution: the shell runs the command with the specific modifiers given as arguments
- finalization: the program waits for the process to end and shows the prompt again, starting the cycle one more time  

## 5 Extending behavior

this section explains how to add new features:

### 5.1 Design goals

  * Keep REPL stable and predictable
  * Avoid regressions in parsing
  * Keep execution behaviour consistent
  * keep changes small and isolated using modules (header files)

### 5.2 Main consideration points

  1. New built-in commands:
     - add builtin name to getBuiltins() completition module
     - Add command handling branch in REPL logic
     - Include in type checks

  2. Parsing:
     - Respect quote/escape semantics
     - Avoid exceed buffer limits: REMEMBER TO TERMINATE STRINGS WITH '\0'

  3. Binaries execution:
     - Keep child/parent responsibilities separated
     - Validate paths and operator before execution

### 5.3 how I think new features

  - Define feature scope and affected modules
  - Add a simple implementation
  - Run manual regression checks
  - Try to provoke a crash
  - Add specific cases handling
  - Update documentation
    

### 5.4 tests you can do when tweaking the program

  - Empty Input does not crash
  - Quoted arguments dont produce any unexpected behaviour
  - Unknown commands return expected errors
  - Builtins keep working
  - Tab completion still works
  - Redirections behave as supposed
  - Terminal works fluently



## 6 Current Known Gaps

  1. lack of proper buffer/memory management
  2. Current line editor does not support advanced editing
  3. Autocompletion is first token only, does not have any class of argument-aware/flags completion
  5. no config system
  6. some buffers are fixed size
  7. Error messages even tho are functional, some outpus are inconsistent in style and detail
  8. quoting can still fail in specific cases
  9. The program does not consider specific cases and states that the terminal can take
  10. history file is created in the relative path where the shell is started
  11. Shell will only work in linux enviroments
  12. Codebase is unnecessary long and could be hard to modify  

## 7 Installation and Quick Test

### 7.1 Requirements 

```
  1. Linux enviroment
  2. gcc
  3. make
  4. cmake
  5. git
  6. python
```

### 7.2 Clone

  In your terminal: 
```bash
git clone https://github.com/lisardowo/Ghost-In-The-Shell.git
cd Ghost-In-The-Shell
```
### 7.3 Build 
  
  In your terminal: 
  ```bash
  make
  ```

### 7.4 Run

In your terminal: 
```bash
./shell
```

### 7.5 Some quick tests

  1. pwd:
    Expected: print of current working directory
  2. echo hello world
    Expected: hello world
  3. type echo
    Expected: echo is a builtin
  4. ls
    Expected: all the content in current directory
  5. echo test > out.txt
    Expected: creates out.txt with "test" inside
  6. cat out.txt
    Expected: prints test
  7. history
    Expected: prints command history of the session
  8. exit
    Expected: executable stops cleanly

### 7.6 Test Scripts

  the directory ./pythonTestScripts as the name implies, contains 10 python scripts that you can use to try out the shell and its features. 

  * Before using the scripts here are some considerations you must cover:
      - The project shall be already compiled (just run make command before trying to use any script)
      - for the script to work you have to be inside "pythonTestScripts" dir, trying to run the command from src will result in a error (just use cd command to move into pythonTestScripts)
      - libraries used are python standard, it doesnt use any dependency that need pip to install
      - file "writingToterminal" is a small library that exposes functions to use in all the tests but the file itself doesnt do anything
      - You do not need to manually run the terminal to use the test scripts, the scripts automatically start a new session and goes to testing
      - Scripts can take up to a couple of seconds to succesfully run, do not think is broken, wait about 15 seconds and should work. If not, feel free to contact me so we can figure out why

  Now that you are ready to use the scripts heres a quick guide:
    move into pythonTestScripts directory

```bash
cd ./pythonTestScripts
```

from pythonTestScripts dir you can run the test with
    
 ```bash
python ./<testScript>.py
```

here is the list of all the available scripts and a overview of what it does:
  1. basics.py: test the basic features (command execution, builtins etc)
  2. navigating.py: test the capacity to navigate through your file system  
  3. quoting.py: test the usage of quotes and how it modifies the behavior in a command
  4. redirection.py: test the redirection feature (forwarding the output of a command to a file)
  5. commandCompletion.py: test the self completion of commands when pressing TAB
  6. filenmeCompletion.py: test the self completion of directories and files when pressing TAB
  7. backgroundJobs.py: test the capacity of queue commands to keep using the program while queued commands keep running 
  8. pipelining.py: test the concatenation of commands using the previous output as argument
  9. history.py: test the command history
  10. historyPersistence.py: test the persistency across sessions

the usage of one of this scripts would look like this:
```bash
python ./basics.py
```

  Output should look like this:
  
  ```
  ==== OUTPUT ====
  src/pythonTestScripts $ echo hello
  hello 
  src/pythonTestScripts $ echo hello world
  hello world 
  src/pythonTestScripts $ type echo
  echo is a shell builtin
  src/pythonTestScripts $ type ls
  ls is /usr/bin/ls
  src/pythonTestScripts $ invalid_command_xyz
  invalid_command_xyz: command not found
  src/pythonTestScripts $ exit 0

  ==== EXPECTED OUTPUT ====
  hello
  hello world
  echo is a shell builtin
  ls is usr/bin/ls
  invalid_command_xyz: command not found
  No output, shell should exit cleanly

  Exit code: 0
  ```

  you can compare output with expected output and notice that are in fact the same
  


### 7.7 Troubleshooting
  1. Command not found for compiler tools:
    Install build tools package for your distro
  2. Build succeeds but shell does not run:
     Verify binary path and run from there
