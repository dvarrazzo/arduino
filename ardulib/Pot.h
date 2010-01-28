#ifndef POT_H
#define POT_H

#include "ardulib/utils.h"

class Pot
{
    public:
        Pot();

        void attach(pin_t pin);
        bool isAttached() const;

        void read();
        int getValue() const;

    protected:
       pin_t pin;
       int value;
};

#endif // POT_H
