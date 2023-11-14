#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdio.h>
#include "IOdef.h"
#include "Cli.h"
#include "Init.h"
#include "string.h"



void CLI_Transmit(uint8_t *pData, uint16_t Size)	
{
	for (uint16_t i=0; i < Size; i++)
	{
		sendbyte(pData[i]);
	}
}

int sendbyte(uint8_t b)
{
	while(!(USART2->SR & USART_SR_TXE)){}
	USART2->DR = b;
	
	return 0;
}

 char getbyte(void) 		//Recieve function
{	
	while(!(USART2->SR & USART_SR_RXNE));
	return (char)(USART2->DR);
}

void set_screen(void)
{
	CLI_Transmit(SCROLL_REGION, sizeof(SCROLL_REGION));	//set scroll window
  CLI_Transmit(CLEAR_SCREEN, sizeof(CLEAR_SCREEN));		//clear the screen 
  CLI_Transmit(MOVE_CURSOR_MIDDLE, sizeof(MOVE_CURSOR_MIDDLE));		//move the cursor to the start of scroll window
	CLI_Transmit(BACKGROUND,sizeof(BACKGROUND));	
}

void CLI_Receive(uint8_t *pData, uint16_t Size)		
{
	for (uint16_t i=0; i < Size;i++)
	{
		uint8_t receivedChar = 0;													
		uint8_t receivedIndex = 0;
		char commandBuffer[10] = {0}; 											// Buffer to store received command

    while (receivedIndex < Size - 1) 
			{ 
       receivedChar = getbyte();									//check getbyte to see if there is any data and store it to recievedChar
       sendbyte(receivedChar); 	
			}
			 if (receivedChar == '\r' || receivedChar == '\n') 
				 {
          pData[receivedIndex] = '\0';					 
					CLI_Transmit((uint8_t*)"Invalid command. Type 'help' for available commands.\r\n", 54);
					memset(commandBuffer, 0, sizeof(commandBuffer));							       // Reset the command buffer
				 }
		commandBuffer[receivedIndex] = receivedChar;        										// Store the received character in the command buffer
    receivedIndex++;
		}
		
}
