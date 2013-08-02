// Control an Eitech Crane using a Nunchuck
//
// Adapted from http://www.windmeadow.com/node/42

#include "ardulib/Nunchuk.h"
#include <Wire.h>
#include <string.h>
#include <utility/twi.h>
#include <stdio.h>

// Output pins for the crane
static const uint8_t PIN_LEFT    = 6;
static const uint8_t PIN_RIGHT   = 2;
static const uint8_t PIN_FORWARD = 3;
static const uint8_t PIN_BACK    = 5;
static const uint8_t PIN_UP      = 8;
static const uint8_t PIN_DOWN    = 9;

// Thresholds to disengage the movements
static const int ACC_IDLE_X_MIN = 400;
static const int ACC_IDLE_X_MAX = 600;
static const int ACC_IDLE_Y_MIN = 400;
static const int ACC_IDLE_Y_MAX = 600;
static const int JOY_IDLE_MIN   = 100;
static const int JOY_IDLE_MAX   = 150;

// Forward declarations
void print_nunchuk(const Nunchuk &nun);

static Nunchuk nck;

void setup()
{
    Serial.begin(19200);

    Wire.begin(); // initialize i2c
    nck.init();

    pinMode(PIN_LEFT, OUTPUT);
    pinMode(PIN_RIGHT, OUTPUT);
    pinMode(PIN_FORWARD, OUTPUT);
    pinMode(PIN_BACK, OUTPUT);
    pinMode(PIN_UP, OUTPUT);
    pinMode(PIN_DOWN, OUTPUT);

    Serial.println("setup ok");

}

void loop()
{
    if (!nck.read()) return;

    print_nunchuk(nck);

    if (nck.btnC() || nck.btnZ()) {
        // a button is pressed: move

        // X accel to move left/right
        if (nck.accX() < ACC_IDLE_X_MIN) {
            Serial.print("left ");
            digitalWrite(PIN_LEFT, 1);
            digitalWrite(PIN_RIGHT, 0);
        }
        else if (nck.accX() > ACC_IDLE_X_MAX) {
            Serial.print("right ");
            digitalWrite(PIN_LEFT, 0);
            digitalWrite(PIN_RIGHT, 1);
        }
        else {
            digitalWrite(PIN_LEFT, 0);
            Serial.print("back ");
            digitalWrite(PIN_FORWARD, 0);
            digitalWrite(PIN_BACK, 1);
            digitalWrite(PIN_RIGHT, 0);
        }

        // Y accel to move forward/back
        if (nck.accY() < ACC_IDLE_Y_MIN) {
            Serial.print("back ");
            digitalWrite(PIN_FORWARD, 0);
            digitalWrite(PIN_BACK, 1);
        }
        else if (nck.accY() > ACC_IDLE_Y_MAX) {
            Serial.print("forward ");
            digitalWrite(PIN_FORWARD, 1);
            digitalWrite(PIN_BACK, 0);
        }
        else {
            digitalWrite(PIN_FORWARD, 0);
            digitalWrite(PIN_BACK, 0);
        }

        // Joy up/down to go up/down with the cable
        if (nck.joyY() < JOY_IDLE_MIN) {
            Serial.print("up ");
            digitalWrite(PIN_UP, 1);
            digitalWrite(PIN_DOWN, 0);
        }
        else if (nck.joyY() > JOY_IDLE_MAX) {
            Serial.print("down ");
            digitalWrite(PIN_UP, 0);
            digitalWrite(PIN_DOWN, 1);
        }
        else {
            digitalWrite(PIN_UP, 0);
            digitalWrite(PIN_DOWN, 0);
        }

        Serial.print("\r\n");
    }
    else {
        // no button pressed: stop everything
        digitalWrite(PIN_FORWARD, 0);
        digitalWrite(PIN_BACK, 0);
        digitalWrite(PIN_LEFT, 0);
        digitalWrite(PIN_RIGHT, 0);
        digitalWrite(PIN_UP, 0);
        digitalWrite(PIN_DOWN, 0);
    }
}

void print_nunchuk(const Nunchuk &nun)
{
    Serial.print(nun.joyX(), DEC);
    Serial.print("\t");

    Serial.print(nun.joyY(), DEC);
    Serial.print("\t");

    Serial.print(nun.accX(), DEC);
    Serial.print("\t");

    Serial.print(nun.accY(), DEC);
    Serial.print("\t");

    Serial.print(nun.accZ(), DEC);
    Serial.print("\t");

    Serial.print(nun.btnC(), DEC);
    Serial.print("\t");

    Serial.print(nun.btnZ(), DEC);
    Serial.print("\r\n");
}


// Michael
