/************************************************************************/
/*                                                                      */
/*    DDR.c                                                             */
/*                                                                      */
/*    Initializes the DDR memory as part of the CRT _on_reset()         */
/*                                                                      */
/*    This code was modified from the Harmony DDR Init code             */
/*    Originated by Microchip                                           */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    6/12/2018 (KeithV): Created                                       */
/************************************************************************/

// Required retaining of the original Microchip License:
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

// Modified by Digilent, Under the Digilent license
/*******************************************************************************
MIT License

Copyright (c) 2018 Digilent

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/
#include "p32xxxx.h"
#include <inttypes.h>

//**************************************************************************
//*******************  NOTES and ASSUMPTIONS *******************************
//**************************************************************************
//******************** These may be inaccurate *****************************
//******************** They are just what I believe ************************
//**************************************************************************

/*
 * The external MZDA clock runs at 24MHz (that I know)
 * The PLL is 24MHz / 1 * 25 / 3 / 1 = 200MHz
 * We run the DDR at 200MHz.
 * The clock period is 5ns, and since the DDR works on half clock
 * a half clock is 2.5ns
 * However timing to the DDR controller is in ps.
 * The clock period is 5000 ps (5ns) or 2 * 2500ps = 5ns
 * CTRL_CLK_PERIOD == 5000ps
 * 
 * 
 * 
 * Table 55-3: SDRAM Timing Parameters (winbond W925G6JB: 4M x 4 Banks x 16bit DDR2 SDRAM)
 * Look at DDR2-800 5-5-5
 * Note: All fractional results are rounded up to the nearest number of clocks.
 * Parameter    Description                                     Units           ps
 * tRFC         Auto-refresh Cycle Time                         75ns            75000; code set to 127500; could be 75000
 * tWR          Write Recovery Time                             15 ns           15000
 * tRP          Precharge-to-Active Command Delay Time          12.5 ns         12500; 11250
 * tRCD         Active-to-Read/Write Command Delay Time         12.5 ns         12500; 11250
 * tRRD         Row-to-Row (RAS to RAS) Command Delay Time      7.5 ns          7500
 * tWTR         Write-to-Read Command Delay Time                7.5 ns          7500
 * tRTP         Read-to-Precharge Command Delay Time            7.5 ns          7500
 * tDLLK        DLL Lock Delay Time                             nClk
 * tRAS         Active-to-Precharge Minimum Command Delay Time  40ns - 70000ns  40000
 * tRC          Row Cycle Time                                  52.5 ns         52500; 51250
 * tFAW         Four Bank Activation Window                     35 ns           35000
 * tMRD         Mode Register Set Command Cycle Delay           2 nClk
 * tXP          Power-Down Exit Delay                           3 nClk
 * tCKE         Power-Down Minimum Delay                        3 nClk
 * tCKESR       Self-Refresh Minimum Delay                      200 nClk
 * RL           CAS Latency                                     5 nClk            Read latency RL = AL + CL; CL = 3,4,5,6,7, AL = Additive Latency 0,1,2,3,4,5,6
 * tRFI/tREFI   Average Periodic Refresh Interval               7.8 us          7800000
 * WL           Write Latency                                   4 nClk            Write Latency WL = RL - 1
 * BL           Burst Length (in cycles)                        2 nClk
 * 
 * // best I can tell!
 * BL is 4 bytes  => 2 bytes(16 bit bus)/cycle * 2 cycles = 4 bytes
 * CL is 5
 * AL is 0
 * RL = CL + AL = 5 + 0 = 5
 * WL = RL - 1 = 4
 */

/*
 * From the family reference, clearly some are wrong...
tRFC    Auto-refresh Cycle Time                         130ns
tWR     Write Recovery Time                             25ns
tRP     Precharge-to-Active Command Delay Time          20ns
tRCD    Active to Read/Write Command Delay Time         20ns
tRRD    Row-to-Row (RAS to RAS) Command Delay Time      7.5ns
tWTR    Write-to-Read Command Delay Time                15ns
tRTP    Read-to-Precharge Command Delay Time            20ns
tDLLK   DLL Lock Delay Time                             200 Clock cycles
tRAS    Active to Precharge Minimum Command Delay Time  40ns
tRC     Row Cycle Time                                  110ns
tFAW    Four Bank Activation Window                     35ns
tMRD    Mode Register Set Command Cycle Delay           4 Clock cycles
tXP     Power Down Exit Delay                           6 Clock cycles
tCKE    Power Down Minimum Delay                        6 Clock cycles
RL      CAS Latency                                     4 Clock cycles
tRFI    Average Periodic Refresh Interval               7.8us
WL      Write Latency                                   3 Clock cycles
BL      Burst Length (in cycles)                        8 Clock cycles
*/

/* Host Commands */
#define DRV_DDR_IDLE_NOP                0x00FFFFFF
#define DRV_DDR_PRECH_ALL_CMD           0x00FFF401
#define DRV_DDR_REF_CMD                 0x00FFF801
#define DRV_DDR_LOAD_MODE_CMD           0x00FFF001
#define DRV_DDR_CKE_LOW                 0x00FFEFFE

/* DDR address decoding */
#define COL_HI_RSHFT            0
#define COL_HI_MASK             0
#define COL_LO_MASK             ((1 << 9) - 1)

#define BA_RSHFT                9
#define BANK_ADDR_MASK          ((1 << 2) - 1)

#define ROW_ADDR_RSHIFT         (BA_RSHFT + 2)
#define ROW_ADDR_MASK           ((1 << 13) - 1)

//#define CS_ADDR_RSHIFT        (ROW_ADDR_RSHIFT + 13)
#define CS_ADDR_RSHIFT          0
#define CS_ADDR_MASK            0

#define CTRL_CLK_PERIOD         (2500 * 2)

#define round_up(x,y) (((x) + (y) - 1) / (y))
#define sys_mem_ddr_max(a,b) (((a)>(b))?(a):(b))
#define sys_mem_ddr_round_up(x,y) (((x) + (y) - 1) / (y))
#define sys_mem_ddr_hc_clk_dly(dly) (sys_mem_ddr_max((sys_mem_ddr_round_up((dly),2500)),2)-2)

#define DDR_CmdDataWrite(cmdReg, cmdData) *(&DDRCMD10 + cmdReg) = cmdData

typedef enum {

    DEVCON_MPLL_VREF_EXT   /*DOM-IGNORE-BEGIN*/ =  0 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_VREF_VDD   /*DOM-IGNORE-BEGIN*/ =  1 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_VREF_VSS   /*DOM-IGNORE-BEGIN*/ =  2 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_VREF_INT   /*DOM-IGNORE-BEGIN*/ =  3 /*DOM-IGNORE-END*/,

} DEVCON_MPLL_VREF_CONTROL;

typedef enum {

    DEVCON_MPLL_ODIV_1   /*DOM-IGNORE-BEGIN*/ =  1 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_2   /*DOM-IGNORE-BEGIN*/ =  2 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_3   /*DOM-IGNORE-BEGIN*/ =  3 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_4   /*DOM-IGNORE-BEGIN*/ =  4 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_5   /*DOM-IGNORE-BEGIN*/ =  5 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_6   /*DOM-IGNORE-BEGIN*/ =  6 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_7   /*DOM-IGNORE-BEGIN*/ =  7 /*DOM-IGNORE-END*/,

} DEVCON_MPLL_OUTPUT_DIVIDER;

typedef enum {

    DDR_PHY_ODT_75_OHM = 0x00,
    DDR_PHY_ODT_150_OHM = 0x01

} DDR_PHY_ODT;

typedef enum {

    DDR_PHY_DRIVE_STRENGTH_60 = 0x00,
    DDR_PHY_DRIVE_STRENGTH_FULL = 0x01

} DDR_PHY_DRIVE_STRENGTH;

typedef enum {

    DDR_PHY_PREAMBLE_DLY_2_0 = 0x00,
    DDR_PHY_PREAMBLE_DLY_1_5 = 0x01,
    DDR_PHY_PREAMBLE_DLY_1_0 = 0x02

} DDR_PHY_PREAMBLE_DLY;

typedef enum {

    DDR_PHY_SCL_BURST_MODE_4 = 0x00,
    DDR_PHY_SCL_BURST_MODE_8 = 0x01

} DDR_PHY_SCL_BURST_MODE;

typedef enum {

    DDR_PHY_DDR_TYPE_DDR2 = 0x00,
    DDR_PHY_DDR_TYPE_DDR3 = 0x01

} DDR_PHY_DDR_TYPE;

typedef enum {

    DDR_PHY_SCL_DELAY_SINGLE = 0x00,
    DDR_PHY_SCL_DELAY_DOUBLE = 0x01

} DDR_PHY_SCL_DELAY;

typedef enum {

    DDR_HOST_CMD_REG_10 = 0x00,
    DDR_HOST_CMD_REG_11 = 0x01,
    DDR_HOST_CMD_REG_12 = 0x02,
    DDR_HOST_CMD_REG_13 = 0x03,
    DDR_HOST_CMD_REG_14 = 0x04,
    DDR_HOST_CMD_REG_15 = 0x05,
    DDR_HOST_CMD_REG_16 = 0x06,
    DDR_HOST_CMD_REG_17 = 0x07,
    DDR_HOST_CMD_REG_18 = 0x08,
    DDR_HOST_CMD_REG_19 = 0x09,
    DDR_HOST_CMD_REG_110 = 0x0A,
    DDR_HOST_CMD_REG_111 = 0x0B,
    DDR_HOST_CMD_REG_112 = 0x0C,
    DDR_HOST_CMD_REG_113 = 0x0D,
    DDR_HOST_CMD_REG_114 = 0x0E,
    DDR_HOST_CMD_REG_115 = 0x0F,
    DDR_HOST_CMD_REG_20 = 0x10,
    DDR_HOST_CMD_REG_21 = 0x11,
    DDR_HOST_CMD_REG_22 = 0x12,
    DDR_HOST_CMD_REG_23 = 0x13,
    DDR_HOST_CMD_REG_24 = 0x14,
    DDR_HOST_CMD_REG_25 = 0x15,
    DDR_HOST_CMD_REG_26 = 0x16,
    DDR_HOST_CMD_REG_27 = 0x17,
    DDR_HOST_CMD_REG_28 = 0x18,
    DDR_HOST_CMD_REG_29 = 0x19,
    DDR_HOST_CMD_REG_210 = 0x1A,
    DDR_HOST_CMD_REG_211 = 0x1B,
    DDR_HOST_CMD_REG_212 = 0x1C,
    DDR_HOST_CMD_REG_213 = 0x1D,
    DDR_HOST_CMD_REG_214 = 0x1E,
    DDR_HOST_CMD_REG_215 = 0x1F

} DDR_HOST_CMD_REG;

void InitDDR(void)
{
    uint32_t tmp;
    uint32_t ba_field, ma_field;
    uint32_t w2rdly, w2rcsdly;
    uint32_t fPMDLock = CFGCONbits.PMDLOCK;
    
    if(fPMDLock)
    {
        SYSKEY = 0x00000000;    
        SYSKEY = 0xAA996655;    
        SYSKEY = 0x556699AA;         
        CFGCONbits.PMDLOCK = 0;
        SYSKEY = 0x33333333;
    }
    PMD7bits.DDR2CMD = 1;       // Power down the DDR controller, so I can set up the PLL
    
    //**************************************************************************
    //***************  Init the DDR PLL and Clock source  **********************
    //**************************************************************************
   
    /* Memory PLL */
	// PLIB_DEVCON_MPLLVregEnable( DEVCON_ID_0 );
    CFGMPLLbits.MPLLVREGDIS = 0;

	//while (!PLIB_DEVCON_MPLLVregIsReady(DEVCON_ID_0));
    while(!CFGMPLLbits.MPLLVREGRDY);

	//PLIB_DEVCON_MPLLVrefSet(DEVCON_ID_0, DEVCON_MPLL_VREF_EXT);
    CFGMPLLbits.INTVREFCON = DEVCON_MPLL_VREF_EXT;
    // PLIB_DEVCON_MPLLInputDivSet( DEVCON_ID_0, 1 );
    CFGMPLLbits.MPLLIDIV = 1;

    // PLIB_DEVCON_MPLLMultiplierSet ( DEVCON_ID_0, 25 );
    CFGMPLLbits.MPLLMULT = 25;

    //PLIB_DEVCON_MPLLODiv1Set( DEVCON_ID_0, DEVCON_MPLL_ODIV_3 );
    CFGMPLLbits.MPLLODIV1 = DEVCON_MPLL_ODIV_3;

    // PLIB_DEVCON_MPLLODiv2Set( DEVCON_ID_0, DEVCON_MPLL_ODIV_1 );
    CFGMPLLbits.MPLLODIV2 = DEVCON_MPLL_ODIV_1;

    // PLIB_DEVCON_MPLLEnable( DEVCON_ID_0 );
    CFGMPLLbits.MPLLDIS = 0;

	// while(!PLIB_DEVCON_MPLLIsReady( DEVCON_ID_0 ));
    while(!CFGMPLLbits.MPLLRDY);
    
    //**************************************************************************
    //***********************  Turn on the DDR *********************************
    //**************************************************************************
    // ONLY AFTER the PLL is init can we power up the DDR Controller
    PMD7bits.DDR2CMD = 0;
    
    // restore the lock if needed
    if(fPMDLock)
    {
        SYSKEY = 0x00000000;    
        SYSKEY = 0xAA996655;    
        SYSKEY = 0x556699AA;         
        CFGCONbits.PMDLOCK = 1;
        SYSKEY = 0x33333333;
    }
     
    //**************************************************************************
    //***********************  Init the DDR PHY ********************************
    //**************************************************************************
    DDRMEMCON = 0;              // clear the DDR controller register.
    
    // PLIB_DDR_PHY_OdtEnable(DDR_ID_0, DDR_PHY_ODT_150_OHM);
    DDRPHYPADCONbits.ODTSEL = DDR_PHY_ODT_150_OHM;
    DDRPHYPADCONbits.ODTEN = 1;
    // PLIB_DDR_PHY_DataDriveStrengthSet(DDR_ID_0, DDR_PHY_DRIVE_STRENGTH_60);
    DDRPHYPADCONbits.DATDRVSEL = DDR_PHY_DRIVE_STRENGTH_60;
    // PLIB_DDR_PHY_AddCtlDriveStrengthSet(DDR_ID_0, DDR_PHY_DRIVE_STRENGTH_60);
    DDRPHYPADCONbits.ADDCDRVSEL = DDR_PHY_DRIVE_STRENGTH_60;
    // PLIB_DDR_PHY_OdtCal(DDR_ID_0, 3, 2);
    DDRPHYPADCONbits.ODTPUCAL = 3;
    DDRPHYPADCONbits.ODTPDCAL = 2;
    // PLIB_DDR_PHY_DrvStrgthCal(DDR_ID_0, 14, 14);
    DDRPHYPADCONbits.DRVSTRNFET = 14;
    DDRPHYPADCONbits.DRVSTRPFET = 14;
    // PLIB_DDR_PHY_ExtraClockDisable(DDR_ID_0);
    DDRPHYPADCONbits.EOENCLKCYC = 0;
    // PLIB_DDR_PHY_InternalDllEnable(DDR_ID_0);
    DDRPHYPADCONbits.NOEXTDLL = 1;
    // PLIB_DDR_PHY_PadReceiveEnable(DDR_ID_0);
    DDRPHYPADCONbits.RCVREN = 1;
    // PLIB_DDR_PHY_PreambleDlySet(DDR_ID_0, DDR_PHY_PREAMBLE_DLY_1_0);
    DDRPHYPADCONbits.PREAMBDLY = DDR_PHY_PREAMBLE_DLY_1_0;
    // PLIB_DDR_PHY_HalfRateSet(DDR_ID_0);
    DDRPHYPADCONbits.HALFRATE = 1;
    // PLIB_DDR_PHY_WriteCmdDelayEnable(DDR_ID_0);
    DDRPHYPADCONbits.WRCMDDLY = 1;
    // PLIB_DDR_PHY_DllRecalibEnable(DDR_ID_0, 16);
    DDRPHYDLLRbits.RECALIBCNT = 16;
    DDRPHYDLLRbits.DISRECALIB = 0;
    // PLIB_DDR_PHY_DllMasterDelayStartSet(DDR_ID_0, 3);
    DDRPHYDLLRbits.DLYSTVAL = 3;
    // PLIB_DDR_PHY_SCLTestBurstModeSet(DDR_ID_0, DDR_PHY_SCL_BURST_MODE_8);
    DDRSCLCFG0bits.BURST8 = DDR_PHY_SCL_BURST_MODE_8;
    // PLIB_DDR_PHY_DDRTypeSet(DDR_ID_0, DDR_PHY_DDR_TYPE_DDR2);
    DDRSCLCFG0bits.DDR2 = !DDR_PHY_DDR_TYPE_DDR2;
    // PLIB_DDR_PHY_ReadCASLatencySet(DDR_ID_0, 5);
    DDRSCLCFG0bits.RCASLAT = 5;
    // PLIB_DDR_PHY_WriteCASLatencySet(DDR_ID_0, 4);
    DDRSCLCFG1bits.WCASLAT = 4;
    // PLIB_DDR_PHY_OdtCSEnable(DDR_ID_0);
    DDRSCLCFG0bits.ODTCSW = 1;
    // PLIB_DDR_PHY_SCLDelay(DDR_ID_0, DDR_PHY_SCL_DELAY_SINGLE);
    DDRSCLCFG1bits.DBLREFDLY = DDR_PHY_SCL_DELAY_SINGLE;
    //PLIB_DDR_PHY_SCLDDRClkDelaySet(DDR_ID_0, 4);
    DDRSCLLATbits.DDRCLKDLY = 4;
    // PLIB_DDR_PHY_SCLCapClkDelaySet(DDR_ID_0, 3);
    DDRSCLLATbits.CAPCLKDLY = 3;
    // PLIB_DDR_PHY_SCLEnable(DDR_ID_0, 0);   NOTHING TO DO
    
    //**************************************************************************
    //***********************  Init the DDR Peripheral Module *****************
    //**************************************************************************
    
    /* Target Arbitration */
    // PLIB_DDR_MinLimit(DDR_ID_0, 0x1F, DDR_TARGET_0);
    DDRTSELbits.TSEL = (0 * 5);
    DDRMINLIMbits.MINLIMIT = 0x1F;
    // PLIB_DDR_ReqPeriod(DDR_ID_0, 0xFF, DDR_TARGET_0);
    DDRTSELbits.TSEL = 0 * 8;
    DDRRQPERbits.RQPER = 0xFF;
    // PLIB_DDR_MinCommand(DDR_ID_0, 0x04, DDR_TARGET_0);
    DDRTSELbits.TSEL = 0 * 8;
    DDRMINCMDbits.MINCMD = 0x04;

    // PLIB_DDR_MinLimit(DDR_ID_0, 0x1F, DDR_TARGET_1);
    DDRTSELbits.TSEL = (1 * 5);
    DDRMINLIMbits.MINLIMIT = 0x1F;
    // PLIB_DDR_ReqPeriod(DDR_ID_0, 0xFF, DDR_TARGET_1);
    DDRTSELbits.TSEL = 1 * 8;
    DDRRQPERbits.RQPER = 0xFF;
    // PLIB_DDR_MinCommand(DDR_ID_0, 0x10, DDR_TARGET_1);
    DDRTSELbits.TSEL = 1 * 8;
    DDRMINCMDbits.MINCMD = 0x10;

    // PLIB_DDR_MinLimit(DDR_ID_0, 0x1F, DDR_TARGET_2);
    DDRTSELbits.TSEL = (2 * 5);
    DDRMINLIMbits.MINLIMIT = 0x1F;
    // PLIB_DDR_ReqPeriod(DDR_ID_0, 0xFF, DDR_TARGET_2);
    DDRTSELbits.TSEL = 2 * 8;
    DDRRQPERbits.RQPER = 0xFF;
    // PLIB_DDR_MinCommand(DDR_ID_0, 0x10, DDR_TARGET_2);
    DDRTSELbits.TSEL = 2 * 8;
    DDRMINCMDbits.MINCMD = 0x10;

    // PLIB_DDR_MinLimit(DDR_ID_0, 0x04, DDR_TARGET_3);
    DDRTSELbits.TSEL = (3 * 5);
    DDRMINLIMbits.MINLIMIT = 0x04;
    // PLIB_DDR_ReqPeriod(DDR_ID_0, 0xFF, DDR_TARGET_3);
    DDRTSELbits.TSEL = 3 * 8;
    DDRRQPERbits.RQPER = 0xFF;
    // PLIB_DDR_MinCommand(DDR_ID_0, 0x04, DDR_TARGET_3);
    DDRTSELbits.TSEL = 3 * 8;
    DDRMINCMDbits.MINCMD = 0x4;

    // PLIB_DDR_MinLimit(DDR_ID_0, 0x04, DDR_TARGET_4);
    DDRTSELbits.TSEL = (4 * 5);
    DDRMINLIMbits.MINLIMIT = 0x04;
    // PLIB_DDR_ReqPeriod(DDR_ID_0, 0xFF, DDR_TARGET_4);
    DDRTSELbits.TSEL = 4 * 8;
    DDRRQPERbits.RQPER = 0xFF;
    // PLIB_DDR_MinCommand(DDR_ID_0, 0x04, DDR_TARGET_4);
    DDRTSELbits.TSEL = 4 * 8;
    DDRMINCMDbits.MINCMD = 0x4;
 
    /* Addressing */
    // PLIB_DDR_RowAddressSet(DDR_ID_0, ROW_ADDR_RSHIFT, ROW_ADDR_MASK);
    DDRMEMCFG0bits.RWADDR = 11;
    DDRMEMCFG1bits.RWADDRMSK = ((1 << 13) - 1);
    // PLIB_DDR_ColumnAddressSet(DDR_ID_0, COL_HI_RSHFT, COL_LO_MASK, COL_HI_MASK);
    DDRMEMCFG0bits.CLHADDR = 0;
    DDRMEMCFG3bits.CLADDRLMSK = ((1 << 9) - 1);
    DDRMEMCFG2bits.CLADDRHMSK = 0;
    // PLIB_DDR_BankAddressSet(DDR_ID_0, BA_RSHFT, BANK_ADDR_MASK);
    DDRMEMCFG0bits.BNKADDR = 9;
    DDRMEMCFG4bits.BNKADDRMSK = ((1 << 2) - 1);
    // PLIB_DDR_ChipSelectAddressSet(DDR_ID_0, CS_ADDR_RSHIFT, CS_ADDR_MASK);
    DDRMEMCFG0bits.CSADDR = 0;
    DDRMEMCFG4bits.CSADDRMSK = 0;
    
    /* Refresh */
    // !!!!PLEASE OBSERVE!!!!
    // I updated the refresh rate to 75ns, but if we have problems back this off to 127.5ns!!!
    // PLIB_DDR_RefreshTimingSet(DDR_ID_0, 127500, 7800000, CTRL_CLK_PERIOD);
    DDRREFCFGbits.REFCNT = (7800000 + CTRL_CLK_PERIOD - 1) / CTRL_CLK_PERIOD - 2;   // tREFI == 7.8us          
//    DDRREFCFGbits.REFDLY = (127500 + CTRL_CLK_PERIOD - 1) / CTRL_CLK_PERIOD - 2;    //tRFC == 127.5ns
    DDRREFCFGbits.REFDLY = (75000 + CTRL_CLK_PERIOD - 1) / CTRL_CLK_PERIOD - 2;    //tRFC == 75ns
    // PLIB_DDR_MaxPendingRefSet(DDR_ID_0, 7);
    DDRREFCFGbits.MAXREFS = 7;
    // PLIB_DDR_AutoSelfRefreshDisable(DDR_ID_0);
    DDRPWRCFGbits.ASLFREFEN = 0;

    /* Power */
    // PLIB_DDR_AutoPowerDownDisable(DDR_ID_0);
    DDRPWRCFGbits.APWRDNEN = 0;
    // PLIB_DDR_AutoPchrgDisable(DDR_ID_0);
    DDRMEMCFG0bits.APCHRGEN = 0;
    // PLIB_DDR_AutoPchrgPowerDownDisable(DDR_ID_0);
    DDRPWRCFGbits.PCHRGPWRDN = 0;

    /* Timing */
    // PLIB_DDR_ReadWriteDelaySet(DDR_ID_0, 2, 4, 5);
    DDRDLYCFG0bits.R2WDLY = 2 + 2u;
    DDRDLYCFG0bits.RMWDLY = 5 - 4 + 3u;
    
    // PLIB_DDR_WriteReadDelaySet(DDR_ID_0, 7500, 2, 4, CTRL_CLK_PERIOD);
    w2rdly = round_up(7500, CTRL_CLK_PERIOD) + 4 + 2;
    w2rcsdly = ((w2rdly - 1u) > 3u) ? (w2rdly - 1u) : 3u;

    DDRDLYCFG0bits.W2RDLY = w2rdly & 0x0Fu;
    DDRDLYCFG1bits.W2RDLY4 = ((w2rdly & 0x10u) != 0) ? 1 : 0;

    DDRDLYCFG0bits.W2RCSDLY = w2rcsdly & 0x0Fu;
    DDRDLYCFG1bits.W2RCSDLY4 = ((w2rcsdly & 0x10u) != 0) ? 1 : 0;
        
    // PLIB_DDR_ReadReadDelaySet(DDR_ID_0, 2);
    DDRDLYCFG0bits.R2RDLY = 2 - 1u;
    DDRDLYCFG0bits.R2RCSDLY = 2;
    // PLIB_DDR_WriteWriteDelaySet(DDR_ID_0, 2);
    DDRDLYCFG0bits.W2WDLY = 2 - 1u;
    DDRDLYCFG0bits.W2WCSDLY = 2 - 1u;
    // PLIB_DDR_SelfRefreshDelaySet(DDR_ID_0, 17, 3, 200);
    DDRPWRCFGbits.SLFREFDLY = 17;
    DDRDLYCFG1bits.SLFREFMINDLY = 3 - 1u;
    DDRDLYCFG1bits.SLFREFEXDLY = (round_up(200, 2u) - 2u) & 0xFFu;
    DDRDLYCFG1bits.SLFREFEXDLY8 = ((round_up(200, 2u) & 0x100u) != 0) ? 1u : 0;
    // PLIB_DDR_PowerDownDelaySet(DDR_ID_0, 8, 3, 2);
    DDRPWRCFGbits.PWRDNDLY = 8;
    DDRDLYCFG1bits.PWRDNMINDLY = 3 - 1;
    DDRDLYCFG1bits.PWRDNEXDLY = (3 > 2 ? 3 : 2) - 1; 
    // PLIB_DDR_PrechargAllBanksSet(DDR_ID_0, 12500, CTRL_CLK_PERIOD);
    DDRDLYCFG2bits.PCHRGALLDLY = round_up(12500, CTRL_CLK_PERIOD); 
    // PLIB_DDR_ReadToPrechargeDelaySet(DDR_ID_0, 7500, 2, CTRL_CLK_PERIOD);
    DDRDLYCFG2bits.R2PCHRGDLY = round_up(7500, CTRL_CLK_PERIOD) + 2 - 2; 
    // PLIB_DDR_WriteToPrechargeDelaySet(DDR_ID_0, 15000, 2, 4, CTRL_CLK_PERIOD);
    DDRDLYCFG2bits.W2PCHRGDLY = (round_up(15000, CTRL_CLK_PERIOD) + 4 + 2) & 0x0F;
    DDRDLYCFG1bits.W2PCHRGDLY4 = (((round_up(15000, CTRL_CLK_PERIOD) + 4 + 2) & 0x10u) != 0) ? 1 : 0;
    // PLIB_DDR_PrechargeToRASDelaySet(DDR_ID_0, 12500, CTRL_CLK_PERIOD);
    DDRDLYCFG2bits.PCHRG2RASDLY = round_up(12500, CTRL_CLK_PERIOD) - 1; 
    // PLIB_DDR_RASToPrechargeDelaySet(DDR_ID_0, 45000, CTRL_CLK_PERIOD);
    DDRDLYCFG3bits.RAS2PCHRGDLY = round_up(45000, CTRL_CLK_PERIOD) - 1;
    // PLIB_DDR_RASToRASBankDelaySet(DDR_ID_0, 57500, CTRL_CLK_PERIOD);
    DDRDLYCFG3bits.RAS2RASSBNKDLY = round_up(57500, CTRL_CLK_PERIOD) - 1;
    // PLIB_DDR_RASToRASDelaySet(DDR_ID_0, 7500, CTRL_CLK_PERIOD);
    DDRDLYCFG2bits.RAS2RASDLY = round_up(7500, CTRL_CLK_PERIOD) - 1;
    // PLIB_DDR_RASToCASDelaySet(DDR_ID_0, 12500, CTRL_CLK_PERIOD);
    DDRDLYCFG2bits.RAS2CASDLY = round_up(12500, CTRL_CLK_PERIOD) - 1;
    // PLIB_DDR_DataDelaySet(DDR_ID_0, 5, 4);    CHECK OUT HARMONY ERROR, wLAT == 4
    DDRDLYCFG2bits.RBENDDLY = 5 + 3u;
    DDRXFERCFGbits.NXTDATRQDLY = 2;
    DDRXFERCFGbits.NXTDATAVDLY = 4;
    DDRDLYCFG1bits.NXTDATAVDLY4 = (((5 + 5u) & 0x10u) != 0) ? 1 : 0;
    DDRXFERCFGbits.RDATENDLY = 2;/*rLat - 1*/
    // PLIB_DDR_TfawDelaySet(DDR_ID_0, 35000, CTRL_CLK_PERIOD);
    DDRDLYCFG3bits.FAWTDLY = round_up(35000, CTRL_CLK_PERIOD) - 1;

    /* On-Die Termination */
    //PLIB_DDR_OdtReadDisable(DDR_ID_0, 0);
    DDRODTCFGbits.ODTCSEN = 0;
    DDRODTENCFGbits.ODTREN = 0;
    // PLIB_DDR_OdtWriteEnable(DDR_ID_0, 0);
    DDRODTCFGbits.ODTCSEN = 0;
    DDRODTENCFGbits.ODTWEN = 1;
    // PLIB_DDR_OdtWriteParamSet(DDR_ID_0, 3, 1);
    DDRODTCFGbits.ODTWLEN = 3;
    DDRODTCFGbits.ODTWDLY = 1;

    /* Controller Settings */
    // PLIB_DDR_LittleEndianSet(DDR_ID_0);
    DDRXFERCFGbits.BIGENDIAN = 0;
    // PLIB_DDR_HalfRateSet(DDR_ID_0);
    DDRMEMWIDTHbits.HALFRATE = 1;
    // PLIB_DDR_MaxCmdBrstCntSet(DDR_ID_0, 3);
    DDRXFERCFGbits.MAXBURST = 3;
    // PLIB_DDR_NumHostCmdsSet(DDR_ID_0, 12);
    DDRCMDISSUEbits.NUMHOSTCMDS = 12;

    /* DRAM Initialization */

    /* bring CKE high after reset and wait 400 nsec */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_10, DRV_DDR_IDLE_NOP);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_20, (0x00 | (0x00 << 8) | (sys_mem_ddr_hc_clk_dly(400000) << 11)));

    /* issue precharge all command */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_11, DRV_DDR_PRECH_ALL_CMD);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_21, (0x04 | (0x00 << 8) | (sys_mem_ddr_hc_clk_dly(12500 + 2500) << 11)));

    /* initialize EMR2 */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_12, DRV_DDR_LOAD_MODE_CMD);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_22, (0x00 | (0x02 << 8) | (sys_mem_ddr_hc_clk_dly(2 * 2500) << 11)));

    /* initialize EMR3 */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_13, DRV_DDR_LOAD_MODE_CMD);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_23, (0x00 | (0x03 << 8) | (sys_mem_ddr_hc_clk_dly(2 * 2500) << 11)));

    /* RDQS disable, DQSB enable, OCD exit, 150 ohm termination, AL=0, DLL enable */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_14, (DRV_DDR_LOAD_MODE_CMD | (0x40 << 24)));
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_24, (0x00 | (0x01 << 8) | (sys_mem_ddr_hc_clk_dly(2 * 2500) << 11)));

    tmp = ((sys_mem_ddr_round_up(15000, 2500) -1 ) << 1) | 1;
    ma_field = tmp & 0xFF;
    ba_field = (tmp >> 8) & 0x03;

    /* PD fast exit, WR REC = tWR in clocks -1, DLL reset, CAS = RL, burst = 4 */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_15, (DRV_DDR_LOAD_MODE_CMD | (((5 << 4) | 2) << 24)));
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_25, (ma_field | (ba_field << 8) | (sys_mem_ddr_hc_clk_dly(2 * 2500) << 11)));

    /* issue precharge all command */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_16, DRV_DDR_PRECH_ALL_CMD);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_26, (0x04 | (0x00 << 8) | (sys_mem_ddr_hc_clk_dly(12500 + 2500) << 11)));

    /* issue refresh command */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_17, DRV_DDR_REF_CMD);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_27, (0x00 | (0x00 << 8) | (sys_mem_ddr_hc_clk_dly(127500) << 11)));

    /* issue refresh command */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_18, DRV_DDR_REF_CMD);
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_28, (0x00 | (0x00 << 8) | (sys_mem_ddr_hc_clk_dly(127500) << 11)));

    tmp = ((sys_mem_ddr_round_up(15000, 2500) -1 ) << 1);
    ma_field = tmp & 0xFF;
    ba_field = (tmp >> 8) & 0x03;

    /* Mode register programming as before without DLL reset */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_19, (DRV_DDR_LOAD_MODE_CMD | (((5 << 4) | 3) << 24)));
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_29, (ma_field | (ba_field << 8) | (sys_mem_ddr_hc_clk_dly(2 * 2500) << 11)));

    /* extended mode register same as before with OCD default */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_110, (DRV_DDR_LOAD_MODE_CMD | (0xC0 << 24)));
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_210, (0x03 | (0x01 << 8) | (sys_mem_ddr_hc_clk_dly(2 * 2500) << 11)));

    /* extended mode register same as before with OCD exit */
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_111, (DRV_DDR_LOAD_MODE_CMD | (0x40 << 24)));
    DDR_CmdDataWrite(DDR_HOST_CMD_REG_211, (0x00 | (0x01 << 8) | (sys_mem_ddr_hc_clk_dly(140 * 2500) << 11)));

    /* Set number of host commands */
    // PLIB_DDR_NumHostCmdsSet(DDR_ID_0, 0x1B);  ERROR in HARMONY, can't set 5 bits in a 4 bit field
    DDRCMDISSUEbits.NUMHOSTCMDS = 0xB;

    // PLIB_DDR_CmdDataValid(DDR_ID_0);
    DDRCMDISSUEbits.VALID = 1;
    // PLIB_DDR_CmdDataSend(DDR_ID_0);
    DDRMEMCONbits.STINIT = 1;
    // while (PLIB_DDR_CmdDataIsComplete(DDR_ID_0));
    while (DDRCMDISSUEbits.VALID);
    // PLIB_DDR_ControllerEnable(DDR_ID_0);
    DDRMEMCONbits.INITDN = 1;
    
    //**************************************************************************
    //***********************  Calibrate ***************************************
    //**************************************************************************
    
    // PLIB_DDR_PHY_SCLStart(DDR_ID_0);
    DDRSCLSTARTbits.SCLEN = 1;
    DDRSCLSTARTbits.SCLSTART = 1;
    // while (!PLIB_DDR_PHY_SCLStatus(DDR_ID_0));
//    while (!(((DDRSCLSTARTbits.SCLLBPASS & DDRSCLSTARTbits.SCLUBPASS) == 0x01) ? true : false));
    while (!(DDRSCLSTARTbits.SCLLBPASS & DDRSCLSTARTbits.SCLUBPASS));

    return;
}  
