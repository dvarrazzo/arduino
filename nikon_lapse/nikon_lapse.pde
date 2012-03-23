/*
 * LUCKYLARRY.CO.UK - IR Remote control for Nikon using Arduino
 *
 * Mimics the infrared signal to trigger the remote for any Nikon camera which
 * can use the ML-L1 and ML-L3 remotes. Can be used as an intervalometer for
 * time lapse photography.
 *
 * The IR sequence I used is originally taken from:
 * http://www.bigmike.it/ircontrol/ You should be able to use my pulse methods
 * to alter to suit other cameras/ hardware.  micros() is an Arduino function
 * that calls the time in Microseconds since your program first ran. Arduino
 * doesn't reliably work with microseconds so we work our timings by taking
 * the current reading and then adding our delay on to the end of it rather
 * than rely on the in built timer.
 */

#include "ardulib/NikonRemote.h"

static const int ir_led_pin = 13;

static NikonRemote remote;

void setup() {
    remote.attach(ir_led_pin);
}

void loop() {
    // Camera in B mode
    remote.shootBulb(5 * 1000);

    /*
     * Regular time lapse.
     *
     * For 10" exposure, delay 10000 + pause 200. With pause 100 1/2 shots
     * are lost.
     *
     * For 15" exposure, delay 16000" (sic) + pause 200, but timing between
     * mirror up and mirror down is irregular, and sporadic shots are lost.
     */

    /*
    remote.shoot();
    delay(16000);
    delay(200);
    */
}

