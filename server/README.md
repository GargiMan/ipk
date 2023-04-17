# IPK Project 2: IPK Calculator Protocol

Server for IPK Calculator Protocol with TCP and UDP support.

## Teoretical background

### IPKCP

[IPK Calculator Protocol][2] is a simple protocol for communication between client and server calculator based on [RFC 5234][4].

### Client and server communication

[Client–server model][6] is a distributed application structure that partitions tasks or workloads between the providers of a resource or service, called servers, and service requesters, called clients. Often clients and servers communicate over a computer network on separate hardware, but both client and server may reside in the same system. A single overall application may have both client and server components, which may interact with each other locally.

Clients and servers exchange messages in a request–response messaging pattern. The client sends a request, and the server returns a response. This exchange of messages is an example of inter-process communication. To communicate, the computers must have a common language, and they must follow rules so that both the client and the server know what to expect. The language and rules of communication are defined in a communications protocol. All protocols operate in the application layer. The application layer protocol defines the basic patterns of the dialogue.

#### TCP

[Transmission Control Protocol][7] is a connection-oriented protocol. It is used for reliable communication between client and server. TCP is a stream protocol, which means that data is sent in a continuous stream of bytes. TCP is a connection-oriented protocol, which means that before any data can be sent, a connection must be established between the client and server. This connection is established using a three-way handshake. The client sends a SYN (synchronize) packet to the server. The server responds with a SYN-ACK (synchronize-acknowledge) packet. The client then sends an ACK (acknowledge) packet back to the server. Once the connection is established, the client and server can begin sending data to each other.

#### UDP

[User Datagram Protocol][8] is a connectionless protocol. It is used for unreliable communication between client and server. UDP is a datagram protocol, which means that data is sent in discrete chunks called datagrams. UDP is a connectionless protocol, which means that the client and server do not need to establish a connection before sending data to each other. This means that UDP is faster than TCP, but it is also less reliable. UDP is faster than TCP because it does not need to establish a connection before sending data. UDP is less reliable than TCP because it does not guarantee that the data will arrive at the destination. UDP is also less reliable because it does not guarantee that the data will arrive in the same order that it was sent.

### Socket, IP Address, Port

[Socket][9] is an endpoint for communication between two programs running on a network. A socket is a combination of an IP address and a port number. The IP address is the address of the computer that the socket is running on. The port number is a number that is used to identify the socket. The port number is used to identify the application that is running on the computer. The port number is used to identify the application that is running on the computer.

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

Client-server communication was tested with [client project][1] and this server running with `make listen-tcp` and `make listen-udp` for TCP and UDP respectively.
Default test hostname and port of server are specified in `Makefile` of client/server. For client part of testing see [client project][1].
Server code needs to be compiled before running `make listen-tcp` or `make listen-udp`.

For proof of correct server implementation of IPK Calculator Protocol, `netcat` was used for testing special cases.
Tests are bash script that can be run using `./testData/test-tcp.sh` or `./testData/test-udp.sh` and each test case can return `OK` or `FAIL`.  
Here are results of tests:

```
./testData/test-tcp.sh
TCP - LF: OK
TCP - no LF: OK
TCP - LF + no LF: OK
./testData/test-udp.sh
UDP - LF: OK
UDP - no LF: OK
```

Testing of correct socket closing was done only manually.

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

Server code for IPK Calculator Protocol that was adjusted from [example codes of IPK][3] and from [Beej's Guide to Network Programming][5].
Server side networking is initialized using function `server_init(char* host, char* port, int mode)`, afterwards server can start listening to incomming connections using `server_listen()` function. Server handles also signal `SIGINT` with correct socket closing which is handled in `server_close()` function.
Maximum allowed clients can be adjusted with `MAX_CLIENTS` macro in `server.h` file. Default value is 10.

For TCP was used `select()` function to handle multiple clients at the same time, function monitors all entered sockets and returns only sockets ready for reading (or writing) including server socket to add new connections.

### error.c, error.h

Files implements `warning_print(char *msg, ...)` and `error_exit(errorCodes_t errcode, char *msg, ...)` functions with message printing to `stderr` and error codes inside `errorCodes_t` structure.

<!-- References -->

[1]: https://git.fit.vutbr.cz/xgerge01/ipk/src/branch/master/client "Client project"
[2]: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/Protocol.md "IPK Calculator protocol"
[3]: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp "IPK Example codes"
[4]: https://www.rfc-editor.org/info/std68 "RFC 5234"
[5]: https://beej.us/guide/bgnet/html/ "Beej's Guide to Network Programming"
[6]: https://en.wikipedia.org/wiki/Client%E2%80%93server_model "Client-server model"
[7]: https://en.wikipedia.org/wiki/Transmission_Control_Protocol "Transmission Control Protocol"
[8]: https://en.wikipedia.org/wiki/User_Datagram_Protocol "User Datagram Protocol"
[9]: https://en.wikipedia.org/wiki/Network_socket "Network socket"
