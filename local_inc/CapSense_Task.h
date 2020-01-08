/*! \file Blink_Task.h
    \brief Blink task
    \author Matthias Wenzl
    \author Michael Kramer

    Blinking LED Task example.

*/

#ifndef CAP_SENSE_TASK_H_
#define CAP_SENSE_TASK_H_

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
#include "Broker_Task.h"

#include "CY8C201A0.h"



typedef struct {
	uint32_t g_ui32SysClock;
	mailbox_descriptor *mailbox_des;
}capSense_descriptor;


/*! \fn setup_CapSense_Task
 *  \brief Setup CapSense task
 *
 *  Setup CapSense task
 *  Task has highest priority and receives 1kB of stack
 *
 *   \param prio the task's priority.
 *   \param name the task's name.
 *   \param capSense_descriptor CapSebse descriptor.
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_CapSense_Task(int prio, xdc_String name, capSense_descriptor *capSense_desc);

#endif
