#pragma once

#ifndef MCP_H
#define MCP_H
#include <SPICREATE.h>
#include <Arduino.h>

#define MCP_GETADCCMD_HEADER 0x06
#define MCP_WHOAMI_ADRESS 0x00

class MCP
{
    int CS;
    int deviceHandle{-1};
    SPICREATE::SPICreate *MCPSPI;

public:
    void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
    uint16_t Get(uint8_t adcChannel);
};

void MCP::begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq)
{
    CS = cs;
    MCPSPI = targetSPI;
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

    deviceHandle = MCPSPI->addDevice(&if_cfg, cs);
    return;
}

uint16_t MCP::Get(uint8_t adcChannel)
{
    spi_transaction_t comm = {};
    comm.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
    comm.length = 24;

    comm.tx_buffer = NULL;
    comm.rx_buffer = NULL;
    comm.user = (void *)CS;

    spi_transaction_ext_t spi_transaction = {};
    spi_transaction.base = comm;
    spi_transaction.command_bits = 8;

    uint8_t rx[3];
    uint8_t tx[3];
    tx[0] = MCP_GETADCCMD_HEADER | (adcChannel >> 2);
    tx[1] = adcChannel << 6;
    tx[2] = 0x00;
    MCPSPI->transmit(tx, rx, 3, deviceHandle);
    return ((uint16_t)(rx[1] & 0x0F)) * 256 + (uint16_t)rx[2];
}

#endif
