#ifndef BUTTON_H
#define BUTTON_H

#include "ardulib/utils.h"

class Button
{
    public:
        Button(pin_t pin);
        bool is_down(void);
        bool is_up(void);

    protected:
       pin_t pin;
};

#endif // BUTTON_H
