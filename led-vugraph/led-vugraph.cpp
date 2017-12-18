/* Use alsa2serial to send data in here */

#include "FastLED.h"

#define NUM_LEDS 49
#define DATA_PIN 5

#define NCOLS 5
#define NROWS 9

#define SHIFT_SAMP 10

CRGB leds[NUM_LEDS];


/* Leds order:
 *
 *    09      29
 * 08   10  28  30  48
 * 07   11  27  31  47
 * 06   12  26  32  46
 * 05   13  25  33  45
 * 04   14  24  34  44
 * 03   15  23  35  43
 * 02   16  22  36  42
 * 01   17  21  37  41
 * 00   18  20  38  40
 *        19      39
 */
CRGB *led_at(int row, int col)
{
    int i;
    switch (col) {
        case 0:
            i = row;
            break;
        case 1:
            i = 18 - row;
            break;
        case 2:
            i = 20 + row;
            break;
        case 3:
            i = 38 - row;
            break;
        case 4:
            i = 40 + row;
            break;
    }

    return leds + i;
}

void setup() {
    Serial.begin(9600);
    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = 0x000000;
    }
}


void vumeter_graph()
{
  static int prev;
  static int nsamp;

  if (Serial.available() > 0) {
    int ser_in = Serial.read();

    if (ser_in < prev) {
      ser_in = max(ser_in, prev - 5);
    }
    prev = ser_in;

    long val = ser_in * NROWS;

    // Shift every SHIFT_SAMP
    if (nsamp++ >= SHIFT_SAMP) {
        nsamp = 0;
        for (int j = NCOLS - 1; j > 0; j--) {
            for (int i = 0; i < NROWS; i++) {
                *led_at(i, j) = *led_at(i, j-1);
            }
        }
    }

    for (int i = 0; i < NROWS; i++) {
      int ledval = min(255, val);
      val -= ledval;
      led_at(i, 0)->setRGB(255, ledval, ledval);
    }

    FastLED.show();
  }
}

void loop() {
  vumeter_graph();
}

