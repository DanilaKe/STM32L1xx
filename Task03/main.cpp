#include "stm32l1xx.h"
#include "stm32l1xx_lcd.h"
#include <stdio.h>
void gpio(void);
void controller(void);

static void clear_lcd_ram(void);
static void write_mask_to_lcd_ram (int, uint16_t, int);
static void write_char_to_lcd_ram (int, uint8_t, bool);
static void write_string_to_lcd_ram (char*,int);
void delay(unsigned long int);
void InitAll(void);

int mode = 0;

extern "C"
void EXTI0_IRQHandler(void);

EXTI_InitTypeDef   EXTI_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

int main()
{
gpio();
controller();
while(LCD->SR & LCD_SR_UDR);
int n = 20;
char str[20] = {"      123"};
InitAll();
  __enable_irq ();
NVIC_EnableIRQ (EXTI0_IRQn);
EXTI->IMR |= EXTI_IMR_MR0;
while(1)
{
  write_string_to_lcd_ram(str,n);
}
}
void gpio(void)
{
RCC->AHBENR |= 0x7;
GPIOA->MODER |= 0x802A00A8;
GPIOB->MODER |= 0xAAAA0A80;
GPIOC->MODER |= 0xAAA0AA;
GPIOA->OTYPER &= ~0x870E;
GPIOB->OTYPER &= ~0xFF38;
GPIOC->OTYPER &= ~0xFCF;
GPIOA->PUPDR &= ~0xC03F00FC;
GPIOB->PUPDR &= ~0xFFFF0FC0;
GPIOC->PUPDR &= ~0xFFF0FF;
GPIOA->OSPEEDR &= ~0xC03F00FC;
GPIOB->OSPEEDR &= ~0xFFFF0FC0;
GPIOC->OSPEEDR &= ~0xFFFFF0FF;
GPIOA->AFR[0] |= 0xBBB0;
GPIOA->AFR[1] |= 0xB0000BBB;
GPIOB->AFR[0] |= 0xBBB000;
GPIOB->AFR[1] |= 0xBBBBBBBB;
GPIOC->AFR[0] |= 0xBB00BBBB;
GPIOC->AFR[1] |= 0xBBBB;
}
void controller(void)
{
RCC->APB1ENR |= 0x10000200;
PWR->CR |= 0x100;
RCC->CSR |= 0x800000;
RCC->CSR &= ~0x800000;
RCC->CSR |= 0x100;
while(!(RCC->CSR&RCC_CSR_LSERDY));
RCC->CSR |= 0x10000;
LCD->CR &= ~0x60;
LCD->CR |= 0x40;
LCD->CR &= ~0x1C;
LCD->CR |= 0xC;
LCD->CR |= 0x80;
LCD->FCR &= ~0x3C00000;
LCD->FCR &= ~0x3C0000;
LCD->FCR |= 0x1040000;
LCD->FCR &= ~0x1C00;
LCD->FCR |= 0x800;
while(!(LCD->SR&LCD_SR_FCRSR));
LCD->CR &= ~0x2;
LCD->CR |= 0x1;
while(!(LCD->SR&LCD_SR_RDY));
while(!(LCD->SR&LCD_SR_ENS));
}

static void clear_lcd_ram(void)
{
	LCD->RAM[0] = 0;
	LCD->RAM[2] = 0;
	LCD->RAM[4] = 0;
	LCD->RAM[6] = 0;
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


static void write_mask_to_lcd_ram (int position, uint16_t mask, int clear_before)
{
	/* Every pixel of character at position can be accessed
	   as LCD_RAM_COMx & (1 << Px) */
	int P1,P2,P3,P4;
	if (position < 2) P1 = 2*position;
	else P1 = 2*position+4;
	
	if (position == 1) P2 = P1+5;
	else P2 = P1+1;
	
	if (position < 3) P3 = (23-2*position)+6;
	else P3 = (23-2*position)+4;
	
	if (position == 5) {
		P4 = P3;
		P3 -= 1;
	} else {
		P4 = P3 - 1;
	}

	uint32_t COM0,COM1,COM2,COM3;
	COM0 = LCD->RAM[0];
	COM1 = LCD->RAM[2];
	COM2 = LCD->RAM[4];
	COM3 = LCD->RAM[6];
	
	if (clear_before) {
		COM0&= ~(1<<P1 | 1<<P2 | 1<<P3 | 1<<P4);
		COM1&= ~(1<<P1 | 1<<P2 | 1<<P3 | 1<<P4);
		COM2&= ~(1<<P1 | 1<<P2 | 1<<P3 | 1<<P4);
		COM3&= ~(1<<P1 | 1<<P2 | 1<<P3 | 1<<P4);
	}
	
	COM0 |= ((mask >> 0x1) & 1) << P4 | ((mask >> 0x4) & 1) << P1
	      | ((mask >> 0x6) & 1) << P3 | ((mask >> 0xA) & 1) << P2;
	COM1 |= ((mask >> 0x0) & 1) << P4 | ((mask >> 0x2) & 1) << P2
	      | ((mask >> 0x3) & 1) << P1 | ((mask >> 0x5) & 1) << P3;
	COM3 |= ((mask >> 0x7) & 1) << P3 | ((mask >> 0x8) & 1) << P4
	      | ((mask >> 0xB) & 1) << P1 | ((mask >> 0xE) & 1) << P2;
	COM2 |= ((mask >> 0x9) & 1) << P4 | ((mask >> 0xC) & 1) << P1
	      | ((mask >> 0xD) & 1) << P3 | ((mask >> 0xF) & 1) << P2;

	LCD->RAM[0] = COM0;
	LCD->RAM[2] = COM1;
	LCD->RAM[4] = COM2;
	LCD->RAM[6] = COM3;
}

static void write_char_to_lcd_ram (int position, uint8_t symbol, bool clear_before)
{
	uint16_t from_ascii[0x60] = {
	  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	  /*         !       "       #       $      %        &       ' */
	  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	  /* (       )       *       +       ,       -       .       / */
	  0x0000, 0x0000, 0x3FC0, 0x1540, 0x0000, 0x0440, 0x4000, 0x2200,
	  /* 0       1       2       3       4       5       6       7 */
	  0x003F, 0x0006, 0x045B, 0x044F, 0x0466, 0x046D, 0x047D, 0x2201,
	  /* 8       9       :       ;       <       =       >       ? */
	  0x047F, 0x046F, 0x8000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	  /* @       A       B       C       D       E       F       G */
	  0x0000, 0x0477, 0x047C, 0x0039, 0x045E, 0x0479, 0x0471, 0x043D,
	  /* H       I       J       K       L       M       N       O */
	  0x0476, 0x1109, 0x001E, 0x1B00, 0x0038, 0x02B6, 0x08B6, 0x003F,
	  /* P       Q       R       S       T       U       V       W */
	  0x0473, 0x0467, 0x0C73, 0x046D, 0x1101, 0x003E, 0x0886, 0x2836,
	  /* X       Y       Z       [       \       ]       ^       _ */
	  0x2A80, 0x1280, 0x2209, 0x0000, 0x0880, 0x0000, 0x0000, 0x0008
	};
	if (symbol > 0x60) return; // masks not defined. Nothing to display
	
	write_mask_to_lcd_ram (position, from_ascii[symbol], clear_before);
}
static void write_string_to_lcd_ram (char* str,int n)
{
  clear_lcd_ram (); 	/* all segments off */
  for(int i = 0;i < n && i < 6; i ++)
    write_char_to_lcd_ram (i,str[i], true);
  LCD->SR |= LCD_SR_UDR;
    delay(100000-6000*mode);
  if(n > 5)
    for(int i = 1; i < n-5; i++)
    {
      for(int j = 0; j < 6; j++)
        write_char_to_lcd_ram (j,str[i+j], true);
      LCD->SR |= LCD_SR_UDR;
      delay(100000-6000*mode);
    }
  delay(100000);
}

void delay(unsigned long int n)
{
  while(n)
    n--;
  return;
}

void EXTI0_IRQHandler(void)
{
  if(mode < 10)
    mode++;
  else
    mode = 0;
  char str[6] = {"MODE"};
  str[5]=48+mode;
  if(mode==10)
  {
    str[4]=49;
    str[5]=48;
  }
  printf(str);
  write_string_to_lcd_ram(str,6);
  delay(30000);
  EXTI->PR |= EXTI_PR_PR0; 
}