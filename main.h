/* main.h */
#ifndef MAIN_H
#define MAIN_H

#include <stm32f4xx.h> // common stuff
#include <stm32f4xx_gpio.h> // gpio control
#include <stm32f4xx_rcc.h> // reset anc clocking

#include <usart.h>
#include "tm_stm32f4_mfrc522.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_gpio.h"
#include "defines.h"
#include "gpio.h"

#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <portmacro.h>

// forward declarations of task functions
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void vTask4(void *pvParameters);

int broj = 2;
int indeks_imena;

#endif
