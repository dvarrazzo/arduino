#ifndef BUTTON_H
#define BUTTON_H

#include "ardulib/utils.h"

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

    protected:
       pin_t pin;
       bool prev_val;
       bool curr_val;
};

#endif // BUTTON_H
