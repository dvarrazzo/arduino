#include "ardulib/Button.h"

#include "wiring.h" // for pinMode, digitalRead

Button::Button()
    : pin(INVALID_PIN)
{ }

void Button::attach(pin_t pin)
{
    pinMode(pin, INPUT);
    this->pin = pin;
}

bool Button::isAttached() const
{
    return pin != INVALID_PIN;
}

void Button::read()
{
    if (!isAttached()) {
        return;
    }

    prev_val = curr_val;
    curr_val = (LOW == digitalRead(pin));
}

bool Button::isDown() const
{
    return curr_val;
}

bool Button::isUp() const
{
    return !curr_val;
}

bool Button::isPressed() const
{
    return curr_val && !prev_val;
}

bool Button::isReleased() const
{
    return !curr_val && prev_val;
}
