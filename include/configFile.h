#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include "config.h"
#include "stdint.h"
#include "BearSSLHelpers.h"

enum MQTTSecurity {
	NONE,
	TLS,
};

extern char wifi_ssid[80];
extern char wifi_password[80];
extern char name[80];

extern char mqtt_server[60];
extern uint16_t mqtt_port;
extern char mqtt_topic[100];

extern MQTTSecurity mqtt_security;
#if defined(ENABLE_TLS)
	extern BearSSL::X509List cert;
	extern BearSSL::X509List client_crt;
	extern BearSSL::PrivateKey client_key;
#endif // ENABLE_TLS

void loadConfig(void);

#endif // CONFIG_FILE_H