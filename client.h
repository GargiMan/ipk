/**
 * @file client.h
 * @author Marek Gergel (xgerge01)
 * @brief declaration of functions and variables for client side communication
 * @version 0.1
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include "error.h"

#define BUFFFER_SIZE 255

#define MODE_UDP 1
#define MODE_TCP 2

void client_init(char *host, int port, int mode);
void client_close();
void get_response(char *request, char *response);

#endif // CLIENT_H