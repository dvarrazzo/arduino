#include "ardulib/ServoMount.h"

#include <wiring.h>             // for millis(), constrain()

ServoMount::ServoMount()
    : direction(1), ramp_t0(NO_RAMP)
{ }

void ServoMount::attach(pin_t pin)
{
    servo.attach(pin);
}

void ServoMount::setSpeed(servo_speed_t speed)
{
    speed = constrain(speed, SPEED_MIN, SPEED_MAX);
    int us = speedToUSec(speed);
    setUSec(us);
}

void ServoMount::setUSec(int us)
{
    servo.writeMicroseconds(us + trim_us);
}

int ServoMount::getUSec()
{
    return servo.readMicroseconds() - trim_us;
}

int ServoMount::speedToUSec(servo_speed_t speed)
{
    speed *= direction;
    return (long)(speed - SPEED_MIN) * (max_us - min_us)
        / (SPEED_MAX - SPEED_MIN) + min_us;
}

static const long RAMP_SCALE = 1024;

void ServoMount::makeRamp(long length_ms, servo_speed_t speed)
{
    ramp_t0 = millis();
    ramp_start_us = getUSec();
    ramp_end_us = speedToUSec(speed);
    ramp_m = ((ramp_end_us - ramp_start_us) * RAMP_SCALE) / length_ms;
    ramp_length_ms = length_ms;
}

void ServoMount::goRamp()
{
    if (NO_RAMP == ramp_t0)
    {
        return;
    }

    long t = millis() - ramp_t0;

    int us;
    if (t < ramp_length_ms)
    {
        us = ((ramp_m * t) / RAMP_SCALE) + ramp_start_us;
    }
    else
    {
        us = ramp_end_us;
        ramp_t0 = NO_RAMP;
    }

    setUSec(us);
}
