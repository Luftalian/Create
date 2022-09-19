#include <Arduino.h>
#include <SPICREATE.h>

#define loggingPeriod 2
#define loggingPeriod1 1
#define loggingPeriod2 0.9

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

IRAM_ATTR void logging(void *parameters)
{
  static int count = 100;
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
    if (count % 500 == 0) // per 1s 1Hz
    {
      Serial.println("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
      Serial.println(xLastWakeTime);
    }
    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS); // 2ms = 500Hz
    // vTaskDelayUntil(&xLastWakeTime, loggingPeriod1 / portTICK_PERIOD_MS); // 1ms = 1000Hz
    // vTaskDelayUntil(&xLastWakeTime, loggingPeriod2 / portTICK_PERIOD_MS); // 0.1ms = 10000Hz
  }
}

void setup()
{
  Serial.begin(115200);

  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
}

void loop()
{
}