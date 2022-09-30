#pragma once

#ifndef LPS_H
#define LPS_H
#include <SPICREATE.h>
#include <Arduino.h>

#define LPS_Data_Adress_0 0x28
#define LPS_Data_Adress_1 0x29
#define LPS_Data_Adress_2 0x2A

#define LPS_WakeUp_Adress 0x20
#define LPS_WakeUp_Value 0x90
#define LPS_WhoAmI_Adress 0x0F


class LPS25
{
    int CS;
    int deviceHandle{-1};
    SPICREATE::SPICreate *LPSSPI;

public:
    void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
    uint8_t WhoAmI();
    void Get(uint8_t *rx);
    uint8_t ReadByte(uint8_t addr);
    // (uint32_t)rx[2] << 16 | (uint32_t)rx[1] << 8 | (uint32_t)rx[0] means pressure
};

void LPS25::begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq)
{
    CS = cs;
    LPSSPI = targetSPI;
    spi_device_interface_config_t if_cfg = {};

    // if_cfg.spics_io_num = cs;
    if_cfg.pre_cb = NULL;
    if_cfg.post_cb = NULL;
    if_cfg.cs_ena_pretrans = 0;
    if_cfg.cs_ena_posttrans = 0;

    if_cfg.clock_speed_hz = freq;

    if_cfg.mode = SPI_MODE3;
    if_cfg.queue_size = 1;

    if_cfg.pre_cb = csReset;
    if_cfg.post_cb = csSet;

    deviceHandle = LPSSPI->addDevice(&if_cfg, cs);
    LPSSPI->setReg(LPS_WakeUp_Adress, LPS_WakeUp_Value, deviceHandle);
    delay(100);
    return;
}
uint8_t LPS25::WhoAmI()
{
    return LPSSPI->readByte(LPS_WhoAmI_Adress | 0x80, deviceHandle);
    // registor 0x0F and you'll get 0d177 or 0xb1 or 0b10110001
}

void LPS25::Get(uint8_t *rx)
{   
    // uint8_t rx_buf[14];
     spi_transaction_t comm = {};
     comm.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
     comm.length = (3) * 8;
     comm.cmd = LPS_Data_Adress_0 | 0x80;

     comm.tx_buffer = NULL;
     comm.rx_buffer = NULL;
     comm.user = (void *)CS;

     spi_transaction_ext_t spi_transaction = {};
     spi_transaction.base = comm;
     spi_transaction.command_bits = 8;

    // LPSSPI->pollTransmit((spi_transaction_t *)&spi_transaction, deviceHandle);

    // rx[0] = rx_buf[0];
    // rx[1] = rx_buf[1];
    // rx[2] = rx_buf[2];
    // rx[0] = LPSSPI->readByte(LPS_Data_Adress_0 | 0x80, deviceHandle);
    // // delay(1);
    // rx[1] = LPSSPI->readByte(LPS_Data_Adress_1 | 0x80, deviceHandle);
    // // delay(1);
    // rx[2] = LPSSPI->readByte(LPS_Data_Adress_2 | 0x80, deviceHandle);
    // // delay(1);
    // rx[3] = LPSSPI->readByte(LPS_Data_Adress_2+1 | 0x80, deviceHandle);

    rx[0] = LPSSPI->readByte(LPS_Data_Adress_0 | 0x80,deviceHandle);
    // // // delay(1);
    rx[1] = LPSSPI->readByte(LPS_Data_Adress_1 | 0x80,deviceHandle);
    // // // delay(1);
    rx[2] = LPSSPI->readByte(LPS_Data_Adress_2 | 0x80,deviceHandle);

    return;
}

// int8_t LPS25::ReuadByte(uint8_t addr){
//     spi_transaction_t comm = {};
//     comm.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
//     comm.tx_data[0] = addr;
//     comm.length = 32;
//     comm.user = (void *)CS;
//     LPSSPI->pollTransmit(&comm, deviceHandle);

//     return comm.rx_data[1];
//  }
#endif