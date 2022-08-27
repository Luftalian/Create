#include <Arduino.h>

#include <SPICREATE.h>
#include <SPIflash.h>

#define S25FL512SSCK 12
#define S25FL512SMISO 27
#define S25FL512SMOSI 14
#define S25FL512SCS 33
// #define S25FL512SSCK 14
// #define S25FL512SMISO 12
// #define S25FL512SMOSI 13
// #define S25FL512SCS 33

#define SPIFREQ 120000
#define DATASETSIZE 256

// S25FL512S S25fl512s;
Flash S25fl512s;

SPICREATE::SPICreate SPIC;

void Serial_printHEX(int n)
{
  char buf[2];
  itoa(n, buf, 16);
  Serial.print(buf);
}

void data_debug_print(const uint8_t *data, uint16_t bytes)
{
  uint16_t depth = bytes / 16;
  for (uint16_t j = 0; j < depth; ++j)
  {
    for (uint16_t i = 0; i < 16; ++i)
    {
      uint8_t d = data[i + (j * 16)];
      Serial_printHEX((d >> 4) & 0x0F);
      Serial_printHEX((d >> 0) & 0x0F);
      Serial.print(",");
    }
    Serial.print("\r\n");
  }
}

void readAllFlash()
{
  for (int j = 0; j < 65536; j++)
  {
    unsigned char readbuffer[DATASETSIZE];
    S25fl512s.read(j * DATASETSIZE, readbuffer);
    for (int i = 0; i < DATASETSIZE; i++)
    {
      Serial.print(readbuffer[i]);
      Serial.print(',');
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("launched");

  SPIC.begin(VSPI, S25FL512SSCK, S25FL512SMISO, S25FL512SMOSI);

  S25fl512s.begin(&SPIC, S25FL512SCS, SPIFREQ);

  S25fl512s.erase();
  // S25fl512s.write(LOGGING::latestFlashPage * 256, dataset);
  // readAllFlash();

  // /*SPIflash用変数*/
  uint8_t rx_buf[256], tx_buf[256], j = 2; //受信用,送信用,SPIのチップセレクト
  // uint32_t addr = 0x00;                        // SPI flashの書き込み先アドレス
  // uint16_t length_ = 0x100, num_use_pages = 1; //データの長さ,SPI flashのページ

  // Serial.println("3");
  for (uint16_t i = 0; i < 256; i++)
  { //書き込み用データの作成 0から255の数字を格納
    tx_buf[i] = i;
  }
  S25fl512s.write(0, tx_buf);
  readAllFlash();

  // //書き込み用データの確認
  // data_debug_print(tx_buf, length_);
  // Serial.println("4");

  // // flash_wren(j); //書き込み有効化
  // // これはない？

  // for (uint16_t i = 0; i < num_use_pages; i++)
  // {
  //   // flash_pp(addr, tx_buf, length_, j); //書き込み
  //   S25fl512s.write(addr, tx_buf);
  //   addr += 0x100; //ページの更新に合わせてアドレス更新（今回は1ページのみなので実際は必要ない）
  // }
  // Serial.println("5");

  // Serial.println("write finish");

  // addr = 0x00;
  // for (uint16_t i = 0; i < num_use_pages; i++)
  // {
  //   // flash_read(addr, rx_buf, length_, j); //データの読みだし
  //   S25fl512s.read(addr, rx_buf);
  //   data_debug_print(rx_buf, length_); //読みだしたデータの出力
  //   addr += 0x100;                     //ページの更新に合わせてアドレス更新（今回は1ページのみなので実際は必要ない）
  // }
  // Serial.println("6");

  // Serial.println("transfer finish");

  // // flash_wren(j); //書き込み有効化
  // Serial.println("Bulk Erase ...");
  // // flash_be(j); //全データを消去，だいたい40秒ほど時間がかかる。
  // S25fl512s.erase();
  // // while (flash_is_wip(j))
  // // { //データ消去が完了するまで"."を送り続ける
  // //   Serial.println(".");
  // // }

  // Serial.println("Erase Done\r\n");

  // addr = 0x00;
  // for (uint16_t i = 0; i < num_use_pages; i++)
  // { //データを送り消去できていることを確認する
  //   // flash_read(addr, rx_buf, length_, j);
  //   S25fl512s.read(addr, rx_buf);
  //   data_debug_print(rx_buf, length_);
  //   addr += 0x100;
  // }

  // Serial.println("transfer finish");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
