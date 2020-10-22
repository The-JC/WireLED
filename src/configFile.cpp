#include "configFile.h"

#include "LittleFS.h"
#include "ArduinoJson.h"

char wifi_ssid[80];
char wifi_password[80];
char name[80];

char mqtt_server[60];
uint16_t mqtt_port;
char mqtt_topic[100];

MQTTSecurity mqtt_security = NONE;
#if defined(ENABLE_TLS)
	BearSSL::X509List cert;
	BearSSL::X509List client_crt;
	BearSSL::PrivateKey client_key;
#endif // ENABLE_TLS

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

				DynamicJsonDocument json(2048);
				auto error = deserializeJson(json, buf.get());
				if(error) {
					Serial.print(F("Parsing Config failed with code "));
					Serial.println(error.c_str());
					return;
				}

				#if DEBUG
					serializeJson(json, Serial);
				#endif // DEBUG

				Serial.println("\nConfig successfully parsed!");

				strcpy(name, json["name"]);
				strcpy(wifi_ssid, json["wifi"]["ssid"]);
				strcpy(wifi_password, json["wifi"]["password"]);

				strcpy(mqtt_server, json["mqtt"]["server"]);
				mqtt_port = json["mqtt"]["port"].as<uint16_t>();
				strcpy(mqtt_topic, json["mqtt"]["topic"]);

				#if defined(ENABLE_TLS)
					uint8_t res = 0;
					size_t len_root = strlen_P(json["mqtt"]["root_ca"]);
					size_t len_cert = strlen_P(json["mqtt"]["client_certificate"]);
					size_t len_key = strlen_P(json["mqtt"]["client_key"]);

					const char* cert_root = json["mqtt"]["root_ca"];
					const char* cert_client = json["mqtt"]["client_certificate"];
					const char* cert_key = json["mqtt"]["client_key"];

					res += cert.append((uint8_t*)cert_root, len_root);
					res += client_crt.append((uint8_t*)cert_client, len_cert);
					res += client_key.parse((uint8_t*)cert_key, len_key);

					String security = json["mqtt"]["security"].as<String>();
					if(security.equals("tls")) {
						mqtt_security = MQTTSecurity::TLS;
					} else {
						mqtt_security = MQTTSecurity::NONE;
					}
				#else
					mqtt_security = MQTTSecurity::NONE;
				#endif // ENABLE_TLS
			} else {
				Serial.println("Failed to open filesystem!");
			}
		} else {
			Serial.println("Warning: Config file does not exist!");
		}
	}
}
