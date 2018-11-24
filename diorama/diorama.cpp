#include "FastLED.h"

static const int NLEDS = 100;
static const int DATA_PIN = 5;
CRGB leds[NLEDS];

int led_state = 0;

void scene();

void setup()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NLEDS);

	delay(500);
    scene();
    FastLED.show();
}

void loop() {
	delay(500);
}

void scene() {
    const long sky = 0x59a8ff;
    const long water = 0x1f604a;
    const long ground = 0xFF8080;

    leds[1]  = ground;
    leds[2]  = ground;
    leds[3]  = ground;
    leds[2]  = ground;
    leds[3]  = ground;
    leds[4]  = ground;
    leds[5]  = ground;
    leds[6]  = ground;
    leds[7]  = ground;
    leds[8]  = ground;
    leds[9]  = ground;
    leds[10] = ground;
    leds[11] = ground;
    leds[12] = ground;
    leds[13] = ground;
    leds[14] = ground;
    leds[15] = ground;
    leds[16] = ground;

    leds[26] = water;
    leds[27] = water;
    leds[28] = water;
    leds[29] = water;
    leds[30] = water;
    leds[31] = water;
    leds[32] = water;
    leds[33] = water;
    leds[32] = water;
    leds[33] = water;
    leds[34] = water;
    leds[35] = water;
    leds[36] = water;
    leds[37] = water;
    leds[38] = water;
    leds[39] = water;
    leds[40] = water;
    leds[41] = water;

    leds[50] = sky;
    leds[51] = sky;
    leds[52] = sky;
    leds[53] = sky;
    leds[54] = sky;
    leds[55] = sky;
    leds[56] = sky;
    leds[57] = sky;
    leds[58] = sky;
    leds[59] = sky;
    leds[60] = sky;
    leds[61] = sky;
    leds[62] = sky;
    leds[63] = sky;
    leds[64] = sky;
    leds[65] = sky;
}
