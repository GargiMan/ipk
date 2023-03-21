/**
 * @file main.c
 * @author Marek Gergel (xgerge01)
 * @brief main function for client side communication
 * @version 0.1
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "error.h"
#include "client.h"

char *host;
int port = 0;
int mode = 0;

void print_help()
{
    printf("Usage: ipkcpc -h <host> -p <port> -m <mode>\n");
    printf("Options:\n");
    printf("  -h <host>    server host name or IPv4 address\n");
    printf("  -p <port>    server port number\n");
    printf("  -m <mode>    communication mode, allowed modes are 'udp' or 'tcp'\n");
    printf("  --help       print this help and exit program\n");
}

void parse_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 && i < argc - 1)
        {
            host = argv[++i];
        }
        else if (strcmp(argv[i], "-p") == 0 && i < argc - 1)
        {
            char *endptr;
            port = (int)strtol(argv[++i], &endptr, 10);

            if (*endptr != '\0' || port < 1 || port > 65535)
            {
                error_exit(invalidArgument, "Invalid port, port must be a max 5 digit integer in range (0 - 65535)\n");
            }
        }
        else if (strcmp(argv[i], "-m") == 0 && i < argc - 1)
        {
            i++;
            if (strcasecmp(argv[i], "tcp") == 0)
            {
                mode = MODE_TCP;
            }
            else if (strcasecmp(argv[i], "udp") == 0)
            {
                mode = MODE_UDP;
            }
            else
            {
                error_exit(invalidArgument, "Invalid mode, allowed modes are 'udp' or 'tcp'\n");
            }
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            print_help();
            exit(0);
        }
        else
        {
            print_help();
            error_exit(invalidArgument, "Unknown argument '%s'\n", argv[i]);
        }
    }

    if (host == NULL || port == 0 || mode == 0)
    {
        print_help();
        error_exit(invalidArgument, "Options -h, -p and -m are required\n");
    }
}

void run_client()
{
    client_init(host, port, mode);

    char request[BUFFFER_SIZE] = "";
    char response[BUFFFER_SIZE] = "";
    while (fgets(request, BUFFFER_SIZE, stdin) != NULL)
    {
        get_response(request, response);
        printf("%s", response);

        if (mode == MODE_TCP && strcmp(response, "BYE\n") == 0)
        {
            break;
        }

        bzero(request, BUFFFER_SIZE);
        bzero(response, BUFFFER_SIZE);
    }

    client_close();
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    run_client();

    return 0;
}