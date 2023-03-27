#ifndef __CMT2300A_PARAMS_X_SENSE_EN_H
#define __CMT2300A_PARAMS_X_SENSE_EN_H

#include "2300a_def.h"
/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.10.28 17:43
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 868.650 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +20 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = On
; Tx Duty-Cycle             = Off
; Sleep Timer               = On
; Sleep Time                = 5000.00000 ms
; Rx Timer                  = On
; Rx Time T1                = 2.00 ms
; Rx Time T2                = 5200.00 ms
; Rx Exit State             = SLEEP
; Tx Exit State             = STBY
; SLP Mode                  = Mode 13
; RSSI Valid Source         = PJD
; PJD Window                = 6 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 6300
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 4-byte
; Sync Value                = 3066209236
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 15
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = At SYNC_OK
; RSSI Filter Setting       = 8-tap
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 26
; RSSI Offset Sign          = 1

;---------------------------------------
;  The following are the Register contents
;---------------------------------------

*/
const uint8_t Hub_CMT_Bank_LowPower[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x91,
	0x02,
	0x02,
	0xD0,
};

const uint8_t Hub_System_Bank_LowPower[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xFA,
	0x34,
	0xE2,
	0x46,
	0x32,
	0x00,
	0xEF,
	0x76,
	0x2D,
	0x12,
	0x41,
};

const uint8_t Hub_Frequency_Bank_LowPower[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x9C,
	0x74,
	0x8D,
	0x42,
	0x91,
	0x1B,
	0x1D,
};
	
const uint8_t Hub_Data_Rate_Bank_LowPower[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};
	
const uint8_t Hub_Baseband_Bank_LowPower[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x9C,
	0x18,
	0xAA,
	0x06,
	0x00,
	0x00,
	0x00,
	0x00,
	0xD4,
	0xA3,
	0xC2,
	0xB6,
	0x00,
	0x0E,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t Hub_TX_Bank_LowPower[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x8A,//Tx Power 20db
	0x18,
	0x3F,
	0x7F,
};

/*
---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.10.28 17:08
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 868.650 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +20 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 6 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 4
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 4-byte
; Sync Value                = 3066209236
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 15
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = At SYNC_OK
; RSSI Filter Setting       = 8-tap
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 26
; RSSI Offset Sign          = 1

;---------------------------------------
;  The following are the Register contents
;---------------------------------------
*/

const uint8_t Hub_CMT_Bank_Normal[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x91,
	0x02,
	0x02,
	0xD0,
};

const uint8_t Hub_System_Bank_Normal[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x12,
	0x41,
};

const uint8_t Hub_Frequency_Bank_Normal[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x9C,
	0x74,
	0x8D,
	0x42,
	0x91,
	0x1B,
	0x1D,
};
	
const uint8_t Hub_Data_Rate_Bank_Normal[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};
	
const uint8_t Hub_Baseband_Bank_Normal[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x04,
	0x00,
	0xAA,
	0x06,
	0x00,
	0x00,
	0x00,
	0x00,
	0xD4,
	0xA3,
	0xC2,
	0xB6,
	0x00,
	0x0E,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t Hub_TX_Bank_Normal[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x8A,
	0x18,
	0x3F,
	0x7F,
};

/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.01.17 10:38
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 869.275 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +18 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = On
; Tx Duty-Cycle             = Off
; Sleep Timer               = On
; Sleep Time                = 5000.00000 ms
; Rx Timer                  = On
; Rx Time T1                = 2.00 ms
; Rx Time T2                = 5200.00 ms
; Rx Exit State             = SLEEP
; Tx Exit State             = STBY
; SLP Mode                  = Mode 13
; RSSI Valid Source         = PJD
; PJD Window                = 4 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 6300
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte 
; Sync Value                = 10826
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 6
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 26
; RSSI Offset Sign          = 1

;---------------------------------------
;  The following are the Register contents
;---------------------------------------

*/

const uint8_t Intranet_CMT_Bank_LowPower[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x91,
	0x02,
	0x02,
	0xD0,
};

const uint8_t Intranet_System_Bank_LowPower[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xFA,
	0x34,
	0xE2,
	0x46,
	0x32,
	0x00,
	0xEF,
	0x76,
	0x2D,
	0x00,
	0x01,
};

const uint8_t Intranet_Frequency_Bank_LowPower[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x89,
	0x39,
	0x8E,
	0x42,
	0x7E,
	0xE0,
	0x1D,
};

const uint8_t Intranet_Data_Rate_Bank_LowPower[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};

const uint8_t Intranet_Baseband_Bank_LowPower[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x9C,
	0x18,
	0xAA,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x4A,
	0x2A,
	0x00,
	0x05,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t Intranet_TX_Bank_LowPower[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x58,
	0x0F,
	0x3F,
	0x7F,
};

const uint8_t Intranet_CMT_Bank_Normal[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x91,
	0x02,
	0x02,
	0xD0,
};

const uint8_t Intranet_System_Bank_Normal[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x00,
	0x01,
};

const uint8_t Intranet_Frequency_Bank_Normal[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x89,
	0x39,
	0x8E,
	0x42,
	0x7E,
	0xE0,
	0x1D,
};

const uint8_t Intranet_Data_Rate_Bank_Normal[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};

const uint8_t Intranet_Baseband_Bank_Normal[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x04,
	0x00,
	0xAA,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x4C,
	0x2C,
	0x00,
	0x05,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t Intranet_TX_Bank_Normal[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x58,
	0x0F,
	0x3F,
	0x7F,
};

/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2021.12.09 17:26
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 869.275 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +18 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 6 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 4
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte 
; Sync Value                = 10826
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 4
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 26
; RSSI Offset Sign          = 1

;---------------------------------------
;  The following are the Register contents
;---------------------------------------
*/
const uint8_t Intranet_CMT_Bank_Normal_Specail[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x91,
	0x02,
	0x02,
	0xD0,
	};

const uint8_t Intranet_System_Bank_Normal_Specail[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x00,
	0x01,
};

const uint8_t Intranet_Frequency_Bank_Normal_Specail[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x89,
	0x39,
	0x8E,
	0x42,
	0x7E,
	0xE0,
	0x1D,
};

const uint8_t Intranet_Data_Rate_Bank_Normal_Specail[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};

const uint8_t Intranet_Baseband_Bank_Normal_Specail[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x04,
	0x00,
	0xAA,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x4A,
	0x2A,
	0x00,
	0x05,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t Intranet_TX_Bank_Normal_Specail[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x58,
	0x0F,
	0x3F,
	0x7F,
};

/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.03.24 10:19
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 865.500 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 13 dBm
; Tx Power                  = +10 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 8 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 4
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte 
; Sync Value                = 11083
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 6
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 26
; RSSI Offset Sign          = 1

;---------------------------------------
;  The following are the Register contents
;---------------------------------------

*/
const uint8_t Factory_Pair_CMT_Bank[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1C,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x91,
	0x02,
	0x02,
	0xD0,
};

const uint8_t Factory_Pair_System_Bank[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x00,
	0x81,
};

const uint8_t Factory_Pair_Frequency_Bank[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x1E,
	0x94,
	0x89,
	0x42,
	0x13,
	0x3B,
	0x19,
};

const uint8_t Factory_Pair_Data_Rate_Bank[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};

const uint8_t Factory_Pair_Baseband_Bank[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x04,
	0x00,
	0xAA,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x4B,
	0x2B,
	0x00,
	0x05,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t Factory_Pair_TX_Bank[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x2A,
	0x05,
	0x3F,
	0x7F,
};

/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.08.05 19:35
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 867.650 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +18 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 8 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 8
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte
; Sync Value                = 11732
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 32
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 0
; RSSI Offset Sign          = 0

;---------------------------------------
;  The following are the Register contents
;---------------------------------------
*/


const uint8_t FSK_Test_CMT_Bank[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x11,
	0x02,
	0x02,
	0x00,
};

const uint8_t FSK_Test_System_Bank[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x00,
	0x81,
};

const uint8_t FSK_Test_Frequency_Bank[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x89,
	0x39,
	0x8C,
	0x42,
	0x7E,
	0xE0,
	0x1B,
};

const uint8_t FSK_Test_Data_Rate_Bank[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0x10,
	0x33,
	0xD2,
	0x69,
	0x0C,
	0x0A,
	0x9F,
	0x39,
	0x29,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x53,
	0x08,
	0x00,
	0xB4,
	0x00,
	0x00,
	0x01,
	0x00,
	0x00,
};

const uint8_t FSK_Test_Baseband_Bank[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x08,
	0x00,
	0xAA,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xD4,
	0x2D,
	0x00,
	0x1F,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x00,
	0x00,
	0x1F,
	0x10,
};

const uint8_t FSK_Test_TX_Bank[CMT2300A_TX_BANK_SIZE]={
	0x50,
	0x4D,
	0x06,
	0x00,
	0x0F,
	0x70,
	0x00,
	0x58,
	0x0F,
	0x3F,
	0x7F,
};

/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.08.05 19:36
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 867.650 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = OOK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = NA
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +18 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; Demod Method              = Middle
; Long-Zero Number          = 31
; CDR Type                  = Counting
; CDR DR Range              = NA
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 8 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 1
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 8
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte
; Sync Value                = 11732
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 32
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 0
; RSSI Offset Sign          = 0

;---------------------------------------
;  The following are the Register contents
;---------------------------------------

*/
const uint8_t OOK_Test_CMT_Bank[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0x70,
	0x80,
	0x14,
	0x08,
	0x11,
	0x02,
	0x02,
	0x00,
};

const uint8_t OOK_Test_System_Bank[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x00,
	0x81,
};

const uint8_t OOK_Test_Frequency_Bank[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x89,
	0x39,
	0x8C,
	0x42,
	0x7E,
	0xE0,
	0x1B,
};

const uint8_t OOK_Test_Data_Rate_Bank[CMT2300A_DATA_RATE_BANK_SIZE]={
	0xD3,
	0x64,
	0xA0,
	0x99,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x29,
	0xC0,
	0x28,
	0x0A,
	0x5B,
	0x07,
	0x00,
	0x50,
	0x2D,
	0x00,
	0x01,
	0x05,
	0x05,
};

const uint8_t OOK_Test_Baseband_Bank[CMT2300A_BASEBAND_BANK_SIZE]={
	0x12,
	0x08,
	0x00,
	0xAA,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xD4,
	0x2D,
	0x00,
	0x1F,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x60,
	0xFF,
	0x01,
	0x00,
	0x1F,
	0x10,
};

const uint8_t OOK_Test_TX_Bank[CMT2300A_TX_BANK_SIZE]={
	0x51,
	0x93,
	0x01,
	0x00,
	0x03,
	0x70,
	0x00,
	0x58,
	0x0F,
	0x3F,
	0x7F,
};

/*
;---------------------------------------
;  CMT2300A Configuration File
;  Generated by CMOSTEK RFPDK 1.46
;  2020.08.05 19:38
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 867.650 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 10.0 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +18 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 8 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 8
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte
; Sync Value                = 11732
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 32
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 0
; RSSI Offset Sign          = 0

;---------------------------------------
;  The following are the Register contents
;---------------------------------------

*/
const uint8_t RX_Test_CMT_Bank[CMT2300A_CMT_BANK_SIZE]={
	0x00,
	0x66,
	0xEC,
	0x1D,
	0xF0,
	0x80,
	0x14,
	0x08,
	0x11,
	0x02,
	0x02,
	0x00,
};

const uint8_t RX_Test_System_Bank[CMT2300A_SYSTEM_BANK_SIZE]={
	0xAE,
	0xE0,
	0x35,
	0x00,
	0x00,
	0xF4,
	0x10,
	0xE2,
	0x42,
	0x20,
	0x00,
	0x81,
};

const uint8_t RX_Test_Frequency_Bank[CMT2300A_FREQUENCY_BANK_SIZE]={
	0x42,
	0x89,
	0x39,
	0x8C,
	0x42,
	0x7E,
	0xE0,
	0x1B,
};

const uint8_t RX_Test_Data_Rate_Bank[CMT2300A_DATA_RATE_BANK_SIZE]={
  0xD3,
  0x64,
  0x10,
  0x33,
  0xD2,
  0x69,
  0x0C,
  0x0A,
  0x9F,
  0x39,
  0x29,
  0x29,
  0xC0,
  0x28,
  0x0A,
  0x53,
  0x08,
  0x00,
  0xB4,
  0x00,
  0x00,
  0x01,
  0x00,
  0x00,
};

const uint8_t RX_Test_Baseband_Bank[CMT2300A_BASEBAND_BANK_SIZE]={
  0x12,
  0x08,
  0x00,
  0xAA,
  0x02,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0xD4,
  0x2D,
  0x00,
  0x1F,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x60,
  0xFF,
  0x00,
  0x00,
  0x1F,
  0x10,
};

const uint8_t RX_Test_TX_Bank[CMT2300A_TX_BANK_SIZE]={
  0x50,
  0x4D,
  0x06,
  0x00,
  0x0F,
  0x70,
  0x00,
  0x58,
  0x0F,
  0x3F,
  0x7F,
};

#endif

