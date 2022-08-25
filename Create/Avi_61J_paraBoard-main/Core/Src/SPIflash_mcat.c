#include <stdio.h>
#include "main.h"
#include "SPIflash_mcat.h"

SPI_HandleTypeDef hspi1; //���ۂɎg�p����spi�ɍ��킹�ĕύX ����c�t�@�C�����̑S�Ă�"hspi1"�����l�ɕύX

#define FLASH_CS 10
#define CMD_RDID 0x9f
#define CMD_READ 0x03
#define CMD_WREN 0x06
#define CMD_WRDI 0x04
#define CMD_P4E 0x20
#define CMD_P8E 0x40
#define CMD_BE 0x60
#define CMD_PP 0x02
#define CMD_RDSR 0x05

void CS_Reset(uint8_t n){ //ChipSelect�s����LOW�ɂ���
	if(n == 0){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //SPI_flash0 ���ۂɎg�p����s���ɍ��킹�ĕύX ��3�����l
	} else if(n == 1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); //SPI_flash1
	}
}
void CS_Set(uint8_t n){ //ChipSelect�s����HIGH�ɂ���
	if(n == 0){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //SPI_flash0 //���ۂɎg�p����s���ɍ��킹�ĕύX ��3�����l
	} else if(n == 1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); //SPI_flash1
	}
}

/*SPIflash�֌W*/
void spi_transfer(const uint8_t* tx_buf, uint8_t* rx_buf, uint16_t length_){
  for(uint16_t i=0; i < length_; ++i){
    uint8_t rdata,tdata;
    if(tx_buf == NULL){
    	tdata = 0x00;
    	HAL_SPI_TransmitReceive(&hspi1, &tdata, &rdata, 1, 1000);
    } else {
    	tdata = tx_buf[i];
    	HAL_SPI_TransmitReceive(&hspi1, &tdata, &rdata,1,1000);
    }

    if(rx_buf != NULL){
      rx_buf[i] = rdata;
    }
  }
}
void flash_send_cmd(uint8_t cmd){
  //spi_transfer(&cmd, NULL, 1);
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
}
void flash_send_addr(uint32_t addr){
  const uint8_t data[] = {
    (addr >> 16) & 0xFF,
    (addr >> 8) & 0xFF,
    (addr >> 0) & 0xFF,
  };
  //spi_transfer(data, NULL, 3);
  HAL_SPI_Transmit(&hspi1, data, 3, 1000);
}
void flash_send_dummy(uint16_t length_){
  spi_transfer(NULL, NULL, length_);
}
void flash_read_data(uint8_t* rx_buf, uint16_t length_){
  spi_transfer(NULL, rx_buf, length_);
}
void flash_write_data(const uint8_t* tx_buf, uint16_t length_){
	HAL_SPI_Transmit(&hspi1, tx_buf, length_, 1000);
}
void flash_rdid(uint8_t* rx_buf, uint16_t length_, uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_RDID);
	flash_read_data(rx_buf, length_);
	CS_Set(n);
}
void flash_read(uint32_t addr, uint8_t* rx_buf, uint16_t length_, uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_READ);
	flash_send_addr(addr);
	flash_read_data(rx_buf, length_);
	CS_Set(n);
}
void flash_wren(uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_WREN);
	CS_Set(n);
}
void flash_wrdi(uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_WRDI);
	CS_Set(n);
}
void flash_p4e(uint32_t addr, uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_P4E);
	flash_send_addr(addr);
	CS_Set(n);
}
void flash_be(uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_BE);
	CS_Set(n);
}
void flash_pp(uint32_t addr, const uint8_t* wr_data, uint16_t length_, uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_PP);
	flash_send_addr(addr);
	flash_write_data(wr_data, length_);
	CS_Set(n);
}
void flash_rdsr(uint8_t* rx_buf, uint16_t length_, uint8_t n){
	CS_Reset(n);
	flash_send_cmd(CMD_RDSR);
	flash_read_data(rx_buf, length_);
	CS_Set(n);
}
uint8_t flash_is_wip(uint8_t n){
  uint8_t data;
  flash_rdsr(&data, 0x01, n);
  return(data & 0x01);
}
void data_debug_print(const uint8_t* data, uint16_t bytes){
  uint16_t depth = bytes / 16;
  for(uint16_t j = 0; j < depth; ++j){
    for(uint16_t i = 0; i < 16; ++i){
      uint8_t d = data[i + (j * 16)];
      uart_putHEX((d >> 4) & 0x0F,2);
      uart_putHEX((d >> 0) & 0x0F,2);
      uart_puts(",",2);
    }
    uart_puts("\r\n",2);
  }
}
