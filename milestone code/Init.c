#include "Init.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f10x.h"
#include "IOdef.h"

void serial_open(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

		GPIOA->CRL |= GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1; // PA2 as alternate function (TX)
  	GPIOA->CRL &= ~GPIO_CRL_CNF2_0; 									 // Clear CNF2 bits to configure as output
  	GPIOA->CRL |= GPIO_CRL_CNF2_1;  									 // Set CNF2 bits for alternate function push-pull
	
  	GPIOA->CRL &= ~GPIO_CRL_MODE3_0 & ~GPIO_CRL_MODE3_1; // PA3 as input (RX)
  	GPIOA->CRL &= ~GPIO_CRL_CNF3_0; 										 // Clear CNF3 bits to configure as input
  	GPIOA->CRL |= GPIO_CRL_CNF3_1;  										 // Set CNF3 bits for pull-up/pull-down input
	
		GPIOA->CRL |= GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1;	//Configure PA5 for led
  	GPIOA->CRL &= ~GPIO_CRL_CNF5_0 & ~GPIO_CRL_CNF5_1;
	
		USART2->CR2 &= ~USART_CR2_STOP;		//1 stop bit
		USART2->CR1 &= ~USART_CR1_M;			// 8 data bits
		USART2->CR1 &= ~USART_CR1_PCE;		// no parity
		USART2->BRR = 0x0138;							// baud rate
		
		USART2->CR1 |=  USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; //Enable USART

}
