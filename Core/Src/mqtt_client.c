#include "mqtt_client.h"
#include "mqtt_api.h"

#include "main.h"
#include "cmsis_os.h"

#include <string.h>
#include "logger.h"

static void incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);


void StartMqttClientTask(void* args)
{
	mqtt_client_t *client = mqtt_client_new();
	const struct mqtt_connect_client_info_t ci = {
		.client_id = "STM Client",
		.keep_alive = 50
	};

	const ip_addr_t mqttServerIP = {
		.addr = 0x4200a8c0
	};

	const char* topic = "Test";

	for(;;)
	{
		if( NULL != client )
		{
			if(mqtt_api_isConnected( client ))
			{
				mqtt_api_publishMsg( client, topic, "Hello from STM" );
			}
			else
			{
				if(ERR_OK == mqtt_api_connect( client, &ci, &mqttServerIP ))
				{
					mqtt_api_subscribe( client, "esp32/temperature", incoming_publish_cb, incoming_data_cb );
				}
			}
			osDelay(600);
		}
	}
}


/* Callback required by mqtt API */
static void incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
	logger_sendMessage("Incoming publish at topic %s with total length %u\r\n", topic, (unsigned int) tot_len);
}

/* Callback required by mqtt API */
static void incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	logger_sendMessage("Incoming publish payload with length %d, flags %u\r\n", len, (unsigned int) flags);
	if ( flags & MQTT_DATA_FLAG_LAST )
	{
        logger_sendMessage( (const char *)data );
	}
	else
	{
		/* Handle fragmented payload, store in buffer, write to file or whatever */
	}
}
