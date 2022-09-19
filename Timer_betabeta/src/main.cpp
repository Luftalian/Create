#include <Arduino.h>
#include <SPICREATE.h>
#include <MainThread.h>

#define loggingPeriod 2

TimerHandle_t thand_test;
xTaskHandle xlogHandle;
uint64_t count = 0;

MainThread mainThread;

void IRAM_ATTR tickProcess()
{

  int startTask = micros();
  if (count %= 5000000000000)
  {
    Serial.println("bbb");
  }
  count++;

  int endTask = micros();
  if (endTask - startTask > 1000)
  {
    // Serial.println(endTask-startTask);
  }
}

IRAM_ATTR void logging(void *parameters)
{
  Serial.print("logging");
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
    tickProcess();
    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello world!");
  // mainThread = MainThread();

  // mainThread.begin();

  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
}

void loop()
{
  // Serial.print("loop");
}