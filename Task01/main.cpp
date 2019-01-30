#include "stm32l1xx.h"

int delay(unsigned long int n)
{
  for(unsigned long int i = 0 ; i<n ; i++)
    asm("NOP");
  return 0;
}

int main()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER |= GPIO_MODER_MODER7_0;
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_7;
  GPIOB->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR7_1;
  while(true)
  {
    GPIOB->ODR |= GPIO_ODR_ODR_7;
    delay(100000);
    GPIOB->ODR &= ~GPIO_ODR_ODR_7;
    delay(100000);
  }
}
