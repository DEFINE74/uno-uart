/** UART Driver
 * We need 4 registers:
 * 1 - UBRR0L (USART Baud Rate Register), 16 bits = UBRR0H + UBRR0L
 *
 * 2 - UCRS0A (USART Control and Status Register A) with main bit - UDRE0
 * if 1 -> buffer is empty; if 0 -> USART is busy
 *
 * 3 - UCSR0B (USART Control and Status Register B) with main bit TXEN0
 * (Transmitter Enable) and RXEN0 (Receiver Enable)
 *
 * 4 - UDR0 (USART I/O Data Register)
 */

#include <avr/io.h>

#define UART_BAUDRATE 9600
#define FREQUENCY     16000000

int main(void)
{
	return 0;
}