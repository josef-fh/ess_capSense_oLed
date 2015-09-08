/*! \file network_events.h
    \brief Netork stack events
    \author Matthias Wenzl

    Provides network interface card event handling for the operating system.

*/


#ifndef NETWORK_EVENTS_H_
#define NETWORK_EVENTS_H_


#define NON_EVENT_MASK (0)
#define EMAC_RX_EVENT (1<<1)
#define EMAC_TX_EVENT (1<<2)
#define EMAC_RX_NO_BUFFER (1<<3)


#endif /* NETWORK_EVENTS_H_ */
