/* Read the temperature from a TMP36 sensor.
 *
 * Sensor is attached to 5v.
 *
 * http://www.ladyada.net/learn/sensors/tmp36.html
 */

static const int thermo_pin = 0;

void setup()
{
    Serial.begin(19200);
}

void loop()
{
    int reading = analogRead(thermo_pin);
    // Serial.print("read: "), Serial.println(reading);

    float voltage = reading * 5.0 / 1024;
    // Serial.print("volt: "); Serial.println(voltage);

    float temp_c = voltage * 100 - 50;
    // Serial.print("temp: "); Serial.println(temp_c);
    Serial.println(temp_c);

    delay(1000);
}
