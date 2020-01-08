/*
 * Broker_Task.h
 *
 *  Created on: 25 Dec 2019
 *      Author: Michael Kranl, Josef Ramsauer
 */

#ifndef LOCAL_INC_BROKER_TASK_H_
#define LOCAL_INC_BROKER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>

#include <stdio.h>
#include <ti/sysbios/knl/Mailbox.h>

#include "CY8C201A0.h"

typedef enum {
    MESSAGE_FROM_CAPSENSE_TO_BROKER,
    MESSAGE_FROM_BROKER_TO_UART,
    MESSAGE_FROM_BROKER_TO_OLED,
    MESSAGE_ENTIRE_NUMBER
}mailboxIndex;


typedef struct{
    Mailbox_Params mboxParams;
    Mailbox_Handle mailboxHandle;
}mailbox_descriptor;


typedef struct{
    uint32_t g_ui32SysClock;
    mailbox_descriptor *mailbox_des;
}broker_descriptor;



typedef struct{
    bool pressedButton0;
    bool pressedButton1;
    uint8_t valueSlider;
}capSense_values;

/*! \fn setup_Broker_Task
 *  \brief Setup Blink task
 *
 *  Setup Broker Task
 *  Task has highest priority and receives 1kB of stack
 *
 *   \param prio the task's priority.
 *   \param name the task's name.
 *   \param broker_descriptor broker descriptor.
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_Broker_Task(int prio, xdc_String name, broker_descriptor *capSense_desc);


#endif /* LOCAL_INC_BROKER_TASK_H_ */
