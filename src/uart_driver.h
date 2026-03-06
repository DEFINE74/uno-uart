#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <avr/io.h>

#define UART_UBRR_HIGH_REG UBRR0H
#define UART_UBRR_LOW_REG  UBRR0L

#define UART_CONTROL_REG      UCSR0B
#define UART_ENABLE_TX_BIT    TXEN0
#define UART_ENABLE_RX_BIT    RXEN0
#define UART_RX_INTERRUPT_BIT RXCIE0

#define UART_PROTOCOL_REG   UCSR0C
#define UART_CHAR_SIZE_BIT1 UCSZ01
#define UART_CHAR_SIZE_BIT2 UCSZ00

static inline void uart_set_baud(uint16_t ubrr_value)
{
	UART_UBRR_HIGH_REG = (uint8_t) (ubrr_value >> 8);
	UART_UBRR_LOW_REG = (uint8_t) ubrr_value;
}
static inline void uart_enable_tx_rx(void)
{
	UCSR0B = (1 << UART_ENABLE_TX_BIT) | (1 << UART_ENABLE_RX_BIT) |
		 (1 << UART_RX_INTERRUPT_BIT);
}

static inline void uart_frame_format(void)
{
	UART_PROTOCOL_REG =
	    (1 << UART_CHAR_SIZE_BIT1) | (1 << UART_CHAR_SIZE_BIT2);
}

#endif