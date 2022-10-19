//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
// This code write data since 0x000
// This code write data since 0x000
// This code write data since 0x000
// This code write data since 0x000
// This code write data since 0x000
// This code write data since 0x000
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
#include <Arduino.h>
#include <SPICREATE.h>
#include <S25FL512S.h>
#include <H3LIS331.h>
#include <LPS25HB.h>
#include <ICM20948_beta.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26

// #define H3LIS331SCK 14
// #define H3LIS331MISO 12
// #define H3LIS331MOSI 13
#define H3LIS331CS 32

#define LPSCS 14

#define ICMCS 13

// bool WhoAmI_Ok_H3LIS331 = false;
// bool WhoAmI_Ok_LPS25HB = false;
// bool WhoAmI_Ok_ICM20948 = false;
bool check = true;
uint8_t count = 0;

int CountH3LIS331DataSetExistInBuff = 0;
// int CountH3LIS331DataSetExistInBuff_READ = 0;

uint8_t FlashBuff[256] = {};
uint8_t tx[256] = {};

uint32_t H3LIS331FlashLatestAddress = 0x000;
// uint32_t H3LIS331FlashLatestAddress_READ = 0x100;

#define SPIFREQ 100000

H3LIS331 H3lis331;
LPS25 Lps25;
ICM icm20948;

SPICREATE::SPICreate SPIC1;
Flash flash1;

// 無駄なものを書いてしまった。
// void returndata(&rx)
// {
//   if (WhoAmI_Ok_H3LIS331 == true)
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

class Timer
{
public:
  unsigned long Gettime_record()
  {
    time = micros();
    time -= start_time;
    return time;
  }
  unsigned long start_time;
  unsigned long time;
  bool start_flag = true;
};

Timer timer;
unsigned long Record_time;

void setup()
{
  digitalWrite(flashCS, HIGH);
  delay(1000);
  digitalWrite(H3LIS331CS, HIGH);
  delay(1000);
  digitalWrite(ICMCS, HIGH);
  delay(1000);
  digitalWrite(LPSCS, HIGH);
  delay(1000);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start");
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  Serial.println("SPIC1");
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  Serial.println("flash1");
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  Serial.println("H3lis331");
  icm20948.begin(&SPIC1, ICMCS, SPIFREQ);
  Serial.println("icm20948");
  Lps25.begin(&SPIC1, LPSCS, SPIFREQ);
  Serial.println("Lps25hb");

  micros();
  delay(100);
  Serial.println("Timer Start!");

  // WhoAmI
  uint8_t a;
  a = H3lis331.WhoImI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0x32)
  {
    Serial.println("H3LIS331 is OK");
  }
  else
  {
    Serial.println("H3LIS331 is NG");
    // while (1)
    // {
    //   delay(1000);
    // }
  }

  a = Lps25.WhoAmI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0b10111101)
  {
    Serial.println("LPS25HB is OK");
    // WhoAmI_Ok_LPS25HB = true;
  }
  else
  {
    Serial.println("LPS25HB is NG");
  }

  // for (int i = 0; i < 5; i++)
  // {
  //   delay(1000);
  //   Serial.print(".");
  // }
  // Serial.println();

  a = icm20948.WhoAmI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0xEA)
  {
    Serial.println("ICM20948 is OK");
    // WhoAmI_Ok_ICM20948 = true;
  }
  else
  {
    Serial.println("ICM20948 is NG");
  }
}

void loop()
{
  // Serial.println("loop");

  // Serial.println("WhoAmI_Ok_H3LIS331");
  if (Serial.available())
  {
    Serial.println("start");
    char cmd = Serial.read();
    if (cmd == 'e')
    {
      Serial.println("e is pushed");
      flash1.erase();
      H3LIS331FlashLatestAddress = 0x000;
    }
    if (cmd == 'r')
    {
      Serial.println("r is pushed");
      for (int i = 0; i < 20; i++)
      {
        uint8_t rx[256];
        flash1.read(256 * i, rx);
        for (int i = 0; i < 256; i++)
        {
          if (i % 16 == 0)
          {
            Serial.println();
          }
          Serial.print(rx[i]);
        }
        Serial.println();
      }
    }
    if (cmd == 't')
    {
      Serial.println("t is pushed");
      // read SPI Flash & check SPI Flash is clear or not
      uint8_t rx[256];
      flash1.read(H3LIS331FlashLatestAddress, rx);
      for (int i = 0; i < 256; i++)
      {
        Serial.print(rx[i]);
      }
      Serial.println();
      int a = 0;
      // Serial.println(a);
      // a++;

      if (timer.start_flag)
      {
        timer.start_time = micros();
        timer.start_flag = false;
      }
      Record_time = timer.Gettime_record();
      // Serial.println(a);
      // a++;

      // delay(100);

      // From SPI, Get data is tx
      int16_t H3lisReceiveData[3];
      uint8_t rx_buf[6];
      int16_t IcmReceiveData[6];
      uint8_t icm_rx_buf[12];
      uint8_t Lps_rx_buf[3];
      // Serial.println(a);
      // a++;
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
      //     FlashBuff[16 * CountH3LIS331DataSetExistInBuff + index] = rx_buf[index];
      //   }
      //   CountH3LIS331DataSetExistInBuff++;
      //   if (CountH3LIS331DataSetExistInBuff == 16)
      //   {
      //     Serial.println("CountH3LIS331DataSetExistInBuff == 16");
      //     // flash_wren(MPUDATAFLASH); // flash1.write(0x1000000, rx_buf);
      //     // flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
      //     flash1.write(H3LIS331FlashLatestAddress, FlashBuff);
      //     H3LIS331FlashLatestAddress += 0x100;
      //     CountH3LIS331DataSetExistInBuff = 0;
      //   }
      // }

      // CountH3LIS331DataSetExistInBuffは列。indexは行。
      for (int CountH3LIS331DataSetExistInBuff = 0; CountH3LIS331DataSetExistInBuff < 8; CountH3LIS331DataSetExistInBuff++)
      {
        //時間をとる
        for (int index = 0; index < 4; index++)
        {
          FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = 0xFF & (Record_time >> (8 * index));
          Serial.println(a);
          a++;
        }
        //加速度をとる
        H3lis331.Get(H3lisReceiveData, rx_buf);
        for (int index = 4; index < 10; index++)
        {
          FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = rx_buf[index - 4];
          Serial.print("\t");
          Serial.println(a);
          a++;
        }
        // ICM20948の加速度をとる
        icm20948.Get(IcmReceiveData, icm_rx_buf);
        for (int index = 10; index < 16; index++)
        {
          FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = icm_rx_buf[index - 10];
        }
        // ICM20948の角速度をとる
        for (int index = 16; index < 22; index++)
        {
          FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = icm_rx_buf[index - 10];
        }
        // ICM20948の地磁気をとる
        for (int index = 22; index < 28; index++)
        {
          // FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = rx_buf[index - 10];
          FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = 111;
        }
        // LPSの気圧をとる
        if (count % 20 == 0)
        {
          Lps25.Get(Lps_rx_buf);
          for (int index = 28; index < 31; index++)
          {
            FlashBuff[32 * CountH3LIS331DataSetExistInBuff + index] = Lps_rx_buf[index - 28];
            count = 0;
          }
        }
        // Serial.println("--------");
        // Serial.println(a);
        // a++;
        delay(100); // 1000Hzより早くたたいてない？
        count++;
      }

      Serial.println("rx");
      for (int i = 0; i < 256; i++)
      {
        Serial.print(FlashBuff[i]);
        Serial.print(",");
        if (i % 32 == 31)
        {
          Serial.println();
        }
      }
      Serial.println();
      Serial.print("CountH3LIS331DataSetExistInBuff: ");
      Serial.println(CountH3LIS331DataSetExistInBuff);
      flash1.write(H3LIS331FlashLatestAddress, FlashBuff);
      delay(100);
      flash1.read(H3LIS331FlashLatestAddress, tx);
      Serial.println("tx");
      for (int i = 0; i < 256; i++)
      {
        Serial.print(tx[i]);
        Serial.print(",");
        if (i % 32 == 31)
        {
          Serial.println();
        }
      }
      Serial.println();
      // End of change
      // flash1.write(0x1000000, rx_buf);

      // delay(100);

      // flash1.read(0x00, rx);
      // for (int i = 0; i < (16 * 16 + 6 + 2); i++) // 2個分はnull確認用
      // {
      //   Serial.println(rx[i]);
      // }

      // delay(10000);

      // for (int i = 0; i < 10; i++)
      // {
      //   Serial.print(".");
      //   delay(1000);
      // }
      // Serial.println();

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
        if (FlashBuff[i] != tx[i])
        {
          // Serial.println("Data is incorrect");
          Serial.print("No.");
          Serial.print(i);
          Serial.print(" rx:");
          Serial.print(FlashBuff[i]);
          Serial.print(" tx:");
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
        Serial.println("error");
      }
      H3LIS331FlashLatestAddress += 0x100;

      Serial.println("end");
      Serial.println(H3LIS331FlashLatestAddress);
    }
  }
  delay(100);
}