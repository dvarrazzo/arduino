#ifndef TWO_SERVOS_CAR_H
#define TWO_SERVOS_CAR_H

#include "ardulib/utils.h"
#include "ardulib/ServoMount.h"
#include "ardulib/Button.h"
#include "ardulib/Pot.h"

class TwoServosCar
{
    public:
        TwoServosCar();

        void attach(pin_t pin_left, pin_t pin_right);
        void attachEnableButton(pin_t pin);
        void attachTrimPot(pin_t pin);

        void setRange(int min_us, int max_us);
        void reverseServos();

        void pulse();

        void setSpeed(float speed);
        void setSpeed(float speed_left, float speed_right);
        void accelerate(unsigned long length_ms, float speed);
        void accelerate(unsigned long length_ms, float speed_left, float speed_right);

        // should be for debug only
        void setUSec(int left_us, int right_us);

    protected:
        ServoMount& leftServo();
        ServoMount& rightServo();

    protected:
        ServoMount servos[2];
        Button enable_button;
        Pot trim_pot;

    protected:
        bool enabled;
};


inline ServoMount& TwoServosCar::leftServo()
{
    return servos[0];
}

inline ServoMount& TwoServosCar::rightServo()
{
    return servos[1];
}

#endif // TWO_SERVOS_CAR_H
