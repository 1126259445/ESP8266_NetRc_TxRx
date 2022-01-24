#ifndef __APP_MIAN_H__
#define __APP_MIAN_H__

extern char deviceUUID[17];
extern char MqttTopicSub[30];
extern char MqttTopicPub[30];

extern bool isConnect2Server;
extern esp_mqtt_client_handle_t client;

#endif

