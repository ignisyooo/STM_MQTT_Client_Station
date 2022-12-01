#include "mqtt_api.h"

#include <string.h>
#include "logger.h"

static void mqtt_subrequestCb( void *arg, err_t result )
{
	logger_sendMessage("Subscribe result: %d\r\n", result);
}

static void mqtt_connectionCb( mqtt_client_t *client, void *arg, mqtt_connection_status_t status )
{
	if ( status == MQTT_CONNECT_ACCEPTED )
    {
		logger_sendMessage("mqtt_api_connectionCb: Successfully connected\r\n");
	}
    else
    {
		logger_sendMessage("mqtt_connection_cb: Disconnected, reason: %d\r\n", status);
	}
}

/* Called when publish is complete either with sucess or failure */
static void mqtt_pubRequestCb( void *arg, err_t result )
{
	if( result != ERR_OK )
    {
		logger_sendMessage("Publish result: %d\r\n", result);
	}
}

err_t mqtt_api_connect( mqtt_client_t *client, const struct mqtt_connect_client_info_t *ci, const ip_addr_t *ip )
{
	err_t err;

	err = mqtt_client_connect( client, ip, MQTT_PORT, mqtt_connectionCb, NULL, ci );

	if( err != ERR_OK )
    {
		logger_sendMessage("mqtt_connect return %d\r\n", err);
	}

	return err;
}

void mqtt_api_publishMsg( mqtt_client_t *client, const char *topic, const char * msg )
{
	err_t err;
	uint8_t qos = 0; /* 0 1 or 2, see MQTT specification */
	uint8_t retain = 0; /* No don't retain such crappy payload... */
	char data[128] = { 0 };

	size_t len = sprintf( data, "MQTT PUBLISHED MESSAGE: %s = %s", topic, msg );

	err = mqtt_publish( client, topic, data, len, qos, retain, mqtt_pubRequestCb, NULL );
	if( err != ERR_OK )
	{
		logger_sendMessage("Publish err: %d\r\n", err);
	}
}

void mqtt_api_subscribe( mqtt_client_t *client, const char *topic,
                         mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb )
{
    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback( client, pub_cb, data_cb, NULL );

    err_t err = mqtt_subscribe( client, topic, 0, mqtt_subrequestCb, NULL );
    if( err != ERR_OK )
    {
        logger_sendMessage("mqtt_subscribe return: %d\n", err);
    }
}

bool mqtt_api_isConnected( mqtt_client_t *client )
{
    return mqtt_client_is_connected( client );
}
