#ifndef BUTTON_H
#define BUTTON_H

#include "ardulib/utils.h"

#include "Arduino.h" // for pinMode, digitalRead

class Button
{
    public:
        Button();

        void attach(pin_t pin);
        bool isAttached() const;

        void read();
        bool isDown() const;
        bool isUp() const;
        bool isPressed() const;
        bool isReleased() const;

    private:
       pin_t pin;
       bool prev_val;
       bool curr_val;
};


inline
Button::Button()
    : pin(INVALID_PIN)
{ }

inline
bool Button::isAttached() const
{
    return pin != INVALID_PIN;
}

inline
bool Button::isDown() const
{
    return curr_val;
}

inline
bool Button::isUp() const
{
    return !curr_val;
}

inline
bool Button::isPressed() const
{
    return curr_val && !prev_val;
}

inline
bool Button::isReleased() const
{
    return !curr_val && prev_val;
}

#endif // BUTTON_H
