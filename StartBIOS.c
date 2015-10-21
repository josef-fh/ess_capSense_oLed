/*
 * CCSv6 project using TI-RTOS and a custom network driver
 * providing an ndk-less base environment
 *
 */

/*
 *  ======== StartBIOS.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/knl/Event.h>

/* TI-RTOS Header files */
#include <ti/drivers/UART.h>

/* Drivers Header files*/
//#include <ti/drivers/GPIO.h>

/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/*application header files*/
#include <ctype.h>
#include <string.h>


#include <Blink_Task.h>
#include <UART_Task.h>



int main(void)
{

    uint32_t ui32SysClock;
    static led_descriptor_t led_desc[2];
	/* Call board init functions. */
	ui32SysClock = Board_initGeneral(120*1000*1000);


    //
    // Initialize the UART for console I/O.
    //
    //    UARTStdioConfig(0, 115200, ui32SysClock);


	led_desc[0].port_base = GPIO_PORTN_BASE;
	led_desc[0].led = GPIO_PIN_1;
	/*Initialize+start Blink Task*/
	(void) setup_Blink_Task(&led_desc[0], 500);
	System_printf("Created Blink Task1\n");

	led_desc[1].port_base = GPIO_PORTF_BASE;
	led_desc[1].led = GPIO_PIN_0;
	/*Initialize+start Blink Task*/
	(void) setup_Blink_Task(&led_desc[1], 250);
	System_printf("Created Blink Task2\n");

	/*Initialize+start UART Task*/
	(void) setup_UART_Task(0,0);
	System_printf("Created UART Task\n");

    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

}
