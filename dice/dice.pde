/*
 * Cast a 6 face dice.
 */

#include "DiceController.h"
#include "Dice.h"

#include "ardulib/ShiftReg.h"
#include "ardulib/Button.h"
#include "ardulib/utils.h"

/* Arduino is connected to the 595 using the following pins: */
static const pin_t PIN_DATA = 2;
static const pin_t PIN_CLOCK = 3;
static const pin_t PIN_LATCH = 4;

/* A switch rolls the dice */
static const pin_t PIN_BUTTON = 5;

static Dice dice;
static Button button(PIN_BUTTON);
static ShiftReg sreg(PIN_DATA, PIN_CLOCK, PIN_LATCH);
static DiceController controller(&dice, &sreg, &button);

void setup(void)
{
    pinMode(PIN_BUTTON, INPUT);
}

void loop(void)
{
    for (;;)
    {
        controller.update();
        delay(10);
    }
}

