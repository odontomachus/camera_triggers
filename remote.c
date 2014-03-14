#include "rf.h"
#include "comm.h"

#include <stdlib.h>
#define F_CPU 1000000UL  // 1 MHz

#include <avr/io.h>
#include <util/delay.h>

#define PIN_PHOTO PB4
#define PIN_FOCUS PB2

#define STATE_WAITING 0
#define STATE_INIT 1
#define STATE_READY 2
#define STATE_RECV 3

typedef struct {
    uint8_t comm;
    uint8_t last;
    uint8_t status;
    uint8_t count;
    uint8_t position;
    uint8_t data;
} state_t;

/**
 * Analyze latest received sample.
 */
void receive(&state) {
    // run for DATA*2 flips, counting duration
    if (state.comm^state.last) { // switch
        state.last = state.comm;
        if (position%2 == 1) {
            state.data <<= 1;
            if (count <= 9) {// short
                state.data += 1;
            }
        }
        if (position==16) {
            // Done receiving
            if (state.data == photo) {
                PORTB |= (1<<PIN_PHOTO);
                _delay_ms(140);
                PORTB &= ~(1<<PINPHOTO);
            }
            state.position = 0;
            state.count = 0;
            state.status = STATE_WAITING;
            state.comm = 0;
            state.last = 0;
            state.data = 0;
            }
        else {
            state.position++;
            state.count = 0;
        }
    }
    if (state.comm) {
        state.count++;
    }

}


void main() {
    state_t state = {0};
    DDRB |= (1<<PIN_COMM);
    while (1) {
        state.comm = PINB & (1<<PIN_COMM);
        switch (state.status) {
        // DEFAULT
        case STATE_WAITING:
            if (state.comm) {
                state.count = 1;
                state.status = STATE_INIT;
            }
            break;
        // LONG ON
        case STATE_INIT:
            if (state.comm) {
                state.count++;
                if (state.count > 64) {
                    state.count = 0;
                    state.status = STATE_WAITING;
                }
            }
            else {
                if (state.count > 30) {
                    state.count = 0;
                    state.status = STATE_RECV;
                }
            }
            break;
        case STATE_READY:
            if (state.comm) {
                state.status = STATE_RECV;
                state.count = 1;
                state.data = 0;
            }
            else {
                state.count++;
                if (state.count > 8) {
                    state.count = 0;
                    state.status = STATE_WAITING;
                }
                break;
            }
        case STATE_RECV:
            receive(&state);
            break;
        }
    }
}
