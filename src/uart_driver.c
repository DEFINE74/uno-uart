/** UART Driver
 * We need 4 registers:
 *
 * 1 - UBRR0 (USART Baud Rate Register), 16 bits = UBRR0H + UBRR0L
 * UBRR0H - Most Significant Bit; UBRR0L - Least Significant Bit
 * UBRR0 is: 0000 0000 0000 0000
 *           ^       ^ ^       ^
 *            UBRR0H     UBRR0L
 *
 *
 * 2 - UCRS0A (USART Control and Status Register A) with main bit -
 * UDRE0 - TX bit
 * if 1 -> buffer is empty; if 0 -> USART is busy, and second main bit
 * RXC0 - RX bit
 * if 1 -> we have new data; if 0 -> buffer is empty
 *
 * 3 - UCSR0B (USART Control and Status Register B) with main bit -
 * TXEN0 (Transmitter Enable) and RXEN0 (Receiver Enable)
 *
 *
 * 4 - UDR0 (USART I/O Data Register)
 */
#define F_CPU 16000000UL

#include "uart_driver.h"

#include <assert.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

// uart configuration
#define UBRRn_VALUE(baud) ((F_CPU / (16UL * baud)) - 1)

// buffer configuration
#define BUFFER_SIZE	   4
#define IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)
_Static_assert(IS_POWER_OF_TWO(BUFFER_SIZE),
	       "error: buffer size should be a power of two");

#define PORT_B_LED_INDEX 5

struct ring_buffer {
	volatile uint8_t buffer[BUFFER_SIZE];
	volatile uint8_t head;
	volatile uint8_t tail;
};
struct ring_buffer rb = {{0}, 0, 0};

ISR(USART_RX_vect)
{
	uint8_t data = UDR0;
	uint8_t new_head = (rb.head + 1) & (BUFFER_SIZE - 1);
	if (new_head == rb.tail) {
		return;
	}

	rb.buffer[rb.head] = data;
	rb.head = new_head;
}

static bool uart_read_from_buffer(uint8_t *out_data)
{
	if (rb.tail == rb.head) {
		return false;
	}
	uint8_t new_tail = (rb.tail + 1) & (BUFFER_SIZE - 1);

	*out_data = rb.buffer[rb.tail];
	rb.tail = new_tail;

	return true;
}

static void uart_init()
{
	// Initialise UBRR0 Register (UBRR0L + UBRR0H)
	// Baudrate is calculated by datasheet formula
	uart_set_baud(UBRRn_VALUE(9600));

	// Initialise Transmitter and Receiver Registers
	// (TXEN0 + RXEN0 + RXCIE0)
	// With TX and RX and interrupts USCR0B is: 1001 1000
	uart_enable_tx_rx();
	// Make the UART Frame format
	// UCSR0C with bits is: 0000 0110
	uart_frame_format();

	// Allow interrupts
	sei();
}
static void uart_transmit(uint8_t data)
{
	// waiting when UCSR0A register ~ will be like: 0000 0000
	// that means with UDR0 bit, our UCSR0A:        0010 0000
	while (!(UCSR0A & (1 << UDRE0))) {
	}
	// writing data to USART data register
	UDR0 = data;
}
// static uint8_t uart_receive(void)
// {
// 	// waiting when UCSR0A register will be like: 0000 0000
// 	// that means with RXC0 bit, our UCSR0A:      1000 0000
// 	while (!(UCSR0A & (1 << RXC0))) {
// 	}

// 	// we are returning the USART data register state
// 	// for example: UDR0: 0000 1100 -> we will get 12
// 	return UDR0;
// }

int main(void)
{
	uart_init();

	DDRB = (1 << 5);
	PORTB &= ~(1 << PORT_B_LED_INDEX);

	while (1) {
		uint8_t received_byte;
		if (uart_read_from_buffer(&received_byte)) {
			uart_transmit(received_byte);

			PORTB |= (1 << PORT_B_LED_INDEX);
			_delay_ms(50);
			PORTB &= ~(1 << PORT_B_LED_INDEX);
		}
	}
	return 0;
}