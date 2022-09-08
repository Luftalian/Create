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
      for (int i = 0; i < 16; i++)
      {
        for (int index = 0; index < 6; index++)
        {
          Serial.print(tx[16 * CountH3LIS331DataSetExistInBuff_READ + index]);
          if (index != 5)
          {
            Serial.print(",");
          }
        }
        Serial.println();

        CountH3LIS331DataSetExistInBuff_READ++;
        if (CountH3LIS331DataSetExistInBuff_READ == 16)
        {
          Serial.println("2nd CountH3LIS331DataSetExistInBuff_READ == 16");
          // flash_wren(MPUDATAFLASH); // flash1.write(0x1000000, rx_buf);
          // flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
          H3LIS331FlashLatestAddress_READ += 0x100;
          CountH3LIS331DataSetExistInBuff_READ = 0;
        }
      }
    }
  }
}