#include "ExposureGauge.h"

#include "Arduino.h"

void ExposureGauge::attach(pin_t pin_stop, pin_t pin_third)
{
    pinMode(pin_stop, OUTPUT);
    this->pin_stop = pin_stop;
    pinMode(pin_third, OUTPUT);
    this->pin_third = pin_third;
}

void ExposureGauge::display()
{
    int thirds_new = getExposureThirds(value);
    int thirds_old = getExposureThirds(value_old);

    if (thirds_new > thirds_old) {
        // going up
        while (thirds_new > thirds_old) {
            thirds_old += 1;
            flash((thirds_old % 3 == 0) ? pin_stop : pin_third);
        }
    }
    else {
        // going down
        while (thirds_new < thirds_old) {
            thirds_old -= 1;
            flash((thirds_old % 3 == 2) ? pin_stop : pin_third);
        }
    }

    value_old = value;
}

int ExposureGauge::getExposureThirds(int value) const
{
    // split the useful range from 1 sec (value 0)
    // to 6553.6 sec (109 minutes, 12 2/3 stops more)
    return ((long)value * 39) / 1024;
}

unsigned long ExposureGauge::getExposureMs() const
{
    unsigned long rv = 1000;
    int thirds = getExposureThirds(value);

    while (thirds >= 3) {
        thirds -= 3;
        rv *= 2;
    }
    if (thirds == 1) { rv = rv * 5 / 4; }     // +25%
    if (thirds == 2) { rv = rv * 8 / 5; }     // +60%

    return rv;
}

void ExposureGauge::flash(pin_t pin)
{
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
}

