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
#define EXAMPLE_10 (10)		/* Blocking when receiving from a queue */
#define EXAMPLE_11 (11)		/* Blocking when sending to a queue or sending structures on a queue */
#define EXAMPLE_12 (12)		/* Using a binary semaphore to synchronize a task with an interrupt */
#define EXAMPLE_13 (13)		/* Using a counting semaphore to synchronize a task with an interrupt */
#define EXAMPLE_14 (14)		/* Sending and receiving on a queue from within an interrupt */
#define EXAMPLE_15 (15)		/* Re-writing vPrintString() to use a semaphore */
#define EXAMPLE_16 (16)		/* Re-writing vPrintString() to use a gatekeeper task */

#define TEST (EXAMPLE_10)

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


#if (TEST == EXAMPLE_10)		/* Blocking when receiving from a queue */

const char *pcTextForMain = "\r\nExample 10 - Blocking when receiving from a queue\r\n";

/* The tasks to be created.  Two instances are created of the sender task while
 * only a single instance is created of the receiver task. */
static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);

/* Declare a variable of type xQueueHandle.  This is used to store the queue
 * that is accessed by all three tasks. */
xQueueHandle xQueue;

/* Sender, UART (or output) & LED ON thread */
static void vSenderTask(void *pvParameters)
{
	long lValueToSend;
	portBASE_TYPE xStatus;

	/* Two instances are created of this task so the value that is sent to the
	 * queue is passed in via the task parameter rather than be hard coded.  This way
	 * each instance can use a different value.  Cast the parameter to the required
	 * 	type. */
	lValueToSend = (long) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* The first parameter is the queue to which data is being sent.  The
		 * queue was created before the scheduler was started, so before this task
		 * started to execute.
		 *
		 * The second parameter is the address of the data to be sent.
		 *
		 * The third parameter is the Block time � the time the task should be kept
		 * in the Blocked state to wait for space to become available on the queue
		 * should the queue already be full.  In this case we don�t specify a block
		 * time because there should always be space in the queue. */
		xStatus = xQueueSendToBack(xQueue, &lValueToSend, (portTickType)0);

		if (xStatus != pdPASS) {
			/* We could not write to the queue because it was full � this must
			 * be an error as the queue should never contain more than one item! */
			DEBUGOUT("Could not send to the queue.\r\n");
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}

/* Receiver, UART (or output) & LED OFF thread */
static void vReceiverTask(void *pvParameters)
{
	/* Declare the variable that will hold the values received from the queue. */
	long lReceivedValue;
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;


	while (1) {
		Board_LED_Set(LED3, LED_OFF);

		/* As this task unblocks immediately that data is written to the queue this
		 * call should always find the queue empty. */
		if (uxQueueMessagesWaiting(xQueue) != 0) {
			DEBUGOUT("Queue should have been empty!\r\n");
		}

		/* The first parameter is the queue from which data is to be received.  The
		 * queue is created before the scheduler is started, and therefore before this
		 * task runs for the first time.
		 *
		 * The second parameter is the buffer into which the received data will be
		 * placed.  In this case the buffer is simply the address of a variable that
		 * has the required size to hold the received data.
		 *
		 * The last parameter is the block time � the maximum amount of time that the
		 * task should remain in the Blocked state to wait for data to be available should
		 * the queue already be empty. */
		xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);

		if (xStatus == pdPASS) {
			/* Data was successfully received from the queue, print out the received
			 * value. */
			DEBUGOUT("Received = %d\r\n", lReceivedValue);
		}
		else {
			/* We did not receive anything from the queue even after waiting for 100ms.
			 * This must be an error as the sending tasks are free running and will be
			 * continuously writing to the queue. */
			DEBUGOUT("Could not receive from the queue.\r\n");
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 10 - Blocking when receiving from a queue
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* The queue is created to hold a maximum of 5 long values. */
    xQueue = xQueueCreate(5, sizeof(long));

	if (xQueue != (xQueueHandle)NULL) {
		/* Create two instances of the task that will write to the queue.  The
		 * parameter is used to pass the value that the task should write to the queue,
		 * so one task will continuously write 100 to the queue while the other task
		 * will continuously write 200 to the queue.  Both tasks are created at
		 * priority 1. */
		xTaskCreate(vSenderTask, (char *) "vSender1", configMINIMAL_STACK_SIZE, (void *) 100,
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		xTaskCreate(vSenderTask, (char *) "vSender2", configMINIMAL_STACK_SIZE, (void *) 200,
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

		/* Create the task that will read from the queue.  The task is created with
		 * priority 2, so above the priority of the sender tasks. */
		xTaskCreate(vReceiverTask, (char *) "vReceiver", configMINIMAL_STACK_SIZE, NULL,
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();
	}
	else {
		/* The queue could not be created. */
	}

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_11)		/* Blocking when sending to a queue or sending structures on a queue */

const char *pcTextForMain = "\r\nExample 10 - Blocking when sending to a queue or sending structures on a queue\r\n";

#define mainSENDER_1		1
#define mainSENDER_2		2

/* The tasks to be created.  Two instances are created of the sender task while
 * only a single instance is created of the receiver task. */
static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);

/* Declare a variable of type xQueueHandle.  This is used to store the queue
 * that is accessed by all three tasks. */
xQueueHandle xQueue;

/* Define the structure type that will be passed on the queue. */
typedef struct {
	unsigned char ucValue;
	unsigned char ucSource;
} xData;

/* Declare two variables of type xData that will be passed on the queue. */
static const xData xStructsToSend[2] = {
	{100, mainSENDER_1}, /* Used by Sender1. */
	{200, mainSENDER_2}  /* Used by Sender2. */
};


/* Sender, UART (or output) & LED ON thread */
static void vSenderTask(void *pvParameters)
{
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* The first parameter is the queue to which data is being sent.  The
		 * queue was created before the scheduler was started, so before this task
		 * started to execute.
		 *
		 * The second parameter is the address of the structure being sent.  The
		 * address is passed in as the task parameter.
		 *
		 * The third parameter is the Block time - the time the task should be kept
		 * in the Blocked state to wait for space to become available on the queue
		 * should the queue already be full.  A block time is specified as the queue
		 * will become full.  Items will only be removed from the queue when both
		 * sending tasks are in the Blocked state.. */
		xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);

		if (xStatus != pdPASS) {
			/* We could not write to the queue because it was full - this must
			 * be an error as the receiving task should make space in the queue
			 * as soon as both sending tasks are in the Blocked state. */
			DEBUGOUT("Could not send to the queue.\r\n");
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}

/* Receiver, UART (or output) & LED OFF thread */
static void vReceiverTask(void *pvParameters)
{
	/* Declare the structure that will hold the values received from the queue. */
	xData xReceivedStructure;
	portBASE_TYPE xStatus;

	/* This task is also defined within an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_OFF);

		/* As this task only runs when the sending tasks are in the Blocked state,
		 * and the sending tasks only block when the queue is full, this task should
		 * always find the queue to be full.  3 is the queue length. */
		if (uxQueueMessagesWaiting(xQueue) != 3) {
			DEBUGOUT("Queue should have been full!\r\n");
		}

		/* The first parameter is the queue from which data is to be received.  The
		 * queue is created before the scheduler is started, and therefore before this
		 * task runs for the first time.
		 *
		 * The second parameter is the buffer into which the received data will be
		 * placed.  In this case the buffer is simply the address of a variable that
		 * has the required size to hold the received structure.
		 *
		 * The last parameter is the block time - the maximum amount of time that the
		 * task should remain in the Blocked state to wait for data to be available
		 * should the queue already be empty.  A block time is not necessary as this
		 * task will only run when the queue is full so data will always be available. */
		xStatus = xQueueReceive(xQueue, &xReceivedStructure, (portBASE_TYPE)0);

		if (xStatus == pdPASS) {
			/* Data was successfully received from the queue, print out the received
			 * value and the source of the value. */
			if (xReceivedStructure.ucSource == mainSENDER_1) {
				DEBUGOUT("From Sender 1 = %d\r\n", xReceivedStructure.ucValue);
			}
			else {
				DEBUGOUT("From Sender 2 = %d\r\n", xReceivedStructure.ucValue);
			}
		}
		else {
			/* We did not receive anything from the queue.  This must be an error
			 * as this task should only run when the queue is full. */
			DEBUGOUT("Could not receive from the queue.\r\n");
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 11 - Blocking when sending to a queue or sending structures on a queue
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* The queue is created to hold a maximum of 3 structures of type xData. */
    xQueue = xQueueCreate(3, sizeof(xData));

	if (xQueue != (xQueueHandle)NULL) {
		/* Create two instances of the task that will write to the queue.  The
		 * parameter is used to pass the structure that the task should write to the
		 * queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		 * while the other task will continuously send xStructsToSend[ 1 ].  Both
		 * tasks are created at priority 2 which is above the priority of the receiver. */
		xTaskCreate(vSenderTask, (char *) "vSender1", configMINIMAL_STACK_SIZE, (void *) &(xStructsToSend[0]),
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);
		xTaskCreate(vSenderTask, (char *) "vSender2", configMINIMAL_STACK_SIZE, (void *) &(xStructsToSend[1]),
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

		/* Create the task that will read from the queue.  The task is created with
		 * priority 1, so below the priority of the sender tasks. */
		xTaskCreate(vReceiverTask, (char *) "vReceiver", configMINIMAL_STACK_SIZE, NULL,
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();
	}
	else {
		/* The queue could not be created. */
	}

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_12)		/* Using a counting semaphore to synchronize a task with an interrupt */

const char *pcTextForMain = "\r\nExample 12 - Using a counting semaphore to synchronize a task with an interrupt\r\n";

/* The interrupt number to use for the software interrupt generation.  This
 * could be any unused number.  In this case the first chip level (non system)
 * interrupt is used, which happens to be the watchdog on the LPC1768.  WDT_IRQHandler */
/* interrupt is used, which happens to be the DAC on the LPC4337 M4.  DAC_IRQHandler */
#define mainSW_INTERRUPT_ID		(0)

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vHandlerTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

/* The service routine for the interrupt.  This is the interrupt that the
 * task will be synchronized with.  void vSoftwareInterruptHandler(void); */
/* the watchdog on the LPC1768 => WDT_IRQHandler */ /* the DAC on the LPC4337 M4.  DAC_IRQHandler */
#define vSoftwareInterruptHandler (DAC_IRQHandler)
/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xBinarySemaphore;


/* Take Semaphore, UART (or output) & LED toggle thread */
static void vHandlerTask(void *pvParameters)
{
	/* As per most tasks, this task is implemented within an infinite loop.
	 *
	 * Take the semaphore once to start with so the semaphore is empty before the
	 * infinite loop is entered.  The semaphore was created before the scheduler
	 * was started so before this task ran for the first time.*/
    xSemaphoreTake(xBinarySemaphore, (portTickType) 0);

	while (1) {
		Board_LED_Toggle(LED3);

		/* Use the semaphore to wait for the event.  The task blocks
         * indefinitely meaning this function call will only return once the
         * semaphore has been successfully obtained - so there is no need to check
         * the returned value. */
        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        /* To get here the event must have occurred.  Process the event (in this
         * case we just print out a message). */
        DEBUGOUT("Handler task - Processing event.\r\n");
    }
}


static void vPeriodicTask(void *pvParameters)
{
    /* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		/* This task is just used to 'simulate' an interrupt.  This is done by
         * periodically generating a software interrupt. */
        vTaskDelay(500 / portTICK_RATE_MS);

        /* Generate the interrupt, printing a message both before hand and
         * afterwards so the sequence of execution is evident from the output. */
        DEBUGOUT("Periodic task - About to generate an interrupt.\r\n");
        mainTRIGGER_INTERRUPT();
        DEBUGOUT("Periodic task - Interrupt generated.\n\n");
    }
}


static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 'Give' the semaphore to unblock the task. */
    xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* Giving the semaphore may have unblocked a task - if it did and the
     * unblocked task has a priority equal to or above the currently executing
     * task then xHigherPriorityTaskWoken will have been set to pdTRUE and
     * portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
     * higher priority task.
     *
     * NOTE: The syntax for forcing a context switch within an ISR varies between
     * FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
     * the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
     * from an ISR! */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 12 - Using a binary semaphore to synchronize a task with an interrupt
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a semaphore is used it must be explicitly created.  In this example
     * a binary semaphore is created. */
    vSemaphoreCreateBinary(xBinarySemaphore);

    /* Check the semaphore was created successfully. */
    if (xBinarySemaphore != (xSemaphoreHandle) NULL) {
    	/* Enable the software interrupt and set its priority. */
    	prvSetupSoftwareInterrupt();

        /* Create the 'handler' task.  This is the task that will be synchronized
         * with the interrupt.  The handler task is created with a high priority to
         * ensure it runs immediately after the interrupt exits.  In this case a
         * priority of 3 is chosen. */
        xTaskCreate(vHandlerTask, (char *) "Handler", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 3UL), (xTaskHandle *) NULL);

        /* Create the task that will periodically generate a software interrupt.
         * This is created with a priority below the handler task to ensure it will
         * get preempted each time the handler task exits the Blocked state. */
        xTaskCreate(vPeriodicTask, (char *) "Periodic", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

        /* Start the scheduler so the created tasks start executing. */
        vTaskStartScheduler();
    }

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}

#endif


#if (TEST == EXAMPLE_13)		/* Using a binary semaphore to synchronize a task with an interrupt */

const char *pcTextForMain = "\r\nExample 13 - Using a binary semaphore to synchronize a task with an interrupt\r\n";

/* The interrupt number to use for the software interrupt generation.  This
 * could be any unused number.  In this case the first chip level (non system)
 * interrupt is used, which happens to be the watchdog on the LPC1768.  WDT_IRQHandler */
/* interrupt is used, which happens to be the DAC on the LPC4337 M4.  DAC_IRQHandler */
#define mainSW_INTERRUPT_ID		(0)

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vHandlerTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

/* The service routine for the interrupt.  This is the interrupt that the
* task will be synchronized with.  void vSoftwareInterruptHandler(void); */
/* the watchdog on the LPC1768 => WDT_IRQHandler */ /* the DAC on the LPC4337 M4.  DAC_IRQHandler */
#define vSoftwareInterruptHandler (DAC_IRQHandler)

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xCountingSemaphore;


/* Take Semaphore, UART (or output) & LED toggle thread */
static void vHandlerTask(void *pvParameters)
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		Board_LED_Toggle(LED3);

		/* Use the semaphore to wait for the event.  The semaphore was created
		 * before the scheduler was started so before this task ran for the first
		 * time.  The task blocks indefinitely meaning this function call will only
		 * return once the semaphore has been successfully obtained - so there is no
		 * need to check the returned value. */
		xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);

		/* To get here the event must have occurred.  Process the event (in this
		 * case we just print out a message). */
		DEBUGOUT("Handler task - Processing event.\r\n");
    }
}


static void vPeriodicTask(void *pvParameters)
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		/* This task is just used to 'simulate' an interrupt.  This is done by
		 * periodically generating a software interrupt. */
		vTaskDelay(500 / portTICK_RATE_MS);

		/* Generate the interrupt, printing a message both before hand and
		 * afterwards so the sequence of execution is evident from the output. */
        DEBUGOUT("Periodic task - About to generate an interrupt.\r\n");
        mainTRIGGER_INTERRUPT();
        DEBUGOUT("Periodic task - Interrupt generated.\r\n");
    }
}


static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* 'Give' the semaphore multiple times.  The first will unblock the handler
	 * task, the following 'gives' are to demonstrate that the semaphore latches
	 * the events to allow the handler task to process them in turn without any
	 * events getting lost.  This simulates multiple interrupts being taken by the
	 * processor, even though in this case the events are simulated within a single
	 * interrupt occurrence.*/
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* Giving the semaphore may have unblocked a task - if it did and the
     * unblocked task has a priority equal to or above the currently executing
     * task then xHigherPriorityTaskWoken will have been set to pdTRUE and
     * portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
     * higher priority task.
     *
     * NOTE: The syntax for forcing a context switch within an ISR varies between
     * FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
     * the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
     * from an ISR! */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 13 - Using a counting semaphore to synchronize a task with an interrupt
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a semaphore is used it must be explicitly created.  In this example
     * a counting semaphore is created.  The semaphore is created to have a maximum
     * count value of 10, and an initial count value of 0. */
    xCountingSemaphore = xSemaphoreCreateCounting(10, 0);

	/* Check the semaphore was created successfully. */
	if (xCountingSemaphore != NULL)
	{
    	/* Enable the software interrupt and set its priority. */
    	prvSetupSoftwareInterrupt();

		/* Create the 'handler' task.  This is the task that will be synchronized
		with the interrupt.  The handler task is created with a high priority to
		ensure it runs immediately after the interrupt exits.  In this case a
		priority of 3 is chosen. */
        xTaskCreate(vHandlerTask, (char *) "Handler", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 3UL), (xTaskHandle *) NULL);

		/* Create the task that will periodically generate a software interrupt.
		This is created with a priority below the handler task to ensure it will
		get preempted each time the handler task exist the Blocked state. */
        xTaskCreate(vPeriodicTask, (char *) "Periodic", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_14)		/* Sending and receiving on a queue from within an interrupt */

const char *pcTextForMain = "\r\nExample 14 - Sending and receiving on a queue from within an interrupt\r\n";

/* The interrupt number to use for the software interrupt generation.  This
 * could be any unused number.  In this case the first chip level (non system)
 * interrupt is used, which happens to be the watchdog on the LPC1768.  WDT_IRQHandler */
/* interrupt is used, which happens to be the DAC on the LPC4337 M4.  DAC_IRQHandler */
#define mainSW_INTERRUPT_ID		(0)

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vIntegerGenerator(void *pvParameters);
static void vStringPrinter(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

/* The service routine for the interrupt.  This is the interrupt that the
* task will be synchronized with.  void vSoftwareInterruptHandler(void); */
/* the watchdog on the LPC1768 => WDT_IRQHandler */ /* the DAC on the LPC4337 M4.  DAC_IRQHandler */
#define vSoftwareInterruptHandler (DAC_IRQHandler)

unsigned long ulNext = 0;
unsigned long ulCount;
unsigned long ul[100];

/* Declare two variables of type xQueueHandle.  One queue will be read from
 * within an ISR, the other will be written to from within an ISR. */
xQueueHandle xIntegerQueue, xStringQueue;


/* Take Semaphore, UART (or output) & LED toggle thread */
static void vIntegerGenerator(void *pvParameters)
{
	portTickType xLastExecutionTime;
	unsigned portLONG ulValueToSend = 0;
	int i;

	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		Board_LED_Toggle(LED3);

		/* This is a periodic task.  Block until it is time to run again.
		 * The task will execute every 200ms. */
		vTaskDelayUntil(&xLastExecutionTime, 200 / portTICK_RATE_MS);

		/* Send an incrementing number to the queue five times.  These will be
		 * read from the queue by the interrupt service routine.  A block time is
		 * not specified. */
		for (i = 0; i < 5; i++) {
			xQueueSendToBack(xIntegerQueue, &ulValueToSend, 0);
			ulValueToSend++;
		}

		/* Force an interrupt so the interrupt service routine can read the
		 * values from the queue. */
		DEBUGOUT("Generator task - About to generate an interrupt.\r\n");
		mainTRIGGER_INTERRUPT();
		DEBUGOUT("Generator task - Interrupt generated.\r\n");
    }
}


static void vStringPrinter(void *pvParameters)
{
	char *pcString;

	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		/* Block on the queue to wait for data to arrive. */
		xQueueReceive(xStringQueue, &pcString, portMAX_DELAY);

		/* Print out the string received. */
        DEBUGOUT(pcString);
    }
}


static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	static unsigned long ulReceivedNumber;

	/* The strings are declared static const to ensure they are not allocated to the
	 * interrupt service routine stack, and exist even when the interrupt service routine
	 * is not executing. */
	static const char *pcStrings[] = {
	    "String 0\r\n",
	    "String 1\r\n",
	    "String 2\r\n",
	    "String 3\r\n"
	};

    /* Loop until the queue is empty. */
    while (xQueueReceiveFromISR(xIntegerQueue, &ulReceivedNumber, &xHigherPriorityTaskWoken) != errQUEUE_EMPTY) {
        /* Truncate the received value to the last two bits (values 0 to 3 inc.), then
         * send the string that corresponds to the truncated value to the other
         * queue. */
        ulReceivedNumber &= 0x03;
        xQueueSendToBackFromISR(xStringQueue, &pcStrings[ulReceivedNumber], &xHigherPriorityTaskWoken);
    }

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* xHigherPriorityTaskWoken was initialised to pdFALSE.  It will have then
     * been set to pdTRUE only if reading from or writing to a queue caused a task
     * of equal or greater priority than the currently executing task to leave the
     * Blocked state.  When this is the case a context switch should be performed.
     * In all other cases a context switch is not necessary.
     *
     * NOTE: The syntax for forcing a context switch within an ISR varies between
     * FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
     * the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
     * from an ISR! */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 14 - Sending and receiving on a queue from within an interrupt
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a queue can be used it must first be created.  Create both queues
     * used by this example.  One queue can hold variables of type unsigned long,
     * the other queue can hold variables of type char*.  Both queues can hold a
     * maximum of 10 items.  A real application should check the return values to
     * ensure the queues have been successfully created. */
    xIntegerQueue = xQueueCreate(10, sizeof(unsigned long));
	xStringQueue = xQueueCreate(10, sizeof(char *));

   	/* Enable the software interrupt and set its priority. */
   	prvSetupSoftwareInterrupt();

	/* Create the task that uses a queue to pass integers to the interrupt service
	routine.  The task is created at priority 1. */
   	xTaskCreate(vIntegerGenerator, (char *) "IntGen", configMINIMAL_STACK_SIZE, NULL,
   				(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

    /* Create the task that prints out the strings sent to it from the interrupt
     * service routine.  This task is created at the higher priority of 2. */
    xTaskCreate(vStringPrinter, (char *) "String", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_15)		/* Re-writing vPrintString() to use a semaphore */

const char *pcTextForMain = "\r\nExample 15 - Re-writing vPrintString() to use a semaphore\r\n";

/* Dimensions the buffer into which messages destined for stdout are placed. */
#define mainMAX_MSG_LEN	( 80 )

/* The task to be created.  Two instances of this task are created. */
static void prvPrintTask(void *pvParameters);

/* The function that uses a mutex to control access to standard out. */
static void prvNewPrintString(const portCHAR *pcString);

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * mutex type semaphore that is used to ensure mutual exclusive access to stdout. */
xSemaphoreHandle xMutex;


/* Take Mutex Semaphore, UART (or output) & LED toggle thread */
static void prvNewPrintString(const portCHAR *pcString)
{
	static char cBuffer[mainMAX_MSG_LEN];

	/* The semaphore is created before the scheduler is started so already
	 * exists by the time this task executes.
	 *
	 * Attempt to take the semaphore, blocking indefinitely if the mutex is not
	 * available immediately.  The call to xSemaphoreTake() will only return when
	 * the semaphore has been successfully obtained so there is no need to check the
	 * return value.  If any other delay period was used then the code must check
	 * that xSemaphoreTake() returns pdTRUE before accessing the resource (in this
	 * case standard out. */
	xSemaphoreTake(xMutex, portMAX_DELAY);
	{
		/* The following line will only execute once the semaphore has been
		 * successfully obtained - so standard out can be accessed freely. */
		sprintf(cBuffer, "%s", pcString);
		DEBUGOUT(cBuffer);
	}
	xSemaphoreGive(xMutex);
}


static void prvPrintTask( void *pvParameters )
{
char *pcStringToPrint;

	/* Two instances of this task are created so the string the task will send
	 * to prvNewPrintString() is passed in the task parameter.  Cast this to the
	 * required type. */
	pcStringToPrint = (char *) pvParameters;

	while (1) {
		/* Print out the string using the newly defined function. */
		prvNewPrintString( pcStringToPrint );

		/* Wait a pseudo random time.  Note that rand() is not necessarily
		 * re-entrant, but in this case it does not really matter as the code does
		 * not care what value is returned.  In a more secure application a version
		 * of rand() that is known to be re-entrant should be used - or calls to
		 * rand() should be protected using a critical section. */
		vTaskDelay((rand() & 0x1FF));
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 15 - Re-writing vPrintString() to use a semaphore
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a semaphore is used it must be explicitly created.  In this example
     * a mutex type semaphore is created. */
    xMutex = xSemaphoreCreateMutex();

	/* The tasks are going to use a pseudo random delay, seed the random number
	 * generator. */
	srand(567);

	/* Only create the tasks if the semaphore was created successfully. */
	if (xMutex != NULL) {
		/* Create two instances of the tasks that attempt to write stdout.  The
		 * string they attempt to write is passed in as the task parameter.  The tasks
		 * are created at different priorities so some pre-emption will occur. */
		xTaskCreate(prvPrintTask, (char *) "Print1", configMINIMAL_STACK_SIZE,
					"Task 1 ******************************************\r\n",
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		xTaskCreate(prvPrintTask, (char *) "Print2", configMINIMAL_STACK_SIZE,
					"Task 2 ------------------------------------------\r\n",
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_16)		/* Re-writing vPrintString() to use a gatekeeper task */

const char *pcTextForMain = "\r\nExample 16 - Re-writing vPrintString() to use a gatekeeper task\r\n";

/* Dimensions the buffer into which messages destined for stdout are placed. */
#define mainMAX_MSG_LEN	( 80 )

/* The task that sends messages to the stdio gatekeeper.  Two instances of this
 * task are created. */
static void prvPrintTask(void *pvParameters);

/* The gatekeeper task itself. */
static void prvStdioGatekeeperTask(void *pvParameters);

/* Define the strings that the tasks and interrupt will print out via the gatekeeper. */
static char *pcStringsToPrint[] = {
	"Task 1 ****************************************************\r\n",
	"Task 2 ----------------------------------------------------\r\n",
	"Message printed from the tick hook interrupt ##############\r\n"
};

/* Declare a variable of type xQueueHandle.  This is used to send messages from
 * the print tasks to the gatekeeper task. */
xQueueHandle xPrintQueue;


/* UART (or output) & LED toggle thread */
static void prvStdioGatekeeperTask(void *pvParameters)
{
	char *pcMessageToPrint;
	static char cBuffer[mainMAX_MSG_LEN];

	/* This is the only task that is allowed to write to the terminal output.
	 * Any other task wanting to write to the output does not access the terminal
	 * directly, but instead sends the output to this task.  As only one task
	 * writes to standard out there are no mutual exclusion or serialization issues
	 * to consider within this task itself. */
	while (1) {
		/* Wait for a message to arrive. */
		xQueueReceive(xPrintQueue, &pcMessageToPrint, portMAX_DELAY);

		/* There is no need to check the return	value as the task will block
		 * indefinitely and only run again when a message has arrived.  When the
		 * next line is executed there will be a message to be output. */
		sprintf(cBuffer, "%s", pcMessageToPrint);
		DEBUGOUT(cBuffer);

		/* Now simply go back to wait for the next message. */
	}
}


static void prvPrintTask(void *pvParameters)
{
	int iIndexToString;

	/* Two instances of this task are created so the index to the string the task
	 * will send to the gatekeeper task is passed in the task parameter.  Cast this
	 * to the required type. */
	iIndexToString = (int) pvParameters;

	while (1) {
		/* Print out the string, not directly but by passing the string to the
		 * gatekeeper task on the queue.  The queue is created before the scheduler is
		 * started so will already exist by the time this task executes.  A block time
		 * is not specified as there should always be space in the queue. */
		xQueueSendToBack(xPrintQueue, &(pcStringsToPrint[iIndexToString]), 0);

		/* Wait a pseudo random time.  Note that rand() is not necessarily
		 * re-entrant, but in this case it does not really matter as the code does
		 * not care what value is returned.  In a more secure application a version
		 * of rand() that is known to be re-entrant should be used - or calls to
		 * rand() should be protected using a critical section. */
		vTaskDelay((rand() & 0x1FF));
	}
}


void vApplicationTickHook(void)
{
	static int iCount = 0;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Print out a message every 200 ticks.  The message is not written out
	 * directly, but sent to the gatekeeper task. */
	iCount++;
	if (iCount >= 200) {
		/* In this case the last parameter (xHigherPriorityTaskWoken) is not
		 * actually used but must still be supplied. */
		xQueueSendToFrontFromISR(xPrintQueue, &(pcStringsToPrint[2]), &xHigherPriorityTaskWoken);

		/* Reset the count ready to print out the string again in 200 ticks
		 * time. */
		iCount = 0;
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 16 - Re-writing vPrintString() to use a gatekeeper task
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a queue is used it must be explicitly created.  The queue is created
     * to hold a maximum of 5 character pointers. */
    xPrintQueue = xQueueCreate( 5, sizeof( char * ) );

	/* The tasks are going to use a pseudo random delay, seed the random number
	 * generator. */
	srand(567);

	/* Check the queue was created successfully. */
	if (xPrintQueue != NULL) {
		/* Create two instances of the tasks that send messages to the gatekeeper.
		The	index to the string they attempt to write is passed in as the task
		parameter (4th parameter to xTaskCreate()).  The tasks are created at
		different priorities so some pre-emption will occur. */
		xTaskCreate(prvPrintTask, (char *) "Print1", configMINIMAL_STACK_SIZE,
					(void *) 0, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		xTaskCreate(prvPrintTask, (char *) "Print2", configMINIMAL_STACK_SIZE,
					(void *) 1, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

		/* Create the gatekeeper task.  This is the only task that is permitted
		 * to access standard out. */
		xTaskCreate(prvStdioGatekeeperTask, (char *) "Gatekeeper", configMINIMAL_STACK_SIZE,
					(void * ) NULL, (tskIDLE_PRIORITY + 0UL), (xTaskHandle *) NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif
/**
 * @}
 */
