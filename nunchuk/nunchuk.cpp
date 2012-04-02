// From http://www.windmeadow.com/node/42#comment-28
//
// read out a Wii Nunchuk controller
// adapted to work with wireless Nunchuk controllers of third party vendors by Michael Dreher

#include "ardulib/Nunchuk.h"
#include <Wire.h>
#include <string.h>
#include <utility/twi.h>
#include <stdio.h>

#undef SHOW_ACC_RGB
#undef SHOW_JOY_XY
#define SHOW_ACC_XY
#define SHOW_BTN_CZ

// if i want to convert acc to rgb led
#ifdef SHOW_ACC_RGB
static const uint8_t PIN_LED_R = 10;
static const uint8_t PIN_LED_G = 11;
static const uint8_t PIN_LED_B = 12;
#endif

// if i want to display the joystick or acc into 4 leds
#if defined(SHOW_JOY_XY) || defined(SHOW_ACC_XY)
static const uint8_t PIN_LED_XP = 6;
static const uint8_t PIN_LED_XN = 5;
static const uint8_t PIN_LED_YP = 10;
static const uint8_t PIN_LED_YN = 11;
#endif

// if i want to display the C and Z buttons
#ifdef SHOW_BTN_CZ
static const uint8_t PIN_LED_C = 2;
static const uint8_t PIN_LED_Z = 8;
#endif

// Forward declarations
void print_nunchuk(const Nunchuk &nun);

static Nunchuk nck;

void setup()
{
    Serial.begin (19200);
    Serial.println("Starting setup");

    Wire.begin(); // initialize i2c

    nck.init();

    // display the identification bytes, must be "00 00 A4 20 00 00" for the Nunchuk
    uint8_t outbuf[WII_TELEGRAM_LEN]; // array to store arduino output
    if (nck.readControllerIdent(outbuf) == 0)
    {
        Serial.print("Ident=");
        for (byte i = 0; i < WII_TELEGRAM_LEN; i++)
        {
            Serial.print(outbuf[i], HEX);
            Serial.print(' ');
        }
        Serial.println();
    }
    Serial.println("Finished setup");

#ifdef SHOW_ACC_RGB
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
#endif

#if defined(SHOW_JOY_XY) || defined(SHOW_ACC_XY)
    pinMode(PIN_LED_XP, OUTPUT);
    pinMode(PIN_LED_XN, OUTPUT);
    pinMode(PIN_LED_YP, OUTPUT);
    pinMode(PIN_LED_YN, OUTPUT);
#endif

#ifdef SHOW_BTN_CZ
    pinMode(PIN_LED_C, OUTPUT);
    pinMode(PIN_LED_Z, OUTPUT);
#endif

}

void loop()
{
    if (nck.read())
    {
        print_nunchuk(nck);

#ifdef SHOW_ACC_RGB
        analogWrite(PIN_LED_R, nck.accX() >> 2);
        analogWrite(PIN_LED_G, nck.accY() >> 2);
        analogWrite(PIN_LED_B, nck.accZ() >> 2);
#endif

#ifdef SHOW_JOY_XY
        if (nck.joyX() < 125) {
            analogWrite(PIN_LED_XN, (125 - nck.joyX()) * 2);
            analogWrite(PIN_LED_XP, 0);
        }
        else if (nck.joyX() > 140) {
            analogWrite(PIN_LED_XN, 0);
            analogWrite(PIN_LED_XP, (nck.joyX() - 140) * 2);
        }
        else
        {
            analogWrite(PIN_LED_XN, 0);
            analogWrite(PIN_LED_XP, 0);
        }

        if (nck.joyY() < 125) {
            analogWrite(PIN_LED_YN, (125 - nck.joyY()) * 2);
            analogWrite(PIN_LED_YP, 0);
        }
        else if (nck.joyY() > 140) {
            analogWrite(PIN_LED_YN, 0);
            analogWrite(PIN_LED_YP, (nck.joyY() - 140) * 2);
        }
        else {
            analogWrite(PIN_LED_YN, 0);
            analogWrite(PIN_LED_YP, 0);
        }
#endif

#ifdef SHOW_ACC_XY
        if (nck.accX() < 512 - 10) {
            analogWrite(PIN_LED_XN, (512 - nck.accX()) / 2);
            analogWrite(PIN_LED_XP, 0);
        }
        else if (nck.accX() > 512 + 10) {
            analogWrite(PIN_LED_XN, 0);
            analogWrite(PIN_LED_XP, (nck.accX() - 512) / 2);
        }
        else
        {
            analogWrite(PIN_LED_XN, 0);
            analogWrite(PIN_LED_XP, 0);
        }

        if (nck.accY() < 512 - 10) {
            analogWrite(PIN_LED_YN, (512 - nck.accY()) / 2);
            analogWrite(PIN_LED_YP, 0);
        }
        else if (nck.accY() > 512 + 10) {
            analogWrite(PIN_LED_YN, 0);
            analogWrite(PIN_LED_YP, (nck.accY() - 512) / 2);
        }
        else {
            analogWrite(PIN_LED_YN, 0);
            analogWrite(PIN_LED_YP, 0);
        }
#endif

#ifdef SHOW_BTN_CZ
        digitalWrite(PIN_LED_C, nck.btnC() ? HIGH : LOW);
        digitalWrite(PIN_LED_Z, nck.btnZ() ? HIGH : LOW);
#endif
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
