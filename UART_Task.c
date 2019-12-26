/*
 *  ======== UART_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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

/* TI-RTOS Header files */
#include <driverlib/sysctl.h>
#include <ti/drivers/UART.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>

/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include "UART_Task.h"


#define SIZE_OUPUT_ARRAY    (30)

/*
 *  ======== UART  ========
 *  Echo Characters recieved and show reception on Port N Led 0
 */
void UARTFxn(UArg arg0, UArg arg1)
{
    struct uart_descriptor *uart_des = (struct uart_descriptor *)arg0;

    UART_Handle uart;
    UART_Params uartParams;
    const char echoPrompt[] = "\fEchoing characters:\r\n";

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    /* Loop forever echoing */
    while (1) {
        char writeToTerminal[SIZE_OUPUT_ARRAY] = {};
        capSense_values mbox = {};


<<<<<<< HEAD
        //if(Mailbox_getNumPendingMsgs(uart_des->mailbox_des->mailboxHandle)!=0)
        //{
            Mailbox_pend(uart_des->mailbox_des[MESSAGE_FROM_BROKER_TO_UART].mailboxHandle, &mbox, BIOS_WAIT_FOREVER);

            if(0 != mbox.pressedButton0)
            {
                snprintf(writeToTerminal,SIZE_OUPUT_ARRAY,"Button 0 pressed: %d\r\n", mbox.pressedButton0);
=======
        if(Mailbox_getNumPendingMsgs(uart_des->mailbox_des->mailboxHandle)!=0)
        {
            Mailbox_pend(uart_des->mailbox_des->mailboxHandle, &mbox, BIOS_WAIT_FOREVER);

            if(0 != mbox.pressedButton0)
            {
                snprintf(writeToTerminal,SIZE_OUPUT_ARRAY,"Button 0 pressed: %d\n\r", mbox.pressedButton0);
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
                UART_write(uart, &writeToTerminal, SIZE_OUPUT_ARRAY);
            }
            if(0 != mbox.pressedButton1)
            {
<<<<<<< HEAD
                snprintf(writeToTerminal,SIZE_OUPUT_ARRAY,"Button 1 pressed: %d\r\n", mbox.pressedButton1);
=======
                snprintf(writeToTerminal,SIZE_OUPUT_ARRAY,"Button 1 pressed: %d\n\r", mbox.pressedButton1);
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
                UART_write(uart, &writeToTerminal, SIZE_OUPUT_ARRAY);
            }

            if(0 != mbox.valueSlider)
            {
<<<<<<< HEAD
                snprintf(writeToTerminal,SIZE_OUPUT_ARRAY,"Slider input value: %d\r\n", mbox.valueSlider);
                UART_write(uart, &writeToTerminal, SIZE_OUPUT_ARRAY);
            }

        //}
=======
                snprintf(writeToTerminal,SIZE_OUPUT_ARRAY,"Slider input value: %d\n\r", mbox.valueSlider);
                UART_write(uart, &writeToTerminal, SIZE_OUPUT_ARRAY);
            }

        }
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
    }
}


/*
 *  Setup task function
 */
int setup_UART_Task(int prio, struct uart_descriptor *uart_des)
{
    Task_Params taskUARTParams;
    Task_Handle taskUART;
    Error_Block eb;

    /* Enable and configure the peripherals used by the UART0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UART_init();

    /* Setup PortN LED1 activity signaling */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
        
    Error_init(&eb);
    Task_Params_init(&taskUARTParams);
    taskUARTParams.stackSize = 1024; /* stack in bytes */
    taskUARTParams.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskUARTParams.arg0 = (UArg)uart_des; /* pass led descriptor as arg0 */
    taskUART = Task_create((Task_FuncPtr)UARTFxn, &taskUARTParams, &eb);
    if (taskUART == NULL) {
        System_abort("TaskUART create failed");
    }

    return (0);
}
