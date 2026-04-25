# DVshell - DAMN Vulnerable Shell

> A linux shell intentionally vulnerable to practice cybersecurity, pentesting and anlysis of C code

---

## Disclaimer

This code contains intentionally introduced vulnerabilities. DO NOT, BY ANY MEANS use this program in production neither expose it to the internet. Use it just in your own, controlled local environment

---

## But, what is this?

DVShell is a vulnerable version of [GIshell](https://github.com/lisardowo/Ghost-In-The-Shell.git), my own linux shell written from scratch. As you should already noticed `main` branch contains the good, patched and audited code. The branch you are currently in (`DamnVulnerableShell`) I on purpose took some steps backs in this patches in order to introduce vulnerabilities organized by difficulty/expertise needed.

The objective is that YOU, user, can practice real techniques of exploitation/pentesting in a production environment. No abstract exercises, no artificial vulnerabilites just actual, simple, bugs that real software can have (most of the vulnerabilities here are some bugs that I had while developing the original branch)

---

## Setup 

### Dependencies

 * makefile
 * gcc
 * gdb
 * valgrind

### recommended for level 2+

 * python
 * pwntools
 * checksec

```bash
# Clone vulnerable version
git clone -b DamnVulnerableShell --single-branch <https://github.com/lisardowo/Ghost-In-The-Shell>

cd Ghost-In-The-Shell
```

### Compiling

```bash

make easy

# No protection - if you're new to pentesting, this is where you start

make hard

# ASLR active, no canary - You feel comfortable with the problems?, then is moment to be a big boy

make expert

# All modern protection - You want some real challenges? try breaking code compiled with all modern securities

make verify

# This is an utility module to see protection levels of each binary 

```

## How to play?

The program is a way to practice your cybersec skills, yes, but is also a fun game. To keep it like that heres how I intended this to work:

### Issues

in the github page theres a lot of issues, each issue represents a vulnerabilty, the issue will have 3 tags and its state (open/closed)

|vuln|type of vulnerabilty|level|state|
|----|---------------------|-----|----|
|This tells you if is a vulnerability to test or any other kind of issue|What is the recommended technique to exploit the program (uaf, TOCTOC, etc)|the level recommended to achieve the challenge|Open means no one has obtained the flag yet, closed means someone found the flag, the vulnerability is still there tho so if you find yourself stuck.. well you know who to ask!|

Each issue contains hints and recommendations if you feel stuck, yet the answer wont be displayed there. Why? well, i thought the issues as a place to talk, to suggest answers, techniques and ask for help, there people can make comunnity and feedback other learners (please, be kind !!). (if you still want the answer well, dont worry, is normal be stuck and I don't judge. Please consult [I want to cheat section](#i-want-to-cheat), don't be ashamed of the name, is not cheating I just found it funny to call it that)

If you want to contribute to this project please refeer to [contributing](CONTRIBUTING.md)

### How to win? - FLAGS

If a exploit effectively worked, the shell prints to stdout a flag similar to:
```
DVShell{VULN01_st4ck_0v3rfl0w_pwn3d}
```

this means that you achieved it, good job!

### I win, then what?

If you achieved the flag, heres some things you can do:

 1. First of all, please take a look at the issues section of this repo and upload your [win report](templates/reportWin_template.md) to the vulnerability you exploited so other people can understand your work and maybe use it as reference if they're stuck
 2. Then, if you want, you can fork the repo and try to patch the bug, so you can have a more secure version

--- 
## Vulnerabilities

Behind is all known vulnerabilites to this day, if you found another and want to contribute, please refeer to [contributing](CONTRIBUTING.md)

| ID | Name | Dificulty | Type | Issue |
|----|--------|-----------|-----------|-------|
| VULN-01 | small buffers and adjacency | easy | Stack overflow | #1 |
| VULN-02 | Readding from the stack | easy | Format string | #2 |
| VULN-03 | shell metacharacter injection | easy | Logic bug | #3 |
| VULN-04 | flying flag | easy | Logic bug | #4 |
| VULN-05 | Permissive | easy | Permissions | #5 |
| VULN-06 | Off-by-one in history | easy | Heap overflow | #6 |
| VULN-07 | Integer overflow in tokenizer | easy | Integer overflow | #7 |
| VULN-08 | Use-After-Free in glob expansion | easy | UAF | #8 |
| VULN-09 | Double-Free | easy | Double-free | #9 |
| VULN-10 | Traversal Path in redirection | easy | Path traversal | #10 |
| VULN-11 | TOCTOU in history | easy | Race condition | #11 |
| VULN-12 | Heap overflow in expansion of environment variables | easy | Heap overflow | #12 |
| VULN-13 | Null pointer dereference in pipeline | easy | Null deref | #13 |
| VULN-14 | Format string with arbitrary structure | hard | Format string | #14 |
| VULN-15 | Heap overflow in autocompletion | hard | Heap overflow | #15 |
| VULN-16 | Stack exhaustion due recursive expansion | hard | Stack overflow | #16 |
| VULN-17 | Tcache poisoning via double-free | expert | Heap exploit | #17 |
| VULN-18 | Arbitrary file write via symlink | hard | Symlink attack | #18 |
| VULN-19 | Race condition in background jobs | hard | Race condition | #19 |
| VULN-20 | ROP entry point | expert | Stack overflow | #20 |
| VULN-21 | Info leak + Arbitrary write | expert | Concadenation | #21 |
| VULN-22 | Heap grooming + UAF | expert | Concadenation | #22 |
| VULN-23 | Full chain — shell escape | expert | Concadenation | #23 |

---

## Is there a roadmap?

There is not, you can go ahead and do whatever vulnerability you feel like it, but if you ask me, I recommend somethings like this: 

```
make easy  →  VULN-01  →  VULN-02  →  VULN-03  →  VULN-04  →  VULN-05
↓
make hard  →  VULN-06  →  VULN-07  →  VULN-08  →  VULN-09  →  VULN-10 →  VULN-11  →  VULN-12  →  VULN-13
↓
make expert →  VULN-14  →  VULN-15  →  VULN-16  →  VULN-17 →  VULN-18 →  VULN-19  →  VULN-20  →  VULN-21  →  VULN-22  →  VULN-23
```
---

## Suggested Tools

| Tool | Why? |
|------|------|
|gdb + pwndbg | debugging and memory inspection|
| valgrind | detect UAF, double-free, leaks |
| pwntools | write exploits in python |
| checksec | see bin protections |
| ltrace / strace | trace calls yo libs and syscalls |
| objdump / realdelf | static binary analysis |

---



## Some resources

Still not sure how to start? don't worry, Ill leave some resources I found that could be of your interest

- pwn.college - free courses of binary exploitation
- liveOverflow - real exploits in video
- how2heap - heap exploitation techniques
- GDB cheatsheet
- pwntools docs

## I want to cheat

Hey, dont worry if you feel that you are stuck is normal, theres no shame in needing some help! each issue has hints and if you really feel like you cant, well then you can [email me](mailto:pkgnn@proton.me) for the file with the answer and a diff patch ! I wont judge and will be more than pleasured to explain why this works like that.