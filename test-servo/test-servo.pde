#include <Servo.h>

static Servo servo;

static const int poten_pin = 2;
static const int servo_pin = 12;

static const int servo_min = 1500 - 1000;
static const int servo_max = 1500 + 1000;

void setup ()
{
	Serial.begin (19200);
	servo.attach(servo_pin, servo_min, servo_max);
}

void loop()
{
	int pval = analogRead(poten_pin);
	int val = (float)val / 1024.0 * 2000. + 500.;

	Serial.print (pval, DEC);
	Serial.print ("\t");
	Serial.print (val, DEC);
	Serial.print ("\n");
	
	servo.writeMicroseconds(val);
	delay(50);
}
