/* You need a comment as first line */

#include "FastLED.h"

#define NUM_LEDS 49
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
}


int valmax = 0;
int nsamp = 0;
const int RESET_MAX = 50;

void vumeter_with_peak() {
  if (Serial.available() > 0) {
    int ser_in = Serial.read();

    if (nsamp++ >= RESET_MAX) {
      valmax = 0;
      nsamp = 0;
    }
    valmax = max(valmax, ser_in);

    long val = ser_in * NUM_LEDS;
    for (int i = 0; i < NUM_LEDS; i++) {
      int ledval = min(255, val);
      val -= ledval;
      leds[i].setRGB(ledval, 0, 0);
    }
    leds[(int)((float)valmax / 256.0 * NUM_LEDS)] = 0xFFFFFF;

    FastLED.show();
  }
}


void vumeter_needle()
{
  static int prev;

  if (Serial.available() > 0) {
    int ser_in = Serial.read();

    if (ser_in < prev) {
      ser_in = max(ser_in, prev - 5);
    }
    prev = ser_in;

    long val = ser_in * NUM_LEDS;

    for (int i = 0; i < NUM_LEDS; i++) {
      int ledval = min(255, val);
      val -= ledval;
      leds[i].setRGB(255, ledval, ledval);
    }

    FastLED.show();
  }
}

void loop() {
  vumeter_needle();
}

