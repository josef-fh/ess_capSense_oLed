/*
 *  ======== Blink_Task.c ========
 *  Author: Michael Kranl, Josef Ramsauer
 */
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include "CapSense_Task.h"
#include "EK_TM4C1294XL.h"
#include <ti/drivers/I2C.h>

/* Application headers */

/*
 *
 */

#define SLAVE_ADDRESS (0x00)


static I2C_Handle i2c_handle;
static I2C_Params i2cparams;
uint8_t buffer[2];
uint8_t buffer_old[2];
static uint16_t ir_raw;

void I2c_init()
{
    Board_initI2C();
    I2C_Params_init(&i2cparams);
    i2cparams.bitRate = I2C_400kHz;/*in case this is too fast use I2C_400kHz*/
    i2cparams.transferMode = I2C_MODE_BLOCKING;/*important if you call I2C_transfer in Task context*/
    i2cparams.transferCallbackFxn = NULL;
    i2c_handle = I2C_open(EK_TM4C1294XL_I2C8, &i2cparams);

    if (i2c_handle == NULL)
    {
        System_abort("I2C was not opened");
    }
}

//--------------- Reads data
uint8_t I2c_read (uint8_t reg)
{

    I2C_Transaction i2c;
    uint8_t readBuffer[2];
    uint8_t writeBuffer[1];

    writeBuffer[0] = reg;

    Bool transferOK;
    i2c.slaveAddress = SLAVE_ADDRESS; /* 7-bit peripheral slave address */
    i2c.writeBuf = writeBuffer; /* Buffer to be written */
    i2c.writeCount = 1; /* Number of bytes to be written */
    i2c.readBuf = (UChar*)readBuffer; /* Buffer to be read */
    i2c.readCount = 2; /* Number of bytes to be read */

    transferOK = I2C_transfer(i2c_handle, &i2c); /* Perform I2C transfer */
    if (!transferOK)
    {
        System_abort("I2C bus fault");
    }

    return readBuffer[0];
}

//--------------- Writes data
void I2c_write (uint8_t reg, uint8_t regbit)
{
    I2C_Transaction i2c;
    uint8_t writeBuffer[2];
    writeBuffer[0] = reg;
    writeBuffer[1] = regbit;
    Bool transferOK;

    i2c.slaveAddress = SLAVE_ADDRESS; /* 7-bit peripheral slave address */
    i2c.writeBuf = writeBuffer; /* Buffer to be written */
    i2c.writeCount = 2; /* Number of bytes to be written */
    i2c.readBuf = NULL; /* Buffer to be read */
    i2c.readCount = 0; /* Number of bytes to be read */

    transferOK = I2C_transfer(i2c_handle, &i2c); /* Perform I2C transfer */
    if (!transferOK)
    {
        System_abort("I2C bus fault");
    }
}


void I2c_capSenseConf(void)
{
    I2c_write(COMMAND_REG, 0x08);
    I2c_write(CS_ENABL1,   0x1F);     // Five pins will be used for Slider pads
    I2c_write(CS_ENABL0,   0x18);     // Two pins will be used for Button pads
    I2c_write(GPIO_ENABLE0,0x03);     // Three pins will be used as GPIO 2 for LED and 1 as GPIO2
    I2c_write(DM_STRONG0,  0x03);     // Enables strong drive mode for GPIOs

    I2c_write(CS_SLID_CONFIG, 0x01);  // Enable slider
    /*
       Configure slider resolution Resolution = (SensorsInSlider - 1) * Multiplier
       Resolution = 4 * 16.00 = 64 so the slider_val will be in range 0 to 64
    */
    I2c_write(CS_SLID_MULM,   0x10);
    I2c_write(CS_SLID_MULL,   0x00);
    I2c_write(COMMAND_REG, 0x01);     // Store Current Configuration to NVM
    SysCtlDelay(15000000);
    I2c_write(COMMAND_REG, 0x06);     // Reconfigure Device (POR)
    SysCtlDelay(600000);
    // Initial ON*OFF*ON LEDs (inverse logic 0-LED ON, 1-LED OFF)
    I2c_write(OUTPUT_PORT0, 0x00);
    SysCtlDelay(600000);
    I2c_write(OUTPUT_PORT0, 0x03);
    SysCtlDelay(600000);

    I2c_write(OUTPUT_PORT0, 0x00);
    SysCtlDelay(600000);
}


void I2c_writeRead(uint8_t reg)
{
    I2C_Transaction i2c;
    uint8_t readBuffer[2] = {};
    uint8_t writeBuffer[1];
    Bool transferOK;

    writeBuffer[0] = reg;

    i2c.slaveAddress = SLAVE_ADDRESS; /* 7-bit peripheral slave address */
    i2c.writeBuf = writeBuffer; /* Buffer to be written */
    i2c.writeCount = 1; /* Number of bytes to be written */
    i2c.readBuf = (UChar*)readBuffer; /* Buffer to be read */
    i2c.readCount = 2; /* Number of bytes to be read */

    transferOK = I2C_transfer(i2c_handle, &i2c); /* Perform I2C transfer */
    if (!transferOK)
    {
        System_abort("I2C bus fault");
    }

    if((readBuffer[0] != 255) && (readBuffer[1] != 255))// && (readBuffer[1] != 64) && (readBuffer[1] != 48))
    {
        buffer[0] = readBuffer[0];
        buffer[1] = readBuffer[1];
        ir_raw = readBuffer[0];
    }
}

void CapSenseMain(UArg arg0,UArg arg1)
{
    static bool ignoreOneTimeSlider;
    uint8_t led0st = 0, led1st = 0;
    uint8_t cs_status;
    uint8_t btn0st, btn1st;
    static uint8_t tgl0, tgl1;

    I2c_init();
    I2c_capSenseConf();

    while(true)
    {
        cs_status = I2c_read(CS_READ_STATUS0);
        btn0st = (cs_status & 0x08) >> 3;
        btn1st = (cs_status & 0x10) >> 4;

        if (btn0st != 0 )
        {
            ignoreOneTimeSlider = true;  //after pressing a button the slider must be ignore once
            printf("data: %d",btn0st);
            printf("\n");
        }

        if (btn1st != 0 )
        {
            ignoreOneTimeSlider = true;
            printf("data1: %d",btn1st);
            printf("\n");
        }

        if(buffer_old[1] != buffer[1])
        {
            if (ignoreOneTimeSlider != true)
            {
                printf("buffer1: %d:  %d",buffer[1], ir_raw);
                printf("\n");
            }

            ignoreOneTimeSlider = false;
            buffer_old[1] = buffer[1];
        }

        I2c_writeRead(CS_READ_CEN_POSM);


        // Check button 0
        if (btn0st == 1)                // Button 0 is pressed
        {
           tgl0 =  1;                   // Update a toggle flag variable
        }
        else
        {
            if (tgl0 == 1)              // Button 0 is pressed and released
            {
                tgl0 = tgl0 | 2;        // Update a toggle flag variable
            }
        }
        if (tgl0 == 3)                  // Toggle flag is active
        {
            led0st = 0x01 ^ led0st;     // Update the LED status
            tgl0 = 0;                   // Reset toggle flag
        }
        // Check button 1
        if(btn1st == 1)                 // Button 1 is pressed
        {
            tgl1 = 1;                   // Update a toggle flag variable
        }
        else
        {
            if(tgl1 == 1)               // Button 1 is pressed and released
            {
                tgl1 = tgl1 | 2;        // Update a toggle flag variable
            }
        }
        if (tgl1 == 3)                  // Toggle flag is active
        {
            led1st = 0x02 ^ led1st;     // Update the LED status
            tgl1 = 0;                   // Reset toggle flag
        }
        I2c_write(OUTPUT_PORT0, 0x1C | led1st  | led0st);
        SysCtlDelay(1200000);
    }
}


/*
 *  Setup task function
 */
int setup_CapSense_Task(int prio, xdc_String name, struct capSense_descriptor *capSense_desc)
{
    Task_Params taskCapSenseParams;
    Task_Handle taskCapSense;
    Error_Block eb;

    /* Create blink task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskCapSenseParams);
    taskCapSenseParams.instance->name = name;
    taskCapSenseParams.stackSize = 2048;//1024; /* stack in bytes */
    taskCapSenseParams.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskCapSenseParams.arg0 = (UArg)capSense_desc; /* pass led descriptor as arg0 */
    taskCapSense = Task_create((Task_FuncPtr)CapSenseMain, &taskCapSenseParams, &eb);
    if (taskCapSense == NULL) {
        System_abort("taskCapSense create failed");
    }

    return (0);
}
