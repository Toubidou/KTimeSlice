#include "app_log.h"

static char _log_buf[256];
void bsp_debug_log_send_buf( const char *pBuf, uint32_t len, uint32_t time_out )
{
#if DEBUG == 1
    bsp_usart_transmit( UART_LOG , (uint8_t * ) pBuf, len, 100);
#endif
}
void app_log( char *format, ... )
{
    va_list ap;

    va_start( ap, format );
    vsnprintf( _log_buf, sizeof( _log_buf ), format, ap );
    va_end( ap );
    bsp_debug_log_send_buf( ( const char * ) _log_buf, strlen( _log_buf ), 10 );
}