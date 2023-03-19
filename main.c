#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "resources.h"
#include "error.h"

#define UDP 1
#define TCP 2

extern char *host;
extern char *port;
int mode = 0;

void parse_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 && i < argc - 1)
        {
            host = argv[++i];

            struct in_addr ipv4;
            int valid = inet_pton(AF_INET, host, &ipv4);

            if (!valid)
            {
                error_exit(invalidArgument, "Invalid host, host must be a valid IPv4 address.\n");
            }
        }
        else if (strcmp(argv[i], "-p") == 0 && i < argc - 1)
        {
            char *endptr;
            long port_l = strtol(argv[++i], &endptr, 10);

            if (*endptr != '\0' || port_l < 1 || port_l > 65535)
            {
                error_exit(invalidArgument, "Invalid port, port must be a max 5 digit integer in range (0 - 65535).\n");
            }

            port = malloc(sizeof(char) * (snprintf(NULL, 0, "%ld", port_l) + 1));
            if (port == NULL)
            {
                error_exit(internalError, "Memory allocation failed.\n");
            }
            sprintf(port, "%ld", port_l);
        }
        else if (strcmp(argv[i], "-m") == 0 && i < argc - 1)
        {
            i++;
            if (strcasecmp(argv[i], "tcp") == 0)
            {
                mode = TCP;
            }
            else if (strcasecmp(argv[i], "udp") == 0)
            {
                mode = UDP;
            }
            else
            {
                error_exit(invalidArgument, "Invalid mode, allowed modes are 'udp' or 'tcp'.\n");
            }
        }
        else
        {
            error_exit(invalidArgument, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
        }
    }

    if (host == NULL || port == NULL || mode == 0)
    {
        error_exit(invalidArgument, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
    }
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    printf("Host: %s\n", host);
    printf("Port: %s\n", port);
    printf("Mode: %s\n", mode == 2 ? "TCP" : "UDP");

    // TODO read from stdin and send to server

    free_resources();

    return 0;
}