#include "stm32l1xx.h"
#include "stm32l1xx_it.h"

extern "C"
void EXTI0_IRQHandler(void);

void InitAll(void);
void delay(unsigned long int n);

EXTI_InitTypeDef   EXTI_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

unsigned long int mode = 0;

int main(void)
{
  InitAll();
  __enable_irq ();
  NVIC_EnableIRQ (EXTI0_IRQn);
  EXTI->IMR |= EXTI_IMR_MR0;
 // EXTI_GenerateSWInterrupt(EXTI_Line0);
  while(1)
  { 
    GPIOB->ODR |= GPIO_ODR_ODR_7;
    delay(30000+mode*30000);
    GPIOB->ODR |= GPIO_ODR_ODR_6;
    delay(30000+mode*30000);
    GPIOB->ODR &= ~GPIO_ODR_ODR_6;
    delay(150000-mode*50000);
    delay(30000+mode*30000);
    GPIOB->ODR |= GPIO_ODR_ODR_6;
    delay(30000+mode*30000);
    GPIOB->ODR &= ~GPIO_ODR_ODR_6;
    GPIOB->ODR &= ~GPIO_ODR_ODR_7;
    delay(150000-mode*50000);
    delay(30000+mode*30000);
    GPIOB->ODR |= GPIO_ODR_ODR_6;
    delay(30000+mode*30000);
    GPIOB->ODR &= ~GPIO_ODR_ODR_6;
  }
}

void InitAll(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //Включение тактирования порта PB
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //Включение тактирования порта PA
  GPIOB->MODER|= GPIO_MODER_MODER6_0; //Инициализация порта PB6 на вывод
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;
  GPIOB->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR6_0 | GPIO_OSPEEDER_OSPEEDR6_1;
  GPIOB->MODER |= GPIO_MODER_MODER7_0;
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_7;
  GPIOB->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR7_1;
  GPIOA->MODER &= ~GPIO_MODER_MODER0; //Инициализация порта PA0 на ввод
  GPIOA->PUPDR &=  ~GPIO_PUPDR_PUPDR0_0 & ~GPIO_PUPDR_PUPDR0_1;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG_EXTILineConfig (EXTI_PortSourceGPIOA, EXTI_PinSource0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  return;
}

void delay(unsigned long int n)
{
  for(unsigned long int i = 0; i < n; i++)
    asm("NOP");
  return;
}

void EXTI0_IRQHandler(void)
{
  if(mode < 3)
    mode++;
  else
    mode = 0;
  delay(30000);
  //EXTI_ClearITPendingBit(EXTI_Line0);
  EXTI->PR |= EXTI_PR_PR0; 
}