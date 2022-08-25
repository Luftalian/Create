#ifndef SPIFLASH_MCAT_H
#define SPIFLASH_MCAT_H

void CS_Reset(uint8_t n);

void CS_Set(uint8_t n);

void spi_transfer(const uint8_t* tx_buf, uint8_t* rx_buf, uint16_t length_);

void flash_send_cmd(uint8_t cmd);

void flash_send_addr(uint32_t addr);

void flash_send_dummy(uint16_t length_);

void flash_read_data(uint8_t* rx_buf, uint16_t length_);

void flash_write_data(const uint8_t* tx_buf, uint16_t length_);

void flash_rdid(uint8_t* rx_buf, uint16_t length_, uint8_t n);

void flash_read(uint32_t addr, uint8_t* rx_buf, uint16_t length_, uint8_t n);

void flash_wren(uint8_t n);

void flash_wrdi(uint8_t n);

void flash_p4e(uint32_t addr, uint8_t n);

void flash_be(uint8_t n);

void flash_pp(uint32_t addr, const uint8_t* wr_data, uint16_t length_, uint8_t n);

void flash_rdsr(uint8_t* rx_buf, uint16_t length_, uint8_t n);

uint8_t flash_is_wip(uint8_t n);

void data_debug_print(const uint8_t* data, uint16_t bytes);

#endif
