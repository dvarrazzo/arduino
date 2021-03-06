/*
 * LUCKYLARRY.CO.UK - IR Remote control for Nikon using Arduino
 *
 * Mimics the infrared signal to trigger the remote for any Nikon camera which
 * can use the ML-L1 and ML-L3 remotes. Can be used as an intervalometer for
 * time lapse photography.
 *
 * The IR sequence I used is originally taken from:
 * http://www.bigmike.it/ircontrol/ You should be able to use my pulse methods
 * to alter to suit other cameras/ hardware.  micros() is an Arduino function
 * that calls the time in Microseconds since your program first ran. Arduino
 * doesn't reliably work with microseconds so we work our timings by taking
 * the current reading and then adding our delay on to the end of it rather
 * than rely on the in built timer.
 */

#ifndef NIKON_REMOTE_H
#define NIKON_REMOTE_H

#include "ardulib/utils.h"

class NikonRemote
{
    public:
        NikonRemote();

        void attach(pin_t pin);
        bool isAttached() const;
        bool isShooting() const;

        void shoot();
        void shootBulb(unsigned long ms);
        void pulse();

    private:
        pin_t pin;
        unsigned long shoot_end_ms;
        bool is_shooting;

        void pulseOn(unsigned long pulse_us);
        void pulseOff(unsigned long delay_us);
};

inline
NikonRemote::NikonRemote()
    : pin(INVALID_PIN), is_shooting(false)
{ }

inline
bool NikonRemote::isAttached() const
{
    return pin != INVALID_PIN;
}

inline
bool NikonRemote::isShooting() const
{
    return is_shooting;
}

#endif // NIKON_REMOTE_H

