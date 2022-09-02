#pragma once

#ifndef H3LIS331_H
#define H3LIS331_H
#include <SPICREATE.h>
#include <Arduino.h>

// 書き換える
// 書き換えた
#define H3LIS331_Data_Address 0x28
//#define H3LIS331_GYRO_CONFIG 0x1B
#define H3LIS331_WhoAmI_Address 0x0F
#define H3LIS331_CTRL_REG1_Address 0x20  // CTRL_REG1 Power Mode & Output Data Rate
#define H3LIS331_CTRL_REG2_Address 0x21  // CTRL_REG2 (Normally they do not have to be changed)
#define H3LIS331_CTRL_REG3_Address 0x22  // CTRL_REG3 Interrupt
#define H3LIS331_CTRL_REG4_Address 0x23  // CTRL_REG4 ACC_CONFIG
#define H3LIS331_CTRL_REG5_Address 0x24  // CTRL_REG5 SleepToWake
#define H3LIS331_STATUS_REG_Address 0x27 // STATUS_REG Overrun

#define H3LIS331_CTRL_REG1 0b00111111 // CTRL_REG1 Normal Mode & OutPut Data Rate 1000Hz ????????????
// #define H3LIS331_CTRL_REG1 0b0001111 // CTRL_REG1 default value
#define H3LIS331_CTRL_REG2 0b00000000      // CTRL_REG2 (Normally they do not have to be changed)
#define H3LIS331_CTRL_REG3 0b00000000      // CTRL_REG3 Interrupt Active High
#define H3LIS331_CTRL_REG4_400G 0b00110000 // CTRL_REG4 400G
#define H3LIS331_CTRL_REG4_200G 0b00010000 // CTRL_REG4 200G
#define H3LIS331_CTRL_REG4_100G 0b00000000 // CTRL_REG4 100G
#define H3LIS331_CTRL_REG5 0b00000000      // CTRL_REG5 SleepToWake_OFF
#define H3LIS331_STATUS_REG 0b11111111     // STATUS_REG Overrun Available

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

    // コメントアウトから外す？
    // if_cfg.spics_io_num = cs;

    if_cfg.pre_cb = NULL;
    if_cfg.post_cb = NULL;
    if_cfg.cs_ena_pretrans = 0;
    if_cfg.cs_ena_posttrans = 0;

    if_cfg.clock_speed_hz = freq;

    if_cfg.mode = SPI_MODE0;
    if_cfg.queue_size = 1;

    if_cfg.pre_cb = csReset;
    if_cfg.post_cb = csSet;

    // 以下必要なんじゃないか？
    if_cfg.input_delay_ns = 50; // 0の部分を変更する
    if_cfg.dummy_bits = 0;      // 0の部分を変更する

    deviceHandle = H3LIS331SPI->addDevice(&if_cfg, cs);

    //ここまで。これより下書き換える
    // 1kHzにする
    H3LIS331SPI->setReg(H3LIS331_CTRL_REG1_Address, H3LIS331_CTRL_REG1, deviceHandle);
    delay(100);
    H3LIS331SPI->setReg(H3LIS331_CTRL_REG2_Address, H3LIS331_CTRL_REG2, deviceHandle);
    delay(100);
    H3LIS331SPI->setReg(H3LIS331_CTRL_REG3_Address, H3LIS331_CTRL_REG3, deviceHandle);
    delay(100);
    // Init
    H3LIS331SPI->setReg(H3LIS331_CTRL_REG4_Address, H3LIS331_CTRL_REG4_400G, deviceHandle);
    delay(100);
    // SleepAndWakeをOFFにする
    H3LIS331SPI->setReg(H3LIS331_CTRL_REG5_Address, H3LIS331_CTRL_REG5, deviceHandle);
    delay(100);
    // H3LIS331SPI->setReg(H3LIS331_GYRO_CONFIG, H3LIS331_2500deg, deviceHandle);
    // H3LIS331SPI->setReg(H3LIS331_CTRL_REG1, H3LIS331_400G, deviceHandle);
    // delay(1);
    H3LIS331SPI->setReg(H3LIS331_STATUS_REG_Address, H3LIS331_STATUS_REG, deviceHandle);
    delay(100);
    return;
}

//読めるはず
uint8_t H3LIS331::WhoImI()
{
    return H3LIS331SPI->readByte(H3LIS331_WhoAmI_Address | 0x80, deviceHandle);
}
void H3LIS331::Get(int16_t *rx)
{
    uint8_t rx_buf[6];
    spi_transaction_t comm = {};
    comm.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
    comm.length = (6) * 8;

    // 0x40？
    // uint8_t register_address = H3LIS331_Data_Address;
    // register_address |= 0x40;
    comm.cmd = H3LIS331_Data_Address | 0x80;

    // uint16_t _address = register_address;
    // uint8_t addrbuffer[2] = {(uint8_t)(_address & 0xFF),
    //                          (uint8_t)(_address >> 8)};
    // addrbuffer[0] |= 0x80;

    // NULL じゃないかも
    comm.tx_buffer = NULL;
    // comm.tx_buffer = addrbuffer;
    comm.rx_buffer = rx_buf;
    comm.user = (void *)CS;

    spi_transaction_ext_t spi_transaction = {};
    spi_transaction.base = comm;
    spi_transaction.command_bits = 8;
    H3LIS331SPI->pollTransmit((spi_transaction_t *)&spi_transaction, deviceHandle);

    // Serial.print(rx_buf[0]);
    // Serial.print(" ");
    // Serial.print(rx_buf[1]);
    // Serial.print(" ");
    // Serial.print(rx_buf[2]);
    // Serial.print(" ");
    // Serial.print(rx_buf[3]);
    // Serial.print(" ");
    // Serial.print(rx_buf[4]);
    // Serial.print(" ");
    // Serial.print(rx_buf[5]);
    // Serial.print(" ");
    // Serial.println(rx_buf[6]);

    // ここ変えた。
    // rx[0] = (rx_buf[1] << 8 | rx_buf[0]);
    // rx[1] = (rx_buf[3] << 8 | rx_buf[2]);
    // rx[2] = (rx_buf[5] << 8 | rx_buf[4]);
    rx[0] = rx_buf[0];
    rx[0] |= ((uint16_t)rx_buf[1]) << 8;
    rx[1] = rx_buf[2];
    rx[1] |= ((uint16_t)rx_buf[3]) << 8;
    rx[2] = rx_buf[4];
    rx[2] |= ((uint16_t)rx_buf[5]) << 8;
    // rx[0] = rx_buf[1];
    // rx[0] |= ((uint16_t)rx_buf[0]) << 8;
    // rx[1] = rx_buf[3];
    // rx[1] |= ((uint16_t)rx_buf[2]) << 8;
    // rx[2] = rx_buf[5];
    // rx[2] |= ((uint16_t)rx_buf[4]) << 8;
    return;
}
// void Adafruit_H3LIS331::_scaleValues(void) {

//   // actually 12 bit but left justified
//   x >>= 4;
//   y >>= 4;
//   z >>= 4;
//   uint8_t range = getRange();
//   uint16_t scale_max = 1;
//   if (range == H3LIS331_RANGE_100_G)
//     scale_max = 100;
//   if (range == H3LIS331_RANGE_200_G)
//     scale_max = 200;
//   if (range == H3LIS331_RANGE_400_G)
//     scale_max = 400;

//   float lsb_value = 2 * scale_max * (float)1 / 4096;

//   x_g = ((float)x * lsb_value);
//   y_g = ((float)y * lsb_value);
//   z_g = ((float)z * lsb_value);
// }

// #define SENSORS_GRAVITY_EARTH (9.80665F) /**< Earth's gravity in m/s^2 */
// #define SENSORS_GRAVITY_MOON (1.6F)      /**< The moon's gravity in m/s^2 */
// #define SENSORS_GRAVITY_SUN (275.0F)     /**< The sun's gravity in m/s^2 */
// #define SENSORS_GRAVITY_STANDARD (SENSORS_GRAVITY_EARTH)

//  event->acceleration.x = x_g * SENSORS_GRAVITY_STANDARD;
//   event->acceleration.y = y_g * SENSORS_GRAVITY_STANDARD;
//   event->acceleration.z = z_g * SENSORS_GRAVITY_STANDARD;

#endif
