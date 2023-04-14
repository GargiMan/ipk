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

# -lws2_32 for windows sockets
ifeq ($(OS),Windows_NT)
  	CCFLAGS += -lws2_32
endif

.PHONY: program listen-tcp listen-udp clean zip

$(PROG_NAME): $(SRC_FILES)
	gcc $(CCFLAGS) $(SRC_FILES) -o $(PROG_NAME)

listen-tcp:
	./$(PROG_NAME) -h $(HOST) -p $(PORT) -m tcp

listen-udp:
	./$(PROG_NAME) -h $(HOST) -p $(PORT) -m udp

clean:
	rm -rf $(PROG_NAME)
	rm -rf $(LOGIN).zip

zip: clean
	zip -r $(LOGIN).zip *.h *.c Makefile README.md CHANGELOG.md LICENSE