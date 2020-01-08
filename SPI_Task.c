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
#include <EK_TM4C1294XL.h>

/* Application headers */
#include "SPI_Task.h"
#include "fonts.h"
#include <ti/drivers/SPI.h>

#include <driverlib/sysctl.h> /* Supplies SysCtl* functions and SYSCTL_* macros */
#include <driverlib/rom.h> /* Supplies ROM_* variations of functions */
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <driverlib/pin_map.h> /* Supplies defines for alternate pin functions */
#include <driverlib/timer.h> /* Supplies Timer* functions for the GP timer*/
#include <driverlib/interrupt.h>
#include <ti/sysbios/knl/Event.h>

#define MAXROWS 300
// global variables
unsigned char display_font;                                                     // valid values are 1 2 3 4     (6 7 8 10 14 16 18 if uncommented)
unsigned int display_color;                                                     // bits 15-0 are r4 r3 r2 r1 r0 g5 g4 g3 g2 g1 g0 b4 b3 b2 b1 b0
unsigned char display_x;                                                        // current x coordinate for OLED color click v100 - 0,0 is top left
unsigned char display_y;                                                        // current y coordinate for OLED color click v100 - 0,0 is top left
unsigned char last_width;                                                       // width of most recent character
unsigned char font_nrows;                                                       // number of rows in most recent font
unsigned char new_y;
uint16_t g_background_color;
uint8_t g_startY;
uint8_t g_font;

void initDisplay(SPIdata *);
uint8_t testPixelinMap(int16_t x, int16_t y);
void clearPixelinMap(int16_t x, int16_t y);
void setPixelinMap(int16_t x, int16_t y);
void OLED_OUT_displayMap(SPIdata *spiData);

//TEST 16 bit sending
void OLED_C_Color16(uint16_t color, SPIdata *spiData);
void OLED_C_data16(uint16_t data_value, SPIdata *spiData);

extern char g_displaytext[400];

typedef uint32_t disp_row[3]; //one row = 96bits = 3*uint32_t

disp_row g_displaymap[MAXROWS];  //Pixelmap for display content
const uint8_t *g_fonts[7];
uint16_t g_rowsFilled;

//int setup_SPI_Task(int prio, xdc_String name, SPIdata *spiData, uint32_t wait_ticks, uint32_t ui32SysClock, Event_data *event)
int setup_SPI_Task(int prio, xdc_String name, spi_descriptor *spi_desc)
{
    Task_Params taskSPIParams;
    Task_Handle taskSPI;
    Error_Block eb;
    int i = 0;

    g_rowsFilled = 95;  //init filled rows with max. displaysize
    g_startY = 0;
    g_background_color = Color_Black;
    display_color = Color_Green;

    g_font = 3;
    g_fonts[0] = guiFont_Tahoma_6_Regular;
    g_fonts[1] = guiFont_Tahoma_7_Regular;
    g_fonts[2] = guiFont_Tahoma_8_Regular;
    g_fonts[3] = guiFont_Tahoma_10_Regular;
    g_fonts[4] = guiFont_Tahoma_14_Regular;
    g_fonts[5] = guiFont_Tahoma_16_Regular;
    g_fonts[6] = guiFont_Tahoma_18_Regular;

    //init displaymap
    for(i = 0; i < MAXROWS; i++)
    {
        g_displaymap[i][0] = 0;
        g_displaymap[i][1] = 0;
        g_displaymap[i][2] = 0;
    }

    //init displaytext
    for(i = 5; i < 400; i++)
        g_displaytext[i] = '\0';
    g_displaytext[0] = 'H';
    g_displaytext[1] = 'a';
    g_displaytext[2]=  'l';
    g_displaytext[3] = 'l';
    g_displaytext[4] = 'o';
    // Enable ports
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //already enabled
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);

    // Configure Reset/Start pin
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, RESET_PIN);

    // Configure R/W pin
    /*  The R/W pin is used only for the parallel communication
     *  When using serial communication (like it is a case for the
     *  OLED C click) this pin should be pulled to a LOW logic level.
     */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, READ_OR_WRITE_PIN);
    GPIOPinWrite(GPIO_PORTE_BASE, READ_OR_WRITE_PIN, READ); //R/W

    // Configure CS pin
    /*  The CS pin is a part of the SPI communication protocol and it
     *  is used to select the device and enable the communication on
     *  the serial bus. When this pin is pulled to a LOW logic level,
     *  the device expects data (or command, depending on the D/C pin
     *  state) to be clocked in from the MOSI pin of the mikroBUS™.     *
     */
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, CHIP_SELECT_PIN);

    // D/C
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN);

    /*START OLED DISPLAY AND INITIALIZE*/
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, SELECT);
    GPIOPinWrite(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN, COMMAND);
    GPIOPinWrite(GPIO_PORTE_BASE, READ_OR_WRITE_PIN, READ);

    // Init sequence for 96x96 OLED color module
    GPIOPinWrite(GPIO_PORTC_BASE, RESET_PIN, RESET);
    //Task_sleep(10); //10ms
    ROM_SysCtlDelay(spi_desc->ui32SysClock/3/10);
    GPIOPinWrite(GPIO_PORTC_BASE, RESET_PIN, SET);
    //Task_sleep(10); //10ms
    ROM_SysCtlDelay(spi_desc->ui32SysClock/3/10);


    Error_init(&eb);
    Task_Params_init(&taskSPIParams);
    taskSPIParams.instance->name = name;
    taskSPIParams.stackSize = 2048; /* stack in bytes */
    taskSPIParams.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskSPIParams.arg0 = (UArg)spi_desc; /* pass led descriptor as arg0 */
    taskSPI = Task_create((Task_FuncPtr)TransferFxn, &taskSPIParams, &eb);
    if (taskSPI == NULL) {
        System_abort("TaskSPI create failed");
    }

    return (0);
}

void TransferFxn(UArg arg0, UArg arg1)
{
    spi_descriptor *spi_desc = (spi_descriptor *)arg0;

    SPIdata *spiData = spi_desc->spiData;
    struct event_data *event = spi_desc->eventData;

    initDisplay(spiData);
    display_color = Color_Green;
    while(1)
    {
        OLED_Out(0, 0, g_displaytext, spiData);
        if(g_rowsFilled > 95)
        {
            g_startY = g_startY + 1 > g_rowsFilled ? 0 : g_startY + 1;
        }
        OLED_OUT_displayMap(spiData);
        Event_pend(event->event, event->num, Event_Id_NONE, BIOS_WAIT_FOREVER);
        //Task_sleep(1);
    }

        /*while(1) {
            if(++linePos == 92)
                linePos = 0;
            DrawLineV(linePos, 0, 95, Color_Orange, spiData);
            DrawLineV(linePos+1, 0, 95, Color_Orange, spiData);
            DrawLineV(linePos+2, 0, 95, Color_Orange, spiData);
            DrawLineH(0, 95, linePos, Color_Cyan, spiData);
            DrawLineH(0, 95, linePos+1, Color_Cyan, spiData);
            DrawLineH(0, 95, linePos+2, Color_Cyan, spiData);
            OLED_CLS(spiData);*/
        //Task_sleep(50);
        //SysCtlDelay((120*1000*1000/3/1000)*(wait_ticks/2));

    //}
}

//void OLED_C_command(unsigned char reg_index, unsigned char reg_value, SPIdata *spiData){
void OLED_C_command(uint16_t reg_index, uint16_t reg_value, SPIdata *spiData){
//Select index address
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, SELECT);
    GPIOPinWrite(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN, COMMAND);
    spiData->transaction.count = 1;
    spiData->transaction.txBuf = &reg_index;
    spiData->transaction.rxBuf = NULL;
    sendSPI(spiData);
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, UNSELECT);

//Write data to register
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, SELECT);
    GPIOPinWrite(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN, DATA);
    spiData->transaction.txBuf = &reg_value;
    sendSPI(spiData);
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, UNSELECT);
}

void DDRAM_access(SPIdata *spiData){
    //uint8_t txBuf = 0x08; //DDRAM access port
    uint16_t txBuf = 0x08; //DDRAM access port

    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, SELECT);
    GPIOPinWrite(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN, COMMAND);
    spiData->transaction.txBuf = &txBuf;
    sendSPI(spiData);
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, UNSELECT);
}

void sendSPI(SPIdata *spiData)
{
    int ret = 0;
    ret = SPI_transfer(spiData->handle, &(spiData->transaction));
    if (!ret) {
        System_printf("SPI transfer failed.\n");
    }
}

void OLED_C_MemorySize(char X1, char X2, char Y1, char Y2,SPIdata *spiData){
    OLED_C_command(SEPS114A_MEM_X1,X1,spiData);
    OLED_C_command(SEPS114A_MEM_X2,X2,spiData);
    OLED_C_command(SEPS114A_MEM_Y1,Y1,spiData);
    OLED_C_command(SEPS114A_MEM_Y2,Y2,spiData);
}

//Send data to OLED C display
void OLED_C_data(unsigned char data_value, SPIdata *spiData){
//Write data to register
    uint16_t data = (uint16_t) data_value;

    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, SELECT);
    GPIOPinWrite(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN, DATA);
    //spiData->transaction.txBuf = &data_value;
    spiData->transaction.txBuf = &data;
    sendSPI(spiData);
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, UNSELECT);
}

void OLED_C_data16(uint16_t data_value, SPIdata *spiData){
//Write data to register

    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, SELECT);
    GPIOPinWrite(GPIO_PORTM_BASE, DATA_OR_COMMAND_PIN, DATA);
    //spiData->transaction.txBuf = &data_value;
    spiData->transaction.txBuf = &data_value;
    sendSPI(spiData);
    GPIOPinWrite(GPIO_PORTH_BASE, CHIP_SELECT_PIN, UNSELECT);
}
void OLED_CLS(SPIdata *spiData)
{
    int j;

    OLED_C_command(0x1D,0x02, spiData);                                                  // Set Memory Read/Write mode
    OLED_C_MemorySize(0x00,0x5F,0x00,0x5F, spiData);                                     // whole screen
    DDRAM_access(spiData);
    for(j = 0 ; j < 9216 ; j++) // black box   96x96=9216
    {
        //OLED_C_Color(0x0,0x0,spiData);                                           // black
        OLED_C_Color16(0, spiData);
    }
    display_x = 0;
    display_y = 0;
    new_y = 0;
}

void OLED_OUT_displayMap(SPIdata *spiData)
{
    int16_t x,y; //display coords

        OLED_C_command(0x1D,0x02, spiData);                                                  // Set Memory Read/Write mode
        OLED_C_MemorySize(0x00,0x5F,0x00,0x5F, spiData);                                     // whole screen
        DDRAM_access(spiData);
        for(y = 95; y >= 0 ; y--){
            for(x = 95; x >= 0; x--)
            {
                if(testPixelinMap(x,y+g_startY) == 0)
                    //OLED_C_Color(g_background_color >> 8,g_background_color & 0xff,spiData); // draw pixel in bg-color
                    OLED_C_Color16(g_background_color,spiData);
                else
                    //OLED_C_Color(display_color >> 8,display_color & 0xff,spiData);  //draw pixel in fg-color
                    OLED_C_Color16(display_color,spiData);
            }
        }
}

//Select color
void OLED_C_Color16(uint16_t color, SPIdata *spiData){
//void OLED_C_Color(char colorMSB, char colorLSB, SPIdata *spiData){

    OLED_C_data16(color, spiData);
    //OLED_C_data(colorLSB, spiData);
}

void OLED_C_Color(char colorMSB, char colorLSB, SPIdata *spiData){

    OLED_C_data(colorMSB, spiData);
    OLED_C_data(colorLSB, spiData);
}

void DrawPixel(char px, char py, uint16_t color, SPIdata *spiData){
char swapped_x;

    if(px <96){                                                                 // check on screen
        swapped_x = 95 - px;                                                    // move x coordinate left/right
    }
    else{
        swapped_x = 95;                                                         // left side of screen
    }
    OLED_C_MemorySize( swapped_x,  swapped_x,  py,  py, spiData);                        // set display position
    DDRAM_access(spiData);                                                             // select data memory on display
    OLED_C_Color(color >> 8,color & 0xff, spiData);                                      // send color
}

void DrawLineH(char px1, char px2, char py, uint16_t color, SPIdata *spiData){                    // draw a horizontal line, 0,0 is top left
char index;

    for(index = px1 ; index <= px2 ; index++){
        DrawPixel(index, py, color, spiData);
    }
}
void DrawLineV(char px, char py1, char py2, uint16_t color, SPIdata *spiData){                    // draw a vertical line, 0,0 is top left
char index;

    for(index = py1 ; index <= py2; index++){
        DrawPixel(px, index, color, spiData);
    }
}

void OLED_C_Out(unsigned char x_co_ord, unsigned char y_co_ord, unsigned char ascii_in, uint16_t color,
                const uint8_t font[], SPIdata *spiData){
signed char row;                                                                // character row
unsigned char col;                                                              // character column
unsigned long font_row;                                                         // data in current row
unsigned long font_pointer;                                                     // table of addresses 1 byte ??, 2 bytes offset, 1 byte ??
unsigned long font_offset;
unsigned char ascii_off;



// see Microchip AN1182 for font coding...
// 0x00 word -  bits 13,12 orient - 11,10 bits per pixel  -  8 extended glyph - 7 to 0 font ID - rest reserved
// 0x02 word - first character
// 0x04 word - last character
// 0x06 word - height
// glyph entries follow - 4 bytes (12 bytes extended glyph) -  1 byte Glyph width - 3 bytes offset
// orient 00=normal, 01=270deg, 10=180deg, 11=90deg
// bits per pixel 00=1, 01=2 (anti alias), 1x reserved
// extended glyph 0=normal, 1=extended

        font_nrows = font[6];                               // number of bytes in each entry

        ascii_off = ascii_in & 0x7f;                                            // map high values to low ones
        if(ascii_off >= 32){                                                    // always assume character 0x20 space as first character
            ascii_off -= 32;                                                    // remap
        }
        for(row = font_nrows; row > 0; row--){                                  // read pixels right to left
            font_pointer = (ascii_off*4)+8;                                     // skip 8 byte header
            last_width = font[font_pointer];                // read out character width (excluding filler bytes)
            font_offset = font[font_pointer+1];             // get offset low
            font_offset += font[font_pointer+2]*256;        // get offset middle
            font_offset += (unsigned long)font[font_pointer+3]*65536;  // get offset high and force long type calculation
            font_offset += row;                                                 // add in the row offset

            if(last_width > 8){                                                 // if width > 8 then we need 2 bytes per row
                font_offset += row;                                             // so double up byte offset
            }
            font_row = font[font_offset];                   // get a row of up to 8 pixels
            if(last_width > 8){                                                 // if width > 8 then need second byte of pixels
                font_row += (font[font_offset+1])*256;      // so go fetch a further byte
            }
            for(col = 0; col <= last_width ; col++){                            // count through pixels in row
                DDRAM_access(spiData);
                if((font_row >> col)&0x01){
                    //DrawPixel(x_co_ord+col,y_co_ord+row,color,spiData);                // if bit is set, set it displaymap
                    setPixelinMap(x_co_ord+col,y_co_ord+row);
                }
                else{
                    //DrawPixel(x_co_ord+col,y_co_ord+row,Color_Black,spiData);           // if bit is clear, clear it in displaymap
                    clearPixelinMap(x_co_ord+col,y_co_ord+row);
                }
            }
        }
        if( display_x <= (90-last_width)){                                      // are we still within display?
            display_x += last_width;                                            // if so move x coordinate right
            display_x++;                                                        // inter-character gap
        }
        else{
            display_x = 1;                                                     // set right hand edge
            display_y += font_nrows;
        }
}

void initDisplay(SPIdata *spiData)
{
    /*  Soft reset */
        OLED_C_command(SEPS114A_SOFT_RESET,0x00,spiData);                                   // reg 0x01 data IDX=0
        /* Standby ON/OFF*/
        OLED_C_command(SEPS114A_STANDBY_ON_OFF,0x01,spiData);                               // reg 0x14 data 0x01 Standby on
        Task_sleep(5);  //5ms                                                              // Wait for 5ms (1ms Delay Minimum)
        OLED_C_command(SEPS114A_STANDBY_ON_OFF,0x00,spiData);                               // Standby off   (start display oscillator)
        Task_sleep(5);  //5ms     // 1ms Delay Minimum (1ms Delay Minimum)
        /* Display OFF */
        OLED_C_command(SEPS114A_DISPLAY_ON_OFF,0x00,spiData);
        /* Set Oscillator operation */
        OLED_C_command(SEPS114A_ANALOG_CONTROL,0x00,spiData);                               // using external resistor and internal OSC
        /* Set frame rate */
        OLED_C_command(SEPS114A_OSC_ADJUST,0x03,spiData);                                   // frame rate : 95Hz
        /* Set active display area of panel */
        OLED_C_command(SEPS114A_DISPLAY_X1,0x00,spiData);
        OLED_C_command(SEPS114A_DISPLAY_X2,0x5F,spiData);                                   // 96 pixels wide
        OLED_C_command(SEPS114A_DISPLAY_Y1,0x00,spiData);
        OLED_C_command(SEPS114A_DISPLAY_Y2,0x5F,spiData);                                   // 96 pixels high
        /* Select the RGB data format and set the initial state of RGB interface port */
        OLED_C_command(SEPS114A_RGB_IF,0x00,spiData);                                       // RGB 8bit interface
        //OLED_C_command(SEPS114A_RGB_IF,0x01,spiData);                                       // RGB 16bit interface
        /* Set RGB polarity */
        OLED_C_command(SEPS114A_RGB_POL,0x00,spiData);
        /* Set display mode control */
        OLED_C_command(SEPS114A_DISPLAY_MODE_CONTROL,0x80,spiData);                         // SWAP:BGR, Reduce current : Normal, DC[1:0] : Normal
        /* Set MCU Interface */
        OLED_C_command(SEPS114A_CPU_IF,0x00,spiData);                                       // MPU External interface mode, 8bits
        /* Set Memory Read/Write mode */
        OLED_C_command(SEPS114A_MEMORY_WRITE_READ,0x00,spiData);
        /* Set row scan direction */
        OLED_C_command(SEPS114A_ROW_SCAN_DIRECTION,0x00,spiData);                           // Column : 0 > Max, Row : 0 > Max
        /* Set row scan mode */
        OLED_C_command(SEPS114A_ROW_SCAN_MODE,0x00,spiData);                                // Alternate scan mode
        /* Set column current */
        OLED_C_command(SEPS114A_COLUMN_CURRENT_R,0x6E,spiData);
        OLED_C_command(SEPS114A_COLUMN_CURRENT_G,0x4F,spiData);
        OLED_C_command(SEPS114A_COLUMN_CURRENT_B,0x77,spiData);
        /* Set row overlap */
        OLED_C_command(SEPS114A_ROW_OVERLAP,0x00,spiData);                                  // Band gap only
        /* Set discharge time */
        OLED_C_command(SEPS114A_DISCHARGE_TIME,0x01,spiData);                               // Discharge time : normal discharge
        /* Set peak pulse delay */
        OLED_C_command(SEPS114A_PEAK_PULSE_DELAY,0x00,spiData);
        /* Set peak pulse width */
        OLED_C_command(SEPS114A_PEAK_PULSE_WIDTH_R,0x02,spiData);
        OLED_C_command(SEPS114A_PEAK_PULSE_WIDTH_G,0x02,spiData);
        OLED_C_command(SEPS114A_PEAK_PULSE_WIDTH_B,0x02,spiData);
        /* Set precharge current */
        OLED_C_command(SEPS114A_PRECHARGE_CURRENT_R,0x14,spiData);
        OLED_C_command(SEPS114A_PRECHARGE_CURRENT_G,0x50,spiData);
        OLED_C_command(SEPS114A_PRECHARGE_CURRENT_B,0x19,spiData);
        /* Set row scan on/off  */
        OLED_C_command(SEPS114A_ROW_SCAN_ON_OFF,0x00,spiData);                              // Normal row scan
        /* Set scan off level */
        OLED_C_command(SEPS114A_SCAN_OFF_LEVEL,0x04,spiData);                               // VCC_C*0.75
        /* Set memory access point */
        OLED_C_command(SEPS114A_DISPLAYSTART_X,0x00,spiData);
        OLED_C_command(SEPS114A_DISPLAYSTART_Y,0x00,spiData);
        /* Display ON */
        OLED_C_command(SEPS114A_DISPLAY_ON_OFF,0x01,spiData);          // reg 0x02 display on
        OLED_CLS(spiData);
}
void OLED_Out(char x_pos, char y_pos, const char *text, SPIdata *spiData){                              // output text to OLED using current font and colour
char ochar;
unsigned int opoint = 0;

    display_x = x_pos;
    display_y = y_pos;
    do{
        ochar = text[opoint];                                                   // get next chc in string
        if(ochar){                                                              // do not process null terminator
            OLED_C_Out(display_x,display_y,ochar,display_color,g_fonts[g_font], spiData);
            opoint++;                                                           // go for next chc
        }
    }while(ochar);                                                              // loop until null terminator found

    new_y = display_y + font_nrows;
    if(new_y > 95)
    {
        g_rowsFilled = new_y;
    }
    if (new_y > (MAXROWS - font_nrows)) new_y = 0;  //text can be longer than max. display size
}

void setPixelinMap(int16_t x, int16_t y)
{

    g_displaymap[y][x/32] |= 1 << (x%32);
}

void clearPixelinMap(int16_t x, int16_t y)
{
    g_displaymap[y][x/32]  &= ~(1 << (x%32));
}

uint8_t testPixelinMap(int16_t x, int16_t y)
{
    return ( (g_displaymap[y][x/32] & (1 << (x%32) )) != 0 );
}
