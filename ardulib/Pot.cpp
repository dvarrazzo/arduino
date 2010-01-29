#include "ardulib/Pot.h"

#include "wiring.h" // for analogRead

void Pot::read()
{
    if (!isAttached()) {
        return;
    }

    value = analogRead(pin);
}
