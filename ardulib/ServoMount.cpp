#include "ardulib/ServoMount.h"

#include <wiring.h>             // for millis()

ServoMount::ServoMount()
    : direction(1), ramp_t0(-1)
{ }

void ServoMount::attach(pin_t pin)
{
    servo.attach(pin);
}

void ServoMount::setRange(int min_us, int max_us)
{
    this->min_us = min_us;
    this->max_us = max_us;
}

void ServoMount::reverse()
{
    direction *= -1;
}

void ServoMount::setTrim(int trim_us)
{
    this->trim_us = trim_us;
}

void ServoMount::setSpeed(float speed)
{
    int us = speedToUSec(speed);
    setUSec(us);
}

void ServoMount::setUSec(int us)
{
    us += trim_us;
    servo.writeMicroseconds(us);
}

int ServoMount::speedToUSec(float speed)
{
    speed *= direction;
    float alpha = (speed - SPEED_MIN) / (SPEED_MAX - SPEED_MIN);
    return alpha * (max_us - min_us) + min_us;
}

void ServoMount::makeRamp(long length_ms, float speed)
{
    ramp_t0 = millis();
    ramp_tf = ramp_t0 + length_ms;
    ramp_start = servo.readMicroseconds();
    ramp_end = speedToUSec(speed);
}

void ServoMount::goRamp()
{
    if (ramp_t0 < 0) {
        return;
    }

    long t = millis();
    int us;
    if (t < ramp_tf) {
        float alpha = (float)(t - ramp_t0) / (ramp_tf - ramp_t0);
        us = alpha * (ramp_end - ramp_start) + ramp_start;
    }
    else
    {
        us = ramp_end;
        ramp_t0 = -1;
    }

    setUSec(us);
}
