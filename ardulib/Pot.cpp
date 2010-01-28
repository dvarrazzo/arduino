#include "ardulib/Pot.h"

#include "wiring.h" // for analogRead

Pot::Pot()
    : pin(INVALID_PIN)
{ }

void Pot::attach(pin_t pin)
{
    this->pin = pin;
}

bool Pot::isAttached() const
{
    return pin != INVALID_PIN;
}

void Pot::read()
{
    if (!isAttached()) {
        return;
    }

    value = analogRead(pin);
}

int Pot::getValue() const
{
    return value;
}

