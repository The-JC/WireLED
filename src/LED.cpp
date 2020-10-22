#include "LED.h"

extern const uint16_t gamma10[];

#define SETPINWITHGAMMA(pin, brightness) (analogWrite(pin, gamma10[brightness]))

#ifdef TYPE_W
	void setBrightness(uint8_t brightness) {
		#ifdef LEDPIN1
			SETPINWITHGAMMA(LEDPIN1, brightness);
		#endif
		#ifdef LEDPIN2
			SETPINWITHGAMMA(LEDPIN2, brightness);
		#endif
		#ifdef LEDPIN3
			SETPINWITHGAMMA(LEDPIN3, brightness);
		#endif
		#ifdef LEDPIN4
			SETPINWITHGAMMA(LEDPIN4, brightness);
		#endif
	}
#endif

#if defined (TYPE_RGB) || defined (TYPE_RGBW)
	uint8_t changed=0;
	struct RGB_t rgb;

	void initLEDs() {
		pinMode(LEDPIN1, OUTPUT);
		pinMode(LEDPIN2, OUTPUT);
		pinMode(LEDPIN3, OUTPUT);
		pinMode(LEDPIN4, OUTPUT);

		digitalWrite(LEDPIN1, LOW);
		digitalWrite(LEDPIN2, LOW);
		digitalWrite(LEDPIN3, LOW);
		digitalWrite(LEDPIN4, LOW);
		#ifdef TYPE_RGB
		analogWrite(LEDPIN4, 1023);
		#endif
	}

	void setPower(uint8_t power) {
		rgb.power = power;
		changed=1;
	}

	void setColor(uint8_t red, uint8_t green, uint8_t blue) {
		rgb.red = red;
		rgb.green = green;
		rgb.blue = blue;
		changed=1;
	}
  #ifdef TYPE_RGBW
	void setWhite(uint8_t white) {
		rgb.white = white;
		changed=1;
	}
  #endif
	void refresh() {
		if(!changed) return;
		if(rgb.power) {
			SETPINWITHGAMMA(LEDPIN1, rgb.green);
			SETPINWITHGAMMA(LEDPIN2, rgb.red);
			SETPINWITHGAMMA(LEDPIN3, rgb.blue);
			#ifdef TYPE_RGBW
				SETPINWITHGAMMA(LEDPIN4, rgb.white);
			#endif
		} else {
			digitalWrite(LEDPIN1, LOW);
			digitalWrite(LEDPIN2, LOW);
			digitalWrite(LEDPIN3, LOW);
			#ifdef TYPE_RGBW
				digitalWrite(LEDPIN4, LOW);
			#endif
		}
	}
#endif

const uint16_t gamma10[] = { 
 0,1,1,1,2,3,4,6,7,9,11,13,15,18,21,25,
 27,30,34,37,41,46,50,55,59,64,70,75,81,87,93,99,
 105,112,119,126,133,141,148,156,164,172,181,190,199,208,217,226,
 236,246,256,267,277,288,299,310,321,333,345,357,369,381,394,407,
 420,433,446,460,474,488,502,516,531,546,561,576,591,607,623,639,
 655,672,688,705,722,740,757,775,793,811,829,848,866,885,904,924,
 943,963,983,1003,1023};
