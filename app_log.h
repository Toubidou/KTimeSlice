#ifndef _APP_LOG_H_
#define _APP_LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "usart.h"

#define DEBUG           1
#define UART_LOG           (&huart3)
// #define UART_LOG        USART_LOG_PERIPH


void app_log( char *format, ... );

#endif
