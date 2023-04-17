/**
 * @file server.c
 * @author Marek Gergel (xgerge01)
 * @brief definition of functions and variables for server side communication
 * @version 0.1
 * @date 2023-04-12
 */

#include "server.h"

// op codes
#define OP_REQ 0
#define OP_RESP 1

// status codes
#define STATUS_OK 0
#define STATUS_ERR 1

// operator - "+" / "-" / "*" / "/"
// tcp = 10 (SOLVE SP "("operator")"LF), udp = 3 "("operator")"
// div 2 - operand takes at least 2 bytes (SP 1*DIGIT)
#define MAX_OPERANDS (BUFFER_SIZE - (server_mode == MODE_TCP ? 10 : 3)) / 2

int server_mode;
int server_socket;
int client_socket[MAX_CLIENTS];
bool client_opened[MAX_CLIENTS];
int num_clients = 0;
int i = 0;
fd_set read_fds;
struct sockaddr_in address;
socklen_t address_len = sizeof(address);

/**
 * @brief Signal handler for SIGINT
 */
void sig_handler()
{
    server_close();
    exit(0);
}

/**
 * @brief Calculate the expression and stores the result or error message in the result parameter
 * @param expression expression to calculate
 * @param result result of the calculation or error message
 * @return int 0 if success, 1 if error
 */
int calculate(char *expression, char *result)
{
    // parse expression
    regex_t regex;
    regmatch_t match[5];

    (void)regcomp(&regex, "^[(](([+]|-|[*]|[/])(( [0-9]+| [(].*?[)]){2,}))[)]$", REG_EXTENDED);

    if (regexec(&regex, expression, 6, match, 0))
    {
        regfree(&regex);
        strcpy(result, "Could not parse the message");
        return 1;
    }
    regfree(&regex);

    // parse operator
    char operator= expression[match[2].rm_so];

    // parse operands
    int num_operands = 0;
    int operands[MAX_OPERANDS];
    bzero(operands, sizeof(operands));

    int nested = 0;
    int start = match[3].rm_so + 1;
    for (int end = start; end < match[3].rm_eo; end++)
    {
        switch (expression[end])
        {
        case '(':
            nested++;
            break;
        case ')':
            nested--;
            if (nested == 0)
            {
                end++;
                char expr[BUFFER_SIZE];
                bzero(expr, sizeof(expr));
                bzero(result, sizeof(result));
                strncpy(expr, expression + start, end - start);
                if (calculate(expr, result))
                {
                    return 1;
                }
                operands[num_operands++] = atoi(result);
                start = end + 1;
            }
            break;
        default:
            if ((expression[end] == ' ' || end + 1 == match[3].rm_eo) && nested == 0)
            {
                bzero(result, sizeof(result));
                strncpy(result, expression + start, (end + 1 == match[3].rm_eo ? end + 1 : end) - start);
                operands[num_operands++] = atoi(result);
                start = end + 1;
            }
            break;
        }
    }

    // evaluate expression
    int result_i = operands[0];
    for (int i = 1; i < num_operands; i++)
    {
        switch (operator)
        {
        case '+':
            result_i += operands[i];
            break;
        case '-':
            result_i -= operands[i];
            if (result_i < 0)
            {
                strcpy(result, "Could not parse the message");
                return 1;
            }
            break;
        case '*':
            result_i *= operands[i];
            break;
        case '/':
            if (operands[i] == 0)
            {
                strcpy(result, "Could not parse the message");
                return 1;
            }
            result_i /= operands[i];
            break;
        }
    }

    snprintf(result, BUFFER_SIZE, "%d", result_i);
    return 0;
}

/**
 * @brief Validates the request and stores the response in the response parameter according to the protocol
 * @param request request to validate
 * @param response response to the request
 * @return int 1 if error or connection should be closed after the response, 0 otherwise
 */
int calculator_protocol(char *request, char *response)
{
    // printf("C(%d): %s%c", i, server_mode == MODE_TCP ? request : request + 2, request[strlen(request) - 1] == '\n' ? '\0' : '\n');

    int status = STATUS_OK;

    // convert to uppercase
    for (int i = 0; request[i]; i++)
    {
        request[i] = toupper(request[i]);
    }

    if (server_mode == MODE_TCP)
    {
        if (strcmp("HELLO\n", request) == 0 && !client_opened[i])
        {
            client_opened[i] = true;
            strcpy(response, "HELLO\n");
            return 0;
        }
        else if (strncmp("SOLVE ", request, 6) == 0 && request[strlen(request) - 1] == '\n')
        {
            request = request + 6;
            request[strlen(request) - 1] = '\0';
            char result[BUFFER_SIZE] = "";
            if ((status = calculate(request, result)))
            {
                strcpy(response, "BYE\n");
                return status;
            }
            sprintf(response, "RESULT %s\n", result);
            return status;
        }
        else
        {
            strcpy(response, "BYE\n");
            return 1;
        }
    }
    else
    {
        char result[BUFFER_SIZE] = "";
        if (request[0] != OP_REQ || request[1] != (char)strlen(request + 2))
        {
            status = 1;
            strcpy(result, "Could not parse the message");
        }
        else
        {
            status = calculate(request + 2, result);
        }

        response[0] = OP_RESP;
        response[1] = status;
        sprintf(response + 2, "%c%s", (char)strlen(result), result);
        return status;
    }
}

/**
 * @brief Closes the client socket
 * @param index index of the client socket in the client_socket array
 */
void client_close(int index)
{
    close(client_socket[index]);
    FD_CLR(client_socket[index], &read_fds);
    for (int i = index; i < num_clients - 1; i++)
    {
        client_socket[i] = client_socket[i + 1];
        client_opened[i] = client_opened[i + 1];
    }
    num_clients--;
    i--;
}

/**
 * @brief Initializes the server socket
 * @param host server host name or IPv4 address
 * @param port server port number
 * @param mode communication mode, allowed modes are 'udp' or 'tcp'
 */
void server_init(char *host, int port, int mode)
{
    server_mode = mode;

    struct hostent *server;
    if ((server = gethostbyname(host)) == NULL)
    {
        error_exit(socketError, "Host '%s' not found\n", host);
    }

    bzero((char *)&address, address_len);
    address.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&address.sin_addr.s_addr, server->h_length);
    address.sin_port = htons(port);

    if ((server_socket = socket(AF_INET, server_mode == MODE_TCP ? SOCK_STREAM : SOCK_DGRAM, 0)) < 0)
    {
        error_exit(socketError, "Socket creation failed\n");
    }

    int opt = 1;
    int opts = SO_REUSEADDR | SO_REUSEPORT;
    if (setsockopt(server_socket, SOL_SOCKET, opts, &opt, sizeof(opt)))
    {
        error_exit(socketError, "Socket options change failed\n");
    }

    if (bind(server_socket, (struct sockaddr *)&address, address_len) < 0)
    {
        error_exit(socketError, "Socket binding to address failed\n");
    }

    if (server_mode == MODE_TCP && listen(server_socket, MAX_CLIENTS) < 0)
    {
        error_exit(socketError, "Socket listening failed\n");
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        error_exit(signalError, "Signal handler registration failed\n");
    }
}

/**
 * @brief Closes the server socket and all client sockets
 */
void server_close()
{
    if (server_mode == MODE_TCP)
    {
        for (i = 0; i < num_clients; i++)
        {
            if (client_opened[i])
            {
                char response[BUFFER_SIZE] = "BYE\n";
                send(client_socket[i], response, strlen(response), 0);
                client_close(i);
            }
        }
    }
    close(server_socket);
}

/**
 * @brief Listens for incoming TCP packets and handles them
 */
void server_listen_tcp()
{
    while (true)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds); // add server socket to the set
        int max_fd = server_socket;
        int current_socket;

        for (i = 0; i < num_clients; i++)
        {
            // socket descriptor
            int current_socket = client_socket[i];

            // if valid socket descriptor then add to read list
            if (current_socket > 0)
                FD_SET(current_socket, &read_fds);

            // highest file descriptor number, need it for the select function
            if (current_socket > max_fd)
                max_fd = current_socket;
        }

        // wait for socket activity
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
            error_exit(socketError, "Socket select failed\n");
        }

        // handle new connections
        if (FD_ISSET(server_socket, &read_fds))
        {
            if ((current_socket = accept(server_socket, (struct sockaddr *)&address, &address_len)) < 0)
            {
                error_exit(socketError, "Socket accept failed\n");
            }

            // add new current_socket to client_sockets
            if (num_clients < MAX_CLIENTS)
            {
                client_socket[num_clients] = current_socket;
                client_opened[num_clients] = false;
                num_clients++;
            }
            else
            {
                warning_print("Maximum number of clients reached\n");
                close(current_socket);
            }
        }

        // handle clients
        for (i = 0; i < num_clients; i++)
        {
            // socket not ready
            if (!FD_ISSET(client_socket[i], &read_fds))
            {
                continue;
            }

            char request[BUFFER_SIZE] = "";

            // handle client
            if (recv(client_socket[i], request, BUFFER_SIZE, 0) <= 0)
            {
                warning_print("Receive failed\n");
                client_close(i);
            }
            else
            {
                // handle request
                char tokens[BUFFER_SIZE];
                bzero(tokens, BUFFER_SIZE);
                strcpy(tokens, request);

                char *token = strtok(tokens, "\n");
                int pos = 0;

                while (token != NULL)
                {
                    char request_token[BUFFER_SIZE] = "";
                    char response[BUFFER_SIZE] = "";

                    // copy token to request_token because of strtok
                    strcpy(request_token, token);

                    // add newline only if present in request
                    pos += strlen(request_token);
                    if (pos < BUFFER_SIZE && request[pos] == '\n')
                    {
                        strcat(request_token, "\n");
                        pos++;
                    }

                    //  calculate
                    int status = calculator_protocol(request_token, response);

                    // send response
                    if (send(client_socket[i], response, strlen(response), 0) < 0)
                    {
                        warning_print("Send failed\n");
                    }

                    if (status)
                    {
                        client_close(i);
                        break;
                    }

                    // move to next token
                    token = strtok(NULL, "\n");
                }
            }
        }
    }
}

/**
 * @brief Listens for incoming UDP packets and handles them
 */
void server_listen_udp()
{
    while (true)
    {
        char request[BUFFER_SIZE + 2] = "";
        char response[BUFFER_SIZE + 3] = "";

        if (recvfrom(server_socket, request, BUFFER_SIZE + 2, 0, (struct sockaddr *)&address, &address_len) < 0)
        {
            warning_print("Receive failed\n");
        }

        (void)calculator_protocol(request, response);

        if (sendto(server_socket, response, strlen(response + 2) + 2, 0, (struct sockaddr *)&address, address_len) < 0)
        {
            warning_print("Send failed\n");
        }

        bzero(request, sizeof(request));
        bzero(response, sizeof(response));
    }
}

/**
 * @brief Listens for incoming packets and handles them
 */
void server_listen()
{
    if (server_mode == MODE_TCP)
    {
        server_listen_tcp();
    }
    else
    {
        server_listen_udp();
    }
}
