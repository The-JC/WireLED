#ifndef CONFIG_H_
#define CONFIG_H_

#define SSID "<WiFi SSID>"
#define PASSWORD "<WiFi Password>"
#define MQTT_SERVER "<MQTT Server>"
#define MQTT_PORT <MQTT Port>

#define MQTT_TOPIC "<MQTT Topic>"

//                              RGBW RGB
#define LEDPIN1 5   // GREEN  // 5   15
#define LEDPIN2 15  // RED    // 15  5
#define LEDPIN3 14  // BLUE   // 14  14
#define LEDPIN4 12  // WHITE  // 12  12

#define USR_LED 4

#define FREQUENCY 1000

/**
 * Types
 * - W
 * - RGB
 * - RGBW
 */
//#define TYPE_W
//#define TYPE_RGB
#define TYPE_RGBW

#endif // CONFIG_H_
