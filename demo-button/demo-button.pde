/* Demo to show how to use the Button class. */
#include "ardulib/Button.h"

#include "ardulib/utils.h"

static const pin_t button_pin = 5;

static Button button;

void setup()
{
    Serial.begin(19200);
    button.attach(button_pin);
}

void loop()
{
    Serial.print("Pin state: ");
    Serial.print((int)digitalRead(button_pin), DEC);

    button.read();

    Serial.print(" isDown: ");
    Serial.print((int)button.isDown(), DEC);

    Serial.print(" isUp: ");
    Serial.print((int)button.isUp(), DEC);

    Serial.print(" isPressed: ");
    Serial.print((int)button.isPressed(), DEC);

    Serial.print(" isReleased: ");
    Serial.print((int)button.isReleased(), DEC);

    Serial.print("\n");
    delay(100);
}

