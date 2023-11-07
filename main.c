#include <stdio.h>
//#include "stm32F103RB.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mainBLINKY_TASK_PRIORITY 		(tskIDLE_PRIORITY + 1 )
#define Cli_TASK_PRIORITY						(tskIDLE_PRIORITY + 2 )

static void vBlinkTask( void * parameters);
static void vCliTask (void * parameters);
int sendbyte(uint8_t b);
char getbyte(void);
static void USART2_IRQHandler(void);
static xQueueHandle xQueue;

int main(void)
{
	xQueue = xQueueCreate(1, sizeof (char));
	xTaskCreate(vBlinkTask, "Blinky", configMINIMAL_STACK_SIZE, NULL, mainBLINKY_TASK_PRIORITY, NULL);
	xTaskCreate(vCliTask, "CLI", configMINIMAL_STACK_SIZE, NULL, Cli_TASK_PRIORITY, NULL);
	
	RCC->APB2ENR |= (1u<<2) | (1u<<4) ;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; //enable USART2 clock
	GPIOA->CRL &= ~(1u<<22) &~ (1u<<23) &~(1u<<10) &~ (1u<<11);
	GPIOA->CRL |=  (1u<<20) |  (1u<<21) | (3<<8) | (2<<10);
	//GPIOA->CRL &= ~(1u<<22) &~ (1u<<23) &~(4<11);
	
	//AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;
	USART2->BRR = (8<<0) | (19<<4);  //hopefully baud 115200
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
	USART2->CR2 |= USART_CR2_CLKEN;
	
	
	
	/* Start the scheduler. */
	vTaskStartScheduler();
	
	
	return 0;
}


static void vBlinkTask( void * parameters)
{
	for (;;)
	{
	
		GPIOA->ODR |= (1u<<5);	
		USART2->DR = 0x60;
		vTaskDelay(1000);
		
		GPIOA->ODR &= ~(1u<<5);
		
		vTaskDelay(1500);
		
		
	}
}

static void vCliTask (void * parameters)
{
	uint8_t charRecieved;
	for(;;)
	{
		xQueueReceive(xQueue, &charRecieved, portMAX_DELAY);
		sendbyte(charRecieved);
	}
	
	
}

int sendbyte(uint8_t b) 	//Transmit function
{
  while (!(USART2->SR & USART_SR_TXE)){}     // Wait for the TXE flag to be set
  USART2->DR = b;		    // Write the character to the data register
	// Wait for the transmission to complete by checking the TC (transmission complete) flag
	// while (!(USART2->SR & USART_SR_TC));	<---{used in Next lab}
  return 0;  // Return 0 to indicate success
}

 char getbyte(void) 		//Recieve function
{	
	while(!(USART2->SR & USART_SR_RXNE));
	return (char)(USART2->DR);
}

static void USART2_IRQHandler(void)
{
	uint8_t RecievedChar;
	if((USART2->SR & USART_SR_RXNE))
	{
		RecievedChar = (char)(USART2->DR);
		xQueueSendToFrontFromISR(xQueue, &RecievedChar, NULL);
		USART2->SR &= ~USART_SR_RXNE;
	}	
}
