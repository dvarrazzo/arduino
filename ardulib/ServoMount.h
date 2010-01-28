#ifndef SERVO_MOUNT_H
#define SERVO_MOUNT_H

#include "ardulib/utils.h"

#include <Servo.h>

class ServoMount
{
    public:
        ServoMount();

        void attach(pin_t pin);
        void setRange(int min_us, int max_us);
        void setTrim(int trim_us);
        void reverse();

        void setSpeed(float speed);
        void setUSec(int us);

        void makeRamp(long length_ms, float speed);
        void goRamp();

    protected:
        Servo servo;
        int min_us;
        int max_us;
        int trim_us;
        int8_t direction;

        static const float SPEED_MIN = -1.0;
        static const float SPEED_MAX =  1.0;
        int speedToUSec(float speed);

    private:
        int ramp_start;
        int ramp_end;
        long ramp_t0;
        long ramp_tf;
};

#endif // SERVO_MOUNT_H

