/*
 * Measure the BPM of a rhythm.
 *
 * Tap on the button following the rhythm. BPM is printed on the serial.
 *
 * Use the same circuit of the metronome: the pause button is used to mark the
 * beats.
 */

static const int led_pin = 3;
static const int piezo_pin = 5;
static const int tap_pin = 7;

/* half period in us
 * 1136 = A - see http://www.oomlout.com/oom.php/products/ardx/circ-06 */
static const unsigned long tick_tone = 1136;
static const unsigned long tick_ms = 50;

/* Smoothing factor for bpm average.
 * With 0.25, a value contributes for about the 1% after 16 iterations. */
static const float smooth_alpha = 0.25;

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
    pinMode(tap_pin, INPUT);
    Serial.begin(19200);
}

void loop()
{
    static unsigned long prev_beat_ms = 0;
    static float bpm_avg = 0.0;
    unsigned long now_ms;

    do {
        /* wait until the tap button is pressed */
        while (digitalRead(tap_pin) == HIGH);

        /* take the time */
        now_ms = millis();

    /* guard from spurious signals from the button: repeat the read if
     * bpm seems > 300 */
    } while (prev_beat_ms != 0 && (now_ms - prev_beat_ms) <= 200);

    /* Mark the beat with a beep and a pulse */
    digitalWrite(led_pin, HIGH);
    beep(tick_ms, tick_tone);
    digitalWrite(led_pin, LOW);

    if (prev_beat_ms) {

        /* calculate the bpm */
        unsigned long len_ms = now_ms - prev_beat_ms;
        float bpm = 1000. * 60. / len_ms;

        /* smoothed average of the last bpm measures */
        if (bpm_avg != 0.0) {
            bpm_avg = smooth_alpha * bpm + (1.0 - smooth_alpha) * bpm_avg;
        }
        else {
            bpm_avg = bpm;
        }

        Serial.print("bpm: ");
        Serial.print(bpm);
        Serial.print(" avg: ");
        Serial.println(bpm_avg);
    }

    prev_beat_ms = now_ms;

    /* if the tap button is still pressed, wait for its release */
    while (digitalRead(tap_pin) == LOW);
}

