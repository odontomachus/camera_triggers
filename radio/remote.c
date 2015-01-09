// Remote

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

void timer_on() {
  // Every 256/1000 of a second
  TCNT1 = 0;
  TCCR1 |= 0b1011<<CS10;
}

void timer_off() {
  // Unset timer1 source
  TCCR1 &= ~(0b1111<<CS10);
}

// Interrupt on button change or timer overflow
ISR(PCINT0_vect) {
  // ~PINB to get falling edge since using pull-up.
  // Except laser button since it is high on standby.
  uint8_t status = (~PINB)^(1<<PIN_BUTTON_LASER);
  if (status & (1<<PIN_BUTTON_PHOTO | 1<<PIN_BUTTON_LASER)) {
    PORTB |= 1<<PIN_LIGHT;
    send(MESSAGE_PHOTO);
    send(MESSAGE_PHOTO);
    timer_on();
  }
  else if (status & (1<<PIN_BUTTON_FOCUS)) {
    send(MESSAGE_FOCUS);
    send(MESSAGE_FOCUS);
    timer_on();
  }
  // Release on rising edge or if released during send
  status = ~PINB^(1<<PIN_BUTTON_LASER);
  if (!(status & (1<<PIN_BUTTON_PHOTO|1<<PIN_BUTTON_FOCUS))) {
    
    PORTB &= ~(1<<PIN_LIGHT);
    send(MESSAGE_RELEASE);
    send(MESSAGE_RELEASE);
    send(MESSAGE_RELEASE);
    timer_off();
  }
}
ISR(TIM1_OVF_vect, ISR_ALIASOF(PCINT0_vect));

void main() {
  // Let time for voltage to stabilize
  DDRB |= 1<<PIN_COMM | 1<<PIN_LIGHT;
  PORTB |= 1<<PIN_LIGHT;
  _delay_ms(200);
  PORTB ^= 1<<PIN_LIGHT;
  // Set pull-up
  PORTB |= (1<<PIN_BUTTON_FOCUS)|(1<<PIN_BUTTON_PHOTO)|(1<<PIN_BUTTON_LASER);
  // Setup interrupts for button presses.
  PCMSK |= (1<<PIN_BUTTON_FOCUS)|(1<<PIN_BUTTON_PHOTO)|(1<<PIN_BUTTON_LASER);
  GIMSK |= (1<<PCIE);
  // Enable timer1 overflow interrupt
  TIMSK |= 1<<TOIE1;

  sei();
  while (1) {
    // Toggle pin every PULSE ms to keep active carrier signal.
    PORTB |= 1<<PIN_COMM;
    _delay_ms(PULSE);
    PORTB &= ~(1<<PIN_COMM);
    _delay_ms(30);
  }
}
