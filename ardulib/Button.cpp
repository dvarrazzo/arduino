#include "ardulib/Button.h"

#include "wiring.h" // for pinMode, digitalRead

Button::Button(pin_t pin)
    : pin(pin)
{
    pinMode(pin, INPUT);
}

bool Button::is_down(void)
{
    return (HIGH == digitalRead(pin));
}

bool Button::is_up(void)
{
    return (LOW == digitalRead(pin));
}
