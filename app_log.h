#ifndef _APP_LOG_H_
#define _APP_LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "bsp_usart.h"

// #define UART_LOG_HANDLER (&huart6)
#define UART_LOG        USART_LOG_PERIPH


void app_log( char *format, ... );

#endif
