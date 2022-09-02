
// Basic demo for accelerometer readings from Adafruit H3LIS331

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_H3LIS331.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define H3LIS331_SCK 14
#define H3LIS331_MISO 12
#define H3LIS331_MOSI 13
// Used for hardware & software SPI
#define H3LIS331_CS 33

Adafruit_H3LIS331 lis = Adafruit_H3LIS331();

bool first = true;
float x_first;
float y_first;
float z_first;

void setup(void)
{
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("H3LIS331 test!!!!!!!!");

  // hardware SPI
  // if (!lis.begin_SPI(H3LIS331_CS))
  // {
  // software SPI
  if (!lis.begin_SPI(H3LIS331_CS, H3LIS331_SCK, H3LIS331_MISO, H3LIS331_MOSI))
  {
    // if (!lis.begin_I2C())
    // { // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1)
      yield();
  }
  Serial.println("H3LIS331 found!");

  // lis.setRange(H3LIS331_RANGE_100_G); // 100, 200, or 400 G!
  Serial.print("Range set to: ");
  switch (lis.getRange())
  {
  case H3LIS331_RANGE_100_G:
    Serial.println("100 g");
    break;
  case H3LIS331_RANGE_200_G:
    Serial.println("200 g");
    break;
  case H3LIS331_RANGE_400_G:
    Serial.println("400 g");
    break;
  }
  lis.setDataRate(LIS331_DATARATE_1000_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate())
  {

  case LIS331_DATARATE_POWERDOWN:
    Serial.println("Powered Down");
    break;
  case LIS331_DATARATE_50_HZ:
    Serial.println("50 Hz");
    break;
  case LIS331_DATARATE_100_HZ:
    Serial.println("100 Hz");
    break;
  case LIS331_DATARATE_400_HZ:
    Serial.println("400 Hz");
    break;
  case LIS331_DATARATE_1000_HZ:
    Serial.println("1000 Hz");
    break;
  case LIS331_DATARATE_LOWPOWER_0_5_HZ:
    Serial.println("0.5 Hz Low Power");
    break;
  case LIS331_DATARATE_LOWPOWER_1_HZ:
    Serial.println("1 Hz Low Power");
    break;
  case LIS331_DATARATE_LOWPOWER_2_HZ:
    Serial.println("2 Hz Low Power");
    break;
  case LIS331_DATARATE_LOWPOWER_5_HZ:
    Serial.println("5 Hz Low Power");
    break;
  case LIS331_DATARATE_LOWPOWER_10_HZ:
    Serial.println("10 Hz Low Power");
    break;
  }
}

void loop()
{
  /* Get a new sensor event, normalized */
  sensors_event_t event;
  lis.getEvent(&event);

  if (first)
  {
    x_first = event.acceleration.x;
    y_first = event.acceleration.y;
    z_first = event.acceleration.z + 1;
    first = false;
  }

  /* Display the results (acceleration is measured in m/s^2) */
  // Serial.print("\t\tX: ");
  // Serial.print(event.acceleration.x);
  // Serial.print(" \tY: ");
  // Serial.print(event.acceleration.y);
  // Serial.print(" \tZ: ");
  // Serial.print(event.acceleration.z);
  // Serial.println(" m/s^2 ");

  /* Alternately, given the range of the H3LIS331, display the results measured in g */
  Serial.print("\t\tX:");
  Serial.print((event.acceleration.x - x_first) / SENSORS_GRAVITY_STANDARD);
  Serial.print(" \tY: ");
  Serial.print((event.acceleration.y - y_first) / SENSORS_GRAVITY_STANDARD);
  Serial.print(" \tZ: ");
  Serial.print((event.acceleration.z - z_first) / SENSORS_GRAVITY_STANDARD);
  Serial.println(" g");

  delay(1000);
}
// #include <Arduino.h>

// #include <SPICREATE.h>
// #include <H3LIS331.h>

// //ココナニ
// #define H3LIS331CS 26
// #define SPIFREQ 12000000

// H3LIS331 H3lis331;

// SPICREATE::SPICreate SPI;

// void setup()
// {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   Serial.println("launched");

//   SPI.begin();
//   //センサのbeginをかく。
//   // void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
//   H3lis331.begin(&SPI, H3LIS331CS, SPIFREQ);
//   // WhoamIをかく
//   int a;
//   a = H3lis331.WhoImI();
//   Serial.print("WhoAmI:");
//   Serial.println(a);
//   if (a == 0x32)
//   {
//     Serial.println("H3LIS331 is OK");
//   }
//   else
//   {
//     Serial.println("H3LIS331 is NG");
//   }
// }

// void loop()
// {
//   // put your main code here, to run repeatedly:
// }