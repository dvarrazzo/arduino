#include "ardulib/Pot.h"

#include "Arduino.h" // for analogRead

void Pot::read()
{
    if (!isAttached())
    {
        return;
    }

    value = analogRead(pin);
}
