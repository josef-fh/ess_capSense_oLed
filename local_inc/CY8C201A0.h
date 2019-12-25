/*
 * CY8C201A0.h
 *
 *  Created on: 15 Dec 2019
 *      Author: JoRam
 */

#ifndef LOCAL_INC_CY8C201A0_H_
#define LOCAL_INC_CY8C201A0_H_

// registers
#define     INPUT_PORT0 0x00
#define     INPUT_PORT1 0x01
#define     STATUS_POR0 0x02
#define     STATUS_POR1 0x03
#define     OUTPUT_PORT0 0x04
#define     OUTPUT_PORT1 0x05
#define     CS_ENABL0 0x06
#define     CS_ENABL1 0x07
#define     GPIO_ENABLE0 0x08
#define     GPIO_ENABLE1 0x09
#define     INVERSION_MASK0 0x0A
#define     INVERSION_MASK1 0x0B
#define     INT_MASK0 0x0C
#define     INT_MASK1 0x0D
#define     STATUS_HOLD_MSK0 0x0E
#define     STATUS_HOLD_MSK1 0x0F
#define     DM_PULL_UP0 0x10
#define     DM_STRONG0 0x11
#define     DM_HIGHZ0 0x12
#define     DM_OD_LOW0 0x13
#define     DM_PULL_UP1 0x14
#define     DM_STRONG1 0x15
#define     DM_HIGHZ1 0x16
#define     DM_OD_LOW1 0x17
#define     OP_SEL_00 0x1C
#define     OPR1_PRT0_00 0x1D
#define     OPR1_PRT1_00 0x1E
#define     OPR2_PRT0_00 0x1F
#define     OPR2_PRT1_00 0x20
#define     OP_SEL_01 0x21
#define     OPR1_PRT0_01 0x22
#define     OPR1_PRT1_01 0x23
#define     OPR2_PRT0_01 0x24
#define     OPR2_PRT1_01 0x25
#define     OP_SEL_02 0x26
#define     OPR1_PRT0_02 0x27
#define     OPR1_PRT1_02 0x28
#define     OPR2_PRT0_02 0x29
#define     OPR2_PRT1_02 0x2A
#define     OP_SEL_03 0x2B
#define     OPR1_PRT0_03 0x2C
#define     OPR1_PRT1_03 0x2D
#define     OPR2_PRT0_03 0x2E
#define     OPR2_PRT1_03 0x2F
#define     OP_SEL_04 0x30
#define     OPR1_PRT0_04 0x31
#define     OPR1_PRT1_04 0x32
#define     OPR2_PRT0_04 0x33
#define     OPR2_PRT1_04 0x34
#define     OP_SEL_10 0x35
#define     OPR1_PRT0_10 0x36
#define     OPR1_PRT1_10 0x37
#define     OPR2_PRT0_10 0x38
#define     OPR2_PRT1_10 0x39
#define     OP_SEL_11 0x3A
#define     OPR1_PRT0_11 0x3B
#define     OPR1_PRT1_11 0x3C
#define     OPR2_PRT0_11 0x3D
#define     OPR2_PRT1_11 0x3E
#define     OP_SEL_12 0x3F
#define     OPR1_PRT0_12 0x40
#define     OPR1_PRT1_12 0x41
#define     OPR2_PRT0_12 0x42
#define     OPR2_PRT1_12 0x43
#define     OP_SEL_13 0x44
#define     OPR1_PRT0_13 0x45
#define     OPR1_PRT1_13 0x46
#define     OPR2_PRT0_13 0x47
#define     OPR2_PRT1_13 0x48
#define     OP_SEL_14 0x49
#define     OPR1_PRT0_14 0x4A
#define     OPR1_PRT1_14 0x4B
#define     OPR2_PRT0_14 0x4C
#define     OPR2_PRT1_14 0x4D
#define     CS_NOISE_TH 0x4E
#define     CS_BL_UPD_TH 0x4F
#define     CS_SETL_TIME 0x50
#define     CS_OTH_SET 0x51
#define     CS_HYSTERISIS 0x52
#define     CS_DEBOUNCE 0x53
#define     CS_NEG_NOISE_TH 0x54
#define     CS_LOW_BL_RST 0x55
#define     CS_FILTERING 0x56
#define     CS_SCAN_POS_00 0x57
#define     CS_SCAN_POS_01 0x58
#define     CS_SCAN_POS_02 0x59
#define     CS_SCAN_POS_03 0x5A
#define     CS_SCAN_POS_04 0x5B
#define     CS_SCAN_POS_10 0x5C
#define     CS_SCAN_POS_11 0x5D
#define     CS_SCAN_POS_12 0x5E
#define     CS_SCAN_POS_13 0x5F
#define     CS_SCAN_POS_14 0x60
#define     CS_FINGER_TH_00 0x61
#define     CS_FINGER_TH_01 0x62
#define     CS_FINGER_TH_02 0x63
#define     CS_FINGER_TH_03 0x64
#define     CS_FINGER_TH_04 0x65
#define     CS_FINGER_TH_10 0x66
#define     CS_FINGER_TH_11 0x67
#define     CS_FINGER_TH_12 0x68
#define     CS_FINGER_TH_13 0x69
#define     CS_FINGER_TH_14 0x6A
#define     CS_IDAC_00 0x6B
#define     CS_IDAC_01 0x6C
#define     CS_IDAC_02 0x6D
#define     CS_IDAC_03 0x6E
#define     CS_IDAC_04 0x6F
#define     CS_IDAC_10 0x70
#define     CS_IDAC_11 0x71
#define     CS_IDAC_12 0x72
#define     CS_IDAC_13 0x73
#define     CS_IDAC_14 0x74
#define     CS_SLID_CONFIG 0x75
#define     CS_SLID_MULM 0x77
#define     CS_SLID_MULL 0x78
#define     I2C_ADDR_LOCK 0x79
#define     DEVICE_ID 0x7A
#define     DEVICE_STATUS 0x7B
#define     I2C_ADDR_DM 0x7C
#define     SLEEP_PIN 0x7E
#define     SLEEP_CTRL 0x7F
#define     SLEEP_SA_CNTR 0x80
#define     CS_READ_BUTTON 0x81
#define     CS_READ_BLM 0x82
#define     CS_READ_BLL 0x83
#define     CS_READ_DIFFM 0x84
#define     CS_READ_DIFFL 0x85
#define     CS_READ_RAWM 0x86
#define     CS_READ_RAWL 0x87
#define     CS_READ_STATUSM 0x88
#define     CS_READ_STATUS0 0x88
#define     CS_READ_STATUSL 0x89
#define     CS_READ_STATUS1 0x89
#define     CS_READ_CEN_POSM 0x8A
#define     CS_READ_CEN_POSL 0x8B
#define     CS_READ_CEN_PEAKM 0x8C
#define     CS_READ_CEN_PEAKL 0x8D
#define     COMMAND_REG 0xA0



#endif /* LOCAL_INC_CY8C201A0_H_ */
