/* Use tools/alsa2spectrum.py to send data in here */

#include <FastLED.h>
#include <PacketSerial.h>
#include <TaskScheduler.h>

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


int npack = 0, nup = 0;

void on_packet_received(const uint8_t* buffer, size_t size);
void update_leds();
void count_packets();

Scheduler ts;
Task t_update(40, TASK_FOREVER, &update_leds, &ts, true);
Task t_print(1000, TASK_FOREVER, &count_packets, &ts, true);

PacketSerial ps;

void setup()
{
    ps.begin(57600);
    ps.setPacketHandler(&on_packet_received);

    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
    update_leds();
}

void loop()
{
    ps.update();
    ts.execute();
}

void count_packets()
{
    Serial.print("np: "),
    Serial.println(npack);
    npack = 0;

    Serial.print("nu: "),
    Serial.println(nup);
    nup = 0;
}

void update_leds()
{
    static CHSV hsv1 = CHSV(0, 255, 255);
    static CHSV hsv2 = CHSV(170, 255, 255);

    // Rotate colours spectrum
    hsv1.h = (hsv1.h + 1) & 255;
    hsv2.h = (hsv2.h + 1) & 255;

    // Paint the buffer state on the leds
    for (int j = 0; j < NCOLS; j++) {
        long val = buf_in[j] * NROWS;
        for (int i = 0; i < NROWS; i++) {
            int ledval = min(255, val);
            val -= ledval;
            *led_at(i, j) = blend(hsv1, hsv2, ledval);
        }
    }

    // Refresh the leds
    FastLED.show();
    nup++;

    // The bars go down if no music is received
    for (int i = 0; i < BUF_IN_SIZE; i++) {
        buf_in[i] = max(buf_in[i] - 1, 0);
    }
}

void on_packet_received(const uint8_t* buffer, size_t size)
{
    if (size >= BUF_IN_SIZE) {
        memcpy(buf_in, buffer, BUF_IN_SIZE);
        npack++;
    }
}
