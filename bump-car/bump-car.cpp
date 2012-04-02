/* A car that changes direction when hits an obstacle.
 */
#include "ardulib/TwoServosCar.h"
#include "ardulib/Pot.h"

#include "ardulib/utils.h"

static const pin_t left_servo_pin = 3;
static const pin_t right_servo_pin = 2;
static const pin_t start_button_pin = 5;
static const pin_t trim_pot_pin = 5; // analog

static const pin_t sense_left_pin = 9;
static const pin_t sense_right_pin = 8;

static TwoServosCar car;
static Button sense_left;
static Button sense_right;

enum State { Attack, Retreat };

static State state;

static const int RETREAT_DURATION_MS = 1000;
static int retreat_until;

void attack()
{
    Serial.println("attack");
    state = Attack;
    car.accelerate(1000, ServoMount::SPEED_MAX);
}

void retreat(int speed_left, int speed_right)
{
    state = Retreat;
    car.setSpeed(speed_left, speed_right);
    retreat_until = millis() + RETREAT_DURATION_MS;
}

void retreat_left()
{
    Serial.println("retreat left");
    retreat(ServoMount::SPEED_MIN / 2, ServoMount::SPEED_MIN / 8);
}

void retreat_right()
{
    Serial.println("retreat right");
    retreat(ServoMount::SPEED_MIN / 8, ServoMount::SPEED_MIN / 2);
}

void setup()
{
    Serial.begin(19200);

    car.attach(left_servo_pin, right_servo_pin);
    car.attachEnableButton(start_button_pin);
    car.attachTrimPot(trim_pot_pin);
    car.setRange(1000, 2000);

    sense_left.attach(sense_left_pin);
    sense_right.attach(sense_right_pin);

    attack();
}

void loop() {
    switch (state)
    {
        case Attack:
            sense_left.read();
            sense_right.read();
            if (sense_left.isPressed())
            {
                retreat_right();
            }
            else if (sense_right.isPressed())
            {
                retreat_left();
            }
            break;
        case Retreat:
            if (millis() >= retreat_until)
            {
                attack();
            }
            break;
    }
    car.pulse();
    delay(20);
}


