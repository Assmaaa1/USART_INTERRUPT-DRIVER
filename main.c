#include "stm32f4xx.h"                  // Device header

/*

This program implement a usart driver and usart interrupt driver  for STM32f411 based board

It allows not only to send and receive data through uart interface but also 

to generate an interrupt each time a receiving data is detected.

A first sketch could be to control the rate of toggling the user led through the communicated 

toggling frequency.

The program is developed under keil uvision ,version 5 and it is tested on STM32F411E-DISCO board 

The baud rate = 9600  bps

Clock frequency = 16 Mhz

*/

#define USART2_CLK_EN (1<<17)
#define USART2_RX_PIN_MODE (1<<13) | (0<<12) // PD6 as alternate function
#define USART2_TX_PIN_MODE (1<<11) | (0<<10) // PD5 as alternate function
#define PORTD_CLK_EN (1<<3)
#define USART2_RX_EN (1<<2)
#define USART2_RX_INT_EN (1<<5)
#define USART2_TX_EN (1<<3)
#define USART2_TX_INT_EN (1<<7)
#define USART2_DIS (0<<13)
#define USART2_EN (1<<13) 
#define SAMPLING_MODE (0<<15) 
#define SET_PD6_ALT_FUN_REG (1<<24)|(1<<25)|(1<<26)|(0<<27)
#define SET_PD5_ALT_FUN_REG (1<<20)|(1<<21)|(1<<22)|(0<<23)
#define SET_LED (1<<15)
#define RESET_LED (1<<31)
#define PD15_MODE (1<<30)

void systickDelayMs(int n);
void UartInit(void);
void LED_PLAY(int val);
void USART2_IRQHandler(void);
void Usart_Write(int cr);

int main (void)

{ 
__disable_irq();
	
UartInit();
	
GPIOD->MODER |= PD15_MODE;
	
USART2->CR1 |= USART2_RX_INT_EN | USART2_TX_INT_EN;
	
NVIC_EnableIRQ(USART2_IRQn);
	
__enable_irq();	
while (1) {}
	
}	 

void UartInit(void)
{
  
  RCC->AHB1ENR |= PORTD_CLK_EN;
	RCC->APB1ENR |= USART2_CLK_EN;
	GPIOD->MODER |= USART2_RX_PIN_MODE | USART2_TX_PIN_MODE;
	GPIOD->AFR[0] |= SET_PD6_ALT_FUN_REG | SET_PD5_ALT_FUN_REG ; // set PD6 as alternate pin for USART2
	USART2->CR1 |= USART2_DIS; 
	USART2->CR1 |= USART2_RX_EN ;
	USART2->CR1 |= USART2_TX_EN ;
	USART2->CR1 |= SAMPLING_MODE ;
	USART2->BRR |= 0x683;		// 9600 baudrate at 16 MH;
	USART2->CR1 |= USART2_EN;
}


void Usart_Write(int cr)

{
   while (!(USART2->SR & 0x80)) {}
	 USART2->DR = (cr & 0xFF);	 

}




void systickDelayMs(int n)
{
	int i;
	SysTick->LOAD =8000-1;
	SysTick->VAL	=0;
	SysTick->CTRL =0x5;
	
	for(i=0;i<n;i++)
	{
		while((SysTick->CTRL & 0x10000)==0){}
	}
	SysTick->CTRL=0;
}



void USART2_IRQHandler(void)
{ 
     int c;
	  if(USART2->SR & 0x20) 
	  { c= USART2->DR ;
			Usart_Write(c);
			Usart_Write('\n');
			Usart_Write('\r');
			LED_PLAY(c);	
			
		}
	 
}

void LED_PLAY(int val)
	
{

	val %=16;
	for (; val>0; val--) 
	{  
		GPIOD->BSRR |= SET_LED;
		systickDelayMs(100);
		GPIOD->BSRR |= RESET_LED;
		systickDelayMs(100);
	}

  systickDelayMs(400);
}

