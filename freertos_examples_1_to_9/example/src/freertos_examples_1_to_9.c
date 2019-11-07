/*
 * @brief FreeRTOS examples
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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
#include "queue.h"
#include "semphr.h"
#include <stdlib.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define EXAMPLE_1 (1)		/* Creating tasks */
#define EXAMPLE_2 (2)		/* Using the task parameter */
#define EXAMPLE_3 (3)		/* Experimenting with priorities */
#define EXAMPLE_4 (4)		/* Using the Blocked state to create delay */
#define EXAMPLE_5 (5)		/* Converting the example tasks to use vTaskDelayUntil */
#define EXAMPLE_6 (6)		/* Combining blocking and non-blocking tasks */
#define EXAMPLE_7 (7)		/* Defining an idle task hook function */
#define EXAMPLE_8 (8)		/* Changing task priorities */
#define EXAMPLE_9 (9)		/* Deleting tasks */

#define TEST (EXAMPLE_1)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED state is off */
	Board_LED_Set(LED3, LED_OFF);
}


#if (TEST == EXAMPLE_1)		/* Creating tasks */

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		(0xfffff)

const char *pcTextForMain = "\r\nExample 1 - Creating tasks\r\n";

/* The two task functions. */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);


/* UART (or output) & LED ON thread */
static void vTask1(void *pvParameters)
{
	const char *pcTaskName = "Task 1 is running\r\n";
	volatile unsigned long ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {
			/* This loop is just a very crude delay implementation.  There is
			 * nothing to do in here.  Later exercises will replace this crude
			 * loop with a proper delay/sleep function. */
		}
	}
}


/* UART (or output) & LED OFF thread */
static void vTask2(void *pvParameters)
{
	const char *pcTaskName = "Task 2 is running\r\n";
	volatile unsigned long ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_OFF);

		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}

/*void vApplicationTickHook()
{
	Board_LED_Toggle(LED2);
}*/

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 1 - Creating tasks
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create one of the two tasks. */
	xTaskCreate(vTask1,										/* Pointer to the function thats implement the task. */
				(char *) "Task1",							/* Text name for the task. This is to facilitate debugging only. */
				configMINIMAL_STACK_SIZE,					/* Stack depth in words. */
				NULL,										/* We are not using the task parameter. */
				(tskIDLE_PRIORITY + 1UL),					/* This task will run at priority 1. */
				(xTaskHandle *) NULL);						/* We are not going to use the task handle. */

	/* Create the other task in exactly the same way. */
	xTaskCreate(vTask2, (char *) "Task2", configMINIMAL_STACK_SIZE,
				NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

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


#if (TEST == EXAMPLE_2)		/* Using the task parameter */

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		(0xfffff)

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 2 - Using the task parameter\r\n";

/* The task function. */
static void vTaskFunction(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vTaskFunction(void *pvParameters) {
	char *pcTaskName;
	volatile unsigned long ul;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		Board_LED_Toggle(LED3);

		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {
			/* This loop is just a very crude delay implementation.  There is
			 * nothing to do in here.  Later exercises will replace this crude
			 * loop with a proper delay/sleep function. */
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 2 - Using the task parameter
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create one of the two tasks. */
	xTaskCreate(vTaskFunction,							/* Pointer to the function thats implement the task. */
			(char *) "Task1",							/* Text name for the task. This is to facilitate debugging only. */
			configMINIMAL_STACK_SIZE,					/* Stack depth in words. */
			(void *) pcTextForTask1,					/* Pass the text to be printed in as the task parameter. */
			(tskIDLE_PRIORITY + 1UL),					/* This task will run at priority 1. */
			(xTaskHandle *) NULL);						/* We are not going to use the task handle. */

	/* Create the other task in exactly the same way.  Note this time that we
	 * are creating the SAME task, but passing in a different parameter.  We are
	 * creating two instances of a single task implementation. */
	xTaskCreate(vTaskFunction, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_3)		/* Experimenting with priorities */

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		(0xfffff)

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 3 - Experimenting with priorities\r\n";

/* The task function. */
static void vTaskFunction(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vTaskFunction(void *pvParameters) {
	char *pcTaskName;
	volatile unsigned long ul;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		Board_LED_Toggle(LED3);

		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {
			/* This loop is just a very crude delay implementation.  There is
			 * nothing to do in here.  Later exercises will replace this crude
			 * loop with a proper delay/sleep function. */
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 3 - Experimenting with priorities
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create the first task at priority 1... */
	xTaskCreate(vTaskFunction, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* ... and the second task at priority 2.  The priority is the second to
	 * last parameter. */
	xTaskCreate(vTaskFunction, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_4)		/* Using the Blocked state to create delay */

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 4 - Using the Blocked state to create delay\r\n";

/* The task function. */
static void vTaskFunction(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vTaskFunction(void *pvParameters) {
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		Board_LED_Toggle(LED3);

		/* Delay for a period.  This time we use a call to vTaskDelay() which
		 * puts the task into the Blocked state until the delay period has expired.
		 * The delay period is specified in 'ticks'. */
		vTaskDelay(250 / portTICK_RATE_MS);
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 4 - Using the Blocked state to create delay
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create the first task at priority 1... */
	xTaskCreate(vTaskFunction, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* ... and the second task at priority 2.  The priority is the second to
	last parameter. */
	xTaskCreate(vTaskFunction, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_5)		/* Converting the example tasks to use vTaskDelayUntil */


/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 5 - Converting the example tasks to use vTaskDelayUntil\r\n";

/* The task function. */
static void vTaskFunction(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vTaskFunction(void *pvParameters) {
	portTickType xLastWakeTime;
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* The xLastWakeTime variable needs to be initialized with the current tick
	 * count.  Note that this is the only time we access this variable.  From this
	 * point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	 * API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		Board_LED_Toggle(LED3);

		/* We want this task to execute exactly every 250 milliseconds.  As per
		 * the vTaskDelay() function, time is measured in ticks, and the
		 * portTICK_RATE_MS constant is used to convert this to milliseconds.
		 * xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
		 * have to be updated by this task code. */
		vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_RATE_MS));
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 5 - Converting the example tasks to use vTaskDelayUntil
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create the first task at priority 1... */
	xTaskCreate(vTaskFunction, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* ... and the second task at priority 2.  The priority is the second to
	 * last parameter. */
	xTaskCreate(vTaskFunction, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_6)		/* Combining blocking and non-blocking tasks */

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		(0x1ffff)

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForPeriodicTask = "Periodic task is running\r\n";

const char *pcTextForMain = "Example 6 - Combining blocking and non-blocking tasks\r\n";

/* The task functions. */
static void vContinuousProcessingTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vContinuousProcessingTask(void *pvParameters) {
	char *pcTaskName;
	volatile unsigned long ul;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task.  This task just does this repeatedly
		 * without ever blocking or delaying. */
		DEBUGOUT(pcTaskName);

		Board_LED_Toggle(LED3);

		/* A null loop has been inserted just to slow down the rate at which
		 * messages are sent down the debug link to the console.  Without this
		 * messages print out too quickly for the debugger display and controls
		 * to keep up.  For clarity this null loop is not shown in the eBook text
		 * as it is not relevant to the behaviour being demonstrated. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {
			/* This loop is just a very crude delay implementation.  There is
			 * nothing to do in here.  Later exercises will replace this crude
			 * loop with a proper delay/sleep function. */
			asm volatile( "NOP" );
		}
	}
}


/* UART (or output) & LED toggle thread */
static void vPeriodicTask(void *pvParameters) {
	portTickType xLastWakeTime;
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* The xLastWakeTime variable needs to be initialized with the current tick
	 * count.  Note that this is the only time we access this variable.  From this
	 * point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	 * API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		Board_LED_Toggle(LED3);

		/* We want this task to execute exactly every 10 milliseconds. */
		vTaskDelayUntil(&xLastWakeTime, (10 / portTICK_RATE_MS));
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 6 - Combining blocking and non-blocking tasks
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create two instances of the continuous processing task, both at priority	1. */
	xTaskCreate(vContinuousProcessingTask, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
	xTaskCreate(vContinuousProcessingTask, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Create one instance of the periodic task at priority 2. */
	xTaskCreate(vPeriodicTask, (char *) "Task 3", configMINIMAL_STACK_SIZE, (void *) pcTextForPeriodicTask,
				(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_7)		/* Defining an idle task hook function */


/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 7 - Defining an idle task hook function\r\n";

/* A variable that is incremented by the idle task hook function. */
unsigned long ulIdleCycleCount = 0UL;

/* The task function. */
static void vTaskFunction(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vTaskFunction(void *pvParameters) {
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Toggle(LED3);

		/* Print out the name of this task AND the number of times ulIdleCycleCount
		 * has been incremented. */
		DEBUGOUT("Idle ulCycleCount %d %s", ulIdleCycleCount, pcTaskName);

		/* Delay for a period.  This time we use a call to vTaskDelay() which
		 * puts the task into the Blocked state until the delay period has expired.
		 * The delay period is specified in 'ticks'. */
		vTaskDelay(250 / portTICK_RATE_MS);
	}
}


void vApplicationIdleHook(void)
{
	/* This hook function does nothing but increment a counter. */
	ulIdleCycleCount++;

	/* Best to sleep here until next systick */
	__WFI();
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 7 - Defining an idle task hook function
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create the first task at priority 1... */
	xTaskCreate(vTaskFunction, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* ... and the second task at priority 2.  The priority is the second to
	 * last parameter. */
	xTaskCreate(vTaskFunction, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_8)		/* Changing task priorities */

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 8 - Changing task priorities\r\n";

/* The two task functions. */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);

/* Used to hold the handle of Task2. */
xTaskHandle xTask2Handle;


/* UART (or output) & LED ON thread */
static void vTask1(void *pvParameters)
{
	unsigned portBASE_TYPE uxPriority;
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* This task will always run before Task2 as it has the higher priority.
	 * Neither Task1 nor Task2 ever block so both will always be in either the
	 * Running or the Ready state.
	 *
	 * Query the priority at which this task is running - passing in NULL means
	 * "return our own priority". */
	uxPriority = uxTaskPriorityGet((xTaskHandle)NULL);

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		/* Setting the Task2 priority above the Task1 priority will cause
		 * Task2 to immediately start running (as then Task2 will have the higher
		 * priority of the    two created tasks). */
		DEBUGOUT("About to raise the Task2 priority\r\n");
		vTaskPrioritySet(xTask2Handle, (uxPriority + 1));

		/* Task1 will only run when it has a priority higher than Task2.
		 * Therefore, for this task to reach this point Task2 must already have
		 * executed and set its priority back down to 0. */
	}
}


/* UART (or output) & LED OFF thread */
static void vTask2(void *pvParameters)
{
	unsigned portBASE_TYPE uxPriority;
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* Task1 will always run before this task as Task1 has the higher priority.
	 * Neither Task1 nor Task2 ever block so will always be in either the
	 * Running or the Ready state.
	 *
	 * Query the priority at which this task is running - passing in NULL means
	 * "return our own priority". */
	uxPriority = uxTaskPriorityGet((xTaskHandle)NULL);

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_OFF);

		/* For this task to reach this point Task1 must have already run and
		 * set the priority of this task higher than its own.
		 *
		 * Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		/* Set our priority back down to its original value.  Passing in NULL
		 * as the task handle means "change our own priority".  Setting the
		 * priority below that of Task1 will cause Task1 to immediately start
		 * running again. */
		DEBUGOUT("About to lower the Task2 priority\r\n");
		vTaskPrioritySet((xTaskHandle)NULL, (uxPriority - 2));
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 8 - Changing task priorities
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create the first task at priority 2.  This time the task parameter is
	 * not used and is set to NULL.  The task handle is also not used so likewise
	 * is also set to NULL. */
	xTaskCreate(vTask1, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);
	/* The task is created at priority 2....................^^^. */

	/* Create the second task at priority 1 - which is lower than the priority
	 * given to Task1.  Again the task parameter is not used so is set to NULL -
	 * BUT this time we want to obtain a handle to the task so pass in the address
	 * of the xTask2Handle variable. */
	xTaskCreate(vTask2, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 1UL), &xTask2Handle);
	/* The task handle is the last parameter......................^^^^^^^^^^^^^ */

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_9)		/* Deleting tasks */

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";

const char *pcTextForMain = "\r\nExample 9 - Deleting tasks\r\n";

/* The two task functions. */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);

/* Used to hold the handle of Task2. */
xTaskHandle xTask2Handle;


/* UART (or output) & LED ON thread */
static void vTask1(void *pvParameters)
{
	char *pcTaskName;
	const portTickType xDelay100ms = 100 / portTICK_RATE_MS;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* Print out the name of this task. */
		DEBUGOUT(pcTaskName);

		/* Create task 2 at a higher priority.  Again the task parameter is not
		 * used so is set to NULL - BUT this time we want to obtain a handle to the
		 * task so pass in the address of the xTask2Handle variable. */
		xTaskCreate(vTask2, (char *) "Task2", configMINIMAL_STACK_SIZE,
					(void *) pcTextForTask2, (tskIDLE_PRIORITY + 2UL), &xTask2Handle);
		/* The task handle is the last parameter......................^^^^^^^^^^^^^ */

		/* Task2 has/had the higher priority, so for Task1 to reach here Task2
		 * must have already executed and deleted itself.  Delay for 100
		 * milliseconds. */
		vTaskDelay(xDelay100ms);
	}
}


/* UART (or output) & LED OFF thread */
static void vTask2(void *pvParameters)
{
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	Board_LED_Set(LED3, LED_OFF);

	/* Task2 does nothing but delete itself.  To do this it could call vTaskDelete()
	 * using a NULL parameter, but instead and purely for demonstration purposes it
	 * instead calls vTaskDelete() with its own task handle. */
	DEBUGOUT(pcTaskName);
	DEBUGOUT("About to delete itself\r\n");
	vTaskDelete(xTask2Handle);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 9 - Deleting tasks
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

	/* Create the first task at priority 1.  This time the task parameter is
	 * not used and is set to NULL.  The task handle is also not used so likewise
	 * is also set to NULL. */
	xTaskCreate(vTask1, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
	/* The task is created at priority 1....................^^^. */

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif
/**
 * @}
 */
