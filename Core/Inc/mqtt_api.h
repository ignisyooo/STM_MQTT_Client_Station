#ifndef _MQTT_API_H_
#define _MQTT_API_H_

#include "mqtt.h"
#include <stdbool.h>

void mqtt_api_publishMsg( mqtt_client_t *client, const char *topic, const char * msg );
void mqtt_api_subscribe( mqtt_client_t *client, const char *topic,
                         mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb );
bool mqtt_api_isConnected( mqtt_client_t *client );
err_t mqtt_api_connect( mqtt_client_t *client, const struct mqtt_connect_client_info_t *ci, const ip_addr_t *ip );

#endif /* _MQTT_API_H_ */
