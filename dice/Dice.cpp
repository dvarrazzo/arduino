#include "Dice.h"

#include "WProgram.h" // for random()
#include "wiring.h" // for delay()

Dice::Dice()
    : _state(Stopped), _face_idx(0)
{}

void Dice::update(void)
{
    switch (state())
    {
        case Stopped:
            break;

        case Rolling:
            delay(step_delay);
            random_face();
            break;

        case Stopping:
            step_delay += 25;
            delay(step_delay);
            random_face();
            if (step_delay >= 300)
            {
                set_state(Stopped);
            }
            break;
    }
}

void Dice::set_state(const State state)
{
    _state = state;
    switch (state)
    {
        case Stopped:
            break;
        case Rolling:
            step_delay = 50;
            break;
        case Stopping:
            break;
    }
}

void Dice::random_face(void)
{
    // Avoid extracting the same value consecutively
    uint8_t new_face_idx;

    do {
        new_face_idx = random(6);
    } while (new_face_idx == face_idx());

    set_face_idx(new_face_idx);
}

