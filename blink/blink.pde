/* The hello world of arduino.
 *
 * Stick a led between pin 13 and ground (pin 13 has already a resistence).
 *
 * I'm doing this to try to remember something after many months.
 */

static const int led_pin = 13;

static int i;

void setup()
{
    pinMode(led_pin, OUTPUT);
    Serial.begin(19200);

    i = 0;
}

void loop()
{
    Serial.print("hi ");
    Serial.println(i, DEC);
    digitalWrite(led_pin, HIGH);
    delay(1000);

    Serial.print("lo ");
    Serial.println(i, DEC);
    digitalWrite(led_pin, LOW);
    delay(1000);

    ++i;
}

