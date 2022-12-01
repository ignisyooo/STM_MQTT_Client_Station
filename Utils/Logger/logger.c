#include "logger.h"
#include "cmsis_os.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MSG_BUFFER_SIZE 512u

typedef struct
{
    UART_HandleTypeDef* instance;
    bool isInit;
} logger_t;


static logger_t logger;
static char msgBuffer[MSG_BUFFER_SIZE];

#ifdef FREERTOS
static osMutexId_t logger_mutexTx;
#endif


/* logger init
    @param uart - handler to uart
*/
void logger_init( UART_HandleTypeDef* uart )
{
    if(!logger.isInit)
    {
#ifdef FREERTOS
    const osMutexAttr_t mutex_attributes = {
        .name = "LoggerMutex"
    };
    logger_mutexTx = osMutexNew(&mutex_attributes);
#endif
        logger.instance = uart;
        logger.isInit = true;
    }
}


void logger_sendMessage( const char* msg, ... )
{
    if(logger.isInit)
    {
        if(NULL != msg)
        {
    #ifdef FREERTOS
            osMutexAcquire(logger_mutexTx, osWaitForever);
    #endif
            va_list args;
            va_start( args, msg );
            size_t length = vsnprintf( msgBuffer, MSG_BUFFER_SIZE, msg, args );
            va_end( args );
            HAL_UART_Transmit( logger.instance, (uint8_t*)msgBuffer, length, HAL_MAX_DELAY );
            memset(msgBuffer, 0, MSG_BUFFER_SIZE);
    #ifdef FREERTOS
            osMutexRelease(logger_mutexTx);
    #endif
        }
    }
}
