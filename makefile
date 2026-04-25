
CC     = gcc
SRC    = main.c commands.c proccesess.c \
         arguments.c history.c utils.c \
         selfCompletion.c
FLAGS_COMMON = -g -O0 -w -Wno-format-security \
               -Wno-deprecated-declarations


# easy — has all protections of the compiler deactivated
# Objective: understand how bugs work
# Here you can practice: shellcode, simple buffer overflows, basic format strings , UAF, double-free

easy: $(SRC)
	$(CC) $(FLAGS_COMMON) $(SRC) -o dvshell_easy -fno-stack-protector -z execstack -no-pie -D DVSHELL_LEVEL=1
	@echo ""
	@echo "  [DVShell EASY] Succesfully compiled without protections"
	@echo "  Active protections: None"
	@echo "  Suggested tecniques: shellcode, buffer overflow, format string"
	@echo ""

# ==========================================================

# hard — ASLR active, no canary, non executable stack
# OBjective: learn info leaks & ret2libc
# Here you should try: format string to read addresses, ret2libc, heap grooming
# Note: ASLR is activated by the OS — this target only deactivate PIE so the binary itself is predictable even tho libc is not

hard: $(SRC)
	$(CC) $(FLAGS_COMMON) $(SRC) -o dvshell_hard -fno-stack-protector -z noexecstack -no-pie -D DVSHELL_LEVEL=2
	@echo ""
	@echo "  [DVShell HARD] Succesfully  compiled with basics protection"
	@echo "  Active protections: ASLR (OS), no-exec stack"
	@echo "  Suggested tecniques: info leak, ret2libc, heap exploitation"
	@echo ""

# ===============================================================

# expert — All modern protection are active
# Objetivo: realist program exploitation
# Techniques: ROP chains, canary leak, PIE bypass via info leak, tcache poisoning

expert: $(SRC)
	$(CC) $(FLAGS_COMMON) $(SRC) -o dvshell_expert -fstack-protector-strong -z noexecstack -pie -fPIE -D DVSHELL_LEVEL=3
	@echo ""
	@echo "  [DVShell EXPERT] Succesfully compiled with all protections"
	@echo "  Active protections: canary, ASLR, PIE, no-exec stack"
	@echo "  Suggested tecniques: ROP, canary leak, PIE bypass, modern heap"
	@echo ""

# =================================================================

# verify — Shows the protections of each binary
# Require: checksec (pip install checksec.sh)

verify:
	@echo " Verifying protections via checksec"
	@for bin in dvshell_easy dvshell_hard dvshell_expert gishell; do \
		if [ -f $$bin ]; then \
			echo "\n--- $$bin ---"; \
			checksec --file=$$bin; \
		fi \
	done

# ===================================================================

safe: $(SRC)
	$(CC) $(SRC) -o gishell -Wall -Wextra -Wshadow -fstack-protector-strong -z noexecstack -pie -fPIE -O2 -D DVSHELL_LEVEL=0
	@echo ""
	@echo "  [GIshell SAFE] Succesfully compilated with modern protections and optimization"
	@echo ""

clean:
	rm -f dvshell_easy dvshell_hard dvshell_expert gishell

.PHONY: easy hard expert verify safe clean