/*
 *  ======== StartBIOS.c ========
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

/* Instrumentation headers */
#include <ti/uia/runtime/LogSnapshot.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <UART_Task.h>
#include "CapSense_Task.h"
#include "Broker_Task.h"

void CreateMailbox(mailbox_descriptor *mailbox_des)
{
    uint8_t idx;
    Error_Block eb;
    Error_init (&eb);

    for(idx = 0; idx < MESSAGE_ENTIRE_NUMBER; idx++)
    {
        Mailbox_Params_init(&mailbox_des[idx].mboxParams);
        mailbox_des[idx].mailboxHandle = Mailbox_create (sizeof(capSense_values), 20, &(mailbox_des[idx].mboxParams), &eb);

        if(NULL == mailbox_des[idx].mailboxHandle)
        {
            System_abort("taskCapSense create failed");
        }
    }
}

int main(void)
{
    uint32_t ui32SysClock;
    static struct capSense_descriptor capSense_des;
    static struct broker_descriptor broker_des;
    static struct uart_descriptor uart_des;
<<<<<<< HEAD
    static mailbox_descriptor mailbox_des[MESSAGE_ENTIRE_NUMBER];
=======
    static mailbox_descriptor mailbox_des0;
    static mailbox_descriptor mailbox_des1;

    Error_Block eb;
    Error_init (&eb);
    Mailbox_Params_init (&(mailbox_des0.mboxParams));
    mailbox_des0.mailboxHandle = Mailbox_create (sizeof(capSense_values), 20, &(mailbox_des0.mboxParams), &eb);

    Mailbox_Params_init (&(mailbox_des1.mboxParams));
    mailbox_des1.mailboxHandle = Mailbox_create (sizeof(capSense_values), 20, &(mailbox_des1.mboxParams), &eb);

    if ((NULL ==mailbox_des0.mailboxHandle) || (NULL ==mailbox_des1.mailboxHandle))
    {
        System_abort("taskCapSense create failed");
    }
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4

    /* Call board init functions. */
    ui32SysClock = Board_initGeneral(120*1000*1000);
    (void)ui32SysClock;

<<<<<<< HEAD
    /* create Mailbox*/
    CreateMailbox(mailbox_des);

    /* Initialize+start Broker Task*/
    broker_des.g_ui32SysClock = ui32SysClock;
    broker_des.mailbox_des = mailbox_des;
=======
    capSense_des.g_ui32SysClock = ui32SysClock;
    capSense_des.mailbox_des = &mailbox_des0;

    broker_des.g_ui32SysClock = ui32SysClock;
    broker_des.mailbox_des[MAILBOX_FROM_CAPSENSE_TO_BROKER] = &mailbox_des0;
    broker_des.mailbox_des[MAILBOX_FROM_BROKER_TO_UART] = &mailbox_des1;

    uart_des.mailbox_des = &mailbox_des1;

    /* Initialize+start CapSense Task*/
    (void)setup_CapSense_Task(15, "CapSense",&capSense_des);
    System_printf("Created CapSense Task1\n");
    System_flush();

    /* Initialize+start Broker Task*/
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
    (void)setup_Broker_Task(14, "Broker",&broker_des);
    System_printf("Created Broker Task1\n");
    System_flush();

    /*Initialize+start UART Task*/
<<<<<<< HEAD
    uart_des.g_ui32SysClock = ui32SysClock;
    uart_des.mailbox_des = mailbox_des;
=======
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
    (void)setup_UART_Task(14,&uart_des);
    System_printf("Created UART Task\n");

    /* Initialize+start CapSense Task*/
    capSense_des.g_ui32SysClock = ui32SysClock;
    capSense_des.mailbox_des = mailbox_des;
    (void)setup_CapSense_Task(15, "CapSense",&capSense_des);
    System_printf("Created CapSense Task1\n");
    System_flush();

    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();
}

/**** The code below is only for Instrumentation purposes! ****/

/* The redefinition of ti_uia_runtime_LogSnapshot_writeNameOfReference
 * is necessary due to bug UIA-23 fixed in UIA 2.00.06.52, cf.
 * http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/uia/2_00_06_52/exports/docs/uia_2_00_06_52_release_notes.html
 */
#undef ti_uia_runtime_LogSnapshot_writeNameOfReference
#define ti_uia_runtime_LogSnapshot_writeNameOfReference(refId, fmt, startAdrs, lengthInMAUs) \
( \
(ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_nameOfReference, Module__MID, \
(IArg)refId,(IArg)__FILE__,(IArg)__LINE__, \
(IArg)fmt, (IArg)startAdrs, (IArg)lengthInMAUs)) \
)

/* Log the task name whenever a task is created.
 * This works around a limitation of UIA where tasks sharing a "main"
 * function do not show up separately in the execution analyzer, cf.
 * http://processors.wiki.ti.com/index.php/System_Analyzer_Tutorial_3A#Going_Further:_How_to_have_Analysis_tables_and_graphs_display_the_names_of_tasks_created_at_run_time
 */
#include <string.h>
Void tskCreateHook(Task_Handle hTask, Error_Block *eb) {
   String name = Task_Handle_name(hTask);
   LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
   ti_uia_runtime_LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
}
