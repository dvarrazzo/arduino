#ifndef EXPOSURE_GAUGE_H
#define EXPOSURE_GAUGE_H

#include "ardulib/utils.h"

class ExposureGauge
{
    public:
        ExposureGauge();

        void attach(pin_t pin_stop, pin_t pin_third);
        bool isAttached() const;

        void setValue(int value);
        unsigned long getExposureMs() const;
        void display();

    private:
        pin_t pin_stop;
        pin_t pin_third;
        int value;           // 0-1023 range
        int value_old;

        void getExposureStops(int value, int *stops, int *thirds) const;
        void flash(pin_t pin);
};

inline
ExposureGauge::ExposureGauge()
    : pin_stop(INVALID_PIN), pin_third(INVALID_PIN),
        value_old(0)
{ }

inline
void ExposureGauge::attach(pin_t pin_stop, pin_t pin_third)
{
    this->pin_stop = pin_stop;
    this->pin_third = pin_third;
}

inline
bool ExposureGauge::isAttached() const
{
    return pin_stop != INVALID_PIN;
}

inline
void ExposureGauge::setValue(int value)
{
    this->value = value;
}

inline
unsigned long ExposureGauge::getExposureMs() const
{
    int stops, thirds;
    unsigned long rv;
    getExposureStops(value, &stops, &thirds);

    rv = 1000;
    while (stops > 0) {
        --stops;
        rv *= 2;
    }
    if (thirds == 1) { rv = rv * 5 / 4; }     // +25%
    if (thirds == 2) { rv = rv * 8 / 5; }     // +60%

    return rv;
}

#endif // EXPOSURE_GAUGE_H

