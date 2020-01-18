/* usart.c */
#include <usart.h>

// RX FIFO buffer
char RX_BUFFER[BUFSIZE];
int RX_BUFFER_HEAD, RX_BUFFER_TAIL;
// TX state flag
uint8_t TxReady;

const char message[] = "now exiting loopback mode";


void USART1_IRQHandler(void)
{
		static char rx_char;
		static char rx_head;

		// RX event
		if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
		{
				USART_ClearITPendingBit(USART1, USART_IT_RXNE);
				rx_char = USART_ReceiveData(USART1);
				// check for buffer overrun:
				rx_head = RX_BUFFER_HEAD + 1;
				if (rx_head == BUFSIZE) rx_head = 0;
				if (rx_head != RX_BUFFER_TAIL)
				{
						// adding new char will not cause buffer overrun:
						RX_BUFFER[RX_BUFFER_HEAD] = rx_char;
						RX_BUFFER_HEAD = rx_head; // update head
				}
		}

		// TX event
		if (USART_GetITStatus(USART1, USART_IT_TC) == SET)
		{
				USART_ClearITPendingBit(USART1, USART_IT_TC);
				TxReady = 1;
		}
}


void USART1_SendChar(char c)
{
		while(!TxReady);
		USART_SendData(USART1, c);
		TxReady = 0;
}

void USART1_printfMessage(char* nes)
{
		while(*nes != 0){
			USART1_SendChar(*nes);
			++nes;
		}
		return;
}

void USART1_SendNumber(uint8_t x)
{
  char value[10]; //a temp array to hold results of conversion
  int i = 0; //loop index
  
  do
  {
    value[i++] = (char)(x % 10) + '0'; //convert integer to character
    x /= 10;
  } while(x);
  
  while(i) //send data
  {
		USART1_SendChar(value[--i]);
  }
	return;
}

void USART1_SendArrayOfNumbers(uint8_t* numbs, int size)
	{
		while(size != 0){
			USART1_SendNumber(*numbs);
			USART1_SendChar(':');
			++numbs;
			--size;
		} 
		return;
}
	
void USART1_DecToHexSend(uint8_t x){    
    // char array to store hexadecimal number 
    char hexaDeciNum[100]; 
      
    // counter for hexadecimal number array 
    int i = 0; 
    while(x!=0) 
    {    
        // temporary variable to store remainder 
        int temp  = 0; 
          
        // storing remainder in temp variable. 
        temp = x % 16; 
          
        // check if temp < 10 
        if(temp < 10) 
        { 
            hexaDeciNum[i] = temp + 48; 
            i++; 
        } 
        else
        { 
            hexaDeciNum[i] = temp + 55; 
            i++; 
        } 
          
        x = x/16; 
    } 
      
    // printing hexadecimal number array in reverse order 
    while(i) //send data
		{
		USART1_SendChar(hexaDeciNum[--i]);
		}
		return;
} 
  

void USART1_DectoHexArray(uint8_t* numbs, int size)
	{
		while(size != 0){
			USART1_DecToHexSend(*numbs);
			USART1_SendChar(':');
			++numbs;
			--size;
		}
		return;
}
	
int USART1_Dequeue(char* c)
{
		int ret;
		ret = 0;
		*c = 0;
		NVIC_DisableIRQ(USART1_IRQn);
		if (RX_BUFFER_HEAD != RX_BUFFER_TAIL)
		{
				*c = RX_BUFFER[RX_BUFFER_TAIL];
				RX_BUFFER_TAIL++;
				if (RX_BUFFER_TAIL == BUFSIZE) RX_BUFFER_TAIL = 0;
				ret = 1;
		}
		NVIC_EnableIRQ(USART1_IRQn);
		return ret;
}


void USART1_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		USART_InitTypeDef USART_InitStruct;
		NVIC_InitTypeDef NVIC_InitStructure;

		// enable peripheral clocks (note: different bus interfaces for each peripheral!)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		// map port B pins for alternate function
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 6 (TX) and 7 (RX) will be used for USART1
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // GPIO pins defined as alternate function
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // I/O pins speed (signal rise time)
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // push-pull output
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // activates pullup resistors
		GPIO_Init(GPIOB, &GPIO_InitStruct); // set chosen pins
		
		// set alternate function to USART1 (from multiple possible alternate function choices)
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); // pins will automatically be assigned to TX/RX - refer to datasheet to see AF mappings
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

		// use USART_InitStruct to config USART1 peripheral
		USART_InitStruct.USART_BaudRate = BAUDRATE; // set baudrate from define
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;// 8 data bits
		USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1 stop bit
		USART_InitStruct.USART_Parity = USART_Parity_No; // no parity check
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // no HW control flow
		USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // enable both character transmit and receive
		USART_Init(USART1, &USART_InitStruct); // set USART1 peripheral

		// set interrupt triggers for USART1 ISR (but do not enable USART1 interrupts yet)
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);// should be disbled
		USART_ITConfig(USART1, USART_IT_TC, ENABLE); // transmission completed event (for reseting TxReady flag)
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // character received (to trigger buffering of new character)

		TxReady = 1; // USART1 is ready to transmit
		RX_BUFFER_HEAD = 0; RX_BUFFER_TAIL = 0; // clear rx buffer

		// prepare NVIC to receive USART1 IRQs
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // configure USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// max. priority
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // max. priority
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // enable USART1 interrupt in NVIC
		NVIC_Init(&NVIC_InitStructure); // set NVIC for USART1 IRQ

		// enables USART1 interrupt generation
		USART_Cmd(USART1, ENABLE);
}
