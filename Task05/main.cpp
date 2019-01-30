#include "stm32l1xx.h"

#define LCD_MAIN_PORT_1_ODR GPIO_ODR_ODR_0
#define LCD_MAIN_PORT_2_ODR GPIO_ODR_ODR_1
#define LCD_MAIN_PORT_3_ODR GPIO_ODR_ODR_2
#define LCD_MAIN_PORT_4_ODR GPIO_ODR_ODR_3

#define LCD_TOP_ODR GPIO_ODR_ODR_15
#define LCD_BOTTOM_ODR GPIO_ODR_ODR_11
#define LCD_MIDDLE_ODR GPIO_ODR_ODR_14
#define LCD_DOT_ODR GPIO_ODR_ODR_12
#define LCD_LEFT_TOP_ODR GPIO_ODR_ODR_1
#define LCD_LEFT_BOTTOM_ODR GPIO_ODR_ODR_10
#define LCD_RIGHT_TOP_ODR GPIO_ODR_ODR_4
#define LCD_RIGHT_BOTTOM_ODR GPIO_ODR_ODR_13

long int LCD_MODER_A = GPIO_MODER_MODER1_0 | GPIO_MODER_MODER4_0;
long int LCD_MODER_B = GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0 |
                       GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;
long int LCD_MODER_C = GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 ;

void init_lcd(void);
void reset_lcd(void);
void delay(unsigned long int);

int number[2][10] = {{0,1,2,3,4,5,6,7,8,9},
                     {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B}};
int lcd_map[2][7] = {{LCD_TOP_ODR, LCD_RIGHT_TOP_ODR, LCD_RIGHT_BOTTOM_ODR, LCD_BOTTOM_ODR, LCD_LEFT_BOTTOM_ODR, LCD_LEFT_TOP_ODR, LCD_MIDDLE_ODR},
                     {0x40,0x20,0x10,0x08,0x04,0x02,0x01}};

int main()
{
  init_lcd();
  int temp[4] = {8,8,8,8};
  while(true)
  {
    reset_lcd();
    GPIOB->ODR |= LCD_DOT_ODR;
    GPIOC->ODR &= ~(LCD_MAIN_PORT_2_ODR);
    delay(300);
    for(int k = 0; k < 7; k++)
    {
      reset_lcd();
      if(k != 1 && k != 5)
        GPIOB->ODR |= lcd_map[0][k];
      else
        GPIOA->ODR |= lcd_map[0][k];
      for(int j = 0; j < 4; j++)
      {
        int i = 0;
        for(i = 0; i < 10 ; i++)
        {
          if(number[0][i] == temp[j])
            break;
        }
        if(number[1][i] & lcd_map[1][k]) 
          GPIOC->ODR &= ~(0x01 << j);
      }
      delay(300);
    }
  }
}

void init_lcd(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER = LCD_MODER_C;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  GPIOA->MODER = LCD_MODER_A;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER = LCD_MODER_B;
  
  return;
}

void delay(unsigned long int n)
{
  while(n)
    n--;
  return;
}

void reset_lcd(void)
{
  GPIOC->ODR |= LCD_MAIN_PORT_1_ODR | LCD_MAIN_PORT_2_ODR | LCD_MAIN_PORT_3_ODR | LCD_MAIN_PORT_4_ODR;
  GPIOB->ODR &= ~(LCD_TOP_ODR | LCD_BOTTOM_ODR | LCD_MIDDLE_ODR | LCD_DOT_ODR | LCD_LEFT_BOTTOM_ODR | LCD_RIGHT_BOTTOM_ODR);
  GPIOA->ODR &= ~(LCD_LEFT_TOP_ODR | LCD_RIGHT_TOP_ODR);
}