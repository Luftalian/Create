#pragma once

#ifndef H3LIS331_H
#define H3LIS331_H
#include <SPICREATE.h>
#include <Arduino.h>

// 書き換える
// 書き換えた
#define H3LIS331_Data_Adress 0x28
//#define H3LIS331_GYRO_CONFIG 0x1B
#define H3LIS331_WhoAmI_Adress 0x0F
#define H3LIS331_CTRL_REG1 0x20    // CTRL_REG1
#define H3LIS331_ACC_CONFIG 0x23   // CTRL_REG4
#define H3LIS331_SleepAndWake 0x24 // CTRL_REG5

#define H3LIS331_OutPut_Data_Rate_1Hz 0b00111111 // CTRL_REG1
#define H3LIS331_SleepAndWake_OFF 0b00000000     // CTRL_REG4
#define H3LIS331_400G 0b00000000                 // CTRL_REG5
#define H3LIS331_200G 0b00010000                 // CTRL_REG5
#define H3LIS331_100G 0b00110000                 // CTRL_REG5
//#define H3LIS331_2G 0b00000000
// #define H3LIS331_2000dps 0b00011000
// #define H3LIS331_1000dps 0b00010000
// #define H3LIS331_500dps 0b00001000
// #define H3LIS331_250dps 0b00000000

// #define H3LIS331_2500deg 0x18

//書き換えない
class H3LIS331
{
    int CS;
    int deviceHandle{-1};
    SPICREATE::SPICreate *H3LIS331SPI;

public:
    void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
    uint8_t WhoImI();
    void Get(int16_t *rx);
};

//書き換えない
void H3LIS331::begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq)
{
    CS = cs;
    H3LIS331SPI = targetSPI;
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

    deviceHandle = H3LIS331SPI->addDevice(&if_cfg, cs);

    //ここまで。これより下書き換える
    // 1kHzにする
    H3LIS331SPI->setReg(H3LIS331_CTRL_REG1, H3LIS331_OutPut_Data_Rate_1Hz, deviceHandle);
    // SleepAndWakeをOFFにする
    H3LIS331SPI->setReg(H3LIS331_SleepAndWake, H3LIS331_SleepAndWake_OFF, deviceHandle);
    // Init
    H3LIS331SPI->setReg(H3LIS331_ACC_CONFIG, H3LIS331_400G, deviceHandle);
    // H3LIS331SPI->setReg(H3LIS331_GYRO_CONFIG, H3LIS331_2500deg, deviceHandle);

    return;
}

//読めるはず
uint8_t H3LIS331::WhoImI()
{
    return H3LIS331SPI->readByte(H3LIS331_WhoAmI_Adress | 0x80, deviceHandle);
}
void H3LIS331::Get(int16_t *rx)
{
    uint8_t rx_buf[6];
    spi_transaction_t comm = {};
    comm.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
    comm.length = (6) * 8;
    comm.cmd = H3LIS331_Data_Adress | 0x80;

    comm.tx_buffer = NULL;
    comm.rx_buffer = rx_buf;
    comm.user = (void *)CS;

    spi_transaction_ext_t spi_transaction = {};
    spi_transaction.base = comm;
    spi_transaction.command_bits = 8;
    H3LIS331SPI->pollTransmit((spi_transaction_t *)&spi_transaction, deviceHandle);

    rx[0] = (rx_buf[0] << 8 | rx_buf[1]);
    rx[1] = (rx_buf[2] << 8 | rx_buf[3]);
    rx[2] = (rx_buf[4] << 8 | rx_buf[5]);
    return;
}
#endif
