#include <stdio.h>
#include "main.h"
#include "uart_mcat2.h"
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;
void uart_putc(uint8_t c, uint8_t select){
	char buf[1];
	buf[0] = c;
	if(select == 2){
		HAL_UART_Transmit(&huart2, (uint8_t *)buf, sizeof(buf), 100);
	} else if(select == 6) {
		HAL_UART_Transmit(&huart6, (uint8_t *)buf, sizeof(buf), 100);
	}
}
void uart_puts(char *str, uint8_t select){
	while(*str){
		uart_putc(*str++, select);
	}
}
void uart_putln(char *str, uint8_t select){
	uart_puts(str, select);
	uart_puts("\r\n", select);
}
void uart_putint(int n, uint8_t select){
	char buf[10];
	itoa(n,buf,10);
	uart_puts(buf, select);
}
void uart_putintln(int n, uint8_t select){
	uart_putint(n, select);
	uart_puts("\r\n", select);
}

void uart_putHEX(int n, uint8_t select){
	char buf[2];
	itoa(n,buf,16);
	uart_puts(buf, select);
}

char uart_receive(uint8_t select){
	char v[1];	
	if(select == 2){
		HAL_UART_Receive( &huart2, (uint8_t*) v, 1, 100);
	} else if(select == 6) {
		HAL_UART_Receive( &huart6, (uint8_t*) v, 1, 100);
	}
	return v[0];
}
