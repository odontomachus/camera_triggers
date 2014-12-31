// Connects to camera

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "trigger.h"

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>

#include "rf.h"

#define PIN_PHOTO PB0
#define PIN_FOCUS PB1

#define PIN_LIGHT PB4

#define STATE_WAITING 0
#define STATE_INIT 1
#define STATE_READY 2
#define STATE_RECV 3

#define RCV_SHORT 1
#define RCV_LONG 2

//@TODO timed release using interrupts
ISR(TIM1_OVF_vect) {
  release();
}


uint8_t count() {
  uint8_t count = 0;
  uint8_t state = 0; 
  while (1) {
    state = PINB & (1<<PIN_COMM);
    if (state) {
      count++;
      count &= 15U;
    }
    else {
      if (count) {
        return count;
      }
    }
    _delay_us(SAMPLE);
  }
}

uint8_t interpret(uint8_t count) {
  if (count < 3) return 0;
  if (count < 7) return RCV_SHORT;
  if (count < 10) return RCV_LONG;
}

void photo() {
  // Set pin to output (ground it)
  // Set focus pin too to make sure camera is awake (focus pin is live
  // during camera sleep)
  DDRB |= (1<<PIN_FOCUS|1<<PIN_PHOTO);
  release_timer_on();
}

void focus() {
  DDRB |= (1<<PIN_FOCUS);
  release_timer_on();
}

void release() {
  // Release photo and focus pins
  DDRB &= ~(1<<PIN_FOCUS|1<<PIN_PHOTO);
  release_timer_off();
}

// Setup release countdown
void release_timer_on() {
  // set timer source to ck/16384 (1Mhz/16k -> 16.384ms/tick)
  // ovf @256 -> 4.194s/ovf
  TCNT1 = 0;
  TCCR1 |= 0b1111<<CS10;
}

void release_timer_off() {
  // Unset timer1 source
  TCCR1 &= ~(0b1111<<CS10);
}

/**
 * Analyze latest received sample.
 */
void listen() {
  uint8_t bit, bits, byte;
  bit = bits = byte = 0;
  while (1) {
    bit = interpret(count());
    if (!bit) {
      byte = bits = 0;
    }
    else {
      if (bit == RCV_SHORT) {
        byte |= 1<<bits;
      }
      bits++;
    }
    if (bits == 8) {
      switch (byte) {
      case MESSAGE_PHOTO:
        photo();
        break;
      case MESSAGE_FOCUS:
        focus();
        break;
      case MESSAGE_RELEASE:
        release();
        break;
      }
    }
  }
}


void main() {
  // Pins set to input (high resistance)
  _delay_ms(10);
  DDRB |= 1<<PIN_LIGHT;
  PORTB |= 1<<PIN_LIGHT;
  _delay_ms(500);
  PORTB ^= 1<<PIN_LIGHT;
  TIMSK |= (1<<TOIE1);
  sei();
  listen();  
}
