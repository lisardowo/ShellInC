CC = gcc
CFLAGS = -Wall -Wextra
SRC = main.c arguments.c commands.c utils.c history.c proccesess.c selfCompletion.c
OUT = shell



all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)