//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
// This code write data since 0x100
// This code write data since 0x100
// This code write data since 0x100
// This code write data since 0x100
// This code write data since 0x100
// This code write data since 0x100
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------#include <Arduino.h>
#include <SPICREATE.h>
#include <S25FL512S.h>
#include <H3LIS331.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define H3LIS331CS 32

H3LIS331 H3lis331;

SPICREATE::SPICreate SPIC1;
Flash flash1;

bool WhoAmI_Ok = false;
bool check = true;
uint8_t count = 0;

int CountSPIFlashDataSetExistInBuff = 0;
int CountSPIFlashDataSetExistInBuff_READ = 0;

uint8_t SPI_FlashBuff[256] = {};
// uint8_t tx[256] = {};
int16_t H3lisReceiveData[3];
uint8_t H3lis_rx_buf[6];

uint32_t SPIFlashLatestAddress = 0x000;
// uint32_t SPIFlashLatestAddress_READ = 0x100;

// timer1 is used to write data to flash
volatile int timeCounter1;
uint64_t totalTimeCounter1 = 0;

// timer2 is used to measure time
volatile int timeCounter2;
int totalTimeCounter2 = 0;

#define SPIFREQ 100000

hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux2 = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimer1()
{

  portENTER_CRITICAL_ISR(&timerMux1);
  timeCounter1++;
  portEXIT_CRITICAL_ISR(&timerMux1);
}
void IRAM_ATTR onTimer2()
{

  portENTER_CRITICAL_ISR(&timerMux2);
  timeCounter2++;
  portEXIT_CRITICAL_ISR(&timerMux2);
}
void RoutineWork()
{
  Serial.println("Running");
  // From SPI, Get data is tx
  H3lisReceiveData[3] = {};
  H3lis_rx_buf[6] = {};
  // CountSPIFlashDataSetExistInBuffは列。indexは行。
  //時間をとる
  for (int index = 0; index < 4; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = 0xFF & (totalTimeCounter2 >> (8 * index));
  }
  // for (int index = 3; index >= 0; index--)
  // {
  //   SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = 0xFF & (totalTimeCounter2 >> (8 * index));
  // }
  //加速度をとる
  H3lis331.Get(H3lisReceiveData, H3lis_rx_buf);
  for (int index = 4; index < 10; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = H3lis_rx_buf[index - 4];
  }
  // MPUの加速度をとる
  for (int index = 10; index < 16; index++)
  {
    // SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = MPU_rx_buf[index - 10];
  }
  // MPUの角速度をとる
  for (int index = 16; index < 22; index++)
  {
    // SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = MPU_rx_buf[index - 10];
  }
  // MPUの地磁気をとる
  for (int index = 22; index < 28; index++)
  {
    // SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = MPU_rx_buf[index - 10];
  }
  // LPSの気圧をとる
  if (count % 20 == 0)
  {
    for (int index = 28; index < 31; index++)
    {
      // SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = LPS_rx_buf[index - 28];
      count = 0;
    }
  }
  // delay(100); // 1000Hzより早くたたいてない？
  count++;
  CountSPIFlashDataSetExistInBuff++;

  // Serial.println("rx");
  // for (int i = 0; i < 256; i++)
  // {
  //   Serial.print(SPI_FlashBuff[i]);
  // }
  // Serial.println();
  // Serial.print("CountSPIFlashDataSetExistInBuff: ");
  // Serial.println(CountSPIFlashDataSetExistInBuff);
  if (CountSPIFlashDataSetExistInBuff == 8)
  {
    flash1.write(SPIFlashLatestAddress, SPI_FlashBuff);
    SPIFlashLatestAddress += 0x100;
    CountSPIFlashDataSetExistInBuff = 0;
    Serial.println("end");
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);

  // timer start
  timer1 = timerBegin(0, 80, true);
  timer2 = timerBegin(1, 80, true);

  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAttachInterrupt(timer2, &onTimer2, true);

  timerAlarmWrite(timer1, 1000, true);   // 1000 // 1 second timer
  timerAlarmWrite(timer2, 100000, true); // 100000 // 1 second timer

  timerAlarmEnable(timer1);
  timerAlarmEnable(timer2);

  Serial.println("Timer Start!");

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
    // if (Serial.available())
    // {
    // Serial.println("Serial.available()");
    // read SPI Flash & check SPI Flash is clear or not
    if (timeCounter1 > 0)
    {
      portENTER_CRITICAL(&timerMux1);
      timeCounter1--;
      portEXIT_CRITICAL(&timerMux1);
      RoutineWork();
      totalTimeCounter1++;
      Serial.print("1: ");
      Serial.println(totalTimeCounter1);
    }
    if (timeCounter2 > 0)
    {
      portENTER_CRITICAL(&timerMux2);
      timeCounter2--;
      portEXIT_CRITICAL(&timerMux2);
      totalTimeCounter2++;
      Serial.print("\t2: ");
      Serial.println(totalTimeCounter2);
    }
    // uint8_t rx[256];
    // flash1.read(SPIFlashLatestAddress, rx);
    // for (int i = 0; i < 256; i++)
    // {
    //   Serial.print(rx[i]);
    // }
    // Serial.println();

    // I have to change raw data to tx data.

    // delay(100);
    // flash1.read(SPIFlashLatestAddress, tx);
    // Serial.println("tx");
    // for (int i = 0; i < 256; i++)
    // {
    //   Serial.print(tx[i]);
    // }
    // Serial.println();
    // End of change

    // for (int i = 0; i < 256; i++)
    // {
    //   if (SPI_FlashBuff[i] != tx[i])
    //   {
    //     check = false;
    //   }
    // }
    // if (check)
    // {
    //   Serial.println("Data is correct");
    // }
    // else
    // {
    //   Serial.println("error");
    // }

    // Serial.println(SPIFlashLatestAddress);
    // }
    // delay(100);
  }
}