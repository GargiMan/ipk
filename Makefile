# Makefile IPK Project
# Marek Gergel (xgerge01)

# test server host and port
HOST := localhost
PORT := 2023

LOGIN := xgerge01
PROG_NAME := ipkcpc
# -g for debug , -O2 for optimization (0 - disabled, 1 - less, 2 - more)
CCFLAGS := -O2 -Wall -Wextra -std=c17 -pedantic
SRC_FILES := $(wildcard *.c)
TESTS_TCP := $(wildcard testData/tcp*.in)
TESTS_UDP := $(wildcard testData/udp*.in)

# -lws2_32 for windows sockets
ifeq ($(OS),Windows_NT)
  	CCFLAGS += -lws2_32
endif

.PHONY: all program test-tcp test-udp clean zip

all: program

%.o: %.c
	gcc $(CCFLAGS) -c $< -o $@

program: $(SRC_FILES:%.c=%.o)
	gcc $(CCFLAGS) $^ -o $(PROG_NAME)

test-tcp:
	@for test in $(TESTS_TCP:%.in=%); do ./$(PROG_NAME) -h $(HOST) -p $(PORT) -m tcp <$$test.in >$$test.out; if diff -q $$test.out $$test.ref >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi done

test-udp:
	@for test in $(TESTS_UDP:%.in=%); do ./$(PROG_NAME) -h $(HOST) -p $(PORT) -m udp <$$test.in >$$test.out; if diff -q $$test.out $$test.ref >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi done

clean:
	rm -rf $(PROG_NAME)
	rm -rf *.o
	rm -rf testData/*.out

zip: clean
	zip -r xgerge01.zip *.h *.c testData Makefile README.md