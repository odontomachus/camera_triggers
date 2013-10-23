#include <stdlib.h>

#define F_CPU 1000000UL  // 1 MHz
#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#define PIN_TRIGGER PB2
#define PIN_FOCUS PB3
#define PIN_TRIGGER_LED PD3
#define PIN_FOCUS_LED PD4


unsigned int ubrr;

void USART_Init( unsigned long baud )
{
    /* Set baud rate (using u2x=1 doubles effective baud rate) */
    ubrr = (unsigned long) (F_CPU/8/baud) - 1;
    UBRRH = (unsigned char) ubrr>>8;
    UBRRL = (unsigned char) ubrr;
    /* Enable receiver and transmitter */
    UCSRB = (1<<RXEN)|(1<<TXEN);
    /* Set frame format: 
     * 8 bit data (UCSZ2:0 = 0b011) 
     * 1 stop bit (USBS = 0) 
     * Async. op (UMSEL = 0) 
     * Odd parity (UPM1:0 = 0b11)*/ 
    UCSRC = (3<<UCSZ0)|(3<<UPM0);
    UCSRA = 1<<U2X; // Double speed
}

void USART_Transmit( unsigned char data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSRA & (1<<UDRE)) )
        ;
    /* Put data into buffer, sends the data */
    UDR = data;
}

unsigned char USART_Receive( void )
{
    /* Wait for data to be received */
    while ( !(UCSRA & (1<<RXC)) )
        ;
    /* Get and return received data from buffer */
    return UDR;
}

void USART_Flush( void )
{
    unsigned char dummy;
    while ( UCSRA & (1<<RXC) ) dummy = UDR;
}

int main () {
    unsigned char message;
    DDRD |= (1<<PIN_FOCUS_LED)|(1<<PIN_TRIGGER_LED);
    PORTD |= (1<<PIN_FOCUS_LED)|(1<<PIN_TRIGGER_LED);
    DDRB = 0;
    USART_Init(9600);
    // Give the bluetooth time to boot...
    _delay_ms(1500);
    USART_Transmit(*"O");
    _delay_ms(15);
    USART_Transmit(*"K");
    _delay_ms(200);
    PORTD &= ~((1<<PIN_FOCUS_LED)|(1<<PIN_TRIGGER_LED));
    while (1) {
        message = USART_Receive();
        if (message == *"P") {
            PORTD |= 1<<PIN_TRIGGER_LED;
            DDRB |= 1<<PIN_TRIGGER;
            _delay_ms(200);
            DDRB &= ~(1<<PIN_TRIGGER);
            PORTD &= ~(1<<PIN_TRIGGER_LED);
            USART_Flush();
        }
        if (message == *"F") {
            PORTD |= 1<<PIN_FOCUS_LED;
            DDRB |= 1<<PIN_FOCUS;
            ~((1<<PIN_FOCUS)|(1<<PIN_FOCUS));
            _delay_ms(50);
        }
        else {
            DDRB &= ~(1<<PIN_FOCUS);
            PORTD &= ~(1<<PIN_FOCUS_LED);
            USART_Flush();
        }
        _delay_ms(1);

    }
}
