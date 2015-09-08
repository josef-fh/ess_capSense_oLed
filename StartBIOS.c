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


/* Drivers Header files*/
#include <ti/drivers/GPIO.h>

/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/*application header files*/
#include <ctype.h>
#include <string.h>
#include <network_events.h>


#include <UIP_Task.h>



int main(void)
{

    uint32_t ui32SysClock;
	/* Call board init functions. */
	ui32SysClock = Board_initGeneral(120*1000*1000);

	// Uncomment this Section if you wan to use the UART for Output - it is tunneled via the Debuggger over USB
    //    GPIOPinConfigure(GPIO_PA0_U0RX);
    //    GPIOPinConfigure(GPIO_PA1_U0TX);
    //    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Initialize the UART for console I/O.
    //
    //    UARTStdioConfig(0, 115200, ui32SysClock);


	/*Initialize UIP Stack - calls led_server, httpd or client depending on configuration of uip/uip-conf.h  */
	(void) setup_UIP_Task(ui32SysClock);
	System_printf("Created UIP Task\n");


    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

}