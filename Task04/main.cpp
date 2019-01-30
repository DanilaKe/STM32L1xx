#include <stdio.h>
//#include <stdlib.h>
#include "stm32l1xx.h"
#include "lcd_ds18b20.cpp"
#include "onewire.h"
int num = 0;
void print_temp(uint8_t*);

void main(void)
{
  OW_Init();
  OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
  delay(1000000);
  uint8_t buf[2];
  while(true)
  {
    OW_Init();
    OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
    OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, buf,12, 2);
    print_temp(buf);
  }
}

void print_temp(uint8_t* buf)
{
  float data;
  uint16_t temperature = buf[0]|(buf[1]<<8);
  if (temperature & 0x8000) 
  {
    temperature = ~temperature + 1;
    data = 0.0 - (temperature / 16.0);
  }
  data = temperature/16.0 ;
  char ss[10];
  //printf("%f\n%s\n",data,ss);
  int lcd_array[4];
  int data_int = data*100;
  for(int i = 3; i >= 0; i--)
  {
    lcd_array[i] = data_int%10;
    data_int /= 10;
  }
  print_lcd(lcd_array);
}