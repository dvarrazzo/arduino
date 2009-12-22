/* An object to control and display a dice.
 *
 * The dice is made up by leds attached to pins 1-7 of a 595 chip
 * (pin 0 is unused) in the following disposition:
 *
 *   1   4
 *   2 7 5
 *   3   6
 */

#ifndef DICECONTROLLER_H
#define DICECONTROLLER_H

#include <stdint.h>

class Dice;
class ShiftReg;
class Button;

class DiceController
{
    public:
        DiceController(Dice *dice, ShiftReg *display, Button *button);
        void update(void);
        static const uint8_t FACE_VALUES[];

    protected:
        Dice *dice;
        ShiftReg *display;
        Button *button;
};

#endif // DICECONTROLLER_H

