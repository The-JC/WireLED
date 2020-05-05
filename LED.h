#ifndef LED_H_
#define LED_H_

#include <ESP8266WiFi.h>
#include "config.h"


#ifdef TYPE_W
	void initLEDs();
	void setBrightness(uint8_t brightness);
	void setPower(uint8_t power);
	void refresh();
#endif

#if defined (TYPE_RGB) || defined (TYPE_RGBW)
	struct RGB_t {
		uint8_t power;
		uint8_t red, green, blue;
		#ifdef TYPE_RGBW
			uint8_t white;
		#endif
	};

	void initLEDs();
	void setPower(uint8_t power);
	void setColor(uint8_t red, uint8_t green, uint8_t blue);
	void refresh();
	#ifdef TYPE_RGBW
		void setWhite(uint8_t white);
	#endif
#endif

#endif // LED_H_
