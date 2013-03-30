/*
 * RGB led controlled by three potentiometers.
 */

// input analog pins
static const int pin_in_r = 2;
static const int pin_in_g = 1;
static const int pin_in_b = 0;

// output analog pins
static const int pin_out_r = 9;
static const int pin_out_g = 10;
static const int pin_out_b = 11;

void setup()
{
    Serial.begin(19200);

    pinMode(pin_in_r, INPUT);
    pinMode(pin_in_g, INPUT);
    pinMode(pin_in_b, INPUT);

    pinMode(pin_out_r, OUTPUT);
    pinMode(pin_out_g, OUTPUT);
    pinMode(pin_out_b, OUTPUT);
}

void loop()
{
    int val_r = analogRead(pin_in_r) / 4;
    int val_g = analogRead(pin_in_g) / 4;
    int val_b = analogRead(pin_in_b) / 4;

    Serial.print(val_r, DEC);
    Serial.print(", ");
    Serial.print(val_g, DEC);
    Serial.print(", ");
    Serial.println(val_b, DEC);

    analogWrite(pin_out_r, val_r);
    analogWrite(pin_out_g, val_g);
    analogWrite(pin_out_b, val_b);

    delay(100);     // ms
}

