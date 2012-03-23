#include "ardulib/NikonRemote.h"

#include "Arduino.h"

void NikonRemote::shoot()
{
    if (!isAttached())
    {
        return;
    }

    for (int i=0; i < 2; i++) {
        pulseON(2000);          // pulse for 2000 uS (Microseconds)
        pulseOFF(27850);        // turn pulse off for 27850 us
        pulseON(390);           // and so on
        pulseOFF(1580);
        pulseON(410);
        pulseOFF(3580);
        pulseON(400);
        pulseOFF(63200);
    }   // twice
}

void NikonRemote::shootBulb(unsigned long time_ms) {
    shoot();            // mirror up!
    delay(time_ms);     // stay open...
    shoot();            // mirror down
    delay(150);         // delay required for bulb. Shorter and we may miss a shot
}

// sets the pulse of the IR signal.
void NikonRemote::pulseON(unsigned long pulse_us)
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

void NikonRemote::pulseOFF(unsigned long delay_us)
{
    // create the microseconds to delay for
    unsigned long delay_end = micros() + delay_us;
    while (micros() < delay_end);
}

