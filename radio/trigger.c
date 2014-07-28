#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>

#include "rf.h"

#define PIN_BUTTON_FOCUS PCINT1
#define PIN_BUTTON_PHOTO PCINT0
#define PIN_BUTTON_LASER PCINT2
#define PIN_LIGHT PB4

void send_bit(uint8_t bit) {
  PORTB &= ~(1<<PIN_COMM);
  _delay_ms(PULSE);
  PORTB |= 1<<PIN_COMM;
  if (bit) {
    _delay_ms(SHORT);
  }
  else {
    _delay_ms(LONG);
  }
  PORTB &= ~(1<<PIN_COMM);
  _delay_ms(PULSE);
}

void send(uint8_t byte) {
  uint8_t i;
  for (i=0; i<8; i++) {
    send_bit(byte&1);
    byte >>=1;
  }
}

//@TODO interrupt.
ISR(PCINT0_vect) {
  // ~PINB to get falling edge since using pull-up.
  uint8_t status = ~PINB;
  PORTB |= 1<<PIN_LIGHT;
  if (status & (1<<PIN_BUTTON_PHOTO)) {
    send(MESSAGE_PHOTO);
    PORTB ^= 1<<PIN_LIGHT;
  }
  // Trigger on rising edge since photo transistor on by default.
  // @TODO Fix this, triggers on release of other buttons now.
  // @solution: detect if it's low at start time and setup only in that situation.
  /* else if (~status & (1<<PIN_BUTTON_LASER)) */
  /*   send(MESSAGE_PHOTO); */
  else if (status & (1<<PIN_BUTTON_FOCUS)) {
    send(MESSAGE_FOCUS);
    // Check if released during message transmission
    uint8_t status = PINB;
    if (status & (1<<PIN_BUTTON_FOCUS)) {
      send(MESSAGE_RELEASE);
    }
  }
  // Release
  else {
    send(MESSAGE_RELEASE);
  }
}


void main() {
  // Let time for voltage to stabilize
  DDRB |= 1<<PIN_COMM | 1<<PIN_LIGHT;
  PORTB |= 1<<PIN_LIGHT;
  _delay_ms(500);
  PORTB ^= 1<<PIN_LIGHT;
  // Set pull-up
  PORTB |= (1<<PIN_BUTTON_FOCUS)|(1<<PIN_BUTTON_PHOTO);
  // Setup interrupts for button presses.
  PCMSK |= (1<<PIN_BUTTON_FOCUS)|(1<<PIN_BUTTON_PHOTO);
  GIMSK |= (1<<PCIE);
  sei();
  while (1) {
    // Toggle pin every PULSE ms to keep active carrier signal.
    PORTB |= 1<<PIN_COMM;
    _delay_ms(PULSE);
    PORTB &= ~(1<<PIN_COMM|1<<PIN_LIGHT);
    _delay_ms(30);
    send(MESSAGE_RELEASE);
  }
}
