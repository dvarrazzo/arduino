/* Use alsa2serial to send data in here */

#include "FastLED.h"

#define NUM_LEDS 49
#define DATA_PIN 5

#define NCOLS 5
#define NROWS 9

/* Read 8 samples, but only display NCOLS */
#define BUF_IN_SIZE 8

CRGB leds[NUM_LEDS];
byte buf_in[BUF_IN_SIZE];


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
 *    19      39
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
    Serial.begin(38400);

    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = 0x000000;
    }
}


void loop()
{
    static int prev;
    static int nsamp;

    if (Serial.readBytes((char *)buf_in, BUF_IN_SIZE)) {
        for (int j = 0; j < NCOLS; j++) {
            long val = buf_in[j] * NROWS;
            for (int i = 0; i < NROWS; i++) {
                int ledval = min(255, val);
                val -= ledval;
                led_at(i, j)->setRGB(255, ledval, ledval);
            }
        }
        FastLED.show();
    }
}
