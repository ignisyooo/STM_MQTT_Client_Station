#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "usart.h" /* uart handler */


void logger_init( UART_HandleTypeDef* uart );
void logger_sendMessage( const char* msg, ... );

#endif /* _LOGGER_H_ */