#ifndef DICE_H
#define DICE_H

#include <stdint.h>

class Dice
{
    public:
        Dice();

        enum State
        {
            Stopped,
            Rolling,
            Stopping
        };

        void update(void);
        inline State state(void) const
        {
            return _state;
        }

        void set_state(const State state);

    protected:
        State _state;
        int step_delay;
        uint8_t _face_idx;

    public:
        uint8_t face_idx() const
        {
            return _face_idx;
        }

        void set_face_idx(const uint8_t new_face_idx)
        {
            _face_idx = new_face_idx;
        }

        void random_face(void);
};

#endif // DICE_H
