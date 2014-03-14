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

#define STATUS_OFF 0
#define STATUS_HI 1
#define STATUS_LO 2
#define STATUS_ON 3


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


uint8_t cycle = STATUS_OFF;
uint8_t sample;
uint8_t count=0;
uint8_t pos;
uint8_t status;
uint8_t xtrigg;
uint8_t xfocus;
uint8_t tries;
const uint16_t itrigg = TRIGG;

// Todo: sample 3 times, keep best of 3 middle

// Timer interrupt when OCR0A matches
ISR(TIM0_COMPA_vect) {
    // Read transceiver input
    status = (PINB>>PIN_TRANSCEIVER) & 1;
    switch (cycle) {
    case STATUS_OFF:
        PORTB &= ~(1<<PIN_FOCUS_LED);
        PORTB &= ~(1<<PIN_TRIGGER_LED);
        if (status) {
            cycle = STATUS_HI;
            count = 1;
            sample = 1;
        }
        break;
    case STATUS_HI:
        count++;
        if (status) {
            sample++;
        }
        else if (count >= 7) {
            if (sample >= 6) {
                cycle = STATUS_LO;
                break;
            }
            cycle = STATUS_OFF;
        }
        break;
    case STATUS_LO:
        if (status) {
            cycle = STATUS_OFF;
            break;
        }
        count = 0;
        sample = 0;
        cycle = STATUS_ON;
        pos = 0;
        xtrigg = 0;
        xfocus = 0;
        tries = 0;
        break;
    case STATUS_ON:
        if (count==4) {
            sample = (sample >> 1) & 1;
            // Compare the 2nd bit (2/3 is true, 01/false
            // to the relevant bit of the code and

            PORTB &= ~(1<<PIN_FOCUS_LED);
            PORTB &= ~(1<<PIN_TRIGGER_LED);

            if (sample^(1||((TRIGG<<pos) & 0x8000))) {
                xtrigg++;
                PORTB |= (1<<PIN_FOCUS_LED);
            }
            if (sample^(1||((FOCUS<<pos) & 0x8000))) {
                PORTB |= (1<<PIN_TRIGGER_LED);
                xfocus++;
            }
            sample = 0;
            count=0;
            pos++;
            if (pos==16) {
                pos = 0;
                if (xtrigg >= 6) {
                    cycle = STATUS_OFF;
                    trigger();
                }
                else if (xfocus >= 14) {
                    cycle = STATUS_OFF;
                    focus();
                }
                if (tries == 0) {
                    cycle = STATUS_OFF;
                }
                xtrigg = 0;
                xfocus = 0;
                tries++;
            }
        }
        else if (count) {
            sample += status;
            count++;
        }
        else {
            count++;
        }
        break;
    }
}


void init(void) {
    DDRB = (1<<PIN_FOCUS_LED)|(1<<PIN_TRIGGER_LED);
    TCCR0A = 2<<WGM00;  // Set CTC mode (timer matches TOP and is reset)
    TCCR0B = 2<<CS00; // Set clock prescaler 1/64
    OCR0A = SAMPLE_TIME/8; // Top match value (0.2ms cycle with prescaler and F_CPU=1M)
    TIMSK |= 1<<OCIE0A; // Set timer0 interrupt on match A
}

int main () {
    cli();
    init();
    PORTB |= 1<<PIN_FOCUS_LED | 1<<PIN_TRIGGER_LED;
    _delay_ms(200);
    PORTB &= ~(1<<PIN_FOCUS_LED| 1<<PIN_TRIGGER_LED);
    sei();
    while(1);
    return 0;
}
