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
8. [Releases and Downloadable Artifacts](#8-releases-and-downloadable-artifacts)

## Project Origin

Ghost in the shell is born out of curiosity about how OS internally work, impulsed by my recent interest in cibersecurity and systems explotation, the program is also thought to have a vulnerable verstryinion that can be easily exploited in systems as metasploitable or similar, granting another way to practice security system by re-parching the vulnerable shell to a most stable version with less risk

## 1. Purpose and Architecture

Main pipeline:

1. Prompting the user.
2. Receive input .
3. Tokenize the input via parsing (similar to how argc and argv work in a main function) and sanitize.
4. search for argv[0] (command) either in PATH or builtin.
5. Execute command passing the other elements in argv as arguments.

Modules:

- `definitions.h` : general definitions
- `commands.*` : implementation of builtin and external commands  
- `arguments.*` : handles arguments parsing related logic 
- `selfCompletion.*` : implementation of selfCompletion with tab
- `history.*` : logic of the history builtin
- `proccesess.*` : handles different signals and behavior of the program
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
- `errno.h`
- `sys/wait.h`
- `sts/stat.h`
- `signal.h`
- `dirent.h` 
- `limits.h`
- `termios.h`
- `stddef.h`

### 2.3 Internal module dependencies

- `main.c` depends on: `definitions.h`, `proccesess.h`,`pipeline.h`,`utils.h`,`arguments.h`, `selfCompletion.h`, `history.h`, `commands.h`
- `history.c` depends on: `history.h`, `errno.h`
- `utils.c` depends on :`fcntl.h`, `utils.h`
- `arguments.c` depends on: `arguments.h`,`string.h`
- `commands.c` depends on: `commands.h`, `proccesess.h`, `definitions.h`
- `selfCompletion.c` depends on: `definitions.h`, `selfCompletion.h`
-`proccesess.c` depends on: `proccesess.h`, `commands.h`, `signal.h`

## 3. Public API (headers)

This section lists all functions declared in headers (expected API surface).

### 3.1 `arguments.h`

- `void argumentCounter(char *userInput, int* argumentCount);`
  - Counts the arguments given in the prompt based in the spaces, if part of the string is quoted then counts as just one argument.
- `void argumentExtractor(char *userInput, int argumentCount);`
  - based in argument count, tokenize the arguments and stores them to a argument vector variable
- `bool toogleState(bool state);`
  - utility function for is quoted logic (used in a primitive version) no longer in use for most code base
- `void expandGlobs(char *argv[]);`
    - expands * and ? operators  
- `bool toogleQuotes(bool activeQuotes);`
    -   change the boolean activeQuotes 
- `void removeQuotes(char *token);`
    -   remove quotes of input to clean up the iterated command
- `void restoreSpaces(char *userInput);`
    -   change the placeholder back to blank spaces  
- `void spacesInQuotes(char *userInput);`
    - change blank spaces inside quotes for a placeholder to operate over the quoted input
- `void expandArguments(char *argv[]);`
    - function that expands arguments marked with $ operator

### 3.2 `commands.h`

- `int history(char **current, char *historyBuffer[], const redirectConfig *redirect);`
  - Loads and show history
- `int pwd(const redirectConfig *redirect);`
  - Print current working directory
- `int cd(char **current, const redirectConfig *redirect);`
  - Move in your directories
- `int echo(char **current, const redirectConfig *redirect);`
  - outputs the aguments you give them
- `int jobs(char **current, const redirectConfig *redirect);`
  - Show queued jobs and their state
- `int type(char **current, const redirectConfig *redirect);`
  - shows if a command is builtin or system binary
- `int executeBin(bool toBackground, const redirectConfig *redirect , char *tokens[]);`
    - Finds and execute the binary you give provide, if the binary is not found shows an error

### 3.3 `history.h`


- `void addHistory(char *command, int *historyCount, char *historyBuffer[]);`
  - Adds the command just used to history

- `void dumpHistory(char *historyBuffer[]);`
  - Opens the history file and dumps the session history buffer

- `int getHistory(char *historyBuffer[]);`
  - reads the history file and loads previous sessions commands to current session buffer

- `bool expandHistory(char userInput[], size_t userInputSize, int historyCount, char *historyBuffer[]);`
    - expand for the program special operators such as !n and !!

### 3.5 `selfCompletion.h`

- `bool startCommandsList(availableCommands *list);`
  - Initialice the structure to store posible commands.
- `void commandsFree(availableCommands *list);`
  - frees the commands list when done.
- `bool commandListGrow(availableCommands *list);`
  - Grows the list capacity as the program need it.
- `bool commandListAdd(availableCommands *list, char *command);`
  - Adds commands to the list.
- `int compareCommands(const void *a, const void *b);`
  - uitlity function to compare commands.
- `void commandListSanitize(availableCommands *list);`
  - Sorts and deletes duplicated commands.
- `bool getBuiltIns(availableCommands *list);`
  - Adds builtIns to the list.
- `bool getBins(availableCommands *list);`
  - Adds all the binaries in the system PATH to command list.
- `bool fillCommands(availableCommands *list);`
  - fills the commands list with sanitized commands
- `size_t lengestCommonPrefix(char **matches, size_t count);`
  - Computes the longest Common prefix in stdin 
- `size_t prefixMatches(availableCommands *list, char *prefix, char ***matches);`
  - looks for commands that match the prefix
- `bool startWith(char *word, char *prefix);`
  - Verifies if a command start with the given prefix.
- `size_t fileMatches(char *prefix, char ***matches);`
  - looks for files or directories that match the prefix
- `void readLineTab(char *prompt, availableCommands *list, char *out, size_t outSize, int *historyCount, char *historyBuffer[]);`
  - reads stdin, allows self completion with TAB and movement with arrows  
- `void redraw(char *prompt, char *buf, size_t cursorPosition);`
  - Redraws stdin matching the modified buffer
- `void disableRaw(void);`
  - Reset to std config of the terminal.
- `bool enableRaw(void);`
  - enable Raw terminal mode to read input as soon as is registered in stdin.

### 3.6 `utils.h`

- `bool isOperator(char *token);`
    - toogle if current token is a operator (|, & etc)  
`void createPrompt(char *prompt, size_t promptSize);`
    - renders the prompt that you see while using the terminal
`int getFileDescriptor(const char* descriptorTarget, int flags);`
    - wrapper for open syscall that gets a file descriptor with standarized permits
`char* getPath(char *command);`
    - gets the path of any binary in system's PATH
`void historyBufferFree(char *historyBuffer[]);`
    - Cleans up the history buffer

### 3.7 `definitions.h`
    - definitions for general use amongst the program
    
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

  1. Autocompletion is first token only, does not have any class of argument-aware/flags completion
  2. no config system
  3. The program does not consider specific cases and states that the terminal can take
  4. Shell will only work in linux enviroments
  

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

## 8. Releases and Downloadable Artifacts

### 8.1 Using GitHub Releases

If you prefer downloading a packaged version instead of cloning directly, you can use the GitHub Releases page.

1. Open the repository Releases section.
2. Download one of the release assets:
   - `Ghost-In-The-Shell-vX.Y.Z.zip`
   - `Ghost-In-The-Shell-vX.Y.Z.tar.gz`
   - or the default GitHub source archives (`Source code (zip)` / `Source code (tar.gz)`) when versioned assets are not available.
3. Extract the archive.
4. Move into the extracted project directory.
5. Build and run as usual:

```bash
make
./shell
```
