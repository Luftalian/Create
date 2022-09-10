#include <Arduino.h>
// #define BTN_STOP_ALARM 0

// hw_timer_t *timer = NULL;
// volatile SemaphoreHandle_t timerSemaphore;
// portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// volatile uint32_t isrCounter = 0;
// volatile uint32_t lastIsrAt = 0;

// void ARDUINO_ISR_ATTR onTimer()
// {
//   // Increment the counter and set the time of ISR
//   portENTER_CRITICAL_ISR(&timerMux);
//   isrCounter++;
//   lastIsrAt = millis();
//   portEXIT_CRITICAL_ISR(&timerMux);
//   // Give a semaphore that we can check in the loop
//   xSemaphoreGiveFromISR(timerSemaphore, NULL);
//   // It is safe to use digitalRead/Write here if you want to toggle an output
// }

// void setup()
// {
//   // put your setup code here, to run once:
//   Serial.begin(115200);

//   // Set BTN_STOP_ALARM to input mode
//   pinMode(BTN_STOP_ALARM, INPUT);

//   // Create semaphore to inform us when the timer has fired
//   timerSemaphore = xSemaphoreCreateBinary();

//   // Use 1st timer of 4 (counted from zero).
//   // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
//   // info).
//   timer = timerBegin(0, 80, true);

//   // Attach onTimer function to our timer.
//   timerAttachInterrupt(timer, &onTimer, true);

//   // Set alarm to call onTimer function every second (value in microseconds).
//   // Repeat the alarm (third parameter)
//   timerAlarmWrite(timer, 1000000, true);

//   // Start an alarm
//   timerAlarmEnable(timer);
// }

// void loop()
// {
//   // put your main code here, to run repeatedly:
//   // If Timer has fired
//   if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE)
//   {
//     uint32_t isrCount = 0, isrTime = 0;
//     // Read the interrupt count and time
//     portENTER_CRITICAL(&timerMux);
//     isrCount = isrCounter;
//     isrTime = lastIsrAt;
//     portEXIT_CRITICAL(&timerMux);
//     // Print it
//     Serial.print("onTimer no. ");
//     Serial.print(isrCount);
//     Serial.print(" at ");
//     Serial.print(isrTime);
//     Serial.println(" ms");
//   }
//   // If button is pressed
//   if (digitalRead(BTN_STOP_ALARM) == LOW)
//   {
//     // If timer is still running
//     if (timer)
//     {
//       // Stop and free timer
//       timerEnd(timer);
//       timer = NULL;
//     }
//   }
// }
#define OnB_LED 2
boolean togLED = false;

volatile int timeCounter1;
int totalTimeCounter1 = 0;

volatile int timeCounter2;
int totalTimeCounter2 = 0;

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
  Serial.print("interrupted   ");
  Serial.print("togLED = ");
  Serial.println(togLED);

  if (togLED == true)
  {
    digitalWrite(OnB_LED, LOW);
    togLED = false;
  }
  else
  {
    digitalWrite(OnB_LED, HIGH);
    togLED = true;
  }
}

void setup()
{
  pinMode(OnB_LED, OUTPUT);
  Serial.begin(115200);

  timer1 = timerBegin(0, 80, true);
  timer2 = timerBegin(1, 80, true);

  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAttachInterrupt(timer2, &onTimer2, true);

  timerAlarmWrite(timer1, 2000000, true); // 1 second timer
  timerAlarmWrite(timer2, 1000000, true); // 1 second timer

  timerAlarmEnable(timer1);
  timerAlarmEnable(timer2);

  Serial.println("Timer Start!");
}

void loop()
{

  if (timeCounter1 > 0)
  {
    portENTER_CRITICAL(&timerMux1);
    timeCounter1--;
    portEXIT_CRITICAL(&timerMux1);
    // RoutineWork();
    totalTimeCounter1++;
    Serial.print("1: ");
    Serial.println(totalTimeCounter1);
  }
  if (timeCounter2 > 0)
  {
    portENTER_CRITICAL(&timerMux2);
    timeCounter2--;
    portEXIT_CRITICAL(&timerMux2);
    // RoutineWork();
    totalTimeCounter2++;
    Serial.print("\t2: ");
    Serial.println(totalTimeCounter2);
  }
}

/****************** Interrupt handler *******************/

// void RoutineWork()
// {
//   Serial.print("interrupted   ");
//   Serial.print("togLED = ");
//   Serial.println(togLED);

//   if (togLED == true)
//   {
//     digitalWrite(OnB_LED, LOW);
//     togLED = false;
//   }
//   else
//   {
//     digitalWrite(OnB_LED, HIGH);
//     togLED = true;
//   }
// }