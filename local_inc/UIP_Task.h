/*! \file UIP_Task.h
    \brief Network stack task
    \author Matthias Wenzl
    \author Michael Kramer


    Provides interfacing with network card driver.

*/

#ifndef UIP_TASK_H_
#define UIP_TASK_H_



/*! \fn NetFxn
 *  \brief Execute uip network stack
 *
 *  Execute uip network stack.
 *  Handle Network interface I/O
 *
 *   \param arg0 System clock
 *   \param arg1 Generic second argument. Not used.
 *
*/
void NetFxn(UArg arg0, UArg arg1);

/*! \fn setup_UIP_Task
 *  \brief Setup network stack task
 *
 *  Setup network stack task
 *  Task has highest priority and receives 4kB of stack
 *
 *   \param sysclock System clock.
 *
 *  \return always zero. In case of error the system halts.
*/
int setup_UIP_Task(uint32_t sysclock);



#endif /* UIP_TASK_H_ */
