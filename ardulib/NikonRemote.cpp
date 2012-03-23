#include "ardulib/NikonRemote.h"

#include "Arduino.h"

void NikonRemote::shoot()
{
    if (!isAttached())
    {
        return;
    }

    for (int i=0; i < 2; i++) {
        pulseOn(2000);          // pulse for 2000 uS (Microseconds)
        pulseOff(27850);        // turn pulse off for 27850 us
        pulseOn(390);           // and so on
        pulseOff(1580);
        pulseOn(410);
        pulseOff(3580);
        pulseOn(400);
        pulseOff(63200);
    }   // twice
}

void NikonRemote::shootBulb(unsigned long time_ms) {
    shoot();            // mirror up!
    delay(time_ms);     // stay open...
    shoot();            // mirror down
    delay(150);         // delay required for bulb. Shorter and we may miss a shot
}

// sets the pulse of the IR signal.
void NikonRemote::pulseOn(unsigned long pulse_us)
{
    // create the microseconds to pulse for
    unsigned long pulse_end = micros() + pulse_us;

    while (micros() < pulse_end) {
        // turn IR on
        digitalWrite(pin, HIGH);
        // half the clock cycle for 38Khz (26.32 x 10^-6 s)
        // e.g. the 'on' part of our wave
        delayMicroseconds(13);
        // turn IR off
        digitalWrite(pin, LOW);
        // delay for the other half of the cycle to generate wave/ oscillation
        delayMicroseconds(13);
    }
}

void NikonRemote::pulseOff(unsigned long delay_us)
{
    // create the microseconds to delay for
    unsigned long delay_end = micros() + delay_us;
    while (micros() < delay_end);
}

