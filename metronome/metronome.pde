/*
 * Metronome program.
 *
 * Time is marked by a led and a piezo emitting a tick tune.
 *
 * A button allows to pause the metronome.
 */

static const int led_pin = 3;
static const int piezo_pin = 5;
static const int pause_pin = 7;
static const int pot_pin = 0;

static const int bpm_min = 40;
static const int bpm_max = 208;

/* half period in us
 * 1136 = A - see http://www.oomlout.com/oom.php/products/ardx/circ-06 */
static const unsigned long tick_tone = 1136;
static const unsigned long tick_ms = 50;

void beep(unsigned long ms, unsigned long tone)
{
    unsigned long end = ms + millis();

    while (millis() < end) {
        digitalWrite(piezo_pin, HIGH);
        delayMicroseconds(tone);
        digitalWrite(piezo_pin, LOW);
        delayMicroseconds(tone);
    }
}

void setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(piezo_pin, OUTPUT);
    pinMode(pause_pin, INPUT);
    Serial.begin(19200);
}

void loop()
{
    /* wait until the pause button is unpressed */
    while (digitalRead(pause_pin) == LOW);

    /* read the bpm from the pot */
    long pot_val = analogRead(pot_pin);
    int bpm = (bpm_max - bpm_min) * pot_val / 1023.0 + bpm_min;
    Serial.println(bpm, DEC);

    /* calculate when the beat will end */
    unsigned long delay_ms = 1000.0 * 60.0 / bpm;
    unsigned long end_ms = millis() + delay_ms;

    /* play/display the beat */
    digitalWrite(led_pin, HIGH);
    beep(tick_ms, tick_tone);
    digitalWrite(led_pin, LOW);

    /* delay until the rest of the beat */
    delay(end_ms - millis());
}

