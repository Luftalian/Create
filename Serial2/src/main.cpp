#include <Arduino.h>

// #define LED1 0
#define RX_PIN 16
#define TX_PIN 17

void setup()
{
  // put your setup code here, to run once:
  // Serial.begin(115200);
  // delay(100);
  delay(1000);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  // Serial2.begin(19200);
  while (!Serial2)
    ;
  // delay(100);
  // Serial.println("start Serial");
  // digitalWrite(LED1, HIGH);
}

void loop()
{
  // Serial2.println('a');
  // put your main code here, to run repeatedly:
  // Serial2.write('e');
  if (Serial2.available())
  {
    char receive = Serial2.read();
    // Serial2.write(receive);
    Serial2.print('z');
  }
}