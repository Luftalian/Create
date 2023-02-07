//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
// DO NOT WRITE ERASE CODE HERE
// DO NOT WRITE ERASE CODE HERE
// DO NOT WRITE ERASE CODE HERE
// DO NOT WRITE ERASE CODE HERE
// DO NOT WRITE ERASE CODE HERE
// DO NOT WRITE ERASE CODE HERE
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

#include <Arduino.h>
#include <SPICREATE.h>
#include <S25FL512S.h>
// #include <H3LIS331.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26
// #define H3LIS331CS 32

bool WhoAmI_Ok = false;
uint8_t count = 0;
uint8_t tx[256] = {};
int16_t rx[3] = {};
uint64_t gettime;
uint64_t time;

// int CountH3LIS331DataSetExistInBuff = 0;
int CountH3LIS331DataSetExistInBuff_READ = 0;

// uint8_t H3LIS331FlashBuff[256] = {0};

// uint32_t H3LIS331FlashLatestAddress = 0x00;
uint32_t H3LIS331FlashLatestAddress_READ = 0x100;

#define SPIFREQ 100000

// H3LIS331 H3lis331;

SPICREATE::SPICreate SPIC1;
Flash flash1;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // read Section
  tx[256] = {};
  rx[3] = {};
  while (1)
  {
    flash1.read(H3LIS331FlashLatestAddress_READ, tx);
    if (tx[0] == 255)
    {
      Serial.println("No Data");
      for (int i = 0; i < 5; i++)
      {
        Serial.print(".");
        delay(1000);
      }
      Serial.print("\n");
      H3LIS331FlashLatestAddress_READ = 0x00;
      break;
    }
    for (int CountH3LIS331DataSetExistInBuff = 0; CountH3LIS331DataSetExistInBuff < 8; CountH3LIS331DataSetExistInBuff++)
    {
      // 時間をとる
      // Serial.println("time");
      time = tx[32 * CountH3LIS331DataSetExistInBuff + 0];
      for (int i = 1; i < 4; i++)
      {
        gettime = tx[32 * CountH3LIS331DataSetExistInBuff + i];
        time |= gettime << (8 * i);
      }
      Serial.print(time);
      Serial.print(",");

      // 加速度をとる 400g
      //  Serial.println("acceleration");
      rx[3] = {};
      rx[0] = tx[32 * CountH3LIS331DataSetExistInBuff + 4];
      rx[0] |= ((uint16_t)tx[32 * CountH3LIS331DataSetExistInBuff + 5]) << 8;
      rx[1] = tx[32 * CountH3LIS331DataSetExistInBuff + 6];
      rx[1] |= ((uint16_t)tx[32 * CountH3LIS331DataSetExistInBuff + 7]) << 8;
      rx[2] = tx[32 * CountH3LIS331DataSetExistInBuff + 8];
      rx[2] |= ((uint16_t)tx[32 * CountH3LIS331DataSetExistInBuff + 9]) << 8;
      for (int i = 0; i < 3; i++)
      {
        Serial.print(rx[i]);
        Serial.print(",");
      }
      // 加速度をとる g
      rx[0] = (tx[32 * CountH3LIS331DataSetExistInBuff + 10] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 11]);
      rx[1] = (tx[32 * CountH3LIS331DataSetExistInBuff + 12] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 13]);
      rx[2] = (tx[32 * CountH3LIS331DataSetExistInBuff + 14] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 15]);
      for (int i = 0; i < 3; i++)
      {
        Serial.print(rx[i]);
        Serial.print(",");
      }
      // 角速度をとる
      rx[0] = (tx[32 * CountH3LIS331DataSetExistInBuff + 16] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 17]);
      rx[1] = (tx[32 * CountH3LIS331DataSetExistInBuff + 18] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 19]);
      rx[2] = (tx[32 * CountH3LIS331DataSetExistInBuff + 20] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 21]);
      for (int i = 0; i < 3; i++)
      {
        Serial.print(rx[i]);
        Serial.print(",");
      }
      // 地磁気をとる
      rx[3] = {};
      rx[0] = tx[32 * CountH3LIS331DataSetExistInBuff + 22];
      rx[0] |= ((uint16_t)tx[32 * CountH3LIS331DataSetExistInBuff + 23]) << 8;
      rx[1] = tx[32 * CountH3LIS331DataSetExistInBuff + 24];
      rx[1] |= ((uint16_t)tx[32 * CountH3LIS331DataSetExistInBuff + 25]) << 8;
      rx[2] = tx[32 * CountH3LIS331DataSetExistInBuff + 26];
      rx[2] |= ((uint16_t)tx[32 * CountH3LIS331DataSetExistInBuff + 27]) << 8;
      for (int i = 0; i < 3; i++)
      {
        Serial.print(rx[i]);
        Serial.print(",");
      }
      // 気圧をとる
      if (tx[32 * CountH3LIS331DataSetExistInBuff + 27] != 0 || tx[32 * CountH3LIS331DataSetExistInBuff + 27] != 0 || tx[32 * CountH3LIS331DataSetExistInBuff + 27])
      {
        Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + 30] << 16 | tx[32 * CountH3LIS331DataSetExistInBuff + 29] << 8 | tx[32 * CountH3LIS331DataSetExistInBuff + 28]);
      }
      Serial.print("\n");
      CountH3LIS331DataSetExistInBuff_READ++;
      if (CountH3LIS331DataSetExistInBuff_READ == 8)
      {
        H3LIS331FlashLatestAddress_READ += 0x100;
        CountH3LIS331DataSetExistInBuff_READ = 0;
      }
    }
  }
}
