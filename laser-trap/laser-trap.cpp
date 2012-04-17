/*
 * Laser trap.
 *
 * Trigger upon laser interruption.
 */

static const int laser_pin = 9;
static const int led_pin = 8;
static const int flash_pin = 2;
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
 * Fire the laser and return the minimum light level.
 */
int armTrap()
{
    // turn the laser on
    setLaser(true);

    // read the minimum level for a second
    int min = getMinLevel(1000);
    Serial.print("min: ");
    Serial.println(min, DEC);

    return min;
}

/*
 * Flash the led for a certain duration.
 *
 * The led stays on for *ms*, then off for *ms*, so the function blocks
 * for 2 * *ms* millis.
 */
void flashLed(unsigned long ms)
{
    digitalWrite(led_pin, HIGH);
    delay(ms);
    digitalWrite(led_pin, LOW);
    delay(ms);
}


/*
 * Read if the trap has snapped for about a second.
 *
 * if the level falls below min, return true, else return false when
 * the time is over.
 */
bool checkTrap(int min)
{
    for (int i = 0; i < 10000; ++i) {
        int level = analogRead(photores_pin);
        if (level < min - photores_threshold) {
            setLaser(false);
            return true;
        }
    }
    return false;
}

/*
 * Fire the flash.
 */
void shoot()
{
    digitalWrite(flash_pin, HIGH);
    delay(100);
    digitalWrite(flash_pin, LOW);
}


/*
 * Read a "command" from the serial.
 *
 * A command is a string terminated by LF.
 */
const char *readCommand() {
    static char buffer[256];
    char *ptr = buffer;
    int c;

    while (-1 != (c = Serial.read())) {
        // command ends on LF
        if (c == '\x0a') {
            *ptr = '\x00';
            return buffer;
        }
        *ptr++ = (char)c;
    }

    // no command in the serial
    return NULL;
}

/*
 * Return *true* if 'buf' contains 'cmd'.
 */
bool isCommand(const char *cmd, const char *buf)
{
    return (0 == strncmp(cmd, buf, strlen(cmd)));
}

/*
 * Parse an unsigned long from a string.
 */
unsigned long parseULong(const char *str)
{
    unsigned long rv = 0UL;
    while (*str) {
        rv *= 10;
        rv += (*str++) - '0';
    }
    return rv;
}


void setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(flash_pin, OUTPUT);
    pinMode(laser_pin, OUTPUT);
    Serial.begin(19200);
}


void loop()
{
    int min = armTrap();
    unsigned long delay_ms = 0UL;

    // flash to confirm the minimum read
    flashLed(50);

    for (;;) {
        if (checkTrap(min)) {
            delay(delay_ms);
            shoot();
            break;
        }

        const char *cmd = readCommand();
        if (cmd) {
            if (isCommand("delay", cmd)) {
                delay_ms = parseULong(cmd + strlen("delay "));
                Serial.print("delay updated: ");
                Serial.println(delay_ms, DEC);
            }
            else {
                Serial.print("unknown command: ");
                Serial.println(cmd);
            }
        }
    }
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


