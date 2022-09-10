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

// int CountH3LIS331DataSetExistInBuff = 0;
int CountH3LIS331DataSetExistInBuff_READ = 0;

// uint8_t H3LIS331FlashBuff[256] = {0};

// uint32_t H3LIS331FlashLatestAddress = 0x00;
uint32_t H3LIS331FlashLatestAddress_READ = 0x00;

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
  // H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  // put your setup code here, to run once:

  // WhoAmI
  // uint8_t a;
  // a = H3lis331.WhoImI();
  // Serial.print("WhoAmI:");
  // Serial.println(a);
  // if (a == 0x32)
  // {
  //   Serial.println("H3LIS331 is OK");
  //   WhoAmI_Ok = true;
  // }
  // else
  // {
  //   Serial.println("H3LIS331 is NG");
  // }
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    Serial.println("start");
    char cmd = Serial.read();
    if (cmd == 'r')
    {
      // read Section
      uint8_t tx[256] = {};
      flash1.read(H3LIS331FlashLatestAddress_READ, tx);
      if (tx[0] == 255)
      {
        Serial.println("No Data");
        for (int i = 0; i < 5; i++)
        {
          Serial.print(".");
          delay(1000);
        }
        Serial.println();
      }
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
      for (int CountH3LIS331DataSetExistInBuff = 0; CountH3LIS331DataSetExistInBuff < 8; CountH3LIS331DataSetExistInBuff++)
      {
        //時間をとる
        Serial.println("time");
        for (int index = 0; index < 4; index++)
        {
          Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + index]);
          if (index != 3)
          {
            Serial.print(",");
          }
        }
        Serial.println();
        //加速度をとる
        Serial.println("acceleration");
        for (int index = 4; index < 10; index++)
        {
          Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + index]);
          if (index != 9)
          {
            Serial.print(",");
          }
        }
        Serial.println();
        // MPUの加速度をとる
        Serial.println("MPU_Acceleration");
        for (int index = 10; index < 16; index++)
        {
          Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + index]);
          if (index != 15)
          {
            Serial.print(",");
          }
        }
        Serial.println();
        // MPUの角速度をとる
        Serial.println("MPU_Angular_Velocity");
        for (int index = 16; index < 22; index++)
        {
          Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + index]);
          if (index != 21)
          {
            Serial.print(",");
          }
        }
        Serial.println();
        // MPUの地磁気をとる
        Serial.println("MPU_Magnetic_Field");
        for (int index = 22; index < 28; index++)
        {
          Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + index]);
          if (index != 27)
          {
            Serial.print(",");
          }
        }
        Serial.println();
        // LPSの気圧をとる
        Serial.println("LPS_Pressure");
        // if (count % 20 == 0)
        // {
        if ((tx[32 * CountH3LIS331DataSetExistInBuff + 28] != 0) || (tx[32 * CountH3LIS331DataSetExistInBuff + 29] != 0) || (tx[32 * CountH3LIS331DataSetExistInBuff + 30] != 0) || (tx[32 * CountH3LIS331DataSetExistInBuff + 31] != 0))
        {
          for (int index = 28; index < 31; index++)
          {
            Serial.print(tx[32 * CountH3LIS331DataSetExistInBuff + index]);
            if (index != 30)
            {
              Serial.print(",");
            }
          }
        }
        Serial.println();
        // }
        // delay(100); // 1000Hzより早くたたいてない？
        count++;

        CountH3LIS331DataSetExistInBuff_READ++;
        if (CountH3LIS331DataSetExistInBuff_READ == 8)
        {
          Serial.println("2nd CountH3LIS331DataSetExistInBuff_READ == 8");
          // flash_wren(MPUDATAFLASH); // flash1.write(0x1000000, rx_buf);
          // flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
          H3LIS331FlashLatestAddress_READ += 0x100;
          CountH3LIS331DataSetExistInBuff_READ = 0;
        }
      }
    }
  }
}