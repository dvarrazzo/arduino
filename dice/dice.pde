/*
 * Cast a 6 face dice.
 */

/* The dice is made up by leds attached to pins 1-7 of a 595 chip
 * (pin 0 is unused) in the following disposition:
 *
 *   1   4
 *   2 7 5
 *   3   6
 */

#define array_length(a) (sizeof(a)/sizeof((a)[0]))

#define dot(n) (1<<(n))
static const uint8_t FACE_VALUES[] = {
    dot(7),
    dot(1) | dot(6),
    dot(1) | dot(7) | dot(6),
    dot(1) | dot(3) | dot(4) | dot(6),
    dot(1) | dot(3) | dot(4) | dot(6) | dot(7),
    dot(1) | dot(2) | dot(3) | dot(4) | dot(5) | dot(6) };

typedef uint8_t pin_t;

/* Arduino is connected to the 595 using the following pins: */
static const pin_t PIN_DATA = 2;
static const pin_t PIN_CLOCK = 3;
static const pin_t PIN_LATCH = 4;

/* A switch rolls the dice */
static const pin_t PIN_BUTTON = 5;

// Forward declarations
void shift_write(uint8_t n);
void wait_for_pin_state(pin_t pin, uint8_t state);
void wait_for_button_click(void);

void test_leds_loop(void);
void test_dice_loop(void);
void test_dice_click(void);


void setup(void)
{
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);
    pinMode(PIN_BUTTON, INPUT);

    // Clear the displayed value
    shift_write(0);
}

void loop(void)
{
    test_dice_click();

}

/* Write a value in a shift register */
void shift_write(uint8_t n)
{
    digitalWrite(PIN_LATCH, LOW);
    for (uint8_t mask = 1 << 7; mask; mask >>= 1)
    {
        digitalWrite(PIN_DATA, (n & mask) ? HIGH : LOW);
        digitalWrite(PIN_CLOCK, HIGH);
        delay(1);
        digitalWrite(PIN_CLOCK, LOW);
    }
    digitalWrite(PIN_LATCH, HIGH);
}

/* Loop single leds. */
void test_leds_loop(void)
{
    for (int i = 0; i < 8; ++i)
    {
        shift_write(1 << i);
        delay(1000);
    }
}

/* Loop dice values */
void test_dice_loop(void)
{
    for (size_t i = 0; i < array_length(FACE_VALUES); ++i)
    {
        shift_write(FACE_VALUES[i]);
        delay(1000);
    }
}

/* Loop dice values, wait for an user click between states` */
void test_dice_click(void)
{
    for (size_t i = 0; i < array_length(FACE_VALUES); ++i)
    {
        shift_write(FACE_VALUES[i]);
        wait_for_button_click();
    }
}

/* Wait for a click, then return */
void wait_for_button_click(void)
{
    // First wait for the button to be up
    wait_for_pin_state(PIN_BUTTON, LOW);

    // Then wait for a button pression
    wait_for_pin_state(PIN_BUTTON, HIGH);
}

/* Wait until an input pin acquires a certain state. */
inline void wait_for_pin_state(pin_t pin, uint8_t state)
{
    while (digitalRead(pin) != state)
    {
        delay(50);
    }
}

