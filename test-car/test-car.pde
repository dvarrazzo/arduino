/* Test for my first car
 */
#include "ardulib/TwoServosCar.h"
#include "ardulib/Pot.h"

#include "ardulib/utils.h"

static const pin_t left_servo_pin = 2;
static const pin_t right_servo_pin = 3;
static const pin_t start_button_pin = 5;
static const pin_t trim_pot_pin = 5; // analog

static const pin_t speed_pot_pin = 4; // analog

static TwoServosCar car;
static Pot speed_pot;

void setup()
{
    Serial.begin(19200);

    car.attach(left_servo_pin, right_servo_pin);
    car.attachEnableButton(start_button_pin);
    car.attachTrimPot(trim_pot_pin);
    car.setRange(1000, 2000); // trimmed: 1010, 2030);
    car.reverseServos();

    speed_pot.attach(speed_pot_pin);
}

void loop_trim()
{
    speed_pot.read();
    servo_speed_t speed = (speed_pot.getValue() - 512) * 2;

    car.setSpeed(speed);
    car.pulse();

    Serial.print("speed: ");
    Serial.print(analogRead(speed_pot_pin), DEC);
    Serial.print(" trim: ");
    Serial.print(analogRead(trim_pot_pin), DEC);
    Serial.print("\n");
}

void keep_for(int ms)
{
    long tf = millis() + ms;
    while (millis() < tf)
    {
        car.pulse();
        delay(50);
    }
}

void loop_ramp()
{
    car.accelerate(1000, ServoMount::SPEED_MAX);
    keep_for(1000);
    car.accelerate(1000, 0);
    keep_for(1000);
    car.accelerate(1000, ServoMount::SPEED_MAX, ServoMount::SPEED_MIN);
    keep_for(1000);
    car.accelerate(1000, 0);
    keep_for(1000);
}

void loop()
{
    // loop_ramp();
    loop_trim();
}
