#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "LED.h"

const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
	Serial.begin(115200);
	delay(10);

	Serial.println("Booting WiFi LED v.2");
	analogWriteFreq(FREQUENCY);

	pinMode(USR_LED, OUTPUT);
	digitalWrite(USR_LED, 1);

	initLEDs();

	Serial.print("\n\nTrying to connect to ");
	Serial.println(SSID);

	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);

	uint8_t tick = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print('.');
	digitalWrite(USR_LED, !digitalRead(USR_LED));
		tick++;
		if(tick > 40)
			ESP.reset();
	}

	Serial.println("\nWiFi connected succesfully");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	digitalWrite(USR_LED, 1);
	delay(250);
	digitalWrite(USR_LED, 0);

	client.setServer(MQTT_SERVER, MQTT_PORT);
	client.setCallback(callback);
}

void loop() {
	if(!client.connected()) {
		reconnect();
	}
	client.loop();
}

void reconnect() {
	while (!client.connected()) {
		Serial.println("Attempting to establish MQTT connection...");

		if(client.connect("WiFILED Client")) {
			Serial.println("Succesfully connected to MQTT Broker");

			client.subscribe(MQTT_TOPIC);
			Serial.print("Subscribing to ");
			Serial.println(MQTT_TOPIC);
			digitalWrite(USR_LED, 1);
			delay(100);
			digitalWrite(USR_LED, 0);
			delay(100);
			digitalWrite(USR_LED, 1);
			delay(100);
			digitalWrite(USR_LED, 0);
		} else {
			Serial.print("Connection failed, rc=");
			Serial.print(client.state());
			Serial.println(" trying again in 5 seconds!");
		digitalWrite(USR_LED, !digitalRead(USR_LED));

			delay(4500);
		}
	}
}

void callback(char* topic, byte* payload, unsigned int length) {
	StaticJsonDocument<BUFFER_SIZE> doc;

	auto error = deserializeJson(doc, (char*)payload);
	if(error) {
		Serial.print(F("Parsing failed with code "));
		Serial.println(error.c_str());
		return;
	}
	Serial.print("Received ");
	Serial.println(topic);
	digitalWrite(USR_LED, 1);
	delay(100);
	digitalWrite(USR_LED, 0);

	if(doc.containsKey("power")) {
		setPower(doc["power"]);
	}

	#if defined(TYPE_RGB) || defined(TYPE_RGBW)
		if(doc.containsKey("rgb")) {

			setColor(doc["rgb"]["r"], doc["rgb"]["g"], doc["rgb"]["b"]);
		}
		#ifdef TYPE_RGBW
			if(doc.containsKey("white")) {
				setWhite(doc["white"]);
			}
		#endif
	#endif

	refresh();
}
