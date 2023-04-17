/**
 * @file server.h
 * @author Marek Gergel (xgerge01)
 * @brief declaration of functions and variables for server side communication
 * @version 0.1
 * @date 2023-04-12
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <ctype.h>
#include "error.h"

#if defined(_WIN32) || defined(_WIN64) // windows

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <io.h>
// regex library for windows needs to be downloaded and linked

#define close(a) (void)closesocket(a)
#define signal(a, b) SetConsoleCtrlHandler(b, true)
#define socklen_t int
#define SIGERR 0
#define SIGINT CTRL_C_EVENT

#else // unix

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <regex.h>

#define SO_REUSEPORT 15

#endif // _WIN32 || _WIN64

// message can be max 255 characters long due to protocol definition
#define BUFFER_SIZE 255
// max clients that can be connected at once
#define MAX_CLIENTS 10

// allowed modes of communication
#define MODE_UDP 1
#define MODE_TCP 2

void server_init(char *host, int port, int mode);
void server_listen();
void server_close();

#endif // SERVER_H