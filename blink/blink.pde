/* The hello world of arduino.
 *
 * Stick a led between pin 13 and ground (pin 13 has already a resistence).
 *
 * I'm doing this to try to remember something after many months.
 */

static const int led_pin = 13;

void setup()
{
    pinMode(led_pin, OUTPUT);
}

void loop()
{
    digitalWrite(led_pin, HIGH);
    delay(1000);
    digitalWrite(led_pin, LOW);
    delay(1000);
}

