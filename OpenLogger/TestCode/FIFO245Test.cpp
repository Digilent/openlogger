/************************************************************************/
/*                                                                      */
/*    FIFO245Test.cpp                                                   */
/*                                                                      */
/*    Code to test if the FIFO245 works                                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent8                                         */
/************************************************************************/
/*  Revision History:                                                   */
/*    6/20/2018(KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

// static const uint8_t  dPMP[2]   = {0b01010101, 0b10101010};
static uint8_t PHY __attribute__((coherent)) dPMP[60001];

// read/write runs at 2.844410 MHz
void __attribute__((nomips16, at_vector(FIFO245DMAVECTOR), interrupt(FIFO245IPLxSRS))) SwitchFIFO(void)
{
    FIFO245DMAIF                = 0;
    FIFO245DMA.DCHxINTClr       = 0xFFFFFFFF;   // clear all interrupts
    FIFO245DMA.DCHxINT.CHBCIE   = 1;            // reset the block end interrupt
    
    // wait for the module to be done, should be done already as it take
    // ~ 620ns to get into the ISR
    while(PMMODEbits.BUSY);

    // currently reading, going to writing
    // Writing runs at about 197nsec per byte (5.076142 MHz)
    if(FIFO245DMA.DCHxSSIZ == 1)
    {
        PMADDRbits.CS2              = 0;        // turn off PMA15 for CS Read, don't do a read cycle
        dPMP[sizeof(dPMP)-1]        = PMDIN;

        FIFO245DMA.DCHxECON.CHSIRQ  = _COMPARATOR_2_VECTOR;
        FIFO245DMA.DCHxSSA          = KVA_2_PA(dPMP)+1;  // transfer source physical address
        FIFO245DMA.DCHxSSIZ         = sizeof(dPMP)- 1;
        FIFO245DMA.DCHxDSA          = KVA_2_PA(&PMDIN);    // our input buffer
        FIFO245DMA.DCHxDSIZ         = 1;      
        
        // CS2 is 100ns + CMP delay time ~ 240ns total
//        PMMODEbits.WAITE    = 3;        // 4 wait states (10ns > 5ns & (WAITM + WAITE (40ns < 50ns))) --- for write this is 2 waits
        PMADDRbits.CS1      = 1;        // turn on PMA14 for CS

//        memcpy(dPMPT, dPMP, sizeof(dPMP));
//        PHY_2_CACHE(dPMP, sizeof(dPMP));
        
        FIFO245DMA.DCHxCON.CHEN    = 1;        // turn on the DMA module
        if(CM2CONbits.COUT == 1 && FIFO245DMA.DCHxCON.CHEDET != 1)  FIFO245DMA.DCHxECON.CFORCE = 1; 
    }
    
    // currently writing going to reading
    // reading runs at 159ns per byte (6.289308 MHz)
    else
    {
        FIFO245DMA.DCHxECON.CHSIRQ  = FIFO245RXFINT;
        FIFO245DMA.DCHxSSA          = KVA_2_PA(&PMDIN);  // transfer source physical address
        FIFO245DMA.DCHxSSIZ         = 1;                    // source size 2 bytes
        FIFO245DMA.DCHxDSA          = KVA_2_PA(dPMP);    // our input buffer
        FIFO245DMA.DCHxDSIZ         = sizeof(dPMP)-1; 

        // CS2 is 60 ns long + 50ns  on RFX# from the FTDI chip after for a total of 110ns
//        PMMODEbits.WAITE    = 0;        // 0 wait states (10ns > 5ns & (WAITM + WAITE (40ns < 50ns))) --- for write this is 2 waits
        PMADDRbits.CS1      = 0;        // turn off PMA14 for CS
        PMADDRbits.CS2      = 1;        // turn off PMA15 for CS

        FIFO245DMA.DCHxCON.CHEN    = 1;        // turn on the DMA module
        if(GetGPIO(FIFO245_PIN_RXF) == 0 &&  FIFO245DMA.DCHxCON.CHEDET != 1)   FIFO245DMA.DCHxECON.CFORCE = 1;    
    }
}

// Main program
STATE TestFIFO245(void) 
{
    // FIFO245 DMA
    FIFO245DMA.DCHxCONClr       = 0xFFFFFFFF;       // clear the DMA control register
    FIFO245DMA.DCHxECONClr      = 0xFFFFFFFF;       // clear extended control register
    FIFO245DMA.DCHxINTClr       = 0xFFFFFFFF;       // clear all interrupts
    FIFO245DMA.DCHxCSIZ         = 1;                // 1 bytes transferred per event (cell transfer)
    FIFO245DMA.DCHxCON.CHPRI    = FIFO245DMAPRI;                // run at priority 2                   
    FIFO245DMA.DCHxECON.SIRQEN  = 1;                // enable IRQ trigger
    FIFO245DMA.DCHxCON.CHAEN    = 0;                // run until full               
    
    // FIFO245 INTs
    FIFO245DMA.DCHxINT.CHBCIE   = 1;                // say we want a block transfer complete interrupt
    FIFO245DMAIF                = 0;                // clear DMA interrupt flag
    FIFO245DMAIE                = 1;                // enable DMA interrupt
    FIFO245DMAIPCP              = FIFO245ISRDMAPRI;  // Set the ISR priority
    FIFO245DMAIPCS              = 0;                // Set the ISR subpriority
 
    memset(dPMP, 0, sizeof(dPMP));

    FIFO245DMA.DCHxECON.CHSIRQ  = FIFO245RXFINT;    // trigger on RXF# going low
    FIFO245DMA.DCHxSSA          = KVA_2_PA(&PMDIN); // read, and start a read cycle
    FIFO245DMA.DCHxSSIZ         = 1;                // 1 byte PMP bus
    FIFO245DMA.DCHxDSA          = KVA_2_PA(dPMP);   // put in our output buffer
    FIFO245DMA.DCHxDSIZ         = sizeof(dPMP)-1;   // size is one less than the full size (leave room for another read)
    
    // set CS for read
    PMADDRbits.CS1      = 0;        // turn off PMA14 for CS
    PMADDRbits.CS2      = 1;        // turn on PMA15 for CS, read
    
    FIFO245DMA.DCHxCON.CHEN    = 1;        // turn on the DMA module
    if(GetGPIO(FIFO245_PIN_RXF) == 0 && FIFO245DMA.DCHxCON.CHEDET != 1)   FIFO245DMA.DCHxECON.CFORCE = 1;

    return Idle;
}

STATE FIFOEcho(void)
{
    static STATE    stateFIFO = Idle;
    static uint8_t  rgbFIFOBuff[8192];
    static uint32_t cbWritten = 0;
    static uint32_t cbRead = 0;
    static uint32_t cbTotal = 0;

    // see if we have data
    if(stateFIFO == Idle && jsonFT245.Available() > 0)
    {
        cbRead = jsonFT245.Available() > sizeof(rgbFIFOBuff) ? sizeof(rgbFIFOBuff) : jsonFT245.Available();

        cbWritten = 0;
        jsonFT245.Read(rgbFIFOBuff, cbRead, (int32_t *) &cbRead);
        cbTotal += cbRead;

        ASSERT(cbRead > 0);
        stateFIFO = jsonFT245.Write(rgbFIFOBuff, cbRead, (int32_t&) cbWritten, 1000ul);
    }

    // on an error, clean up and try again
    else if(IsStateAnError(stateFIFO))
    {
        if(cbWritten > 0)
        {
            memcpy(rgbFIFOBuff, &rgbFIFOBuff[cbWritten], cbWritten);
            cbRead -= cbWritten;
        }
        cbWritten = 0;
        stateFIFO = Pending;
    }

    // keep waiting until it is done.
    else if(stateFIFO != Idle)
    {
        stateFIFO = jsonFT245.Write(rgbFIFOBuff, cbRead, (int32_t&) cbWritten, 1000ul);
    }    

    if(stateFIFO == Idle && jsonFT245.Available() == 0) return(Idle);
    else return(Working);
}


