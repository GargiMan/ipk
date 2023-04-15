/**
 * @file client.h
 * @author Marek Gergel (xgerge01)
 * @brief declaration of functions and variables for client side communication
 * @version 0.1
 * @date 2023-03-19
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "error.h"

#if defined(_WIN32) || defined(_WIN64) // windows

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#define close(a) (void)closesocket(a)
#define signal(a, b) SetConsoleCtrlHandler(b, true)
#define socklen_t int
#define SIGERR 0
#define SIGINT CTRL_C_EVENT

#else // unix

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#endif // _WIN32 || _WIN64

// message can be max 255 characters long due to protocol definition
#define BUFFER_SIZE 255
// max number of failed transfers before client exits
#define MAX_TRANSFER_FAILS 10

// allowed modes of communication
#define MODE_UDP 1
#define MODE_TCP 2

void client_init(char *host, int port, int mode);
void client_close();
void get_response(char *request, char *response);

#endif // CLIENT_H