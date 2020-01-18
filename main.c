#include <main.h>

uint8_t CardID[4], pom[4];
uint8_t MyID[][5] = {{91,102,46,12,31},
										 {91,125,129,2,1}};
//{{91,102,46,12,31}, {91,153,129,15,93}}; ispravni brojevi
char imena[5][20] = { "\tTomislav\n\r", "\tKristijan\n\r"}; 
int tsk2=0, tsk3=0;

xTaskHandle xTaskHandle1, xTaskHandle2, xTaskHandle3;

int main(void)
{	
		USART1_Init();
		TM_MFRC522_Init();
		gpio_init();
		timer2_init();
		xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE, NULL, 1, &xTaskHandle1);
		vTaskStartScheduler();
}
	void vTask1(void *pvParameters){
				while(1){
					if (TM_MFRC522_Check(CardID) == MI_OK) {
						
						
						
							if (tsk2!=0 && tsk3!=0){
									xTaskCreate(vTask2,"TASK2", configMINIMAL_STACK_SIZE, NULL, 1, &xTaskHandle2);
									xTaskCreate(vTask3,"TASK3", configMINIMAL_STACK_SIZE, NULL, 1, &xTaskHandle3);}
							else{
								vTaskDelete(xTaskHandle2);
								vTaskDelete(xTaskHandle3);
					}
			/*		else{
						USART1_printfMessage("\n\n\rPogresni tip kartice");
						USART1_printfMessage("\n\rbroj kartice u DEC:\t");
						USART1_SendArrayOfNumbers(CardID, sizeof(CardID)); 
						USART1_printfMessage("\n\rbroj kartice u HEX:\t");
						USART1_DectoHexArray(CardID, sizeof(CardID));
						while(TM_MFRC522_Check(CardID) != MI_OK);
		*/
						}
					}}
		
	void vTask2(void *pvParameters){
					tsk2=1;
							if(TM_MFRC522_Compare(CardID, MyID, broj, &indeks_imena) == MI_OK){
									USART1_printfMessage("\n\n\rKartica ok, korisnik:\t");
								  USART1_printfMessage(imena[indeks_imena]);
									USART1_printfMessage("\n\rbroj kartice u DEC:\t");
									USART1_SendArrayOfNumbers(CardID, sizeof(CardID)); 
									USART1_printfMessage("\n\rbroj kartice u HEX:\t");
									USART1_DectoHexArray(CardID, sizeof(CardID));
									}
							else{
									USART1_printfMessage("\n\n\rNije dobra\n\r");
									USART1_printfMessage("\rbroj kartice u DEC:\t");
									USART1_SendArrayOfNumbers(CardID, sizeof(CardID)); 
									USART1_printfMessage("\n\rbroj kartice u HEX:\t");
									USART1_DectoHexArray(CardID, sizeof(CardID));}
					tsk2=0;	
						}
	
	void vTask3(void *pvParameters){
					tsk3=1;
					 		if(TM_MFRC522_Compare(CardID, MyID, broj, &indeks_imena) == MI_OK){
									gpio_led_state(LED4_GREEN_ID, 1);
									gpio_led_state(LED5_RED_ID, 0); 
									timer2_wait_millisec(2000); 
									gpio_led_state(LED4_GREEN_ID, 0);
									gpio_led_state(LED5_RED_ID, 0); 
									} 
							else {
									gpio_led_state(LED4_GREEN_ID, 0);
									gpio_led_state(LED5_RED_ID, 1);
									timer2_wait_millisec(2000); 
									gpio_led_state(LED4_GREEN_ID, 0);
									gpio_led_state(LED5_RED_ID, 0); 
									}  
					tsk3=0;
						}
					
				


/*dodat FreeRTOS da ceka u ovoj zadnjoj while petlji i kad nastane MIOK tek ondaa da se stvore zadaci. 
paralelno se mogu slati podaci preko USARTa i provjeravat jesu li dozvoljeni*/						