/*
 * Laser trap.
 *
 * Trigger upon laser interruption.
 */

static const int laser_pin = 9;
static const int led_pin = 8;
static const int photores_pin = 0;

static const int photores_threshold = 5;


/*
 * Return the minimum value read from the photores.
 *
 * Keep the reading for *msec* millisecs.
 */

int getMinLevel(unsigned long msec)
{
    int min = 1024;

    /* waste some of the time: if the function is called straight after the
     * laser is turned on, a dimmed value will be read, making the trap less
     * sensitive.
     */
    delay(msec / 2);

    unsigned long end_msec = millis() + msec / 2;
    do {
        int value = analogRead(photores_pin);
        if (value < min) {
            min = value;
        }
    } while (millis() < end_msec);

    return min;
}


/*
 * Turn on and off the laser
 */

int setLaser(bool on)
{
    digitalWrite(laser_pin, on ? LOW : HIGH);
}

/*
 * Arm the laser trap.
 *
 * Read the minimum value from the laser, then block until something further
 * lowering the minimum read is detected.
 */
void trap()
{
    // turn the laser on
    setLaser(true);

    // read the minimum level for a second
    int min = getMinLevel(1000);
    Serial.print("min: ");
    Serial.println(min, DEC);

    // flash to confirm the minimum read
    digitalWrite(led_pin, HIGH);
    delay(50);
    digitalWrite(led_pin, LOW);

    // wait until the level goes below the threshold
    int level = min;
    int tmin = 1024;
    int i = 0;
    while (level > min - photores_threshold) {
        level = analogRead(photores_pin);

        /*
        // for photores debugging
        if (level < tmin) {
            tmin = level;
        }
        if (++i >= 10000) {
            i = 0;
            Serial.print("current min: ");
            Serial.println(tmin, DEC);
            tmin = 1024;
        }
        */
    }

    /* Serial.print("level: "); */
    /* Serial.println(level, DEC); */
}


void setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(laser_pin, OUTPUT);
    Serial.begin(19200);

    trap();
    digitalWrite(led_pin, HIGH);
}

void loop()
{
}

/* used to study the values from the photores. */
void debug_loop()
{
    int i;
    int min = 1024, max = -1;
    for (i = 0; i < 10000; i++) {
        long value = analogRead(photores_pin);
        if (value < min) min = value;
        if (value > max) max = value;
    }
    Serial.print("min: ");
    Serial.print(min, DEC);
    Serial.print(" max: ");
    Serial.println(max, DEC);
}


