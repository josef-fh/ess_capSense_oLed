/*
 * Broker_Task.h
 *
 *  Created on: 25 Dec 2019
 *      Author: JoRam
 */

#ifndef LOCAL_INC_BROKER_TASK_H_
#define LOCAL_INC_BROKER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>

#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/debug.h>
#include <driverlib/fpu.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/i2c.h>
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <stdio.h>
#include <ti/sysbios/knl/Mailbox.h>

#include "CY8C201A0.h"

enum mailboxIndex{
<<<<<<< HEAD
    MESSAGE_FROM_CAPSENSE_TO_BROKER,
    MESSAGE_FROM_BROKER_TO_UART,
    MESSAGE_FROM_BROKER_TO_OLED,
    MESSAGE_ENTIRE_NUMBER
=======
    MAILBOX_FROM_CAPSENSE_TO_BROKER,
    MAILBOX_FROM_BROKER_TO_UART,
    MAILBOX_FROM_BROKER_TO_OLED,
    MAILBOX_ENTIRE_NUMBER
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
};


typedef struct{
    Mailbox_Params mboxParams;
    Mailbox_Handle mailboxHandle;
}mailbox_descriptor;


struct broker_descriptor {
    uint32_t g_ui32SysClock;
<<<<<<< HEAD
    mailbox_descriptor *mailbox_des;
=======
    mailbox_descriptor *mailbox_des[MAILBOX_ENTIRE_NUMBER];
>>>>>>> 7e822160321311000fe972d67ed5f8092afe3ba4
};



typedef struct{
    bool pressedButton0;
    bool pressedButton1;
    uint8_t valueSlider;
}capSense_values;

/*! \fn setup_Blink_Task
 *  \brief Setup Blink task
 *
 *  Setup Blink task
 *  Task has highest priority and receives 1kB of stack
 *
 *   \param prio the task's priority.
 *   \param name the task's name.
 *   \param led_desc LED descriptor.
 *   \param time to wait in ticks for led to toggle
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_Broker_Task(int prio, xdc_String name, struct broker_descriptor *capSense_desc);


#endif /* LOCAL_INC_BROKER_TASK_H_ */
