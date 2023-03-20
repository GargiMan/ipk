/**
 * @file client.h
 * @author Marek Gergel (xgerge01)
 * @brief definition of functions and variables for client side communication
 * @version 0.1
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "client.h"

#define OP_REQ 0
#define OP_RESP 1

#define STATUS_OK 0
#define STATUS_ERR 1

int client_mode;
int client_socket;
struct sockaddr_in server_address;

void sig_handler()
{
    char response[BUFFFER_SIZE] = "";
    get_response("BYE\n", response);
    printf("%s", response);
    // shutdown(client_socket, SHUT_RDWR);
    client_close();
    exit(0);
}

void client_init(char *host, int port, int mode)
{
    client_mode = mode;

    struct hostent *server;
    if ((server = gethostbyname(host)) == NULL)
    {
        error_exit(socketError, "Host '%s' not found\n", host);
    }

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

    // printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    if ((client_socket = socket(AF_INET, (client_mode == MODE_TCP ? SOCK_STREAM : SOCK_DGRAM), 0)) <= 0)
    {
        error_exit(socketError, "Socket creation failed.\n");
    }

    if (client_mode == MODE_TCP && connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
        error_exit(socketError, "Socket connection failed.\n");
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        error_exit(signalError, "Signal handler registration failed.\n");
    }
}

void client_close()
{
    close(client_socket);
}

void get_tcp_response(char *request, char *response)
{
    // Send request to server
    if (send(client_socket, request, strlen(request), 0) == -1)
    {
        warning_print("Send failed.\n");
    }

    // Receive response from server
    if (recv(client_socket, response, BUFFFER_SIZE, 0) == -1)
    {
        warning_print("Receive failed.\n");
    }
}

void get_udp_response(char *request, char *response)
{
    socklen_t serverlen = sizeof(server_address);
    char request_packet[BUFFFER_SIZE + 2] = "";
    char response_packet[BUFFFER_SIZE + 3] = "";

    request[strlen(request) - 1] = '\0'; // Remove newline character

    // Build request packet
    sprintf(request_packet, "%c%c%s", OP_RESP, (char)strlen(request), request);
    size_t request_packet_len = strlen(request_packet);
    request_packet[0] = OP_REQ;

    // Send request to server
    if (sendto(client_socket, request_packet, request_packet_len, 0, (struct sockaddr *)&server_address, serverlen) == -1)
    {
        warning_print("Send failed.\n");
    }

    // Receive response from server
    if (recvfrom(client_socket, response_packet, BUFFFER_SIZE, 0, (struct sockaddr *)&server_address, &serverlen) == -1)
    {
        warning_print("Receive failed.\n");
    }

    // Read response packet
    char op_code;
    char status_code;
    char length;
    sscanf(response_packet, "%c", &op_code);
    sscanf(response_packet + 1, "%c", &status_code);
    sscanf(response_packet + 2, "%c", &length);

    // Check response packet
    if (op_code != OP_RESP)
    {
        warning_print("Invalid response packet.\n");
    }

    // Read response message
    if (status_code != STATUS_OK)
    {
        warning_print("Server error.\n");
        strcat(response, "ERR: ");
        strcat(response, response_packet + 3);
        strcat(response, "\n");
    }
    else
    {
        strcat(response, "OK: ");
        strcat(response, response_packet + 3);
        strcat(response, "\n");
    }
}

void get_response(char *request, char *response)
{
    if (client_mode == MODE_TCP)
    {
        get_tcp_response(request, response);
    }
    else if (client_mode == MODE_UDP)
    {
        get_udp_response(request, response);
    }
}