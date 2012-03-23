#include "ShiftReg.h"

#include "Arduino.h" // for pinMode, digitalWrite

void ShiftReg::attach(pin_t data_pin, pin_t clock_pin, pin_t latch_pin)
{
    this->data_pin = data_pin;
    pinMode(data_pin, OUTPUT);

    this->clock_pin = clock_pin;
    pinMode(clock_pin, OUTPUT);

    this->latch_pin = latch_pin;
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
