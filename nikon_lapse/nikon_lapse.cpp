/*
 * Programmable Bulb mode continuous shooting for Nikon cameras.
 *
 * This program allows to choose a long shutter speed (> 1 sec) control a
 * sequence of Bulb-mode shots.
 *
 * Use the potentiometer to set the timer.  Two blinking leds will display the
 * chosen time: at the minimum value the shutter speed is 1 sec. Increasing
 * the potentiometer value, the first led will blink at each stop: every blink
 * means twice the time; the second led will blink at each third of stop.
 *
 * Example: you want to make a startrail with 20 seconds exposures:
 *
 * - reset the potentiometer - all zero.
 * - slowly increase the value, and count the first led blinks:
 *
 *   - 1st blink: 2 sec
 *   - 2nd blink: 4 sec
 *   - 3nd blink: 8 sec
 *   - 4nd blink: 16 sec
 *
 * - now count the second led blinks:
 *
 *    - 1 blink: 25% more than 16 seconds: you are at 20 seconds.
 *
 * (the second blink of the second led means 60% more than the stop value)
 */

#include "ardulib/NikonRemote.h"
#include "ardulib/Pot.h"
#include "ExposureGauge.h"

static const pin_t ir_led_pin = 2;
static const pin_t stop_led_pin = 4;
static const pin_t third_led_pin = 3;
static const pin_t poten_pin = 5;

static NikonRemote remote;
static Pot pot;
static ExposureGauge gauge;

void setup()
{
    Serial.begin(19200);

    remote.attach(ir_led_pin);
    pot.attach(poten_pin);
    gauge.attach(stop_led_pin, third_led_pin);
}

void loop()
{
    remote.pulse();

    pot.read();
    gauge.setValue(pot.getValue());
    gauge.display();

    // Previous shot is over, and there's been a delay so the camera is
    // ready to shoot again: do it.
    if (!remote.isShooting()) {
        Serial.print("shoot sec: ");
        Serial.println(gauge.getExposureMs() / 1000, DEC);
        remote.shootBulb(gauge.getExposureMs());
    }
}

