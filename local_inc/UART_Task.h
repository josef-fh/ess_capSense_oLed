/*! \file UART_Task.h
    \brief UART task

*/

#ifndef UART_TASK_H_
#define UART_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include "Broker_Task.h"
#include "SPI_Task.h"

typedef struct  {
    uint32_t g_ui32SysClock;
    mailbox_descriptor *mailbox_des;
    Event_data event;
}uart_descriptor;


/*! \fn setup_UART_Task
 *  \brief Setup UART task
 *
 *  Setup UART task
 *  Task has highest priority and receives 1kB of stack
 *
 *  \param prio the task's priority.
 *  \param uart_descriptor Uart descriptor
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_UART_Task(int prio, uart_descriptor *uart_des);

#endif
