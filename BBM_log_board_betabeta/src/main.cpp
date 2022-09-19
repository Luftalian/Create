#include <Arduino.h>
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

#define SPIFREQ 100000

#define loggingPeriod 2

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

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
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = 0xFF & (0 >> (8 * index));
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

IRAM_ATTR void logging(void *parameters)
{
  static int count = 0;
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    char bfChar[128] = "very tekitou na data\n";

    // if (SDIO.appendQueue(bfChar) == 1)
    // {
    //   vTaskDelete(&xlogHandle);
    //   Serial.println("queue filled!");
    //   ESP.restart();
    // }
    count++;
    if (count % 5 == 0) // per 0.01s = 1000Hz
    {
      Serial.println("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
      RoutineWork();
    }

    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start");
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  Serial.println("SPI1");
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  Serial.println("flash1");
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  Serial.println("H3lis331");

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
    while (1)
    {
      delay(1000);
    }
  }
  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
}

void loop()
{
  // put your main code here, to run repeatedly:
}