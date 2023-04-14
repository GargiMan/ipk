/**
 * @file client.c
 * @author Marek Gergel (xgerge01)
 * @brief definition of functions and variables for client side communication
 * @version 0.1
 * @date 2023-03-19
 */

#include "client.h"

// op codes
#define OP_REQ 0
#define OP_RESP 1

// status codes
#define STATUS_OK 0
#define STATUS_ERR 1

int client_mode;
int client_socket;
struct sockaddr_in server_address;
bool server_closed = false;
bool server_opened = false;

/**
 * @brief Signal handler for SIGINT
 */
void sig_handler()
{
    client_close();
    exit(0);
}

/**
 * @brief Initialize the client socket and the connection with the server
 * @param host server host
 * @param port server port
 * @param mode communication mode
 */
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

    if ((client_socket = socket(AF_INET, (client_mode == MODE_TCP ? SOCK_STREAM : SOCK_DGRAM), 0)) <= 0)
    {
        error_exit(socketError, "Socket creation failed\n");
    }

    if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
        error_exit(socketError, "Socket connection failed\n");
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        error_exit(signalError, "Signal handler registration failed\n");
    }
}

/**
 * @brief Close the client socket and the connection with server
 */
void client_close()
{
    if (client_mode == MODE_TCP && !server_closed && server_opened)
    {
        server_closed = true;
        char response[BUFFER_SIZE] = "";
        get_response("BYE\n", response);
        printf("%s", response);
    }
    close(client_socket);
}

/**
 * @brief Get the tcp response message
 * @param request message to send
 * @param response message received
 */
void get_tcp_response(char *request, char *response)
{
    // Add newline character if missing
    if (request[strlen(request) - 1] != '\n')
    {
        request[strlen(request)] = '\n';
    }

    // Send request to server
    int send_fails = 0;
    while (send(client_socket, request, strlen(request), 0) == -1)
    {
        if (++send_fails >= MAX_TRANSFER_FAILS)
        {
            client_close();
            error_exit(transferError, "Send failed %d times\n", send_fails);
        }
        warning_print("Send failed\n");
    }

    // Receive response from server
    int recv_fails = 0;
    while (recv(client_socket, response, BUFFER_SIZE, 0) == -1)
    {
        if (++recv_fails >= MAX_TRANSFER_FAILS)
        {
            client_close();
            error_exit(transferError, "Receive failed %d times\n", recv_fails);
        }
        warning_print("Receive failed\n");
    }

    // Check if HELLO was recieved
    if (strcmp(response, "HELLO\n") == 0)
    {
        server_opened = true;
    }
    // Check if BYE was recieved
    if (strcmp(response, "BYE\n") == 0)
    {
        server_closed = true;
    }
}

/**
 * @brief Get the udp response message
 * @param request message to send
 * @param response message received
 */
void get_udp_response(char *request, char *response)
{
    socklen_t server_adress_len = sizeof(server_address);

    char request_packet[BUFFER_SIZE + 2] = "";
    char response_packet[BUFFER_SIZE + 3] = "";

    // Remove newline character if present
    if (request[strlen(request) - 1] == '\n')
    {
        request[strlen(request) - 1] = '\0';
    }

    // Build request packet
    sprintf(request_packet, "%c%c%s", OP_RESP, (char)strlen(request), request);
    size_t request_packet_len = strlen(request_packet);
    request_packet[0] = OP_REQ;

    // Send request to server
    int send_fails = 0;
    while (sendto(client_socket, request_packet, request_packet_len, 0, (struct sockaddr *)&server_address, server_adress_len) == -1)
    {
        if (++send_fails >= MAX_TRANSFER_FAILS)
        {
            client_close();
            error_exit(transferError, "Send failed %d times\n", send_fails);
        }
        warning_print("Send failed\n");
    }

    // Receive response from server
    int recv_fails = 0;
    while (recvfrom(client_socket, response_packet, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, &server_adress_len) == -1)
    {
        if (++recv_fails >= MAX_TRANSFER_FAILS)
        {
            client_close();
            error_exit(transferError, "Receive failed %d times\n", recv_fails);
        }
        warning_print("Receive failed\n");
    }

    // Read response packet
    char op_code = 0;
    char status_code = 0;
    char length = 0;
    sscanf(response_packet, "%c", &op_code);
    sscanf(response_packet + 1, "%c", &status_code);
    sscanf(response_packet + 2, "%c", &length);

    // Check response packet
    if (op_code != OP_RESP)
    {
        warning_print("Invalid response packet\n");
    }

    // Read response message
    if (status_code != STATUS_OK)
    {
        strcat(response, "ERR:");
        strcat(response, response_packet + 3);
        strcat(response, "\n");
    }
    else
    {
        strcat(response, "OK:");
        strcat(response, response_packet + 3);
        strcat(response, "\n");
    }
}

/**
 * @brief Get the response message from server based on client mode
 * @param request message to be sent to server
 * @param response message received from server
 */
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