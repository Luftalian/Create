#include <Arduino.h>
#include <SPICREATE.h>
#include <S25FL512S.h>
#include <H3LIS331_beta.h>
#include <ICM20948_beta.h>
#include <LPS25HB.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define SCK2 19
#define MISO2 22
#define MOSI2 23

// #define H3LIS331SCK 14
// #define H3LIS331MISO 12
// #define H3LIS331MOSI 13
#define H3LIS331CS 32

#define LPSCS 14

#define ICMCS 13

// bool setcheck = false;

// センサのクラス
H3LIS331 H3lis331;

ICM icm20948;

LPS Lps25;

SPICREATE::SPICreate SPIC1;
SPICREATE::SPICreate SPIC2;
Flash flash1;

uint8_t count_lps = 0;

int CountSPIFlashDataSetExistInBuff = 0;
int CountSPIFlashDataSetExistInBuff_READ = 0;

uint8_t SPI_FlashBuff[256] = {};
// uint8_t tx[256] = {};
int16_t H3lisReceiveData[3];
uint8_t H3lis_rx_buf[6];

int16_t Icm20948ReceiveData[6];
uint8_t Icm20948_rx_buf[12];
uint8_t lps_rx[3];

uint32_t SPIFlashLatestAddress = 0x000;
// uint32_t SPIFlashLatestAddress_READ = 0x100;

#define SPIFREQ 100000

#define loggingPeriod 2
#define loggingPeriod2 1

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

unsigned long Record_time;

uint8_t checker = 0;

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
void setupcheck()
{
  delay(1000);
  digitalWrite(flashCS, HIGH);
  delay(100);
  digitalWrite(H3LIS331CS, HIGH);
  delay(100);
  digitalWrite(ICMCS, HIGH);
  delay(100);
  digitalWrite(LPSCS, HIGH);
  delay(100);
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("start");
  delay(100);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  Serial.println("SPI1");
  delay(100);
  SPIC2.begin(HSPI, SCK2, MISO2, MOSI2);
  delay(100);
  Serial.println("SPI2");
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  delay(100);
  Serial.println("flash1");
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  delay(100);
  Serial.println("H3lis331");
  icm20948.begin(&SPIC2, ICMCS, SPIFREQ);
  delay(100);
  Serial.println("icm20948");
  Lps25.begin(&SPIC2, LPSCS, SPIFREQ);
  delay(100);
  Serial.println("Lps25hb");

  micros();
  delay(100);
  Serial.println("Timer Start!");

  // WhoAmI
  uint8_t a;
  a = H3lis331.WhoAmI();
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
  delay(1000);
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
  // setcheck = true;
}

void RoutineWork()
{
  Serial.println("Running");
  if (timer.start_flag)
  {
    timer.start_time = micros();
    timer.start_flag = false;
  }
  Record_time = timer.Gettime_record();
  // From SPI, Get data is tx
  H3lisReceiveData[3] = {};
  H3lis_rx_buf[6] = {};
  Icm20948ReceiveData[6] = {};
  Icm20948_rx_buf[12] = {};
  lps_rx[3] = {};
  // CountSPIFlashDataSetExistInBuffは列。indexは行。
  //時間をとる
  for (int index = 0; index < 4; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = 0xFF & (Record_time >> (8 * index));
  }
  // for (int index = 3; index >= 0; index--)
  // {
  //   SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = 0xFF & (totalTimeCounter2 >> (8 * index));
  // }

  //加速度をとる
  H3lis331.Get(H3lisReceiveData, H3lis_rx_buf);
  icm20948.Get(Icm20948ReceiveData, Icm20948_rx_buf);
  for (int index = 4; index < 10; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = H3lis_rx_buf[index - 4];
  }
  // ICM20948の加速度をとる
  for (int index = 10; index < 16; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = Icm20948_rx_buf[index - 10];
  }
  // ICM20948の角速度をとる
  for (int index = 16; index < 22; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = Icm20948_rx_buf[index - 10];
  }
  // ICM20948の地磁気をとる
  // for (int index = 22; index < 28; index++)
  // {
  //   SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = Icm20948_rx_buf[index - 10];
  // }
  // LPSの気圧をとる
  if (count_lps % 20 == 0)
  {
    Lps25.Get(lps_rx);
    for (int index = 28; index < 31; index++)
    {
      SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = lps_rx[index - 28];
      count_lps = 0;
    }
  }
  // delay(100); // 1000Hzより早くたたいてない？
  count_lps++;
  CountSPIFlashDataSetExistInBuff++;

  // Serial.println("rx");
  // for (int i = 0; i < 256; i++)
  // {
  //   Serial.print(SPI_FlashBuff[i]);
  // }
  // Serial.println();
  // Serial.print("CountSPIFlashDataSetExistInBuff: ");
  // Serial.println(CountSPIFlashDataSetExistInBuff);
  if (CountSPIFlashDataSetExistInBuff >= 8)
  {
    flash1.write(SPIFlashLatestAddress, SPI_FlashBuff);
    SPIFlashLatestAddress += 0x100;
    CountSPIFlashDataSetExistInBuff = 0;
    // Serial.println("end");
  }
}

IRAM_ATTR void logging(void *parameters)
{
  // static uint64_t count = 0;
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    // count++;
    // if (count % 5 == 0) // per 0.01s = 1000Hz
    // {
    //   Serial.println("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
    // }
    // RoutineWork();
    checker++;
    vTaskDelayUntil(&xLastWakeTime, loggingPeriod2 / portTICK_PERIOD_MS); // 1ms = 1000Hz
  }
}

void setup()
{
  delay(5000);
  digitalWrite(flashCS, HIGH);
  delay(100);
  digitalWrite(H3LIS331CS, HIGH);
  delay(100);
  digitalWrite(ICMCS, HIGH);
  delay(100);
  digitalWrite(LPSCS, HIGH);
  delay(100);
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("start");
  delay(100);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  delay(100);
  SPIC2.begin(HSPI, SCK2, MISO2, MOSI2);
  delay(100);
  Serial.println("SPI1");
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  delay(100);
  Serial.println("flash1");
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  delay(100);
  Serial.println("H3lis331");
  icm20948.begin(&SPIC2, ICMCS, SPIFREQ);
  delay(100);
  Serial.println("icm20948");
  Lps25.begin(&SPIC2, LPSCS, SPIFREQ);
  delay(100);
  Serial.println("Lps25hb");

  Serial.println("Timer Start!");

  // WhoAmI
  uint8_t a;
  a = H3lis331.WhoAmI();
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

  // a = Lps25.WhoAmI();
  // Serial.print("WhoAmI:");
  // Serial.println(a);
  // if (a == 0b10111101)
  // {
  //   Serial.println("LPS25HB is OK");
  //   // WhoAmI_Ok_LPS25HB = true;
  // }
  // else
  // {
  //   Serial.println("LPS25HB is NG");
  // }

  Serial.println();

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
  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // if (!setcheck)
  // {
  //   setupcheck();
  // }
  if (checker > 0)
  {
    checker = 0;
    RoutineWork();
  }
}