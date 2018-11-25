#include <FastLED.h>
#include <PacketSerial.h>

static const int NLEDS = 100;
static const int DATA_PIN = 5;

CRGB leds[NLEDS];
PacketSerial cobs;


void scene(CRGB sky, CRGB water, CRGB ground);
void on_packet_received(const uint8_t* buffer, size_t size);


void setup()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NLEDS);

    cobs.begin(57600);
    cobs.setPacketHandler(&on_packet_received);
}

void loop() {
    cobs.update();
	delay(500);
}


void scene(CRGB sky, CRGB water, CRGB ground)
{

    for (int i = 1; i <= 16; i++) {
        leds[i] = ground;
    }

    for (int i = 26; i <= 41; i++) {
        leds[i] = water;
    }

    for (int i = 50; i <= 65; i++) {
        leds[i] = sky;
    }
}

void on_packet_received(const uint8_t* buffer, size_t size)
{
    CRGB sky, water, ground;

    /* Serial.print("size: "), */
    /* Serial.println(size); */

    if (size != 9) { return; }

    sky.red   = buffer[0];
    sky.green = buffer[1];
    sky.blue  = buffer[2];

    water.red   = buffer[3];
    water.green = buffer[4];
    water.blue  = buffer[5];

    ground.red   = buffer[6];
    ground.green = buffer[7];
    ground.blue  = buffer[8];

    scene(sky, water, ground);
    FastLED.show();
}
