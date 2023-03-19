# Makefile IPK Project
# Marek Gergel (xgerge01)

LOGIN = xgerge01
PROG_NAME = ipkcp
# -g for debug , -O2 for optimization (0 - disabled, 1 - less, 2 - more)
CCFLAGS := -g -O0 -Wall -Wextra -std=c17 -pedantic
SRC_FILES := $(wildcard *.c)
HEADER_FILES := $(wildcard *.h)
#TODO check on test server
OBJ_FILES := $(patsubst %.c,%.o,$(SRC_FILES))
TESTS := $(wildcard testData/*)

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