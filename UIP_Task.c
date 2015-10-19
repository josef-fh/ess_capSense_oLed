/*! \file UIP_Task.c
    \brief CCSv6 project using TI-RTOS and a custom network driver providing an ndk-less base environment
    \author Matthias Wenzl

*/



/*
 *  ======== UIP_Task.c ========
 */
#include <stdbool.h>
#include <inc/hw_memmap.h>


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


#include <ctype.h>
#include <string.h>

#include <network_events.h>
#include <driverlib/emac.h>
/*access statically created symbols from cfg file*/
#include <xdc/cfg/global.h>


#if ETH_USE_SEM
static Semaphore_Handle my_emac_rx_sem;
#endif

#if ETH_USE_EVENT
static Event_Handle my_network_events;
#endif

#define ETH_BUFSIZE 1536/*TBD*/

/*
 *  ======== netFxn ========
 *  Perform Networking operation
 */
void NetFxn(UArg arg0, UArg arg1)
{

	uint32_t event_mask = 0;
	uint8_t *eth_buf = NULL;
	uint8_t *eth_rx_ptr = NULL;
	uint16_t eth_len;
	uint32_t sysclock = (uint32_t) arg0;
	Error_Block eb;



    /**
     * setup network stack
     **/

    /*prepare network interface (incl. MAC address) and initialize stack event handler, or semaphore*/
#if ETH_USE_SEM
    my_emac_rx_sem = EK_TM4C1294XL_initEMAC(sysclock,NULL);
    if(my_emac_rx_sem == NULL) {
       	System_abort("Could not initialize EMAC");
    }
#endif
#if ETH_USE_EVENT
    my_network_events = EK_TM4C1294XL_initEMAC(sysclock,NULL/*use built in mac*/);
    if(my_network_events == NULL) {
    	System_abort("Could initialize EMAC");
    }
#endif

    /*start network interface*/
    EK_TM4C1294XL_startEMAC();


	//get ONLY ONE memory block from network stack heap - optimize - this is YOUR! task
	Error_init(&eb);
	eth_buf = (uint8_t *)Memory_alloc((xdc_runtime_IHeap_Handle)cfg_netHeap, ETH_BUFSIZE, 0, &eb);
	if (eth_buf == NULL) {
	    System_abort("Memory allocation for eth_buf failed");
	}

	/*free memory again example
		Memory_free((xdc_runtime_IHeap_Handle)cfg_netHeap, eth_buf, ETH_BUFSIZE);
	*/


	while(1) {

	    //postpones execution if no event is available
		event_mask = Event_pend(my_network_events,NON_EVENT_MASK,(EMAC_RX_EVENT | EMAC_TX_EVENT | EMAC_RX_NO_BUFFER),BIOS_WAIT_FOREVER);


		/*no receiver buffer was available
		 * */
		if(event_mask & EMAC_RX_NO_BUFFER) {
		    FreeEmacRxBuf();
		    EMACRxDMAPollDemand(EMAC0_BASE);
		}


		 //
		 // Check for an RX Packet and read it.
		 //
		 if(event_mask & EMAC_RX_EVENT)
		 {
		     // Get the packet and set uip_len for uIP stack usage.
		     //

			 eth_rx_ptr =  PacketReceive((int32_t *)(&eth_len));
			 if(eth_len > ETH_BUFSIZE)
			     eth_len = ETH_BUFSIZE;//truncate received packet length if necessary

			 /* copy to private buffer and free */
			 memcpy(eth_buf,eth_rx_ptr,eth_len);
			 FreeEmacRxBuf();
			 eth_rx_ptr = NULL;

			 /*exchange mac addresses here*/

			 /*transmit altered packet*/
		     (void) PacketTransmit(eth_buf, eth_len);
		      eth_len = 0;
		 }
	}


}


/*
 *  setup task function
 */
int setup_UIP_Task(uint32_t sysclock)
{
	Task_Params taskNetworkParams;
	Task_Handle taskNetwork;
	Error_Block eb;

    /* Create networking task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskNetworkParams);
    taskNetworkParams.stackSize = 4096;/*stack in bytes*/
    taskNetworkParams.priority = 15;/*15 is default 16 is highest priority -> see RTOS configuration*/
    taskNetworkParams.arg0 = (UArg) sysclock;/*pass sysclock to arg0*/
    taskNetwork = Task_create((Task_FuncPtr)NetFxn, &taskNetworkParams, &eb);
    if (taskNetwork == NULL) {
    	System_abort("Task create failed");
    }

    return (0);
}