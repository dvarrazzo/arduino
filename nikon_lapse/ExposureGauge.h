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

        int getExposureThirds(int value) const;
        void flash(pin_t pin);
};

inline
ExposureGauge::ExposureGauge()
    : pin_stop(INVALID_PIN), pin_third(INVALID_PIN),
        value_old(0)
{ }

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

#endif // EXPOSURE_GAUGE_H

