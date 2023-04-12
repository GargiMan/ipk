# Makefile IPK Project 2
# Marek Gergel (xgerge01)

# test server host and port
HOST := localhost
PORT := 2023

LOGIN := xgerge01
PROG_NAME := ipkcpd
# -g for debug , -O2 for optimization (0 - disabled, 1 - less, 2 - more)
CCFLAGS := -O2 -Wall -Wextra -std=c17 -pedantic
SRC_FILES := $(wildcard *.c)
TESTS_TCP := $(wildcard testData/tcp*.in)
TESTS_UDP := $(wildcard testData/udp*.in)

# -lws2_32 for windows sockets
ifeq ($(OS),Windows_NT)
  	CCFLAGS += -lws2_32
endif

.PHONY: program test-tcp test-udp clean zip

$(PROG_NAME): $(SRC_FILES)
	gcc $(CCFLAGS) $(SRC_FILES) -o $(PROG_NAME)

listen-tcp:
	./$(PROG_NAME) -h $(HOST) -p $(PORT) -m tcp

listen-udp:
	./$(PROG_NAME) -h $(HOST) -p $(PORT) -m udp

test-tcp:
	@for test in $(TESTS_TCP:%.in=%); do netcat $(HOST) $(PORT) <$$test.in >$$test.out; if diff -q $$test.out $$test.ref >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi done

test-udp:
	@for test in $(TESTS_UDP:%.in=%); do netcat $(HOST) $(PORT) -u <$$test.in >$$test.out; if diff -q $$test.out $$test.ref >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi done

clean:
	rm -rf $(PROG_NAME)
	rm -rf testData/*.out
	rm -rf $(LOGIN).zip

zip: clean
	zip -r $(LOGIN).zip *.h *.c testData Makefile README.md CHANGELOG.md LICENSE