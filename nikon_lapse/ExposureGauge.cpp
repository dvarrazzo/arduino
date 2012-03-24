#include "ExposureGauge.h"

#include "Arduino.h"

void ExposureGauge::display() {
    // No change at all
    if (value_old == INVALID_VALUE) { return; }

    int stops_new, thirds_new;
    int stops_old, thirds_old;

    getExposureStops(value, &stops_new, &thirds_new);
    getExposureStops(value_old, &stops_old, &thirds_old);

    if ((stops_new * 3 + thirds_new) > (stops_old * 3 + thirds_old)) {
        // going up
        while ((stops_new * 3 + thirds_new) > (stops_old * 3 + thirds_old)) {
            thirds_old += 1;
            if (thirds_old < 3) {
                flash(pin_third);
            }
            else {
                thirds_old = 0;
                stops_old += 1;
                flash(pin_stop);
            }
        }
    }
    else {
        // going down
        while ((stops_new * 3 + thirds_new) < (stops_old * 3 + thirds_old)) {
            thirds_old -= 1;
            if (thirds_old >= 0) {
                flash(pin_third);
            }
            else {
                thirds_old = 2;
                stops_old -= 1;
                flash(pin_stop);
            }
        }
    }

    value_old = INVALID_VALUE;
}

void ExposureGauge::getExposureStops(int value,
        int *stops, int *thirds) const
{
    // split the useful range from 1 sec (value 0)
    // to 6553.6 sec (109 minutes, 12 2/3 stops more)
    int value_thirds = ((long)value * 39) / 1024;
    *stops = value_thirds / 3;
    *thirds = value_thirds % 3;
}

void ExposureGauge::flash(pin_t pin) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
}

