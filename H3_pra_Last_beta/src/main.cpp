#include <Arduino.h>

#include <SPICREATE.h>
#include <H3LIS331_var2.h>

#define H3LIS331SCK 14
#define H3LIS331MISO 12
#define H3LIS331MOSI 13
#define H3LIS331CS 33

// count WhoAmIの回数
// int count = 0;

// #define LIS331HH_SCK 13
// #define LIS331HH_MISO 12
// #define LIS331HH_MOSI 11
// #define LIS331HH_CS 10

bool WhoAmI_Ok = false;
int64_t avg_u[3];
int64_t avg_d[3];
int64_t count_u = 1;
int64_t count_d = 1;
int count = 0;

bool first = true;
float x_first;
float y_first;
float z_first;

#define SPIFREQ 100000

H3LIS331 H3lis331;

SPICREATE::SPICreate SPIC;

#define SENSORS_GRAVITY_EARTH (9.80665F) /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON (1.6F)      /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN (275.0F)     /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD (SENSORS_GRAVITY_EARTH)

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("launched");

  SPIC.begin(VSPI, H3LIS331SCK, H3LIS331MISO, H3LIS331MOSI);
  //センサのbeginをかく。
  // void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
  H3lis331.begin(&SPIC, H3LIS331CS, SPIFREQ);
  uint8_t a;
  a = H3lis331.WhoImI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0x32)
  {
    Serial.println("H3LIS331 is OK");
    WhoAmI_Ok = true;
    //  count++;
    //  Serial.println(count);
  }
  else
  {
    Serial.println("H3LIS331 is NG");
    // Serial.println(count);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (WhoAmI_Ok == true)
  {
    if (count > 100)
    {
      Serial.println("end");
      while (1)
      {
      }
    }
    int16_t rx[3];
    H3lis331.Get(rx);

    // float rx_400g[3];
    // rx_400g[0] = (float)rx[0] * 2 * 400 / 65536;
    // rx_400g[1] = (float)rx[1] * 2 * 400 / 65536;
    // rx_400g[2] = (float)rx[2] * 2 * 400 / 65536;

    // change value
    // Serial.print("\t\tx: ");
    Serial.print(rx[0]);
    Serial.print(",");
    // Serial.print("\ty: ");
    Serial.print(rx[1]);
    Serial.print(",");
    // Serial.print("\tz: ");
    Serial.println(rx[2]);
    count++;

    // char cmd = Serial.read();
    // if (cmd == 'u')
    // {
    //   avg_u[0] += rx[0];
    //   avg_u[1] += rx[1];
    //   avg_u[2] += rx[2];
    //   count_u++;
    // }
    // if (cmd == 'd')
    // {
    //   avg_d[0] += rx[0];
    //   avg_d[1] += rx[1];
    //   avg_d[2] += rx[2];
    //   count_d++;
    // }
    // if (cmd == 'q')
    // {
    //   Serial.print("u: ");
    //   Serial.print(avg_u[0] / count_u);
    //   Serial.print(" ");
    //   Serial.print(avg_u[1] / count_u);
    //   Serial.print(" ");
    //   Serial.println(avg_u[2] / count_u);
    //   Serial.print("d: ");
    //   Serial.print(avg_d[0] / count_d);
    //   Serial.print(" ");
    //   Serial.print(avg_d[1] / count_d);
    //   Serial.print(" ");
    //   Serial.println(avg_d[2] / count_d);
    //   while (1)
    //   {
    //     yield();
    //   }
    // }

    // if (first)
    // {
    //   x_first = rx[0];
    //   y_first = rx[1];
    //   z_first = rx[2] + 1;
    //   first = false;
    // }
    // int16_t x_r = rx[0] - x_first;
    // int16_t y_r = rx[1] - y_first;
    // int16_t z_r = rx[2] - z_first;
    // x_r >>= 4;
    // y_r >>= 4;
    // z_r >>= 4;
    // float lsb_value = 2 * 400 * (float)1 / 4096;

    // float x_g = ((float)x_r * lsb_value);
    // float y_g = ((float)y_r * lsb_value);
    // float z_g = ((float)z_r * lsb_value);
    // float x = x_g * SENSORS_GRAVITY_STANDARD;
    // float y = y_g * SENSORS_GRAVITY_STANDARD;
    // float z = z_g * SENSORS_GRAVITY_STANDARD;
    // Serial.print("\t\tx: ");
    // Serial.print(x);
    // Serial.print("\ty: ");
    // Serial.print(y);
    // Serial.print("\tz: ");
    // Serial.println(z);

    // Serial.print("\t\tx: ");
    // Serial.print(rx[0]);
    // Serial.print("\ty: ");
    // Serial.print(rx[1]);
    // Serial.print("\tz: ");
    // Serial.println(rx[2]);

    // Serial.println(rx[3]);
    // Serial.println(sizeof(rx));
    // for (int t = 0; t < 10; t++)
    // {
    //   Serial.print(t);
    //   Serial.print(" ");
    //   Serial.println(rx[t]);
    //   delay(1000);
    // }
    delay(100);
  }
}