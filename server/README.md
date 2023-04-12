# IPK Project 2: IPK Calculator Protocol

Server for IPK Calculator Protocol[1] with TCP and UDP support.

## Usage

### Compilation

Project can be compiled on Unix based systems and Windows using `make` or `make ipkcpd`.

### Execution

Program can be run using `ipkcpd -h <host> -p <port> -m <mode>` or `ipkcpc --help` for help.  
Communication messages are read from `stdin` and written to `stdout`.
Program does not have any default values for host, port or mode and will exit with error if any of them is not specified.

During communication warning of failed operation can be printed to stderr.

### Arguments

| Switch      | Description                        | Required |
| ----------- | ---------------------------------- | -------- |
| `-h <host>` | Hostname or IPv4 address of server | yes      |
| `-p <port>` | Port of server                     | yes      |
| `-m <mode>` | Mode of communication              | yes      |
| `--help`    | Prints help and exits program      | no       |

### Exit codes

| Code | Description            |
| ---- | ---------------------- |
| 0    | Success                |
| 1    | Program argument error |
| 2    | Program input error    |
| 3    | Socket error           |
| 4    | Transfer error         |
| 5    | Signal handler error   |

## Tests

Client-server communication was tested with input/output tests from `testData/` directory. Tests can be run using `make test-tcp` or `make test-udp` for TCP and UDP respectively.  
Program needs to be compiled before running tests.

Output of tests is stored in `testData/` directory in files with `.out` extension, reference output with `.ref` extension and input with `.in` extension.
Reference output were created with `netcat` on reference environment.

Test output is printed to `stdout` in following format: `Test <State> : <Test file>`

| State | Description                                                |
| ----- | ---------------------------------------------------------- |
| OK    | Test passed                                                |
| FAIL  | Test failed, test output `.out` does not match with `.ref` |

## Source code

### Makefile

| Target                  | Description                                                  |
| ----------------------- | ------------------------------------------------------------ |
| `make` or `make ipkcpd` | compiles project                                             |
| `make test-tcp`         | runs TCP tests with specified host and port in Makefile      |
| `make test-udp`         | runs UDP tests with specified host and port in Makefile      |
| `make clean`            | removes compilation files, tests outputs and created archive |
| `make zip`              | creates zip archive of project                               |

### main.c

Program entry point. Parses arguments using `parse_args(int argc, char *argv[])` function.
All server communication protocol is handled in `run_server()` function.

### server.c, server.h

Server code for IPK Calculator Protocol.
Communication codes were adjusted from Gitea project repository[1].
Correct client-server communication is established using `server_init(char* host, char* port, int mode)`, request-response messages are transfered using function `get_request(char* response, char* request)` and connection can be closed using `server_close()`. Server handles also signal `SIGINT` and end of `stdin` with correct connection closing.

### error.c, error.h

Error handling functions created in another project for IJC.
Files implements `warning_print(char *msg, ...)` and `error_exit(errorCodes_t errcode, char *msg, ...)` functions with message printing to `stderr` and error codes inside `errorCodes_t` structure.

## References

[1] [Calculator protocol and example codes](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master)
