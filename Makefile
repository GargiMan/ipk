# Makefile IPK Project
# Marek Gergel (xgerge01)

LOGIN = xgerge01
PROG_NAME = ipkcpc
# -g for debug , -O2 for optimization (0 - disabled, 1 - less, 2 - more)
CCFLAGS := -g -O2 -Wall -Wextra -std=c17 -pedantic
HEAD_FILES := client.h error.h
SRC_FILES := client.c error.c main.c
OBJ_FILES := client.o error.o main.o
TESTS := $(wildcard testData/*)

# -lws2_32 for windows sockets
ifeq ($(OS),Windows_NT)
  	CCFLAGS+= -lws2_32
endif

.PHONY: all program clean zip

all: program

%.o: %.c
	gcc $(CCFLAGS) -c $< -o $@

program: $(OBJ_FILES)
	gcc $(CCFLAGS) $^ -o $(PROG_NAME)

clean:
	rm -rf $(PROG_NAME)*
	rm -rf *.o

zip: clean
	zip -r xgerge01.zip *.h *.c testData Makefile README.md