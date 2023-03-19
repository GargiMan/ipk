/**
 * @file resources.h
 * @author Marek Gergel (xgerge01)
 * @brief definition of functions and variables for resource handling
 * @version 0.1
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "resources.h"

char *host = NULL;
char *port = NULL;

void free_resources()
{
    // free(host);
    free(port);
}