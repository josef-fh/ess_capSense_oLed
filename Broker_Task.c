/*
 *  ======== Broker_Task.c ========
 *  Author: Michael Kranl, Josef Ramsauer
 */
#include <stdbool.h>
#include <stdint.h>
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

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include "CapSense_Task.h"
#include "EK_TM4C1294XL.h"
#include <ti/drivers/I2C.h>
#include "Broker_Task.h"

/* Application headers */

/*
 *
 */

void BrokerMain(UArg arg0,UArg arg1)
{
    struct broker_descriptor *broker_desc = (struct broker_descriptor *)arg0;
    capSense_values mbox = {};
    while(true)
    {
        //if(Mailbox_getNumPendingMsgs(broker_desc->mailbox_des[MAILBOX_FROM_CAPSENSE_TO_BROKER]->mailboxHandle)!=0)       // Mailbox wird erst ausgelesen wenn nachrichten vorhanden sind
        //{
            Mailbox_pend(broker_desc->mailbox_des[MESSAGE_FROM_CAPSENSE_TO_BROKER].mailboxHandle, &mbox, BIOS_WAIT_FOREVER);
            Mailbox_post(broker_desc->mailbox_des[MESSAGE_FROM_BROKER_TO_UART].mailboxHandle,&mbox,BIOS_WAIT_FOREVER);
        //}
    }
}

/*
 *  Setup task function
 */
int setup_Broker_Task(int prio, xdc_String name, struct broker_descriptor *broker_desc)
{
    Task_Params taskBrokerParams;
    Task_Handle taskBroker;
    Error_Block eb;

    /* Create blink task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskBrokerParams);
    taskBrokerParams.instance->name = name;
    taskBrokerParams.stackSize = 1024;//1024; /* stack in bytes */
    taskBrokerParams.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskBrokerParams.arg0 = (UArg)broker_desc; /* pass led descriptor as arg0 */
    taskBroker = Task_create((Task_FuncPtr)BrokerMain, &taskBrokerParams, &eb);
    if (taskBroker == NULL) {
        System_abort("taskCapSense create failed");
    }

    return (0);
}
