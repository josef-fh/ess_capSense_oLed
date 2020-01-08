#ifndef SPI_TASK_H_
#define SPI_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <ti/drivers/SPI.h>
#include <ti/sysbios/knl/Event.h>
#include "Broker_Task.h"

// Communication
//RESET
#define TARGET_IS_TM4C129_RA2
#define RESET 0x00
#define SET 0x80
//R/W
#define READ 0x00
#define WRITE 0x10
//CS
#define SELECT 0x00
#define UNSELECT 0x04
//D/S
#define COMMAND 0x00
#define DATA 0x08

// Pin Configuration
#define RESET_PIN 0x00000080            //Output        Output          PC7 PP4         RST(X6)                 RSTB
#define SPI_READ_PIN 0x00000001         //SSI2XDAT1     SSI3XDAT1       PD0 PQ3         SDO(X9)                 D1          MPU RX
#define SPI_WRITE_PIN 0x00000002        //SSI2XDAT0     SSI3XDAT0       PD1 PQ2         SDI(X8)                 D2          MPU TX
#define CLOCK_PIN 0x00000008            //SSI2Clk       SSI3Clk         PD3 PQ0         SCK(X8)                 D0          Clock In
#define FSS 0x00000004                  //SSI2Fss       SSI3Fss         PD2 PQ1         -                       -           -
#define READ_OR_WRITE_PIN 0x00000010    //Output        Output          PE4 PD2         R/W(X8)                 D3          Low
#define CHIP_SELECT_PIN 0x00000004      //Output        Output          PH2 PP5         CS(X9)                  CSB         Low = Communication, High = Idle
#define DATA_OR_COMMAND_PIN 0x00000008

#define SEPS114A_SOFT_RESET 0x01
#define SEPS114A_DISPLAY_ON_OFF 0x02
#define SEPS114A_ANALOG_CONTROL 0x0F
#define SEPS114A_STANDBY_ON_OFF 0x14
#define SEPS114A_OSC_ADJUST 0x1A
#define SEPS114A_ROW_SCAN_DIRECTION 0x09
#define SEPS114A_DISPLAY_X1 0x30
#define SEPS114A_DISPLAY_X2 0x31
#define SEPS114A_DISPLAY_Y1 0x32
#define SEPS114A_DISPLAY_Y2 0x33
#define SEPS114A_DISPLAYSTART_X 0x38
#define SEPS114A_DISPLAYSTART_Y 0x39
#define SEPS114A_CPU_IF 0x0D
#define SEPS114A_MEM_X1 0x34
#define SEPS114A_MEM_X2 0x35
#define SEPS114A_MEM_Y1 0x36
#define SEPS114A_MEM_Y2 0x37
#define SEPS114A_MEMORY_WRITE_READ 0x1D
#define SEPS114A_DDRAM_DATA_ACCESS_PORT 0x08
#define SEPS114A_DISCHARGE_TIME 0x18
#define SEPS114A_PEAK_PULSE_DELAY 0x16
#define SEPS114A_PEAK_PULSE_WIDTH_R 0x3A
#define SEPS114A_PEAK_PULSE_WIDTH_G 0x3B
#define SEPS114A_PEAK_PULSE_WIDTH_B 0x3C
#define SEPS114A_PRECHARGE_CURRENT_R 0x3D
#define SEPS114A_PRECHARGE_CURRENT_G 0x3E
#define SEPS114A_PRECHARGE_CURRENT_B 0x3F
#define SEPS114A_COLUMN_CURRENT_R 0x40
#define SEPS114A_COLUMN_CURRENT_G 0x41
#define SEPS114A_COLUMN_CURRENT_B 0x42
#define SEPS114A_ROW_OVERLAP 0x48
#define SEPS114A_SCAN_OFF_LEVEL 0x49
#define SEPS114A_ROW_SCAN_ON_OFF 0x17
#define SEPS114A_ROW_SCAN_MODE 0x13
#define SEPS114A_SCREEN_SAVER_CONTEROL 0xD0
#define SEPS114A_SS_SLEEP_TIMER 0xD1
#define SEPS114A_SCREEN_SAVER_MODE 0xD2
#define SEPS114A_SS_UPDATE_TIMER 0xD3
#define SEPS114A_RGB_IF 0xE0
#define SEPS114A_RGB_POL 0xE1
#define SEPS114A_DISPLAY_MODE_CONTROL 0xE5


// RGB 565 colours       bits 15to0 are r4 r3 r2 r1 r0 g5 g4 g3 g2 g1 g0 b4 b3 b2 b1 b0
#define Color_Black           0x0000      /*   0,   0,   0 */
#define Color_Navy            0x000F      /*   0,   0, 128 */
#define Color_DarkGreen       0x03E0      /*   0, 128,   0 */
#define Color_DarkCyan        0x03EF      /*   0, 128, 128 */
#define Color_Maroon          0x7800      /* 128,   0,   0 */
#define Color_Purple          0x780F      /* 128,   0, 128 */
#define Color_Olive           0x7BE0      /* 128, 128,   0 */
#define Color_LightGrey       0xC618      /* 192, 192, 192 */
#define Color_DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Color_Blue            0x001F      /*   0,   0, 255 */
#define Color_Green           0x07E0      /*   0, 255,   0 */
#define Color_Cyan            0x07FF      /*   0, 255, 255 */
#define Color_Red             0xF800      /* 255,   0,   0 */
#define Color_Magenta         0xF81F      /* 255,   0, 255 */
#define Color_Mustard         0xff00
#define Color_Yellow          0xFFE0      /* 255, 255,   0 */
#define Color_White           0xFFFF      /* 255, 255, 255 */
#define Color_Orange          0xFD20      /* 255, 165,   0 */
#define Color_GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Color_Pink            0xFB1B
typedef struct SPI_data
{
    SPI_Transaction transaction;
    SPI_Handle handle;
}SPIdata;

typedef struct event_data {
    Event_Handle event;
    UInt num;
}Event_data;

typedef struct {
    SPIdata *spiData;
    Event_data *eventData;
    mailbox_descriptor *mailbox_des;
    uint32_t wait_ticks;
    uint32_t ui32SysClock;
}spi_descriptor;



//globals
char g_displaytext[400];

int setup_SPI_Task(int prio, xdc_String name, spi_descriptor *spi_desc);
void TransferFxn(UArg arg0, UArg arg1);
void OLED_C_data(unsigned char data_value, SPIdata *);
//void OLED_C_command(unsigned char reg_index, unsigned char reg_value, SPIdata *spidata);
void OLED_C_command(uint16_t reg_index, uint16_t reg_value, SPIdata *spiData);
void sendSPI(SPIdata *spiData);
void DDRAM_access(SPIdata *spiData);
void OLED_C_Color(char colorMSB, char colorLSB, SPIdata *spiData);
void OLED_C_MemorySize(char X1, char X2, char Y1, char Y2,SPIdata *spiData);
void OLED_CLS(SPIdata *spiData);
void DrawLineV(char px, char py1, char py2, uint16_t color, SPIdata *spiData);
void DrawLineH(char px1, char px2, char py, uint16_t color, SPIdata *spiData);
void DrawPixel(char px, char py, uint16_t color, SPIdata *spiData);
void OLED_C_Out(unsigned char x_co_ord, unsigned char y_co_ord, unsigned char ascii_in, uint16_t color,
                const uint8_t font[], SPIdata *spiData);
void OLED_Out(char x_pos, char y_pos, const char *text, SPIdata *spiData);
#endif
