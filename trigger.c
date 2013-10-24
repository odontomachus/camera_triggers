#include <stdlib.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>
#include <avr/interrupt.h>
#include "trigger.h"

#define PIN_TRIGGER PB1
#define PIN_FOCUS PB0
#define PIN_TRIGGER_LED PB3
#define PIN_FOCUS_LED PB4
#define PIN_TRANSCEIVER PB2



/**
 * Note: this function needs to be wrapped in (cli(), sei()).
 */
void trigger(void) {
    DDRB |= 1<<PIN_TRIGGER;
    PORTB |= 1<<PIN_TRIGGER_LED;
    _delay_ms(200);
    DDRB &= ~(1<<PIN_TRIGGER);
    PORTB &= ~(1<<PIN_TRIGGER_LED);
}

/**
 * Note: this function needs to be wrapped in (cli(), sei()).
 * Note: This is a terrible way to focus (blocking, short).
 * There should be a release event? or a timeout, but it should be interrupt driven.
 */
void focus(void) {
    DDRB |= 1<<PIN_FOCUS;
    PORTB |= 1<<PIN_FOCUS_LED;
    _delay_ms(200);
    DDRB &= ~(1<<PIN_FOCUS);
    PORTB &= ~(1<<PIN_FOCUS_LED);
}


uint16_t bitstream = 0;

uint8_t i;
uint8_t err_trigg;
uint8_t err_focus;
uint8_t status;
uint16_t xtrigg;
uint16_t xfocus;

// Timer interrupt when OCR0A matches
ISR(TIM0_COMPA_vect) {
// Read transceiver input
    status = (PINB>>PIN_TRANSCEIVER) & 1;
    // Add it to the bitstream
    bitstream <<= 1;
    bitstream += status;

    // Compare to trigger and focus signals
    xtrigg = bitstream^TRIGG;
    xfocus = bitstream^FOCUS;
    // Do action if we have a match
    if (!xtrigg) {
        trigger();
    }
    else if (!xfocus) {
        focus();
    }
    // Find close matches
    else {
        err_trigg = 0;
        err_focus = 0;
        for (i=0; i<16; i++) {
            err_trigg += (xtrigg >> i)&1;
            err_focus += (xfocus >> i)&1;
        }
        // Close enough, take picture
        if (err_trigg < 3)
            trigger();
        else if (err_focus < 3)
            focus();
    }
}


void init(void) {
    DDRB = (1<<PIN_FOCUS_LED)|(1<<PIN_TRIGGER_LED);
    TCCR0A = 2<<WGM00;  // Set CTC mode (timer matches TOP and is reset)
    TCCR0B = 1<<CS01; // Set clock prescaler 1/8
    OCR0A = 62; // Top match value (0.5ms cycle with prescaler and F_CPU=1M)
    TIMSK = 1<<OCIE0A; // Set timer0 interrupt on match A
}

int main () {
    init();
    sei();
    while(1);
    return 0;
}
