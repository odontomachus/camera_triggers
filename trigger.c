#include "rf.h"

void send(uint8_t byte) {
    DDRB |= (1<<PIN_COMM);
    delay_ms(LONG);
    DDRB &= ~(1<<PIN_COMM);
    delay_ms(short);
    for (uint8_t i=0; i<DATA; i++) {
        send_bit(byte&1);
        byte >>=1;
        string++;
    }
    DDRB |= 1<<PIN_COMM;
    delay_ms(SHORT);
    DDRB &= ~(1<<PIN_COMM);
    delay_ms(SHORT);
}

void send_bit(uint8_t bit) {
    DDRB |= 1<<PIN_COMM;
    delay_ms(SHORT);
    DDRB &= ~(1<<PIN_COMM);
    if (bit) {
        delay_ms(LONG);
    }
    else {
        delay_ms(SHORT);
    }
}

