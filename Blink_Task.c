/*
 *  ======== Blink_Task.c ========
 */
#include <stdbool.h>
#include <inc/hw_memmap.h>


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>


/*Board Header files */
#include <Blink_Task.h>
#include <Board.h>
#include <EK_TM4C1294XL.h>


#include <ctype.h>
#include <string.h>

/*
 *  ======== Blink  ========
 *  Perform Blink Operation on led given at arg0
 */
void BlinkFxn(UArg arg0, UArg arg1)
{

	led_descriptor_t *led_desc = (led_descriptor_t *)arg0;
	uint32_t wait_ticks = (uint32_t) arg1;
	/*gpio driverlib api uses same bit pattern for gpio mask and value*/
	uint8_t ui8val = (uint8_t)led_desc->led;

	while(1) {

	ui8val ^= (uint8_t)led_desc->led;//initially off
	GPIOPinWrite (led_desc->port_base, led_desc->led, ui8val);
	Task_sleep(wait_ticks);

	}


}


/*
 *  setup task function
 */
int setup_Blink_Task(led_descriptor_t *led_desc, uint32_t wait_ticks)
{
	Task_Params taskLedParams;
	Task_Handle taskLed;
	uint32_t ui32Strength, ui32PinType;
	Error_Block eb;

    /*configure gpio port_base according to led*/
	GPIOPinTypeGPIOOutput(led_desc->port_base, led_desc->led);
	
    /* Create Blink task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.stackSize = 1024;/*stack in bytes*/
    taskLedParams.priority = 15;/*15 is default 16 is highest priority -> see RTOS configuration*/
    taskLedParams.arg0 = (UArg) led_desc;/*pass led descriptor to arg0*/
    taskLedParams.arg1 = (UArg) wait_ticks;
    taskLed = Task_create((Task_FuncPtr)BlinkFxn, &taskLedParams, &eb);
    if (taskLed == NULL) {
    	System_abort("TaskLed create failed");
    }

    return (0);
}
