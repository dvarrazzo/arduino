#include "ShiftReg.h"

#include "wiring.h" // for pinMode

ShiftReg::ShiftReg(const pin_t data_pin, const pin_t clock_pin, const pin_t latch_pin)
    : data_pin(data_pin), clock_pin(clock_pin), latch_pin(latch_pin)
{
    pinMode(data_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(latch_pin, OUTPUT);
}

void ShiftReg::write(const uint8_t n)
{
    digitalWrite(latch_pin, LOW);
    for (uint8_t mask = 1 << 7; mask; mask >>= 1)
    {
        digitalWrite(data_pin, (n & mask) ? HIGH : LOW);
        digitalWrite(clock_pin, HIGH);
        delay(1);
        digitalWrite(clock_pin, LOW);
    }
    digitalWrite(latch_pin, HIGH);
}
