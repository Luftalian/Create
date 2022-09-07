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

      if (cmd == 't')
      {
        Serial.println("t is pushed");
        // read SPI Flash & check SPI Flash is clear or not
        uint8_t rx[256];
        flash1.read(0x1000000, rx);
        for (int i = 0; i < 256; i++)
        {
          Serial.print(rx[i]);
        }
        Serial.println();

        delay(100);

        // From SPI, Get data is tx
        int16_t H3lisReceiveData[3];
        uint8_t rx_buf[6];
        Serial.println("rx_buf");
        for (int i = 0; i < 6; i++)
        {
          Serial.print(rx_buf[i]);
        }
        H3lis331.Get(H3lisReceiveData, rx_buf);

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

        // End of change
        flash1.write(0x1000000, rx_buf);

        delay(100);

        flash1.read(0x1000000, rx);
        for (int i = 0; i < 6; i++)
        {
          Serial.println(rx[i]);
        }
        Serial.println();

        // 書き込んだのと読み込んだのが一致しているか確認する。
        for (int i = 0; i < 6; i++)
        {
          if (rx[i] != rx_buf[i])
          {
            Serial.print(i);
            Serial.println(" NG");
          }
        }
        Serial.println("end");
      }
    }
    delay(100);
  }
}