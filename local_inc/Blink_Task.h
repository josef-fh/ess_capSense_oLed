/*! \file Blink_Task.h
    \brief Blink task
    \author Matthias Wenzl
    \author Michael Kramer


    Blinking LED Task example.

*/

#include <stdbool.h>
#include <stdint.h>
/* Drivers Header files - fall back to driverlib for gpio*/
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h>


#ifndef BLINK_TASK_H_
#define BLINK_TASK_H_


typedef struct {
	uint32_t port_base;
	uint8_t led;
}led_descriptor_t;



/*! \fn BlinkFxn
 *  \brief Execute Blink Task
 *
 *
 *   \param arg0 led descriptor struct
 *   \param arg1 Ticks to wait
 *
*/
void BlinkFxn(UArg arg0, UArg arg1);

/*! \fn setup_Blink_Task
 *  \brief Setup Blink task
 *
 *  Setup Blink task
 *  Task has highest priority and receives 1kB of stack
 *
 *   \param led description.
 *   \param time wo wait in ticks for led to toggle
 *
 *  \return always zero. In case of error the system halts.
*/
int setup_Blink_Task(led_descriptor_t *led_desc, uint32_t wait_ticks);



#endif /* UIP_TASK_H_ */
