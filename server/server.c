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
int comm_socket;
struct sockaddr_in server_address;
bool server_opened = false;

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
    unsigned int result_i = operands[0];
    for (int i = 1; i < num_operands; i++)
    {
        switch (operator)
        {
        case '+':
            result_i += operands[i];
            break;
        case '-':
            result_i -= operands[i];
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
    int status = STATUS_OK;

    if (server_mode == MODE_TCP)
    {
        if (strcmp("HELLO\n", request) == 0 && !server_opened)
        {
            server_opened = true;
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

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

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

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        error_exit(socketError, "Socket binding to address failed\n");
    }

    if (server_mode == MODE_TCP && listen(server_socket, 5) < 0)
    {
        error_exit(socketError, "Socket listening failed\n");
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        error_exit(signalError, "Signal handler registration failed\n");
    }
}

/**
 * @brief Closes the server socket
 */
void server_close()
{
    close(server_socket);
}

/**
 * @brief Listens for incoming TCP connections and handles them
 * @return int 0 if the server is closed, 1 otherwise
 */
int server_listen_tcp()
{
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    server_opened = false;

    comm_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (comm_socket > 0)
    {
        char request[BUFFER_SIZE] = "";
        char response[BUFFER_SIZE] = "";

        while (recv(comm_socket, request, BUFFER_SIZE, 0) > 0)
        {
            int status = calculator_protocol(request, response);

            send(comm_socket, response, strlen(response), 0);

            if (status)
            {
                break;
            }

            bzero(request, sizeof(request));
            bzero(response, sizeof(response));
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Listens for incoming UDP messages and handles them
 * @return int 0 if the server is closed, 1 otherwise
 */
int server_listen_udp()
{
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    char request[BUFFER_SIZE + 2] = "";
    char response[BUFFER_SIZE + 3] = "";

    if (recvfrom(server_socket, request, BUFFER_SIZE + 2, 0, (struct sockaddr *)&client_address, &client_address_len) > 0)
    {
        (void)calculator_protocol(request, response);

        sendto(server_socket, response, strlen(response + 2) + 2, 0, (struct sockaddr *)&client_address, client_address_len);

        bzero(request, sizeof(request));
        bzero(response, sizeof(response));
    }
    return 1;
}

/**
 * @brief Listens for incoming messages and handles them
 * @return int 0 if the server is closed, 1 otherwise
 */
int server_listen()
{
    if (server_mode == MODE_TCP)
    {
        return server_listen_tcp();
    }
    else
    {
        return server_listen_udp();
    }
}
