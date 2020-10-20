#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "config.h"
#include "LED.h"

static const char root_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

static const char client_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUJdNcqyA4wzLh/15S+HLnURoBkWowDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMDcyNzIwMzEw
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL8VmT7hsEBuSA5P9oIF
kXpk7a4AZeZguhhA20juLarv0RRP1AcViC3WwzoICzz4YWqB6iFLhmApWWkAAEAH
LwUkvCzXtl4T5onkYiXFVBOI+gO0T7yk3FLUbZNWWdqEE+uA22hlNpEOsxYyXXmS
5Xd3NOzEjLm805qWkrrCuGUHQv8904LL2ATss05IBcn41z/MizCjBGbuvN1q3u4l
+rKjEHJZ9i4KOZ7oJoyBkgGcyt76cQra0bzpZSu7vi0mR9rY/yUJUxnrT+XRSRQd
Fg8P9s+X0PuWHy1suTBCe86gcbeWqNw0HJrFTZ+vvYB+weZWPayJVfDXhUWH3TGM
VkcCAwEAAaNgMF4wHwYDVR0jBBgwFoAUIpv9anP+8PoSB/Xpc7YJ7/3juqEwHQYD
VR0OBBYEFOTEedlCGdCQVRY0t4NeyMTpWm81MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBZACmB0JLHooqso7A3em967u+v
LJiU/sxWq8c0nosqzjJNBwsxlFvTdbOZRGZtqQD4C5BtdjsqMnlw5qLJcmaZbwuE
qyT6AbzilgohFd9D4zoRqex6yYNX8RF3bracCrIaGk7H4LG6kZ3G9fdzXexMTUvB
+bvvIsNWo5Djat6WPQWzeq25RXqcAMZjTZwVetTrqxb88w7C2mExGr3J3/6LUez9
/nNCasf0OHbKwFo63jYrwx/R2ySxR6ZGrxJYEhVapRH/Ix4G2R64+8joBrY5NmKU
la8PbT6ECdO48c6oOL9x0xjGYs96DqLjf5+8bjabCUvN5hYDa+rV9nIESA97
-----END CERTIFICATE-----
)EOF";

static const char client_key[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAvxWZPuGwQG5IDk/2ggWRemTtrgBl5mC6GEDbSO4tqu/RFE/U
BxWILdbDOggLPPhhaoHqIUuGYClZaQAAQAcvBSS8LNe2XhPmieRiJcVUE4j6A7RP
vKTcUtRtk1ZZ2oQT64DbaGU2kQ6zFjJdeZLld3c07MSMubzTmpaSusK4ZQdC/z3T
gsvYBOyzTkgFyfjXP8yLMKMEZu683Wre7iX6sqMQcln2Lgo5nugmjIGSAZzK3vpx
CtrRvOllK7u+LSZH2tj/JQlTGetP5dFJFB0WDw/2z5fQ+5YfLWy5MEJ7zqBxt5ao
3DQcmsVNn6+9gH7B5lY9rIlV8NeFRYfdMYxWRwIDAQABAoIBAQCALES24TwQe+jp
cMmxPXrv1nN3m9y+Q3OQgTCWIu2PE33zWfinJ9JIrR4W9v5Sz2jIwIwD+V0Rc6r9
u3pveGIZ2SJAEhHJ2R9yrzcGju5ujBroETLLHuqKxOMbSXjMEnwqJC0IQZvwgSOv
Z+k7e0VgdR2TE+QxUm+96fJSQ/rcCHGmqJNPCR+labqRBtrU0fU58f53oVZrSWT5
LLkgoXhJ+WlCoUqq0f9iOrC9nfMAGGTJXLsZ8b7mWT5q78PqCP8bf/SBZl80FCjW
DSVlTY4HIA7EIfzqitiMtJiQGHeGQZ+U+U9Mu3LsO8Vm79lTuBfwgW4wSFbZ6FZo
UFv4lmPBAoGBAOtWCrdlvM4czb9qDxmCoeJEaPuubvX/uJJPUxU/U8lRY4N1Ydhc
BMVCYBvt1ljX5N5MJ42snGHrHEt8uKVvMawap44m31AGzfA2Zt7GN2Pn937dzD5r
DntMBQYqaDDpJGi4zQOOAxinTQ2n2vdrKUEC6ri/ieRHYkQmkjA4RdSVAoGBAM/c
2kFi0ku+arRgUvFtszxJAjx6zAl8CZSHAG8Vw9anQoN18WCCm8fSessSfmb4XLkt
TNQ8HQdGpH6ddYXjAsOyaJnvhM7hziVYKyEB5TXsIGaW5eN2EcFHrpNE7TWc/Lkm
5QO0J/q6gB0aybsdStBFPxUDCtzwyecQGvex74xrAoGABASpFC6RhbdqL+2Aqn/X
gcSHQlzdGigun5jpI0C8joBsQY9ch1Qy5KdYesGZ2lR9XOpR9pJO7UEVb5iGkpSO
dZDS8ZNqVrR3CbX0FbOuuG76g9kHpr0vNx26LDvHIaapb3xQS9I97UrV/vixvW2/
+5MwLM1gblxlqiI6dsZPMu0CgYB/Thn19doqPaMIef4Sk0c04toVxHd2T5tmdhBT
+JaKvsHD1dZRb2yNQ2GlQdxcoDvZxd7KFU8KB7/zM+ioaVJmHLxnmwRR2IXy5T70
5j5FeiqV6lE9sDBLmeVXmN1W+qDlnOaiEBJ0Qwj3ylJG4TfSurZZ3hUnH+noNDrv
/PBjpQKBgCyZ1A0sf6vFwSIptR7ZrRDWGBJ3ObJkbq8a6+6DAML+gVgv2RfnDjjQ
zotLmorCoU/lnOE6+fSvrgOW/8zIXsCzpR8pn86WBXj7B6XX2JjOhAPNObY6wd20
P3iJzkMuk7PSPvG+7AlyLoiHQf/7YuLREAZR/e1+gen1ErHSomZm
-----END RSA PRIVATE KEY-----
)KEY";

const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);

BearSSL::X509List cert(root_ca);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(client_key);

void loadConfig();

void loadcerts() {
  espClient.setTrustAnchors(&cert);
  espClient.setClientRSACert(&client_crt, &key);
}

void setup() {
	Serial.begin(115200);
	delay(10);

	Serial.println("Booting WiFi LED v.2");
	analogWriteFreq(FREQUENCY);

	pinMode(USR_LED, OUTPUT);
	digitalWrite(USR_LED, 1);

	initLEDs();

	loadConfig();

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

  Serial.println("Syncronisying time...");
  timeClient.begin();
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }
  Serial.print("Time :");
  Serial.println(timeClient.getFormattedTime());

  espClient.setX509Time(timeClient.getEpochTime());

  Serial.printf("Heap size: %u\n", ESP.getFreeHeap());
  
  loadcerts();
  
  Serial.printf("Heap size: %u\n", ESP.getFreeHeap());
  
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

		if(client.connect(NAME)) {
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
      char buf[256];
      espClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

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

void loadConfig() {
	Serial.print("Mounting filesystem...");
	if(LittleFS.begin()) {
		Serial.println(" done!");
		if(LittleFS.exists(CONFIG_FILE)) {
			// File exists, reading and loading
			Serial.print("Reading config file...");
			File configFile = LittleFS.open(CONFIG_FILE, "r");
			if(configFile) {
				Serial.println(" loaded!");
				size_t size = configFile.size();
				// Allocate a buffer to store contents of the file.
				std::unique_ptr<char[]> buf(new char[size]);

				configFile.readBytes(buf.get(), size);
				Serial.println(buf.get());
				DynamicJsonDocument configDoc(2048);
				auto error = deserializeJson(configDoc, buf.get());
				if(error) {
					Serial.print(F("Parsing Config failed with code "));
					Serial.println(error.c_str());
					return;
				}
				serializeJson(configDoc, Serial);
			}
		}
	}
}
