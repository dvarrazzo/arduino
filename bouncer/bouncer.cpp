/* Turn on a led when it's dark, turn it off when there is light.
 *
 * Why does dad want to do such thing?
 */

static const int led_pin = 8;
static const int photo_pin = 0;

int lo_thr;
int hi_thr;

void setup()
{
    pinMode(led_pin, OUTPUT);
    Serial.begin(19200);

    lo_thr = 650;
    hi_thr = 700;
}

void loop()
{
    int photo_level;

    photo_level = analogRead(photo_pin);
    Serial.println(photo_level, DEC);

    if (photo_level < lo_thr) {
        digitalWrite(led_pin, HIGH);
    }
    else if (photo_level > hi_thr) {
        digitalWrite(led_pin, LOW);
    }

    delay(100);
}

