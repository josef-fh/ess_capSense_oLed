/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       EK_TM4C1294XL.h
 *
 *  @brief      EK_TM4C1294XL Board Specific APIs
 *
 *  The EK_TM4C1294XL header file should be included in an application as follows:
 *  @code
 *  #include <EK_TM4C1294XL.h>
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef __EK_TM4C1294XL_H
#define __EK_TM4C1294XL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <Board.h>

#define ETH_USE_SEM 0
#define ETH_USE_EVENT 1
#define ETH_INIT_AND_RUN 0


#if(!(ETH_USE_SEM ^ ETH_USE_EVENT))
	#error "Use either ETH_USE_SEM or ETH_USE_EVENT"
#endif


/*EMAC specifics*/

/*transmitter, receiver descriptor amount - 3 is the absolute minimum (see datahseet).
 * increase with respect to available memory*/
/*However, due to internal uip stack operation concepts, only one 1536 byte long
 * buffer will be used for transmission purposes*/
#define NUM_TX_DESCRIPTORS 3
#define NUM_RX_DESCRIPTORS 5



/* LEDs on EK_TM4C1294XL are active high. */
#define EK_TM4C1294XL_LED_OFF ( 0)
#define EK_TM4C1294XL_LED_ON  (~0)

/* GPIO_Callbacks structure for GPIO interrupts */
/* handled by <Board.h>
 * extern const GPIO_Callbacks EK_TM4C1294XL_gpioPortJCallbacks;
 *
 */

/*!
 *  @def    EK_TM4C1294XL_EMACName
 *  @brief  Enum of EMAC names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_EMACName {
    EK_TM4C1294XL_EMAC0 = 0,

    EK_TM4C1294XL_EMACCOUNT
} EK_TM4C1294XL_EMACName;

/*!
 *  @def    EK_TM4C1294XL_GPIOName
 *  @brief  Enum of LED names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_GPIOName {
	EK_TM4C1294XL_D1 = 0,
	EK_TM4C1294XL_D2,
	EK_TM4C1294XL_USR_SW1,
	EK_TM4C1294XL_USR_SW2,

    EK_TM4C1294XL_GPIOCOUNT
} EK_TM4C1294XL_GPIOName;

/*!
 *  @def    EK_TM4C1294XL_I2CName
 *  @brief  Enum of I2C names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_I2CName {
	EK_TM4C1294XL_I2C0 = 0,
	EK_TM4C1294XL_I2C2,
    EK_TM4C1294XL_I2C7,
    EK_TM4C1294XL_I2C8,

    EK_TM4C1294XL_I2CCOUNT
} EEK_TM4C1294XL_I2CName;

/*!
 *  @def    EK_TM4C1294XL_SDSPIName
 *  @brief  Enum of SDSPI names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_SDSPIName {
    EK_TM4C1294XL_SDSPI0 = 0,
    EK_TM4C1294XL_SDSPI1,

    EK_TM4C1294XL_SDSPICOUNT
} EK_TM4C1294XL_SDSPIName;

/*!
 *  @def    EK_TM4C1294XL_SPIName
 *  @brief  Enum of SPI names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_SPIName {
    EK_TM4C1294XL_SPI2 = 0,
    EK_TM4C1294XL_SPI3,

    EK_TM4C1294XL_SPICOUNT
} EK_TM4C1294XL_SPIName;

/*!
 *  @def    EK_TM4C1294XL_UARTName
 *  @brief  Enum of UARTs on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_UARTName {
    EK_TM4C1294XL_UART0 = 0,
    EK_TM4C1294XL_UART2,
    EK_TM4C1294XL_UART6,
    EK_TM4C1294XL_UART7,

    EK_TM4C1294XL_UARTCOUNT
} EK_TM4C1294XL_UARTName;

/*!
 *  @def    EK_TM4C1294XL_USBMode
 *  @brief  Enum of USB setup function on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_USBMode {
    EK_TM4C1294XL_USBDEVICE,
    EK_TM4C1294XL_USBHOST
} EK_TM4C1294XL_USBMode;

/*!
 *  @def    EK_TM4C1294XL_USBMSCHFatFsName
 *  @brief  Enum of USBMSCHFatFs names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_USBMSCHFatFsName {
    EK_TM4C1294XL_USBMSCHFatFs0 = 0,

    EK_TM4C1294XL_USBMSCHFatFsCOUNT
} EK_TM4C1294XL_USBMSCHFatFsName;

/*
 *  @def    EK_TM4C1294XL_WatchdogName
 *  @brief  Enum of Watchdogs on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_WatchdogName {
    EK_TM4C1294XL_WATCHDOG0 = 0,

    EK_TM4C1294XL_WATCHDOGCOUNT
} EK_TM4C1294XL_WatchdogName;

/*!
 *  @def    EK_TM4C1294XL_WiFiName
 *  @brief  Enum of WiFi names on the EK_TM4C1294XL dev board
 */
typedef enum EK_TM4C1294XL_WiFiName {
    EK_TM4C1294XL_WIFI = 0,

    EK_TM4C1294XL_WIFICOUNT
} EK_TM4C1294XL_WiFiName;


/*EMAC specific functions*/
/*initialize receive and transmit DMA descriptors*/
static void InitDescriptors(uint32_t ui32Base);

/*Ethernet Interrupt handler*/
void EthernetIntHandler(UArg arg);

/*
 * @brief - Read a completely received ethernet frame
 * @param - pointer to a variable holding the length of the received frame
 * @return - pointer to the received frame
 * */
uint8_t * PacketReceive(int32_t *len);


/*
 * @brief - Write a complete frame to the transmit buffer in a blocking way (wait until a DMA descriptior is ready)
 * @param *pui8Buf - pointer to a variable holding the frame
 * @param i32BufLen -  variable holding the length of the received frame
 * @return - number of bytes sent
 * */
int32_t PacketTransmit(uint8_t *pui8Buf, int32_t i32BufLen);
void FreeEmacRxBuf(void);

/* @brief - Initialize the EMAC core. Blocks until a link is established
 * @param *mac_addr - a pointer to a buffer holding the to be configured mac address, if a NULL pointer is passed,
 * the pre configured mac address is used, if non is found a NULL pointer is returned
 * @return On success a semaphore handle which can be used to wait for a completely received frame in a task is returned
 * otherwise a NULL pointer is returned
 * */
#if ETH_USE_SEM
Semaphore_Handle EK_TM4C1294XL_initEMAC(uint32_t sysclock, uint8_t *mac_addr);
#endif
#if ETH_USE_EVENT
Event_Handle EK_TM4C1294XL_initEMAC(uint32_t sysclock, uint8_t *mac_addr);
#endif

#if !ETH_INIT_AND_RUN
/*
 * @brief - a seperate start emac function in case ETH_INIT_AND_RUN is not defined
 *
 */
void EK_TM4C1294XL_startEMAC(void);
#endif

/*
 * @biref - get mac address from device
 *
 * @return - pointer to mac address buffer
 *
 */
uint8_t * EK_TM4C1294XL_getMACAddr(void);


void EK_TM4C1294XL_initDMA(void);

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings. This include
 *     - Enable clock sources for peripherals
 */
extern uint32_t EK_TM4C1294XL_initGeneral(uint32_t sysclock);

/*!
 *  @brief Initialize board specific EMAC settings
 *
 *  This function initializes the board specific EMAC settings and
 *  then calls the EMAC_init API to initialize the EMAC module.
 *
 *  The EMAC address is programmed as part of this call.
 *
 */
//extern void EK_TM4C1294XL_initEMAC(void);

/*!
 *  @brief  Initialize board specific GPIO settings
 *
 *  This function initializes the board specific GPIO settings and
 *  then calls the GPIO_init API to initialize the GPIO module.
 *
 *  The GPIOs controlled by the GPIO module are determined by the GPIO_config
 *  variable.
 */
extern void EK_TM4C1294XL_initGPIO(void);

/*!
 *  @brief  Initialize board specific I2C settings
 *
 *  This function initializes the board specific I2C settings and then calls
 *  the I2C_init API to initialize the I2C module.
 *
 *  The I2C peripherals controlled by the I2C module are determined by the
 *  I2C_config variable.
 */
extern void EK_TM4C1294XL_initI2C(void);

/*!
 *  @brief  Initialize board specific SDSPI settings
 *
 *  This function initializes the board specific SDSPI settings and then calls
 *  the SDSPI_init API to initialize the SDSPI module.
 *
 *  The SDSPI peripherals controlled by the SDSPI module are determined by the
 *  SDSPI_config variable.
 */
extern void EK_TM4C1294XL_initSDSPI(void);

/*!
 *  @brief  Initialize board specific SPI settings
 *
 *  This function initializes the board specific SPI settings and then calls
 *  the SPI_init API to initialize the SPI module.
 *
 *  The SPI peripherals controlled by the SPI module are determined by the
 *  SPI_config variable.
 */
extern void EK_TM4C1294XL_initSPI(void);

/*!
 *  @brief  Initialize board specific UART settings
 *
 *  This function initializes the board specific UART settings and then calls
 *  the UART_init API to initialize the UART module.
 *
 *  The UART peripherals controlled by the UART module are determined by the
 *  UART_config variable.
 */
extern void EK_TM4C1294XL_initUART(void);

/*!
 *  @brief  Initialize board specific USB settings
 *
 *  This function initializes the board specific USB settings and pins based on
 *  the USB mode of operation.
 *
 *  @param      usbMode    USB mode of operation
 */
extern void EK_TM4C1294XL_initUSB(EK_TM4C1294XL_USBMode usbMode);

/*!
 *  @brief  Initialize board specific USBMSCHFatFs settings
 *
 *  This function initializes the board specific USBMSCHFatFs settings and then
 *  calls the USBMSCHFatFs_init API to initialize the USBMSCHFatFs module.
 *
 *  The USBMSCHFatFs peripherals controlled by the USBMSCHFatFs module are
 *  determined by the USBMSCHFatFs_config variable.
 */
extern void EK_TM4C1294XL_initUSBMSCHFatFs(void);

/*!
 *  @brief  Initialize board specific Watchdog settings
 *
 *  This function initializes the board specific Watchdog settings and then
 *  calls the Watchdog_init API to initialize the Watchdog module.
 *
 *  The Watchdog peripherals controlled by the Watchdog module are determined
 *  by the Watchdog_config variable.
 */
extern void EK_TM4C1294XL_initWatchdog(void);

/*!
 *  @brief  Initialize board specific WiFi settings
 *
 *  This function initializes the board specific WiFi settings and then calls
 *  the WiFi_init API to initialize the WiFi module.
 *
 *  The hardware resources controlled by the WiFi module are determined by the
 *  WiFi_config variable.
 */
extern void EK_TM4C1294XL_initWiFi(void);

#ifdef __cplusplus
}
#endif

#endif /* __EK_TM4C1294XL_H */
