#include <stdlib.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>
#include <avr/interrupt.h>
#include "trigger.h"

#define PIN_TRIGG PB3
#define PIN_FOCUS PB4
#define PIN_EMITT PB0

void trigger(void) {
    int i;
    for (i=0;i<16;i++) {
        if ((TRIGG<<i) & 0x8000)
            DDRB |= 1<<PIN_EMITT;
        else
            DDRB &= ~(1<<PIN_EMITT);
        _delay_us(490);
    }
    DDRB &= ~(1<<PIN_EMITT);
}

void focus(void) {
    int i;
    for (i=0;i<16;i++) {
        if ((FOCUS<<i) & 0x8000)
            DDRB |= 1<<PIN_EMITT;
        else
            DDRB &= ~(1<<PIN_EMITT);
        _delay_us(4900);
    }
    DDRB &= ~(1<<PIN_EMITT);
}


uint8_t status;

ISR(PCINT0_vect) {
    status = PINB;
    if (status & (1<<PIN_TRIGG)) {
        // Repeat a few times to make sure they got it.
        trigger();
        _delay_ms(2);
        trigger();
        _delay_ms(2);
        trigger();
    }
    else if (status & (1<<PIN_FOCUS)) {
        focus();
        _delay_ms(2);
        focus();
        _delay_ms(2);
        focus();
    }
}

int main () {
    PORTB = 1<<PIN_EMITT;
    GIMSK = 1<<PCIE;
    PCMSK = (1<<3)|(1<<4);
    sei();
    while (1);
    return 0;
}
