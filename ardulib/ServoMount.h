#ifndef SERVO_MOUNT_H
#define SERVO_MOUNT_H

#include "ardulib/utils.h"

#include <Servo.h>

typedef int servo_speed_t;

class ServoMount
{
    public:
        ServoMount();

        void attach(pin_t pin);
        void setRange(int min_us, int max_us);
        void setTrim(int trim_us);
        void reverse();

        static const servo_speed_t SPEED_MIN = -1000;
        static const servo_speed_t SPEED_MAX = 1000;
        void setSpeed(servo_speed_t speed);

        int getUSec();
        void setUSec(int us);

        void makeRamp(long length_ms, servo_speed_t speed);
        void goRamp();

    protected:
        int speedToUSec(servo_speed_t speed);

    protected:
        Servo servo;

    private:
        int min_us;
        int max_us;
        int trim_us;
        char direction;

        static const long NO_RAMP = -1;
        int ramp_start_us;
        int ramp_end_us;
        long ramp_t0;
        long ramp_length_ms;
        long ramp_m;
};


inline
void ServoMount::setRange(int min_us, int max_us)
{
    this->min_us = min_us;
    this->max_us = max_us;
}

inline
void ServoMount::reverse()
{
    direction *= -1;
}

inline
void ServoMount::setTrim(int trim_us)
{
    this->trim_us = trim_us;
}

#endif // SERVO_MOUNT_H

