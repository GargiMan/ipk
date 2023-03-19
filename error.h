/**
 * @file error.h
 * @author Marek Gergel (xgerge01)
 * @brief declaration of functions and variables for error handling
 * @version 0.1
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "resources.h"

typedef enum errorCodes
{
    invalidArgument = 1,
    internalError = 99
} errorCodes_t;

void error_exit(errorCodes_t errcode, char *msg, ...);
void warning_print(char *msg, ...);

#endif // ERROR_H
