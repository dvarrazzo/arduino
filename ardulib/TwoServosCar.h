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

        void setSpeed(servo_speed_t speed);
        void setSpeed(servo_speed_t speed_left, servo_speed_t speed_right);
        void accelerate(unsigned long length_ms, servo_speed_t speed);
        void accelerate(unsigned long length_ms, servo_speed_t speed_left, servo_speed_t speed_right);

        // should be for debug only
        void setUSec(int left_us, int right_us);

    protected:
        ServoMount& leftServo();
        ServoMount& rightServo();

    protected:
        ServoMount servos[2];
        Button enable_button;
        Pot trim_pot;

    private:
        bool enabled;
};


inline
ServoMount& TwoServosCar::leftServo()
{
    return servos[0];
}

inline
ServoMount& TwoServosCar::rightServo()
{
    return servos[1];
}

inline
void TwoServosCar::attach(pin_t pin_left, pin_t pin_right)
{
    leftServo().attach(pin_left);
    rightServo().attach(pin_right);
}

inline
void TwoServosCar::attachEnableButton(pin_t pin)
{
    enable_button.attach(pin);
}

inline
void TwoServosCar::attachTrimPot(pin_t pin)
{
    trim_pot.attach(pin);
}

inline
void TwoServosCar::reverseServos()
{
    leftServo().reverse();
    rightServo().reverse();
}

inline
void TwoServosCar::setSpeed(servo_speed_t speed)
{
    setSpeed(speed, speed);
}

inline
void TwoServosCar::accelerate(unsigned long length_ms, servo_speed_t speed)
{
    accelerate(length_ms, speed, speed);
}

inline
void TwoServosCar::accelerate(unsigned long length_ms, servo_speed_t speed_left, servo_speed_t speed_right)
{
    leftServo().makeRamp(length_ms, speed_left);
    rightServo().makeRamp(length_ms, speed_right);
}

#endif // TWO_SERVOS_CAR_H
