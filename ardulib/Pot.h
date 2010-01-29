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

inline
Pot::Pot()
    : pin(INVALID_PIN)
{ }

inline
void Pot::attach(pin_t pin)
{
    this->pin = pin;
}

inline
bool Pot::isAttached() const
{
    return pin != INVALID_PIN;
}

inline
int Pot::getValue() const
{
    return value;
}

#endif // POT_H
