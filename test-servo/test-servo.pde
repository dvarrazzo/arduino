/* Simple program to test a continuous servo.
 * A potentiometer sets the speed of the servo.
 */
#include <Servo.h>

static Servo servo;

static const int poten_pin = 2;
static const int servo_pin = 12;

static const int servo_stop = 1500;
static const int servo_min = servo_stop - 500;
static const int servo_max = servo_stop + 500;

void setup ()
{
	Serial.begin (19200);
	servo.attach(servo_pin, servo_min, servo_max);
}

void loop()
{
    const int flat_zone = 50;

	int pval = analogRead(poten_pin);
	int val = map(pval, 0, 1023, 
            servo_min - flat_zone, servo_max + flat_zone);

    if (val <= servo_stop)
    {
        val += flat_zone;
        if (val > servo_stop) { val = servo_stop; }
    }
    else
    {
        val -= flat_zone;
        if (val < servo_stop) { val = servo_stop; }
    }

	Serial.print (pval, DEC);
	Serial.print ("\t");
	Serial.print (val, DEC);
	Serial.print ("\n");

	servo.writeMicroseconds(val);
	delay(50);
}
