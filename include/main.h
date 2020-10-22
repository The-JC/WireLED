#ifndef MAIN_H_
#define MAIN_H_

#include <ESP8266WiFi.h>

#include "config.h"
#include "configFile.h"

void setup(void);
void loop(void);
void reconnect(void);
void callback(char* topic, byte* payload, unsigned int length);

#endif // MAIN_H_