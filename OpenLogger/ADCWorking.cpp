/************************************************************************/
/*                                                                      */
/*    ADC.cpp                                                           */
/*                                                                      */
/*    ADC code                                           */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/17/2016 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>
#include <math.h>

static bool fDMA1HalfFull = false; 
static bool fDMA2HalfFull = false; 

static uint32_t    cSampleTmr = 0;
static uint32_t    cCurSampleTmr = 0;

// nVin = (328,853.21)Dadc - 10,775,862,000
static POLYCAL calDAQ = {CALVER, Idle, 328853, 0, 10775862000};

// keep as constant so it goes in flash, otherwise we need to make it a mulitple of 16 and align for DMA
static const uint16_t adcReadData = 0b1101000000000000;

typedef enum
{
    cmrChSel0   = 0x0,
    cmrChSel1   = 0x1,
    cmrChSel2   = 0x2,
    cmrChSel3   = 0x3,
    cmrChSel4   = 0x4,
    cmrChSel5   = 0x5,
    cmrChSel6   = 0x6,
    cmrChSel7   = 0x7,
    cmrWakeUp   = 0xB,
    cmrReadCFR  = 0xC,
    cmrReadData = 0xD,
    cmrWriteCFR = 0xE,
    cmrDefault  = 0xF
} CMR;

    // write CFR
    // D11: Manual Select:      0
    // D10: Internal clock:     1
    // D9:  Manual conversion   1   - Auto trigger 0
    // D8:  500ksps:            0
    // D7:  EOC active low      1
    // D6:  EOC as INT          1   - keep as EOC, rises at end of conversion
    // D5:  Pin 10 as EOC       1
    // D4:  Auto-NAP disabled   1
    // D3:  NAP disabled        1
    // D2:  Deep Sleep disabled 1
    // D1:  Tag Output disabled 0
    // D0:  Don't reset         1

#define DEFCFR 0b011011111101
typedef union
{
    struct
    {
        unsigned    reset           : 1;        // active low
        unsigned    tagOutput       : 1;        // active high     
        unsigned    deepPWRdwn      : 1;        // active low
        unsigned    napPWRdwn       : 1;        // active low
        unsigned    autoNapPWRdwn   : 1;        // active low
        unsigned    CDI_INT         : 1;        // 0->CDI, 1->INT
        unsigned    INT_ECO         : 1;        // 0->INT, 1->EOC
        unsigned    intPolarity     : 1;        // 0->active high, 1-> active low
        unsigned    autoTrigRate    : 1;        // 0->500ksps, 1->250ksps
        unsigned    triggerMode     : 1;        // 0->auto, 1->manual (set convst low to trigger)
        unsigned    clockSource     : 1;        // 0->SCLK/2, 1->internal
        unsigned    autoSequence    : 1;        // 0->Single, 1->auto-sequence
        unsigned                    : 4;
    };
    struct
    {
        unsigned    cfr             : 12;       // cfr bits
        unsigned    cmr             : 4;        // cmr bits
    };
    unsigned        adcmd           : 16;       // the full thing
    unsigned        adcData         : 16;       // the full thing
} ADCMD;

void ISR(ADCSAMPTMRVECTOR, ADCSAMPTMRIPLxSRS) ADCHolding2SampleISR(void)
{
    ADCSAMPTMRIF                = 0;

    if(cCurSampleTmr == 0)
    {
        cCurSampleTmr = cSampleTmr;
        ADCSAMPDMA1.DCHxECON.CFORCE = 1; 
        ADCSAMPDMA2.DCHxECON.CFORCE = 1; 
    }
    else
    {
        cCurSampleTmr--;
    }
}

void ISR(ADCSAMPDMA1VECTOR, ADCSAMPDMA1IPLxSRS) ADCToggle1to2ISR(void)
{
    // turn off INTs for this DMA
    ADCSAMPDMA1.DCHxINT.CHDHIE  = 0;
    ADCSAMPDMA1.DCHxINT.CHDHIF  = 0;
    ADCSAMPDMA1IF               = 0;
    
    // set the address for the next DMA
    ADCSAMPDMA2.DCHxDSA = ADCSAMPDMA1.DCHxDSA + CBADCDMA;

    // this should never point to the last buffer
    ASSERT(((ADCSAMPDMA2.DCHxDSA - KVA_2_PA(adcRawBuffer)) < sizeof(adcRawBuffer)));

    // set the hal full, reset the other DMA
    fDMA1HalfFull   = true; 
    fDMA2HalfFull   = false; 

    // turn on INTs for the next DMA
    ADCSAMPDMA2.DCHxINT.CHDHIF  = 0;
    ADCSAMPDMA2.DCHxINT.CHDHIE  = 1;

}

void ISR(ADCSAMPDMA2VECTOR, ADCSAMPDMA2IPLxSRS) ADCToggle2to1ISR(void)
{
    
    // turn off INTs for this DMA
    ADCSAMPDMA2.DCHxINT.CHDHIE  = 0;
    ADCSAMPDMA2.DCHxINT.CHDHIF  = 0;
    ADCSAMPDMA2IF               = 0;
    
    // set the address for the next DMA
    ADCSAMPDMA1.DCHxDSA = ADCSAMPDMA2.DCHxDSA + CBADCDMA;

    // roll if pointing to the last buffer
    if((ADCSAMPDMA1.DCHxDSA - KVA_2_PA(adcRawBuffer)) >= sizeof(adcRawBuffer)) 
    {
        ASSERT(ADCSAMPDMA1.DCHxDSA == (KVA_2_PA(adcRawBuffer) + sizeof(adcRawBuffer)));
        ADCSAMPDMA1.DCHxDSA = KVA_2_PA(adcRawBuffer);
    }

    // set the hal full, reset the other DMA
    fDMA1HalfFull   = false; 
    fDMA2HalfFull   = true; 

    // turn on INTs for the next DMA
    ADCSAMPDMA1.DCHxINT.CHDHIF  = 0;
    ADCSAMPDMA1.DCHxINT.CHDHIE  = 1;
}

uint32_t ADCStopSampling(void)
{
    uint32_t            tStart = 0;
    ADCMD               adcCmd = {.adcmd = (cmrWriteCFR << 12) | DEFCFR};
    uint32_t volatile   du32;     // force the compiler to update this everytime it is used

    // Stop the DMAs; no more SPI commands to get data
    ADCSAMPTMR.TxCON.ON = 0;
    ADCSAMPDMA1.DCHxCON.CHEN = 0; 
    ADCSAMPDMA2.DCHxCON.CHEN = 0; 
    ADCREQDMA.DCHxCON.CHEN  = 0;     

    // let the DMAs all stop
    tStart = ReadCoreTimer();

//    logIParam.state = Stopped;
    // stop occurs in the ADCTastk
    logIParam.state = Finishing;

    ADCSAMPDMA1IE   = 0;
    ADCSAMPDMA1IF   = 0;
    ADCSAMPDMA2IE   = 0;
    ADCSAMPDMA2IF   = 0;
    ADCSAMPTMRIE    = 0;   
    ADCSAMPTMRIF    = 0;   

    // wait 4 usec, a sample takes 2usec, let the SPI's stop and complete 
    while(ReadCoreTimer() - tStart < (4*CORE_TMR_TICKS_PER_USEC));

    ADCREADDMA.DCHxCON.CHEN = 0;   

    // empty the SPIxRXB 
    ADCSPI.SPIxSTAT.SPIROV = 0;
    while(ADCSPI.SPIxSTAT.SPIRBF) du32 = ADCSPI.SPIxBUF;

    // reset the ADC, stop it, put it in manual trigger mode
    while(ADCSPI.SPIxSTAT.SPITBF);
    ADCSPI.SPIxBUF = adcCmd.adcmd;

    // wait for SPI return
    while(!ADCSPI.SPIxSTAT.SPIRBF); // wait for something to come in
    du32 = ADCSPI.SPIxBUF;

    // this is just to force the compiler to use the variable and not 
    // remove it with optimizations
    return(du32);
}

static STATE ADCSetSampleTimer(uint16_t period, uint16_t preScalar, uint32_t cnt)
{

    ADCSAMPTMR.TxCON.w      = 0;
    ADCSAMPTMR.PRx          = PeriodToPRx(period);
    ADCSAMPTMR.TxCON.TCKPS  = preScalar;
    ADCSAMPTMR.TMRx         = ADCSAMPTMR.PRx;       // trigger on the very first tick

    ASSERT(cnt>=1);
    cSampleTmr      = cnt-1;    // we are zero based, so take one off
    cCurSampleTmr   = 0;        // want to trigger immediately on the first tick
       
    return(Idle);
}

static STATE ADCSetChannels(uint32_t channel, uint32_t nCh, bool fMaxAdcFreq)
{
    uint16_t sCh = nCh > 1 ? (NBRLOGCH-nCh) : (NBRLOGCH-channel);
    ADCMD   chSelect = {.adcmd = (uint16_t) ((sCh << 12) | DEFCFR)};
    ADCMD   volatile retCmd = {.adcmd = (cmrWriteCFR << 12) | DEFCFR};
 
    (void) retCmd;

    ASSERT(1 <= nCh && nCh <= NBRLOGCH && 1 <= channel && channel <= NBRLOGCH && (nCh == 1 || channel == 1) && ADCSAMPTMR.TxCON.ON == 0);

    // make sure all DMA channels are off
    ADCREADDMA.DCHxCON.CHEN     = 0;                // turn on get data DMA
    ADCREQDMA.DCHxCON.CHEN      = 0;                // turn on conversion done 
    ADCSAMPDMA1.DCHxCON.CHEN    = 0;                // turn on get data DMA
    ADCSAMPDMA2.DCHxCON.CHEN    = 0;                // turn on conversion done 
 
    // Some NOTES:
    // we may have to go into manual trigger mode before setting the new channel
    // we may have to do a manual conversion to set the start channel.

    // set start channel 
    while(ADCSPI.SPIxSTAT.SPITBF);
    ADCSPI.SPIxBUF = chSelect.adcmd;

    // wait for SPI return
    while(!ADCSPI.SPIxSTAT.SPIRBF); // wait for something to come in
    retCmd.adcData = ADCSPI.SPIxBUF;

    // put in auto sequence mode; force manual triggering.
    chSelect.cmr            = cmrWriteCFR;          // write to the cmd register
    chSelect.triggerMode    = 1;                    // stay manual
    chSelect.autoSequence   = (nCh == 1) ? 0 : 1;   // if more than one channel, auto sequence
    while(ADCSPI.SPIxSTAT.SPITBF);
    ADCSPI.SPIxBUF = chSelect.adcmd;                // send the command

    // read the dummy byte back.
    while(!ADCSPI.SPIxSTAT.SPIRBF); // wait for something to come in
    retCmd.adcData = ADCSPI.SPIxBUF;

    // clear buffer and INTs
    memset(KVA_2_KSEG1(adcHoldingCell), 0, sizeof(adcHoldingCell));
    ADCREQDMA.DCHxINTu32        = 0;
    ADCREADDMA.DCHxINTu32       = 0;
    ADCSAMPDMA1.DCHxINTu32      = _DCH0INT_CHDHIE_MASK;
    ADCSAMPDMA2.DCHxINTu32      = 0;
    
    // just to clear the DMA pointers
    ADCREQDMA.DCHxDSA           = KVA_2_PA(&ADCSPI.SPIxBUF);    // ADC result buffer
    ADCREADDMA.DCHxSSA          = KVA_2_PA(&ADCSPI.SPIxBUF);    // source buffer
    
    // put in the result pointers
    ADCREADDMA.DCHxDSA          = KVA_2_PA(&adcHoldingCell[sCh]);      // ADC result buffer
    ADCREADDMA.DCHxDSIZ         = nCh * sizeof(adcHoldingCell[0]);     // result buffer size

    ADCSAMPDMA1.DCHxSSA         = ADCREADDMA.DCHxDSA;       // source buffer, same as sample buffer destination
    ADCSAMPDMA1.DCHxSSIZ        = ADCREADDMA.DCHxDSIZ;      // source size, same as sample buffer destination
    ADCSAMPDMA1.DCHxCSIZ        = ADCREADDMA.DCHxDSIZ;      // cell transfer size, same as sample buffer destination
    ADCSAMPDMA1.DCHxDSA         = KVA_2_PA(adcRawBuffer);      // ADC result buffer

    ADCSAMPDMA2.DCHxSSA         = ADCREADDMA.DCHxDSA;       // source buffer, same as sample buffer destination
    ADCSAMPDMA2.DCHxSSIZ        = ADCREADDMA.DCHxDSIZ;      // source size, same as sample buffer destination
    ADCSAMPDMA2.DCHxCSIZ        = ADCREADDMA.DCHxDSIZ;      // cell transfer size, same as sample buffer destination
    ADCSAMPDMA2.DCHxDSA          = KVA_2_PA(&adcRawBuffer[CSADCDMA]);      // ADC result buffer

    // start the DMA channels; do not start the sampling DMA
    ADCREADDMA.DCHxCON.CHEN    = 1;                // turn on get data DMA
    ADCREQDMA.DCHxCON.CHEN    = 1;                // turn on conversion done 

    // WARNING
    // we are going to have 2 bogus (actually start channel values) in the DMA buffer that are junk
    // The DMA will read the return from setting the auto trigger
    // and we read on the EOC going into conversion (low), and the first data
    // point will not be converted at that time.
    // So auto-trigger result, bogus first point.

    // if nCh <= 2, then we are either just going into the 1 DMA register, or we will be in sync as there are 2 extra DMA
    // let's just force the DMA to 2 before the roll

    if(nCh > 2)
    {
        uint16  iStart = ADCREADDMA.DCHxDSIZ - (2 * sizeof(adcHoldingCell[0]));
        ASSERT(ADCREADDMA.DCHxINT.CHCCIF == 0);
//        while((ADCREADDMA.DCHxDSA + ADCREADDMA.DCHxDPTR) != KVA_2_PA(&adcHoldingCell[6]))
        while(ADCREADDMA.DCHxDPTR < iStart)
        {
            ADCREADDMA.DCHxECON.CFORCE = 1;
            while(!ADCREADDMA.DCHxINT.CHCCIF);
            ADCREADDMA.DCHxINTu32       = 0;
        }
    }

    // put in auto trigger mode
    chSelect.cmr            = cmrWriteCFR;          // write to the cmd register
    chSelect.autoTrigRate   = fMaxAdcFreq ? 0 : 1;  // 0->500ksps, 1->250ksps
//    chSelect.autoTrigRate   = 1;  // 0->500ksps, 1->250ksps
//    chSelect.autoTrigRate   = 0;  // 0->500ksps, 1->250ksps
    chSelect.triggerMode    = 0;                    // Now go into auto trigger
    chSelect.autoSequence   = (nCh <= 1) ? 0 : 1;   // if more than one channel, auto sequence

    while(ADCSPI.SPIxSTAT.SPITBF);
    ADCSPI.SPIxBUF = chSelect.adcmd;                // put in auto trigger mode

    // Maybe we need to wait until the sample buffer is full with new data?
    // do this twice to ensure the whole destination buffer has valid data.
    // at 500kS/s for 8 channels, that is 32 ticks or 32/500000 = 64 usec
    while(!ADCREADDMA.DCHxINT.CHDDIF);

    // make sure the holding cells have valid data
    ADCREADDMA.DCHxINT.CHDDIF = 0;      // do a second round
    while(!ADCREADDMA.DCHxINT.CHDDIF);

    return(Idle);
}

STATE ADCRun(void)
{
    uint32_t sCh = logIParam.sCh - 1;
    uint32_t eCh = sCh + logIParam.nCh;
    uint32_t i;

    ADCSetChannels(logIParam.sCh, logIParam.nCh, logIParam.fAdcFreqMax);
    ADCSetSampleTimer(logIParam.smpTmrPeriod, logIParam.smpTmrPrescalar, logIParam.smpTmrISRCnt);

    // have the timer trigger the DMA
    if(logIParam.smpTmrISRCnt <= 1)
    {
        ADCSAMPDMA1.DCHxECON.SIRQEN = 1;                                // enable start IRQ
        ADCSAMPDMA2.DCHxECON.SIRQEN = 1;                                // enable start IRQ
    }

    // have the TMR ISR trigger the DMA
    else
    {
        ADCSAMPDMA1.DCHxECON.SIRQEN = 0;                                // force transfer from ISR
        ADCSAMPDMA2.DCHxECON.SIRQEN = 0;                                // force transfer from ISR

        // enable ISR to sample timer
        ADCSAMPTMRIF                = 0;  
        ADCSAMPTMRIE                = 1;   
    }

    ADCSAMPDMA1.DCHxDSA         = KVA_2_PA(adcRawBuffer);           // ADC result buffer
    ADCSAMPDMA1.DCHxDSIZ        = CBADCDMA;                         // result buffer size
    ADCSAMPDMA1IF               = 0;

    ADCSAMPDMA2.DCHxDSA         = KVA_2_PA(&adcRawBuffer[CSADCDMA]);      // ADC result buffer
    ADCSAMPDMA2.DCHxDSIZ        = CBADCDMA;                         // result buffer size
    ADCSAMPDMA2IF               = 0;

    // turn on the half way DMA ISRs
    ADCSAMPDMA1IE               = 1;
    ADCSAMPDMA2IE               = 1;

    // only enable the first DMA, the second will come on by chaining
    ADCSAMPDMA1.DCHxCON.CHEN    = 1;

    // set my raw buffer pointers
    logIParam.padcRawStart = (uint16_t *) KVA_2_KSEG1(adcRawBuffer);
    logIParam.padcRawEnd = (uint16_t *) KVA_2_KSEG1(adcRawBuffer);

    //set the voltage buffer pointers
    logIParam.padcVoltNext  = (int16_t *) KVA_2_KSEG1(adcVoltageBuffer);
    logIParam.cTotalSamples = 0;

    logIParam.state         = Running;
    logIParam.adcTrg.state  = Running;
    fDMA1HalfFull           = false; 
    fDMA2HalfFull           = false; 

    // precharge the averaging cells
    for(i=sCh; i<eCh; i++)
    {
        LOGCHANNEL& ch  = logIParam.rgLogCh[i];
        uint32_t iHoldingCell = NBRLOGCH - i - 1;
        
        ch.sumRaw = adcHoldingCell[iHoldingCell] << ch.pwr2Ave;
        ch.sumRaw -= adcHoldingCell[iHoldingCell];
    }

    // turn on the timer and get things going.
    ADCSAMPTMR.TxCON.ON = 1;

    return(Idle);
}

STATE InitADC(void) 
{
    ADCMD volatile  adcCmd = {.adcmd = (cmrWriteCFR << 12) | DEFCFR};
    uint32_t        tStart = 0;

    // set pin direction
    // Reset
    GPIO(ADC_PIN_RESET, CLR);
    INITRIS(ADC_PIN_RESET,CLR);

    // convst
    GPIO(ADC_PIN_CONVST, SET);
    INITRIS(ADC_PIN_CONVST,CLR);

    // CS
    GPIO(ADC_PIN_CS, SET);
    INITRIS(ADC_PIN_CS,CLR);

    // Set INT as input
    INITRIS(ADC_PIN_INT,SET);       // keep as input
    ADC_PPS_INT;                    // INT2->RD4    
    INTCONbits.ADCINTEP     = 0;    // Falling edge after sampling is done.

    // set up PPS
    ADC_PPS_SS;         // SS,          RB8
    ADC_PPS_MISO;       // MISO/SDI,    RB9
    ADC_PPS_MOSI;       // MOSI/SDO,    RB10

    // SPI is on PBCLK2, at 100MHz
    // set up the SPI control registers
    // the ADC is on SPI2
    ADCSPI.SPIxCON.w        = 0;
    ADCSPI.SPIxCON2.w       = 0;

    ADCSPI.SPIxCON.MSTEN    = 1;        // we are in master mode
    ADCSPI.SPIxCON.MSSEN    = 1;        // we want to control the CS pin automatically
    ADCSPI.SPIxCON.FRMPOL   = 0;        // we want the CS to be low active. 

    ADCSPI.SPIxCON.MCLKSEL  = 0;        // we want to use the PBCLK as our clock source

    // we do NOT want to run enhanced mode, we want to very carefully track the ins and outs of the SPI
//    ADCSPI.SPIxCON.ENHBUF   = 1;        // we want to use the FIFOs

    ADCSPI.SPIxCON.MODE16   = 1;        // we want to be in 16 bits per value
    ADCSPI.SPIxCON.MODE32   = 0;        // we want to be in 16 bits per value

    //    SPI Mode    Clock Polarity  Clock Phase Clock Edge
    //                (CPOL/CKP)      (CPHA)      (CKE/NCPHA)   
    //        0         0               0           1
    //        1	        0	            1	        0
    //        2	        1	            0	        1
    //        3	        1	            1	        0
    // ADC CPOL == 1; CPHA == 0
    // MZ   CKP == 1; CKE == 1
    ADCSPI.SPIxCON.CKP      = 1;        // CPOL == 1; Idle high, active low
    ADCSPI.SPIxCON.CKE      = 1;        // CPHA == 0; SDO changes on trailing edge of preceeding clock
    ADCSPI.SPIxCON.SMP      = 0;        // Read data at the middle of the output time. The output is put applied at the end of the last clock (rising edge), this will read the beginning of the current clock (falling edge).

    // no effect if enhanced mode is not used.
    ADCSPI.SPIxCON.STXISEL  = 0b00;     // generate interrupt when transmit is done.
    ADCSPI.SPIxCON.SRXISEL  = 0b01;     // generate a DMA interrupt when the FIFO is not empty

    ADCSPI.SPIxSTAT.SPIROV  = 0;        // clear the overrun flag

    // Fsck = Fpb / (2 * (SPIxBRG + 1)); Fpb = 100,000,000, we must run faster than 14MHz
    ADCSPI.SPIxBRG          = 1;        // set to 1 == 25MHz, 2 == 16.667MHz   

    ADCSPI.SPIxCON.ON = 1;

    // pull ADC out of reset
    GPIO(ADC_PIN_RESET, SET);

    // wait 2usec, lets wait 5usec
    tStart = ReadCoreTimer();
    while(ReadCoreTimer() - tStart < (5 * CORE_TMR_TICKS_PER_USEC));

    // write CFR
    // D11: Manual Select:      0
    // D10: Internal clock:     1
    // D9:  Manual conversion   1   - Auto trigger 0
    // D8:  500ksps:            0
    // D7:  EOC active low      1
    // D6:  EOC as INT          1   - keep as EOC, rises at end of conversion
    // D5:  Pin 10 as EOC       1
    // D4:  Auto-NAP disabled   1
    // D3:  NAP disabled        1
    // D2:  Deep Sleep disabled 1
    // D1:  Tag Output disabled 0
    // D0:  Don't reset         1
    while(ADCSPI.SPIxSTAT.SPITBF);
    ADCSPI.SPIxBUF = adcCmd.adcmd;    // keep in manual trigger mode

    while(!ADCSPI.SPIxSTAT.SPIRBF); // wait for something to come in
    adcCmd.adcmd = ADCSPI.SPIxBUF;

    // read CFR
    adcCmd.cmr = cmrReadCFR;
    while(ADCSPI.SPIxSTAT.SPITBF);
    ADCSPI.SPIxBUF = adcCmd.adcmd;

    while(!ADCSPI.SPIxSTAT.SPIRBF); // wait for something to come in
    adcCmd.adcmd = ADCSPI.SPIxBUF;

    ASSERT(adcCmd.cfr == DEFCFR);
 
    // clear DMA 3,4,5,6
    ADCREQDMA.DCHxCONu32        = 0;
    ADCREADDMA.DCHxCONu32       = 0;
    ADCSAMPDMA1.DCHxCONu32      = 0;
    ADCSAMPDMA2.DCHxCONu32      = 0;

    ADCREQDMA.DCHxECONu32       = 0;
    ADCREADDMA.DCHxECONu32      = 0;
    ADCSAMPDMA1.DCHxECONu32     = 0;
    ADCSAMPDMA2.DCHxECONu32     = 0;

    ADCREQDMA.DCHxINTu32        = 0;
    ADCREADDMA.DCHxINTu32       = 0;
    ADCSAMPDMA1.DCHxINTu32      = 0;
    ADCSAMPDMA2.DCHxINTu32      = 0;

   // DMA channels 3/4
    // DMA channel 3 move in the read data request when conversion complete
    ADCREQDMA.DCHxCON.CHPRI     = ADC_REQ_READ_DMAPRI;              // Low priority on the DMA; but higher than the sample timer
    ADCREQDMA.DCHxCON.CHAEN     = 1;                                // auto enable, keep cycling on block transfer 
    ADCREQDMA.DCHxECON.CHSIRQ   = ADCINTVECTOR;                     // extract the data on the EOC completion
    ADCREQDMA.DCHxECON.SIRQEN   = 1;                                // enable start IRQ
    ADCREQDMA.DCHxSSA           = KVA_2_PA(&adcReadData);           // source buffer
    ADCREQDMA.DCHxSSIZ          = 2;                                // source size
    ADCREQDMA.DCHxCSIZ          = 2;                                // cell transfer size 2 byte
    ADCREQDMA.DCHxDSA           = KVA_2_PA(&ADCSPI.SPIxBUF);        // ADC result buffer
    ADCREQDMA.DCHxDSIZ          = 2;                                // result buffer size

    // DMA channel 4 read out the value when in the rx buffer
    ADCREADDMA.DCHxCON.CHPRI    = ADC_REQ_READ_DMAPRI;              // Low priority on the DMA; but higher than the sample timer
    ADCREADDMA.DCHxCON.CHAEN    = 1;                                // auto enable, keep cycling on block transfer 
    ADCREADDMA.DCHxECON.CHSIRQ  = ADCSPIRXVECTOR;                   // There is a word in the recieve buffer
    ADCREADDMA.DCHxECON.SIRQEN  = 1;                                // enable start IRQ
    ADCREADDMA.DCHxSSA          = KVA_2_PA(&ADCSPI.SPIxBUF);        // source buffer
    ADCREADDMA.DCHxSSIZ         = 2;                                // source size
    ADCREADDMA.DCHxCSIZ         = 2;                                // cell transfer size 2 byte
    //ADCREADDMA.DCHxDSA          = KVA_2_PA(rgADC);                // ADC result buffer; depends on the nbr of channels
    //ADCREADDMA.DCHxDSIZ         = sizeof(rgADC);                  // result buffer size

    // DMA channel 5 is the ADC sample buffer
    ADCSAMPDMA1.DCHxCON.CHPRI   = ADC_SAMP_DATA_DMAPRI;             // Next to highest priority, it is our timing 
    ADCSAMPDMA1.DCHxCON.CHAEN   = 0;                                // do not cycle
    ADCSAMPDMA1.DCHxCON.CHCHNS  = 1;                                // chain to lower prioity, higher channel, chain to channel 6
    ADCSAMPDMA1.DCHxCON.CHCHN   = 1;                                // turn on channel chaining
    ADCSAMPDMA1.DCHxCON.CHAED   = 0;                                // Do not record events when disabled
    ADCSAMPDMA1.DCHxECON.CHSIRQ = ADCSAMPTMRVECTOR;                 // Triggered from sample timer
    ADCSAMPDMA1.DCHxECON.SIRQEN = 1;                                // enable start IRQ
    ADCSAMPDMA1.DCHxINT.CHDHIE  = 1;                                // enable the half full destination interrupt
//    ADCSAMPDMA.DCHxSSA          = KVA_2_PA(&adcHoldingCell);      // source buffer
//    ADCSAMPDMA.DCHxSSIZ         = 2;                              // source size; depends on nbr of channels
//    ADCSAMPDMA.DCHxCSIZ         = 2;                              // cell transfer size 2 byte
    ADCSAMPDMA1.DCHxDSA         = KVA_2_PA(adcRawBuffer);           // ADC result buffer
    ADCSAMPDMA1.DCHxDSIZ        = CBADCDMA;                         // result buffer size
    ADCSAMPDMA1IE               = 0;
    ADCSAMPDMA1IF               = 0;
    ADCSAMPDMA1IPCP             = ADC_SAMP_DATA_DMA_ISRPRI;         // same priority as the ISR
    ADCSAMPDMA1IPCS             = 0;                                // no subpriority

    // DMA channel 6 is the ADC sample buffer
    ADCSAMPDMA2.DCHxCON.CHPRI   = ADC_SAMP_DATA_DMAPRI;             // Next to highest priority, it is our timing
    ADCSAMPDMA2.DCHxCON.CHAEN   = 0;                                // do not cycle 
    ADCSAMPDMA2.DCHxCON.CHCHNS  = 0;                                // chain to high priority, lower channel number, chain to channel 5
    ADCSAMPDMA2.DCHxCON.CHCHN   = 1;                                // turn on channel chaining
    ADCSAMPDMA2.DCHxCON.CHAED   = 0;                                // Do not record events when disabled
    ADCSAMPDMA2.DCHxECON.CHSIRQ = ADCSAMPTMRVECTOR;                 // Triggered from sample timer
    ADCSAMPDMA2.DCHxECON.SIRQEN = 1;                                // enable start IRQ
    ADCSAMPDMA2.DCHxINT.CHDHIE  = 0;                                // enable the half full destination interrupt; let the ISR do this
//    ADCSAMPDMA.DCHxSSA          = KVA_2_PA(&adcHoldingCell);      // source buffer; depends on nbr of channels
//    ADCSAMPDMA.DCHxSSIZ         = 2;                              // source size
//    ADCSAMPDMA.DCHxCSIZ         = 2;                              // cell transfer size 2 byte
    ADCSAMPDMA2.DCHxDSA         = KVA_2_PA(&adcRawBuffer[CSADCDMA]);      // ADC result buffer
    ADCSAMPDMA2.DCHxDSIZ        = CBADCDMA;                         // result buffer size
    ADCSAMPDMA2IE               = 0;
    ADCSAMPDMA2IF               = 0;
    ADCSAMPDMA2IPCP             = ADC_SAMP_DATA_DMA_ISRPRI;         // same priority as the ISR
    ADCSAMPDMA2IPCS             = 0;                                // no subpriority

    ASSERT(!ADCSPI.SPIxSTAT.SPIRBF);

    // init the sample timer
    ADCSAMPTMR.TxCONu32         = 0;
    ADCSAMPTMRIE                = 0;
    ADCSAMPTMRIF                = 0;
    ADCSAMPTMRIPCP              = ADC_SAMP_TMR_ISRPRI;
    ADCSAMPTMRIPCS              = 0;

#if 0
    // DEBUG DEBUG, to be REMOVED
    // for timing purposes
    {
        ADCSAMPDMA1.DCHxSSA         = KVA_2_PA(adcHoldingCell);       // source buffer, same as sample buffer destination
        ADCSAMPDMA1.DCHxSSIZ        = sizeof(adcHoldingCell);      // source size, same as sample buffer destination
        ADCSAMPDMA1.DCHxCSIZ        = ADCSAMPDMA1.DCHxDSIZ;      // cell transfer size, same as sample buffer destination
        ADCSAMPDMA1.DCHxCON.CHEN    = 1;

 //       for(int i = 0; i < 2048; i++)
        {

            tManual = ReadCoreTimer(); 
            ADCSAMPDMA1.DCHxECON.CFORCE = 1;
            while(!ADCSAMPDMA1.DCHxINT.CHCCIF);
            loopStats.UpdateEntry(LOOPSTATS::MANTIME, ReadCoreTimer() - tManual);
            ADCSAMPDMA1.DCHxINTu32       = 0;
        }

        ADCSAMPDMA1.DCHxCON.CHEN    = 0;
        ADCSAMPDMA1.DCHxSSA         = 0;       // source buffer, same as sample buffer destination
        ADCSAMPDMA1.DCHxSSIZ        = 0;      // source size, same as sample buffer destination
        ADCSAMPDMA1.DCHxCSIZ        = 0;      // cell transfer size, same as sample buffer destination
    }
#endif

    return(Idle);
}

STATE DAQCalibrate(uint32_t ch, uint32_t chDC)
{
    uint32_t        fbCh    = (chDC == 1) ? CH_DC1_FB : CH_DC2_FB;
    int32_t         uV;
    
    // valid channel?
    ASSERT(1 <= ch && ch <= NBRLOGCH && 1 <= chDC && chDC <=2);

    if(calDAQ.state != Idle && calDAQ.B != ch)
    {
        return(WaitingForResources);
    }

    switch(calDAQ.state)
    {
        case Idle:
            // Reading the DC stimulas, we need to stop the LEDs
            LEDTask(LEDPause);
            arCalIDAQ[ch].state = Calibrating;

            // no PWM gain, so use B as the channel indicator
            calDAQ.B            = ch;
            calDAQ.A            = 0;
            calDAQ.C            = 0;
            DCSetvoltage(chDC, 3500);

            // set up the ADC channels for calibration
            // this gets things started
            ADCStopSampling();

            calDAQ.time         = ReadCoreTimer();
            calDAQ.state        = ADCWaitPWM;
            break;

        case ADCWaitPWM:
            if(ReadCoreTimer() - calDAQ.time >= PWM_SETTLING_TIME)
            {

                // Max adc sample rate
//                ADCSetChannels(1, NBRLOGCH, true);
//                ADCSetSampleTimer(2000, 0, 1);
 
                // half max sample rate
                ADCSetChannels(1, NBRLOGCH, false);
                ADCSetSampleTimer(4000, 0, 1);
 
                // start sampling
                ADCSAMPDMA2.DCHxCON.CHCHN    = 0;                            // Do not allow chaining to channel 2
                ADCSAMPDMA1.DCHxCON.CHEN     = 1;
                ADCSAMPTMR.TxCON.ON = 1;

                calDAQ.state = ADCSamplingComplete;    
            }
            break;

        case ADCSamplingComplete:
            if(!ADCSAMPDMA1.DCHxCON.CHEN)
            {
                ADCStopSampling();

                // restore the sample DMA to running conditions
                ADCSAMPDMA2.DCHxCON.CHCHN   = 1;                            // restore chaining to channel 2
 
                calDAQ.state = ADCCalAverage;    
            }
            break;

        case ADCCalAverage:
            {
                uint16_t *  pValue  = (uint16_t *) KVA_2_KSEG1(adcRawBuffer);
                uint16_t *  pEnd    = &pValue[CSADCDMA];
                uint32_t    ich     = NBRLOGCH - ch;
                uint32_t    Dadc    = 0;
                uint32_t i;

                for(i=0; &pValue[ich + (i*NBRLOGCH)] < pEnd; i++) Dadc += pValue[ich + i*NBRLOGCH];

                if(calDAQ.C == 0) calDAQ.A  = Dadc / i;
                else calDAQ.time            = Dadc / i;
 
                calDAQ.state = ADCReadDC; 
            }
            break;

        case ADCReadDC:
            if(FBAWGorDCuV(fbCh, &uV) == Idle)
            {

                if(calDAQ.C == 0)
                {
                    calDAQ.C        = uV;
                    DCSetvoltage(chDC, -3500);
                    calDAQ.time     = ReadCoreTimer();
                    calDAQ.state    = ADCWaitPWM;
                }

                else
                {
                    int64_t dnVin   = (calDAQ.C - uV) * 1000ll;
                    int32_t dDadc   = (int32_t) (calDAQ.A - calDAQ.time);

                    // we are swinging the ADC 7v
                    // 7 * 0.1392 * 65536 / 3 = 21,286
                    // we should at least swing 20,000
                    if(dDadc < 20000)
                    {
                        
                        // restore our previous state
                        arCalIDAQ[ch].state = FailedCalibration;
                        DCSetvoltage(chDC, 0);
                        LEDTask(LEDResume);
                        calDAQ.state = Idle;
                        return(MkStateAnError(FailedCalibration));
                    }

                    else
                    {
                        // nVin = A * Dadc - C
                        // nVin = (328,853.21)Dadc - 10,775,862,000
                        // dnVin = (328,853.21)dDadc
                        // A ~ (328,853.21) ~ dnVin/dDadc
                        calDAQ.A = (dnVin +  dDadc/2) / dDadc;

                        // nVin = A * Dadc - C
                        // C = A * Dadc - nVin
                        calDAQ.C = (calDAQ.A * calDAQ.time) - (uV * 1000ll); 

                        calDAQ.state = Done; 
                    }
                }
            }
            break;

        case Done:

            // B is unused
            calDAQ.B = 0;

            // put the version in
            calDAQ.version = CALVER;

            // say we are calibrated
            calDAQ.state = Calibrated;

            // move it into our calibration constant for the channel
            memcpy(&arCalIDAQ[ch], &calDAQ, sizeof(POLYCAL));
                
            // now set the channel to zero volts
            DCSetvoltage(chDC, 0);

            // we are done
            calDAQ.state = Idle;
            LEDTask(LEDResume);
            fCalibrated = fIsCalibrated();
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            return(STATEError);
    }
    
    return(calDAQ.state);  
}

GCMD::ACTION OSPAR::ReadVoltageLog(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead)
{
    uint8_t *   pbEnd   = (uint8_t *) KVA_2_KSEG1(adcVoltageBuffer+CSVBUFF);

    if(oslex.odata[iOData].cb == 0) return(GCMD::DONE);

    pbRead = oslex.odata[iOData].pbOut;
    cbRead = min(min((uint32_t) (pbEnd - pbRead), oslex.odata[iOData].cb), (uint32_t) CBADCDMA);

    if((pbRead + cbRead) == pbEnd)  oslex.odata[iOData].pbOut = (uint8_t *) KVA_2_KSEG1(adcVoltageBuffer);
    else                            oslex.odata[iOData].pbOut += cbRead;

    oslex.odata[iOData].cb -= cbRead;

    return(GCMD::WRITE);
}

static void __attribute__((optimize("-O3"))) ADCToVoltage(uint16_t * pADC, uint32_t cGroup)
{
    uint32_t i;
    uint32_t j;
    uint32_t eCh = logIParam.sCh + logIParam.nCh - 1;

    for(i=logIParam.sCh-1, j=0; i<eCh; i++)
    {
        LOGCHANNEL& ch      = logIParam.rgLogCh[i];

        // only look at active channels
        if(ch.cAve != 0)
        {
            uint32_t    k;
            uint32_t    iVolt;
            uint32_t    iRaw;
        
            for(k=0, iRaw=0, iVolt=0; k<cGroup; k++, iRaw+=logIParam.nCh, iVolt+=logIParam.cActiveCh)
            {
                uint32_t Dadc;

                // add in the new value
                ch.sumRaw += pADC[iRaw + ch.iADCValue];

                // find the averaage
                Dadc = ch.sumRaw >> ch.pwr2Ave;

                // pull out an average for the next pass
                ch.sumRaw -= Dadc;

                // apply the polynomial
                // mV = ((u2A*Dadc + u2C) >> 10) - 2^^15
                logIParam.padcVoltNext[iVolt + j] = (int16_t) (((int32_t) ((ch.u2A * Dadc + ch.u2C) >> 10)) - 32768);
            }
            j++;
        }
    }

    i                           = cGroup * logIParam.cActiveCh;
    logIParam.padcVoltNext     += i;
    logIParam.cTotalSamples    += cGroup;

    if(logIParam.padcVoltNext >= (int16_t *) KVA_2_KSEG1(adcVoltageBuffer+CSVBUFF))         
    {
//int16_t   curData[2*840];
//memcpy(curData, KVA_2_KSEG1(adcVoltageBuffer+CBADCDMA-840), sizeof(curData));


        // this only works because our raw buffer is <= and an integer multple of the
        // voltage buffer. Otherwise it would be possible for the raw buffer not to
        // wrap at the same point as the voltage buffer.
        ASSERT(logIParam.padcVoltNext == (int16_t *) KVA_2_KSEG1(adcVoltageBuffer+CSVBUFF)) ;
        logIParam.padcVoltNext = (int16_t *) KVA_2_KSEG1(adcVoltageBuffer);
    }
}

STATE ADCTask(void)
{
    uint32_t    phyAddr     = 0;

    // we have background timing to consider
    // 1. Easy, one of the DMA channels are running and we get the pointer
    // 2. We switch as we are setting up the pointer, so try again
    // 3. We switch as we are checking the channels, if so, then one of the channels will be set so check again
    // 4. no channels are active, then do nothing.
    while(logIParam.state == Running)
    {
        uint32_t    phyOffset;

//#if 0
{
    static uint32 tCopy = ReadCoreTimer();
    int16_t volts[840];

    if(ReadCoreTimer() - tCopy > 2*CORE_TMR_TICKS_PER_SEC)
    {
        memcpy(volts, KVA_2_KSEG1(adcVoltageBuffer), sizeof(volts));
        tCopy = ReadCoreTimer();
    }
}
//#endif    
        
        // we need to initialize this on every loop
        // so if we exit with no update, the end pointer is not updated
        phyAddr = 0;

        // DMA channel 1 is active
        if(ADCSAMPDMA1.DCHxCON.CHEN)
        {
            phyOffset   = ADCSAMPDMA1.DCHxDPTR;
            phyAddr     = ADCSAMPDMA1.DCHxDSA + phyOffset;
           
            // if this DMA channel is still enabled, we are done
            // we know the channel will reset the DPTR to zero when done
            // we also know the half full flag will be set from our ISR
            // these two conditions, say we are done.
            // or, in this case, the logical NOT, that we are still active
            if(!(phyOffset == 0 && fDMA1HalfFull)) break;

            // The following does not work
            // the DPTR pointer will get reset to zero before the DMA is disabled
            // this cause a window where we will get the base address for the DMA channel
            // before the channel is turned off, and this will look like we went backwards
            // on the phy address
            // if(ADCSAMPDMA1.DCHxCON.CHEN) break;
        }

        // DMA channel 2 is active
        else if(ADCSAMPDMA2.DCHxCON.CHEN)
        {
            phyOffset   = ADCSAMPDMA2.DCHxDPTR;
            phyAddr     = ADCSAMPDMA2.DCHxDSA + phyOffset;
            
            // if this DMA channel is still enabled, we are done
            // we know the channel will reset the DPTR to zero when done
            // we also know the half full flag will be set from our ISR
            // these two conditions, say we are done.
            // or, in this case, the logical NOT, that we are still active
            if(!(phyOffset == 0 && fDMA2HalfFull)) break;
            
            // The following does not work
            // the DPTR pointer will get reset to zero before the DMA is disabled
            // this cause a window where we will get the base address for the DMA channel
            // before the channel is turned off, and this will look like we went backwards
            // on the phy address
            // if(ADCSAMPDMA2.DCHxCON.CHEN) break;
        }

        // neither channel was active when checked, but is it active now?
        // there is a very tiny window from when the DMA switch from channel 2 back to 1
        // right after the check for ch 1 fails, but ch 1 is enabled by the time we get to
        // the channel 2 test.
        else if(ADCSAMPDMA1.DCHxCON.CHEN || ADCSAMPDMA2.DCHxCON.CHEN)
        {
            continue;
        }

        // neither channel is active, get out
        // BEWARE, there is a possibility that there is a gap
        // between when both channels are off... we may need to 
        // just exit and try on the next loop.
        else
        {
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
        }
    }

    // update the end pointer
    if(phyAddr != 0) 
    {
        uint16_t *  padcDDRBufCur = (uint16_t *) KVA_2_KSEG1(phyAddr);

        while(padcDDRBufCur < logIParam.padcRawEnd) padcDDRBufCur += CSRBUFF;
        logIParam.padcRawEnd = padcDDRBufCur;
    }

    // see if we have a new value
    // we might want to use padcDDRBufCur, but if NULL, it is really 0xA0000000
    // because of the virtual address change, so look at phyAddr to see if it was set.
    if(logIParam.padcRawEnd > logIParam.padcRawStart)
    {
        uint32_t csDDR = logIParam.padcRawEnd - logIParam.padcRawStart;

        // right now put an assert in if we fall too far behind
        // in the future we may want to issue warning LEDs
        ASSERT(csDDR < 2000000);  // about 4 seconds behind; half our 8MB buffer

        // record the count
        loopStats.RecordCount(LOOPSTATS::ADCBACKLOG, csDDR);

        // if we have at least one samples worth of data
        if(csDDR >= logIParam.nCh) 
        {
            // roughly speaking, a conversion takes about half the time
            // it takes to sample at a maximum sample rate (conversions run 1 - 1.5us; max sample rate at 500000 is 2us/S).
            // if we have something that stalls the super loop for 2 seconds it will take ~1 second to process the samples
            // we can't do that in one conversion, especially if we stalled for a second already.
            // if we process 4ms, we will catch up 2ms, if we are 2 seconds behind, that would take 1000 loops to catch up.
            // if our average loop time is 0.2ms and we add 4ms, that would take 4200ms to catch up.
            // if we process for 10ms, and catch up 5ms per, that would be 400 loops at 10.2 or 4080ms
            // as you can see increasing the number process per pass only slowly reduces catch up time, but dramaticall
            // increases loop time. in fact, if we process only 2ms, catching up by 1ms, our loop time is 1.2 * 2000 = 4400ms
            // we might as well limit our loop time to 2ms, or about 2000 samples
            // we have 8 channels and we need a mult of 1,2,3,4,5,6,7,8 which is our 480 so say 480 * 5 = 2400 samples
            uint32_t csThisPass = min(min(csDDR, (uint32_t) (((uint16_t *) KVA_2_KSEG1(adcRawBuffer+CSRBUFF)) - logIParam.padcRawStart)), 2400ul);
            uint32_t cGroup = csThisPass / logIParam.nCh;

            // make the count a multiple of channels
            csThisPass = (cGroup * logIParam.nCh);

            // process the data
            ADCToVoltage(logIParam.padcRawStart, cGroup);

            logIParam.padcRawStart += csThisPass;
            csDDR                  -= csThisPass;

            if(logIParam.padcRawStart >= (uint16_t *) KVA_2_KSEG1(adcRawBuffer+CSRBUFF))
            {
                ASSERT(logIParam.padcRawStart == (uint16_t *) KVA_2_KSEG1(adcRawBuffer+CSRBUFF));
                logIParam.padcRawStart -= CSRBUFF;
                logIParam.padcRawEnd   -= CSRBUFF;
            }
        }

        // if we are done and only have a fractional group
        // kill the fractional group
        else if(logIParam.state != Running)
        {
            logIParam.padcRawEnd = logIParam.padcRawStart;
        }
    }

    return(Idle);
}


