#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "error.h"
#include "client.h"

char *host;
int port = 0;
int mode = 0;

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
                error_exit(invalidArgument, "Invalid port, port must be a max 5 digit integer in range (0 - 65535).\n");
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
                error_exit(invalidArgument, "Invalid mode, allowed modes are 'udp' or 'tcp'.\n");
            }
        }
        else
        {
            error_exit(invalidArgument, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
        }
    }

    if (host == NULL || port == 0 || mode == 0)
    {
        error_exit(invalidArgument, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
    }
}

void calculator_protocol()
{
    client_init(host, port, mode);

    char request[BUFFFER_SIZE] = "";
    while (fgets(request, BUFFFER_SIZE, stdin) != NULL)
    {
        char response[BUFFFER_SIZE] = "";
        get_response(request, response);
        printf("%s", response);

        if (mode == MODE_TCP && strcmp(response, "BYE\n") == 0)
        {
            break;
        }
    }

    client_close();
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    calculator_protocol();

    return 0;
}