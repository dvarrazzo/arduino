#include "ardulib/Button.h"

#include "wiring.h" // for pinMode, digitalRead

void Button::attach(pin_t pin)
{
    pinMode(pin, INPUT);
    this->pin = pin;
}

void Button::read()
{
    if (!isAttached())
    {
        return;
    }

    prev_val = curr_val;
    curr_val = (LOW == digitalRead(pin));
}

