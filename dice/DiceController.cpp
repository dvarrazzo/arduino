#include "DiceController.h"

#include "Dice.h"
#include "ardulib/ShiftReg.h"
#include "ardulib/Button.h"

#define dot(n) (1<<(n))
const uint8_t DiceController::FACE_VALUES[] = {
    dot(7),
    dot(1) | dot(6),
    dot(1) | dot(7) | dot(6),
    dot(1) | dot(3) | dot(4) | dot(6),
    dot(1) | dot(3) | dot(4) | dot(6) | dot(7),
    dot(1) | dot(2) | dot(3) | dot(4) | dot(5) | dot(6) };

DiceController::DiceController(Dice *dice, ShiftReg *display, Button *button)
    : dice(dice), display(display), button(button)
{ }

void DiceController::update(void)
{
    switch (dice->state())
    {
        case Dice::Stopped:
            if (button->isDown())
            {
                dice->set_state(Dice::Rolling);
            }
            break;

        case Dice::Rolling:
            if (button->isUp())
            {
                dice->set_state(Dice::Stopping);
            }
            break;

        case Dice::Stopping:
            break;
    }
    dice->update();
    display->write(FACE_VALUES[dice->face_idx()]);
}


