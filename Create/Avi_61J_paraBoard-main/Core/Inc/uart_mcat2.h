#ifndef UART_MCAT2_H
#define UART_MCAT2_H

void uart_putc(uint8_t c, uint8_t select);

void uart_puts(char *str, uint8_t select);

void uart_putln(char *str, uint8_t select);

void uart_putint(int n, uint8_t select);

void uart_putintln(int n, uint8_t select);

void uart_putHEX(int n, uint8_t select);

char uart_receive(uint8_t select);

#endif