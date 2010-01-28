#include "ardulib/TwoServosCar.h"

TwoServosCar::TwoServosCar()
    : enabled(true)
{
    rightServo().reverse();
}

void TwoServosCar::attach(pin_t pin_left, pin_t pin_right)
{
    leftServo().attach(pin_left);
    rightServo().attach(pin_right);
}

void TwoServosCar::attachEnableButton(pin_t pin)
{
    enable_button.attach(pin);
}

void TwoServosCar::attachTrimPot(pin_t pin)
{
    trim_pot.attach(pin);
}

void TwoServosCar::setRange(int min_us, int max_us)
{
    leftServo().setRange(min_us, max_us);
    rightServo().setRange(min_us, max_us);
    setSpeed(0.0);
}

void TwoServosCar::reverseServos()
{
    leftServo().reverse();
    rightServo().reverse();
}

void TwoServosCar::pulse()
{
    // Check if the car must be stopped
    if (enable_button.isAttached()) {
        enable_button.read();
        if (enable_button.isPressed())
        {
            if (enabled) {
                setSpeed(0.0);
            }
            enabled = !enabled;
        }
    }

    // Correct the servo trim according to the pot reading
    if (trim_pot.isAttached())
    {
        trim_pot.read();
        int trim_us = (trim_pot.getValue() - 512) / 10;
        leftServo().setTrim(trim_us);
        rightServo().setTrim(trim_us);
    }

    // Change servos speed if a ramp has been programmed
    if (enabled)
    {
        leftServo().goRamp();
        rightServo().goRamp();
    }
}

void TwoServosCar::setSpeed(float speed)
{
    setSpeed(speed, speed);
}

void TwoServosCar::setSpeed(float speed_left, float speed_right)
{
    if (!enabled) { return; }

    leftServo().setSpeed(speed_left);
    rightServo().setSpeed(speed_right);
}

void TwoServosCar::setUSec(int left_us, int right_us)
{
    if (!enabled) { return; }

    leftServo().setUSec(left_us);
    rightServo().setUSec(right_us);
}

void TwoServosCar::accelerate(unsigned long length_ms, float speed)
{
    accelerate(length_ms, speed, speed);
}

void TwoServosCar::accelerate(unsigned long length_ms, float speed_left, float speed_right)
{
    leftServo().makeRamp(length_ms, speed_left);
    rightServo().makeRamp(length_ms, speed_right);
}
