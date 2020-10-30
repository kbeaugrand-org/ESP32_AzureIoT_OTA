// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "logging.h"

#ifndef NO_LOGGING

void global_log(LOG_LEVEL log_category, const char *file, const char *func, int line, unsigned int options, const char *format, ...)
{
    time_t t;
    va_list args;
    va_start(args, format);

    t = time(NULL);

    switch (log_category)
    {
        case INFO:
            printf("Info: ");
            break;
        case TRACE:
            printf("Trace: ");
            break;
        case DEBUG:
            printf("Debug: ");
            break;
        case ERROR:
            printf("Error: at %.24s File:%s, func:%s, line:%d \n\t>> ", ctime(&t), file, func, line);
            break;
        default:
            break;
    }

    // use vprintf to print variable arguments
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

LOGGER global_log_func = global_log;

void logging_set_log_function(LOGGER log_function)
{
    global_log_func = log_function;
}

LOGGER logging_get_log_function(void)
{
    return global_log_func;
}

#endif // NO_LOGGING