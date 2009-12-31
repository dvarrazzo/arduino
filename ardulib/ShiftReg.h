#ifndef SHIFTREG_H
#define SHIFTREG_H

#include "ardulib/utils.h"

class ShiftReg
{
    public:
        ShiftReg(const pin_t data_pin, const pin_t clock_pin, const pin_t latch_pin);

        void write(const uint8_t n);

    protected:
        pin_t data_pin;
        pin_t clock_pin;
        pin_t latch_pin;
};

#endif // SHIFTREG_H