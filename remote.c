#include <stdlib.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>
#include <avr/interrupt.h>
#include "trigger.h"

#define PIN_TRIGG PB3
#define PIN_FOCUS PB4
#define PIN_EMITT PB0

uint8_t status;

ISR(PCINT0_vect) {
    int i;
    status = PINB;

    // trigger
    if (status) {
        // Initialize
        for (i=1;i<21;i++) {
            // Leading digit 
            if (i%2) {
                PORTB |= 1<<PIN_EMITT;
            }
            else {
                PORTB &= ~(1<<PIN_EMITT);
            }
            _delay_us(SAMPLE_TIME+JITTER);
        }
        PORTB |= 1<<PIN_EMITT;
        _delay_us(7*SAMPLE_TIME+JITTER);
        PORTB &= ~(1<<PIN_EMITT);
        _delay_us(2*SAMPLE_TIME+JITTER);
        
        if ((1<<PIN_TRIGG)&status) {
            // Repeat 3 times to make sure it gets it.
            for (i=0;i<48;i++) {
                // Leading digit 
                if (TRIGG<<(i%16) & 0x8000) {
                    PORTB |= 1<<PIN_EMITT;
                }
                else {
                    PORTB &= ~(1<<PIN_EMITT);
                }
                _delay_us(5*SAMPLE_TIME);
            }
        }
        else if ((1<<PIN_FOCUS)&status) {
            for (i=0;i<48;i++) {
                // Leading digit 
                if (FOCUS<<(i%16) & 0x8000) {
                    PORTB |= 1<<PIN_EMITT;
                }
                else {
                    PORTB &= ~(1<<PIN_EMITT);
                }
                _delay_us(5*SAMPLE_TIME);
            }
        }
        // No repeats for 300ms
        PORTB &= ~(1<<PIN_EMITT);
        _delay_ms(300);
    }

}

int main () {
    cli();
    DDRB = 1<<PIN_EMITT;
    GIMSK = 1<<PCIE;
    PCMSK = (1<<3)|(1<<4);
    sei();
    while (1);
    return 0;
}
