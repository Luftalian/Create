#include <Arduino.h>
#include <SPICREATE.h>
#include <S25FL512S.h>
#include <H3LIS331.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26

// #define H3LIS331SCK 14
// #define H3LIS331MISO 12
// #define H3LIS331MOSI 13
#define H3LIS331CS 32

bool WhoAmI_Ok = false;
bool check = true;

int CountH3LIS331DataSetExistInBuff = 0;
int CountH3LIS331DataSetExistInBuff_READ = 0;

uint8_t H3LIS331FlashBuff[256] = {};
uint8_t tx[256] = {};

uint32_t H3LIS331FlashLatestAddress = 0x00;
uint32_t H3LIS331FlashLatestAddress_READ = 0x00;

#define SPIFREQ 100000

H3LIS331 H3lis331;

SPICREATE::SPICreate SPIC1;
Flash flash1;

// 無駄なものを書いてしまった。
// void returndata(&rx)
// {
//   if (WhoAmI_Ok == true)
//   {
//     int16_t rx[3];
//     H3lis331.Get(rx);

//     // change value
//     float rx_400g[3];
//     rx_400g[0] = (float)rx[0] * 2 * 400 / 65536;
//     rx_400g[1] = (float)rx[1] * 2 * 400 / 65536;
//     rx_400g[2] = (float)rx[2] * 2 * 400 / 65536;

//     Serial.print("\t\tx: ");
//     Serial.print(rx_400g[0]);
//     Serial.print(",");
//     Serial.print("\ty: ");
//     Serial.print(rx_400g[1]);
//     Serial.print(",");
//     Serial.print("\tz: ");
//     Serial.println(rx_400g[2]);
//     // delay(100);
//   }
// }

void setup()
{
  Serial.begin(115200);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  // put your setup code here, to run once:

  // WhoAmI
  uint8_t a;
  a = H3lis331.WhoImI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0x32)
  {
    Serial.println("H3LIS331 is OK");
    WhoAmI_Ok = true;
  }
  else
  {
    Serial.println("H3LIS331 is NG");
  }
}

void loop()
{
  // Serial.println("loop");
  if (WhoAmI_Ok == true)
  {
    // Serial.println("WhoAmI_Ok");
    if (Serial.available())
    {
      Serial.println("start");
      char cmd = Serial.read();
      if (cmd == 'e')
      {
        Serial.println("e is pushed");
        flash1.erase();
      }
      if (cmd == 'r')
      {
        Serial.println("r is pushed");
        uint8_t rx[256 * H3LIS331FlashLatestAddress / 0x100];
        flash1.read(0x00, rx);
        for (int i = 0; i < 256; i++)
        {
          Serial.print(rx[i]);
        }
        Serial.println();
      }
      if (cmd == 't')
      {
        Serial.println("t is pushed");
        // read SPI Flash & check SPI Flash is clear or not
        uint8_t rx[256];
        flash1.read(0x00, rx);
        for (int i = 0; i < 256; i++)
        {
          Serial.print(rx[i]);
        }
        Serial.println();

        delay(100);

        // From SPI, Get data is tx
        int16_t H3lisReceiveData[3];
        uint8_t rx_buf[6];
        // Serial.println("rx_buf");
        // for (int i = 0; i < 6; i++)
        // {
        //   Serial.print(i);
        //   Serial.print(": ");
        //   Serial.println(rx_buf[i]);
        // }

        // I have to change raw data to tx data.

        // // make mpu dataset
        // for (int index = 0; index < 6; index++)
        // {
        //   MPUFlashBuff[16 * CountMPUDataSetExistInBuff + index] = MPURecievedData[index];
        // }
        // for (int index = 8; index < 14; index++)
        // {
        //   MPUFlashBuff[16 * CountMPUDataSetExistInBuff + index - 2] = MPURecievedData[index];
        // }
        // for (int index = 0; index < 4; index++)
        // {
        //   MPUFlashBuff[16 * CountMPUDataSetExistInBuff + index + 12] = 0xFF & (clock_1kHz >> (8 * index));
        // }
        // CountMPUDataSetExistInBuff++;
        // if (CountMPUDataSetExistInBuff == 16)
        // {
        //   flash_wren(MPUDATAFLASH);
        //   flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
        //   MPUFlashLatestAddress += 0x100;
        //   CountMPUDataSetExistInBuff = 0;
        // }

        // for (int i = 0; i < 16; i++)
        // {
        //   H3lis331.Get(H3lisReceiveData, rx_buf);

        //   for (int index = 0; index < 6; index++)
        //   {
        //     H3LIS331FlashBuff[16 * CountH3LIS331DataSetExistInBuff + index] = rx_buf[index];
        //   }
        //   CountH3LIS331DataSetExistInBuff++;
        //   if (CountH3LIS331DataSetExistInBuff == 16)
        //   {
        //     Serial.println("CountH3LIS331DataSetExistInBuff == 16");
        //     // flash_wren(MPUDATAFLASH); // flash1.write(0x1000000, rx_buf);
        //     // flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
        //     flash1.write(H3LIS331FlashLatestAddress, H3LIS331FlashBuff);
        //     H3LIS331FlashLatestAddress += 0x100;
        //     CountH3LIS331DataSetExistInBuff = 0;
        //   }
        // }
        for (int CountH3LIS331DataSetExistInBuff = 0; CountH3LIS331DataSetExistInBuff < 8; CountH3LIS331DataSetExistInBuff++)
        {
          //時間をとる
          for (int index = 0; index < 4; index++)
          {
            // H3LIS331FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = 0xFF & (clock_1kHz >> (8 * index));
          }
          //加速度をとる
          H3lis331.Get(H3lisReceiveData, rx_buf);
          for (int index = 4; index < 10; index++)
          {
            H3LIS331FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = rx_buf[index - 4];
          }
          // MPUの加速度をとる
          for (int index = 10; index < 16; index++)
          {
            // H3LIS331FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = MPU_rx_buf[index - 10];
          }
          // MPUの角速度をとる
          for (int index = 16; index < 22; index++)
          {
            // H3LIS331FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = MPU_rx_buf[index - 10];
          }
          // MPUの地磁気をとる
          for (int index = 22; index < 28; index++)
          {
            // H3LIS331FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = MPU_rx_buf[index - 10];
          }
          // LPSの気圧をとる
          if (count % 20 == 0)
          {
            for (int index = 28; index < 32; index++)
            {
              // H3LIS331FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = LPS_rx_buf[index - 28];
            }
          }
          count++;
        }
        Serial.print("CountH3LIS331DataSetExistInBuff: ");
        Serial.println(CountH3LIS331DataSetExistInBuff);
        flash1.write(H3LIS331FlashLatestAddress, H3LIS331FlashBuff);
        flash.read(H3LIS331FlashLatestAddress, tx);
        // End of change
        // flash1.write(0x1000000, rx_buf);

        // delay(100);

        // flash1.read(0x00, rx);
        // for (int i = 0; i < (16 * 16 + 6 + 2); i++) // 2個分はnull確認用
        // {
        //   Serial.println(rx[i]);
        // }
        // delay(10000);
        for (int i = 0; i < 10; i++)
        {
          Serial.print(".");
          delay(1000);
        }

        // // read Section
        // uint8_t tx[256] = {};
        // flash1.read(H3LIS331FlashLatestAddress_READ, tx);
        // for (int i = 0; i < 16; i++)
        // {
        //   for (int index = 0; index < 6; index++)
        //   {
        //     Serial.print(tx[16 * CountH3LIS331DataSetExistInBuff_READ + index]);
        //     if (index != 5)
        //     {
        //       Serial.print(",");
        //     }
        //   }
        //   Serial.println();

        //   CountH3LIS331DataSetExistInBuff_READ++;
        //   if (CountH3LIS331DataSetExistInBuff_READ == 16)
        //   {
        //     Serial.println("2nd CountH3LIS331DataSetExistInBuff_READ == 16");
        //     // flash_wren(MPUDATAFLASH); // flash1.write(0x1000000, rx_buf);
        //     // flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
        //     H3LIS331FlashLatestAddress_READ += 0x100;
        //     CountH3LIS331DataSetExistInBuff_READ = 0;
        //   }
        // }
        for (int i = 0; i < 256; i++)
        {
          if (H3LIS331FlashBuff[i] != tx[i])
          {
            Serial.println("Data is incorrect");
            Serial.print(i);
            Serial.print(" ");
            Serial.print(H3LIS331FlashBuff[i]);
            Serial.print(" ");
            Serial.println(tx[i]);
            check = false;
          }
        }
        if (check)
        {
          Serial.println("Data is correct");
        }
        else
        {
          Serial.println("Data is incorrect");
        }
        H3LIS331FlashLatestAddress += 0x100;

        Serial.println("end");
      }
    }
    delay(100);
  }
}