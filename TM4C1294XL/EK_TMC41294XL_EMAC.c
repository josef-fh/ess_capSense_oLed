/*! \file EK_TMC41294XL_EMAC.c
    \brief EMAC driver for the tm4c1294ncpdt
    \author Matthias Wenzl

    Implements the network csard interface driver.
    Based on NDK EMAC driver, but modified to fit NDK less network stacks.

*/



#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>
#include <inc/hw_emac.h>

#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/sysctl.h>
#include <driverlib/udma.h>
#include <driverlib/pin_map.h>

//emac specific
#include <driverlib/emac.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/cfg/global.h>

#include <EK_TM4C1294XL.h>


#include <network_events.h>

//*****************************************************************************
//
// Ethernet DMA descriptors.
//
// The MAC hardware needs a minimum of 3 receive descriptors to operate. The
// number used will be application-dependent and should be tuned for best
// performance.
//
//*****************************************************************************
static tEMACDMADescriptor g_psRxDescriptor[NUM_RX_DESCRIPTORS];
static tEMACDMADescriptor g_psTxDescriptor[NUM_TX_DESCRIPTORS];
static uint32_t g_ui32RxDescIndex;
static uint32_t g_ui32TxDescIndex;

static unsigned char gmacAddress[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#if ETH_USE_EVENT
static Event_Handle _networkEvents;
#endif

#if ETH_USE_SEM
static Semaphore_Handle _emac_rx_lock;
#endif

//enough space to hold a complete ethernet frame (excluding jumbo frames)
#define RX_BUFFER_SIZE 1536

static void InitDescriptors(uint32_t ui32Base) {

	uint32_t ui32Loop;
	Error_Block eb;

	/*
	Initialize each of the transmit descriptors. Note that we leave the
	buffer pointer and size empty and the OWN bit clear here since we have
	not set up any transmissions yet.
	*/
	for(ui32Loop = 0; ui32Loop < NUM_TX_DESCRIPTORS; ui32Loop++) {
		g_psTxDescriptor[ui32Loop].ui32Count = DES1_TX_CTRL_SADDR_INSERT;
		g_psTxDescriptor[ui32Loop].DES3.pLink = (ui32Loop == (NUM_TX_DESCRIPTORS - 1)) ?
												g_psTxDescriptor : &g_psTxDescriptor[ui32Loop + 1];
		g_psTxDescriptor[ui32Loop].ui32CtrlStatus = (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
													DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_CHAINED |
													DES0_TX_CTRL_IP_ALL_CKHSUMS);
	}
	/*
 	 Initialize each of the receive descriptors. We clear the OWN bit here
 	 to make sure that the receiver doesn’t start writing anything
 	 immediately.
	 */
	for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++) {
		g_psRxDescriptor[ui32Loop].ui32CtrlStatus = 0;
		g_psRxDescriptor[ui32Loop].ui32Count = (DES1_RX_CTRL_CHAINED | (RX_BUFFER_SIZE << DES1_RX_CTRL_BUFF1_SIZE_S));

		Error_init(&eb);
		g_psRxDescriptor[ui32Loop].pvBuffer1 = (uint8_t *)Memory_alloc((xdc_runtime_IHeap_Handle)cfg_netHeap, (RX_BUFFER_SIZE), 0, &eb);
		if (g_psRxDescriptor[ui32Loop].pvBuffer1 == NULL) {
		    System_abort("Memory allocation for global receiver buffer failed");
		}


		g_psRxDescriptor[ui32Loop].DES3.pLink = (ui32Loop == (NUM_RX_DESCRIPTORS - 1)) ?
												g_psRxDescriptor : &g_psRxDescriptor[ui32Loop + 1];
	}

	/*
	 Set the descriptor pointers in the hardware.
	 */
	EMACRxDMADescriptorListSet(ui32Base, g_psRxDescriptor);
	EMACTxDMADescriptorListSet(ui32Base, g_psTxDescriptor);
	/*
	Start from the beginning of both descriptor chains. We actually set
	the transmit descriptor index to the last descriptor in the chain
	since it will be incremented before use and this means the first
	transmission we perform will use the correct descriptor.
	 */
	g_ui32RxDescIndex = 0;
	g_ui32TxDescIndex = NUM_TX_DESCRIPTORS - 1;

}


//*****************************************************************************
//
// The interrupt handler for the Ethernet interrupt.
//
//*****************************************************************************
void EthernetIntHandler(UArg arg) {

	uint32_t ui32Temp;
	/*Read and Clear the interrupt.*/
	ui32Temp = EMACIntStatus(EMAC0_BASE, true);
	EMACIntClear(EMAC0_BASE, ui32Temp);

	/*Check to see if an RX Interrupt has occurred*/
	if(ui32Temp & EMAC_INT_RECEIVE) {
		// Indicate that a packet has been received.
#if ETH_USE_SEM
		Semaphore_post(_emac_rx_lock);
#endif
#if ETH_USE_EVENT
		Event_post(_networkEvents,EMAC_RX_EVENT);
#endif
	/*in case no rx buffer is available*/
	}
	if(ui32Temp & EMAC_INT_RX_NO_BUFFER) {
#if ETH_USE_EVENT
	    Event_post(_networkEvents,EMAC_RX_NO_BUFFER);
#endif
	}
#if ETH_USE_EVENT
	if(ui32Temp & EMAC_INT_TRANSMIT ) {
		Event_post(_networkEvents,EMAC_TX_EVENT);
	}
#endif

}


//*****************************************************************************
//
// Advance Receiver DMA Buffer (Free Memory from application context)
//
//*****************************************************************************
//does this really set the pointer right?, which descriptor is freed here?
void FreeEmacRxBuf(void) {

	g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus = DES0_RX_CTRL_OWN;
	/* Move on to the next descriptor in the chain.*/
	g_ui32RxDescIndex++;
	if(g_ui32RxDescIndex == NUM_RX_DESCRIPTORS) {
		g_ui32RxDescIndex = 0;
	}

}


//*****************************************************************************
//
// Read a packet from the DMA receive buffer and return a pointer to the receiver buffer
// the number of bytes, len to the amount of bytes received.
// Descriptor has to be freed manually after copying received frame to internal buffer.
//
//
//*****************************************************************************
uint8_t * PacketReceive(int32_t *len) {

	int_fast32_t i32FrameLen;

	// By default, we assume we got a bad frame.
	i32FrameLen = 0;

	// Make sure that we own the receive descriptor.
	if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_CTRL_OWN)) {

		// We own the receive descriptor so check to see if it contains a valid
		// frame.
		if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_STAT_ERR)) {

			// We have a valid frame. First check that the "last descriptor"
			// flag is set. We sized the receive buffer such that it can
			// always hold a valid frame so this flag should never be clear at
			// this point but...
			if(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_STAT_LAST_DESC) {
				//
				// What size is the received frame?
				//
				i32FrameLen = ((g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
							DES0_RX_STAT_FRAME_LENGTH_M) >> DES0_RX_STAT_FRAME_LENGTH_S);

				*len = (int32_t)i32FrameLen;
				return (((uint8_t *)g_psRxDescriptor[g_ui32RxDescIndex].pvBuffer1));
			}
		}
	}

	// Return default Frame handle and length
	*len = 0;
	return((uint8_t *)NULL);
}


//*****************************************************************************
//
// Transmit a packet from the supplied buffer. This function would be called
// directly by the application. pui8Buf points to the Ethernet frame to send
// and i32BufLen contains the number of bytes in the frame.
// This functon does busy waiting until the next transmit descriptor is free.
//
//*****************************************************************************
int32_t PacketTransmit(uint8_t *pui8Buf, int32_t i32BufLen) {

	// Wait for the transmit descriptor to free up.
	while(g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus & DES0_TX_CTRL_OWN);


	// Move to the next descriptor.
	// To be valid for the first transmission, the initial descriptor pointer
	//points to the last element
	g_ui32TxDescIndex++;
	if(g_ui32TxDescIndex == NUM_TX_DESCRIPTORS) {
		g_ui32TxDescIndex = 0;
	}

	// Fill in the packet size and pointer, and tell the transmitter to start
	// work.
	g_psTxDescriptor[g_ui32TxDescIndex].ui32Count = (uint32_t)i32BufLen;
	g_psTxDescriptor[g_ui32TxDescIndex].pvBuffer1 = pui8Buf;
	g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus = (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
														DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_IP_ALL_CKHSUMS |
														DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_OWN);

	// Tell the DMA to reacquire the descriptor now that we’ve filled it in.
	// This call is benign if the transmitter hasn’t stalled and checking
	// the state takes longer than just issuing a poll demand so we do this
	// for all packets.
	EMACTxDMAPollDemand(EMAC0_BASE);

	// Return the number of bytes sent.
	return(i32BufLen);
}



uint8_t * EK_TM4C1294XL_getMACAddr(void) {

    EMACAddrGet(EMAC0_BASE, 0, gmacAddress);
    return (uint8_t *)&gmacAddress[0];

}


/*
 * EK_TM4C1294XL specific ethernet configuration
 * statically sets and configures everything
 *
 */
#if ETH_USE_SEM
Semaphore_Handle EK_TM4C1294XL_initEMAC(uint32_t sysclock, uint8_t *mac_addr) {
#endif
#if ETH_USE_EVENT
Event_Handle EK_TM4C1294XL_initEMAC(uint32_t sysclock, uint8_t *mac_addr) {
#endif

	uint32_t ui32Loop;
	uint8_t ui8PHYAddr;
    uint32_t ulUser0, ulUser1;
#if ETH_USE_SEM
    Semaphore_Params sem_param;
#endif

    /*hardware interrupt object handling*/
    Hwi_Params hwiParams;
    Hwi_Handle emacHwi;
    Error_Block eb;

    /*retreive and configure this device's mac address*/
    if(mac_addr != NULL) {
        memcpy(gmacAddress,mac_addr,6);
    } else {
        /* Get the MAC address */
        FlashUserGet(&ulUser0, &ulUser1);

        if ((ulUser0 != 0xffffffff) && (ulUser1 != 0xffffffff)) {
            /*
             *  Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
             *  address needed to program the hardware registers, then program the MAC
             *  address into the Ethernet Controller registers.
             */
            gmacAddress[0] = ((ulUser0 >>  0) & 0xff);
            gmacAddress[1] = ((ulUser0 >>  8) & 0xff);
            gmacAddress[2] = ((ulUser0 >> 16) & 0xff);
            gmacAddress[3] = ((ulUser1 >>  0) & 0xff);
            gmacAddress[4] = ((ulUser1 >>  8) & 0xff);
            gmacAddress[5] = ((ulUser1 >> 16) & 0xff);

          /*no  mac address given and none already available*/
        } else {
            return NULL;
        }
    }

    //Connect LEDs to PHY
    GPIOPinConfigure(GPIO_PF0_EN0LED0);  /* EK_TM4C1294XL_USR_D3 */
    GPIOPinConfigure(GPIO_PF4_EN0LED1);  /* EK_TM4C1294XL_USR_D4 */
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);


    // Connect Ethernet Pulse Per Second Output
    GPIOPinConfigure(GPIO_PG0_EN0PPS);/*GPIO_PJ0_EN0PPS if pg0 does not work*/

    // Enable and reset the Ethernet modules.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);

    // Wait for the MAC to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0));


    // Configure for use with the internal PHY.
    ui8PHYAddr = 0;
    EMACPHYConfigSet(EMAC0_BASE, (EMAC_PHY_TYPE_INTERNAL |
    								EMAC_PHY_INT_MDIX_EN |
    								EMAC_PHY_AN_100B_T_FULL_DUPLEX));

    // Reset the MAC to latch the PHY configuration.
    EMACReset(EMAC0_BASE);

    // Initialize the MAC and set the DMA mode.
    EMACInit(EMAC0_BASE,sysclock,
            (EMAC_BCONFIG_MIXED_BURST |
             EMAC_BCONFIG_PRIORITY_FIXED), 4, 4, 0);

    // Set MAC configuration options.
    EMACConfigSet(EMAC0_BASE,
    (EMAC_CONFIG_FULL_DUPLEX |
    EMAC_CONFIG_CHECKSUM_OFFLOAD |/*calculate ip, ipv6, tcp,udp icmp checksums in hardware*/
    EMAC_CONFIG_7BYTE_PREAMBLE |
    EMAC_CONFIG_IF_GAP_96BITS |
    EMAC_CONFIG_USE_MACADDR0 |
    EMAC_CONFIG_SA_FROM_DESCRIPTOR |
    EMAC_CONFIG_BO_LIMIT_1024),
    (EMAC_MODE_RX_STORE_FORWARD |
    EMAC_MODE_TX_STORE_FORWARD |
    EMAC_MODE_TX_THRESHOLD_64_BYTES |
    EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);

    // Initialize the Ethernet DMA descriptors.
    InitDescriptors(EMAC0_BASE);

    // Program the hardware with its MAC address (for filtering).
    EMACAddrSet(EMAC0_BASE, 0, gmacAddress);

    // Wait for the link to become active.
    //!! INFO: cable MUST be plugged in for this to return! !!
     while((EMACPHYRead(EMAC0_BASE, ui8PHYAddr, EPHY_BMSR) & EPHY_BMSR_LINKSTAT) == 0);


    // Set MAC filtering options. We receive all broadcast and multicast
    // packets along with those addressed specifically for us.
    EMACFrameFilterSet(EMAC0_BASE, (EMAC_FRMFILTER_SADDR |
    					EMAC_FRMFILTER_PASS_MULTICAST |
    					EMAC_FRMFILTER_PASS_NO_CTRL));

    // Clear any pending interrupts.
    EMACIntClear(EMAC0_BASE, EMACIntStatus(EMAC0_BASE, false));


    // Mark the receive descriptors as available to the DMA to start
    // the receive processing.
    for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
    	g_psRxDescriptor[ui32Loop].ui32CtrlStatus |= DES0_RX_CTRL_OWN;

#if ETH_USE_SEM
    /*create emac interrupt locking*/
    Error_init(&eb);
    Semaphore_Params_init(&sem_param);
    sem_param.mode = ti_sysbios_knl_Semaphore_Mode_BINARY;/*locked or unlocked only, no counting*/
    _emac_rx_lock = Semaphore_create(0,&sem_param,&eb);/*set to locked*/
    if(_emac_rx_lock == NULL)
    	return NULL;
#endif

#if ETH_USE_EVENT
    /*setup event handler - used to inform network task about rx,tx,timeout events*/
    Error_init(&eb);
    _networkEvents = Event_create(NULL,&eb);
    if (_networkEvents == NULL)
       	return NULL;
#endif

    /*
     * register emac isr at the rtos
     * */
    Error_init(&eb);
    Hwi_Params_init(&hwiParams);

    hwiParams.arg = 0;//don't want to pass any arguments to the isr
    hwiParams.enableInt = false;
    hwiParams.maskSetting = ti_sysbios_interfaces_IHwi_MaskingOption_SELF;//cannot nest itself

    emacHwi = Hwi_create(INT_EMAC0, EthernetIntHandler,&hwiParams,&eb);
    if(emacHwi == NULL) {
    	System_abort("Emac HWi creation failed");
    }

#if ETH_INIT_AND_RUN
    // Enable the Ethernet MAC transmitter and receiver.
    EMACTxEnable(EMAC0_BASE);
    EMACRxEnable(EMAC0_BASE);

    // Enable the Ethernet interrupt.
    Hwi_enableInterrupt(INT_EMAC0);

    // Enable the Ethernet RX Packet and no buffer rx available interrupt source as well as the transmit completed interrupt.
    EMACIntEnable(EMAC0_BASE, EMAC_INT_RECEIVE | EMAC_INT_RX_NO_BUFFER | EMAC_INT_TRANSMIT);
#endif

#if ETH_USE_SEM
    return _emac_rx_lock;
#endif
#if ETH_USE_EVENT
    return _networkEvents;
#endif

}

#if  !ETH_INIT_AND_RUN
void EK_TM4C1294XL_startEMAC(void) {

    // Enable the Ethernet MAC transmitter and receiver.
    EMACTxEnable(EMAC0_BASE);
    EMACRxEnable(EMAC0_BASE);

    // Enable the Ethernet interrupt.
    Hwi_enableInterrupt(INT_EMAC0);

    // Enable the Ethernet RX Packet and no buffer rx available interrupt source as well as the transmit completed interrupt.
    EMACIntEnable(EMAC0_BASE, EMAC_INT_RECEIVE);// | EMAC_INT_RX_NO_BUFFER | EMAC_INT_TRANSMIT);

}
#endif
