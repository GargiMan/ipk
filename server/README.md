# IPK Project 2: IPK Calculator Protocol

Server for IPK Calculator Protocol[2] with TCP and UDP support.

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

Client-server communication was tested with client project[1] and this server running with `make listen-tcp` and `make listen-udp` for TCP and UDP respectively.
Default test hostname and port of server are specified in `Makefile` of client/server.
Server code needs to be compiled before running `make listen-tcp` or `make listen-udp`.

## Source code

### Makefile

| Target                  | Description                                                  |
| ----------------------- | ------------------------------------------------------------ |
| `make` or `make ipkcpd` | compiles project                                             |
| `make listen-tcp`       | runs TCP server with specified host and port in Makefile     |
| `make listen-udp`       | runs UDP server with specified host and port in Makefile     |
| `make clean`            | removes compilation files, tests outputs and created archive |
| `make zip`              | creates zip archive of project                               |

### main.c

Program entry point. Parses arguments using `parse_args(int argc, char *argv[])` function.
All server communication protocol is handled in `run_server()` function.

### server.c, server.h

Server code for IPK Calculator Protocol.
Communication codes were adjusted from Gitea project repository[2].
Correct client-server communication is established using `server_init(char* host, char* port, int mode)`, request-response messages are transfered using function `server_listen()` and connection can be closed using `server_close()`. Server handles also signal `SIGINT` with correct connection closing.

### error.c, error.h

Files implements `warning_print(char *msg, ...)` and `error_exit(errorCodes_t errcode, char *msg, ...)` functions with message printing to `stderr` and error codes inside `errorCodes_t` structure.

## References

[1] [Client project](https://git.fit.vutbr.cz/xgerge01/ipk/src/branch/master/client)
[2] [Calculator protocol and example codes](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master)  
[3] [RFC 5234](https://www.rfc-editor.org/info/std68)  
[4] [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)  
[5] [Transmission Control Protocol](https://en.wikipedia.org/wiki/Transmission_Control_Protocol)  
[6] [User Datagram Protocol](https://en.wikipedia.org/wiki/User_Datagram_Protocol)
