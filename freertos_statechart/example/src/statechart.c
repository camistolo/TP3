/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#include "src-gen/Prefix.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define EXAMPLE_1 (1)		/* Blink LED3 */
#define EXAMPLE_2 (2)		/* */
#define EXAMPLE_3 (3)		/* */
#define EXAMPLE_4 (4)		/* */
#define EXAMPLE_5 (5)		/* */
#define EXAMPLE_6 (6)		/* */
#define EXAMPLE_7 (7)		/* */
#define EXAMPLE_8 (8)		/* */
#define EXAMPLE_9 (9)		/* */

#define TEST (EXAMPLE_1)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
volatile bool SysTick_Time_Flag = false;

static Prefix statechart;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
}

#if (TEST == EXAMPLE_1)

void vApplicationTickHook()
{
	SysTick_Time_Flag = true;
}

void prefixIface_opLED(Prefix* handle, sc_integer LEDNumber, sc_boolean State)
{
	Board_LED_Set((uint8_t) LEDNumber, State);
}

const char *pcTextForMain = "\r\nExample 1 - Blink LED3\r\n";


/* LED3 toggle thread */
static void vLED3Task(void *pvParameters) {
	const char *pcTaskName = "LED3Task is running\r\n";

	/* Statechart Initialization */
	prefix_init(&statechart);
	prefix_enter(&statechart);

	/* Print out the name of this task. */
	DEBUGOUT(pcTaskName);
	while (1) {

		if (SysTick_Time_Flag == true) {
			SysTick_Time_Flag = false;
			prefixIface_raise_evTick(&statechart);					// Event -> evTick => OK

			prefix_runCycle(&statechart);							// Run Cycle of Statechart
		}
	}
}


/* UART (or output) thread */
static void vUARTTask(void *pvParameters) {
	int tickCnt = 0;

	while (1) {
		DEBUGOUT("Tick: %d \r\n", tickCnt);
		tickCnt++;

		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ);
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Blink LED3 thread */
	xTaskCreate((TaskFunction_t) vLED3Task,					/* Pointer to the function thats implement the task. */
				(const char * const) "LED3Task",			/* Text name for the task. This is to facilitate debugging only. */
				(uint16_t) configMINIMAL_STACK_SIZE,		/* Stack depth in words. */
				(void *) NULL,								/* We are not using the task parameter. */
				(UBaseType_t) (tskIDLE_PRIORITY + 1UL),		/* This task will run at priority 1. */
				(TaskHandle_t *) NULL);						/* We are not going to use the task handle. */

	/* UART output thread, simply counts seconds */
	xTaskCreate((TaskFunction_t) vUARTTask, (const char * const) "vTaskUart", (uint16_t) configMINIMAL_STACK_SIZE,
				(void *) NULL, (UBaseType_t) (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif
