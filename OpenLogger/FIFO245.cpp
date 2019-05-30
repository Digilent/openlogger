/************************************************************************/
/*                                                                      */
/*    FIFO245.cpp                                                       */
/*                                                                      */
/*    Implements the FIFO245 IO                                         */
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

void JSONFT245::PeriodicTask(void)
{
    uint32_t irq = FIFO245DMA.DCHxECON.CHSIRQ;
    
    // If someone else has the DMA, get out
    if(!(irq == irqDMANone || irq == FIFO245RXFINT))
    {
        return;
    }
    
    // see if we have a read to clean up.
    else if(irq == FIFO245RXFINT) 
    {
        // we better bereading
        ASSERT(PMADDRbits.CS2 == 1); 
        
        // if it is me, and the DMA is still running, see if we moved.
        if(IsDMAInUse(FIFO245DMA))
        {
            uint32_t iDMA = FIFO245DMA.DCHxDPTR;

            // we need to move in at least one loop, or nothing is coming in
            if(iDMACur < iDMA)
            {
                iDMACur = iDMA;
                cPass = 0;
                return;
            }
            else if(cPass < MAXPASSCNT)
            {
                cPass++;
                return;
            }

            // otherwise just suspend the DMA channel
            else 
            {
                FIFO245DMA.DCHxCON.CHEN = 0; 
                // wait for any transfer to complete.
                while(FIFO245DMA.DCHxCON.CHEDET);

                // we didn't read everything, but here is where we got too.
                cbDMA = (FIFO245DMA.DCHxINT.CHBCIF) ? cbDMA : FIFO245DMA.DCHxDPTR;
            }
        }

        // if we get here, we know the DMA channel is off
        // we also know that cbDMA will contain the number of bytes the DMA transferred

        // update the end pointer.
        iRdEnd += cbDMA;

        // we also know there is at least one valid byte in PMDIN
        ASSERT(FIFO245DMA.DCHxCON.CHEN == 0);
        PMADDRbits.CS2 = 0;                         // make sure this is zero so we don't cause another read cycle
        ffReadK1[(iRdEnd % sizeof(ffRead))] = PMDIN;  // read the byte, no read cycle generated.
        iRdEnd++;                                   // add the byte to the count
        cbDMA = 0;
        FIFO245DMA.DCHxECON.CHSIRQ = irqDMANone;    // give up the shared DMA
    }

    // if there is an input, set up the DMA for input
    else if(GetGPIO(FIFO245_PIN_RXF) == 0 && (iRdEnd - iRdStart) < (int32_t) sizeof(ffRead))
    {
        volatile uint32_t bPMP;
        uint32_t tStart = ReadCoreTimer();
        
        cbDMA = ((int32_t) sizeof(ffRead)) - iRdEnd + (iRdEnd < (int32_t) sizeof(ffRead) ? 0 : iRdStart);

        ASSERT(iRdEnd - iRdStart <= (int32_t) sizeof(ffRead));
        ASSERT(cbDMA >= 1);
        
        cbDMA--;           // always allow for the final read of PMDIN to take place.

        // FIFO245 DMA
        FIFO245DMA.DCHxCONClr       = 0xFFFFFFFF;       // clear the DMA control register
        FIFO245DMA.DCHxECONClr      = 0xFFFFFFFF;       // clear extended control register
        FIFO245DMA.DCHxINTClr       = 0xFFFFFFFF;       // clear all interrupts
        FIFO245DMA.DCHxCSIZ         = 1;                // 1 bytes transferred per event (cell transfer)
        FIFO245DMA.DCHxCON.CHPRI    = FIFO245DMAPRI;    // run at priority 2                   
        FIFO245DMA.DCHxECON.SIRQEN  = 1;                // enable IRQ trigger
        FIFO245DMA.DCHxCON.CHAEN    = 0;                // run until full  

        FIFO245DMA.DCHxECON.CHSIRQ  = irqDMANone;       // Don't set the trigger yet, if we don't turn on the DMA, we want this irqDMANone
        FIFO245DMA.DCHxSSA          = KVA_2_PA(&PMDIN); // read, and start a read cycle
        FIFO245DMA.DCHxSSIZ         = 1;                // 1 byte PMP bus
        FIFO245DMA.DCHxDSA          = KVA_2_PA(ffRead) + (iRdEnd % sizeof(ffRead)) ; // put in our output buffer
        FIFO245DMA.DCHxDSIZ         = cbDMA;   // size is one less than the full size (leave room for another read)

        iDMACur                     = 0;
        cPass                       = 0;
        cStart++;
                                                        
        // make sure at least 10ns (set to 50ns) have passed for RXF glitch control 
        // if we execute 10 MIPS instructions, from the last RXF pin read, this assert should pass
        ASSERT(ReadCoreTimer() - tStart > 5);           

        // iff we still have indication of an input, any 10ns glitch has passed and it is real data to read
        // the glitch comes from switching from tri-state (MPSSE) to RXF# (FIFO245) on the FTDI chip
        if(GetGPIO(FIFO245_PIN_RXF) == 0)
        {
            // even if there is only 1 byte to read, and we don't turn on the DMA, 
            // we need to set these so we will read the primed value out of the PMDIN buffer.
            PMADDRbits.CS1              = 0;                // turn off PMA14 for CS
            PMADDRbits.CS2              = 1;                // turn on PMA15 for CS, read            
            FIFO245DMA.DCHxECON.CHSIRQ  = FIFO245RXFINT;    // now trigger on RXF# going low
            
            // we know the RXF pin is low, and staying low... so we know we will not get a high-low int until after
            // the next read. Turning this on now will not cause a DMA transfer as there is no edge on the RXF# line
            // also, only turn this on if we have more than 1 byte to read because we will prime PMDIN with one byte
            if(cbDMA > 0) FIFO245DMA.DCHxCON.CHEN = 1;                    // turn on the DMA module
            
            // we need to do a dummy read to prime the pump
            // on completion it will cause the DMA to continue until complete or we stop it
            bPMP = PMDIN;
            
            // to remove a "set but not used" warning.
            (void) bPMP;
        }
    }
}

uint32_t JSONFT245::Available(void)
{
    return(iRdEnd - iRdStart);
}

STATE JSONFT245::Read(void * const pBuff, uint32_t cbBuff, int32_t * const pcbRead)
{
    uint8_t *   pbBuff          = (uint8_t *) pBuff;
    uint32_t    cbAvailable     = Available();

    if(cbBuff > cbAvailable) cbBuff = cbAvailable;

    if(cbBuff > 0)
    {
        uint32_t cb1 = (sizeof(ffRead) - iRdStart) > cbBuff ? cbBuff : sizeof(ffRead) - iRdStart;
        uint32_t cb2 = cbBuff - cb1;

        // copy out the data
        memcpy(pbBuff, &ffReadK1[iRdStart], cb1);
        if(cb2 > 0) memcpy(&pbBuff[cb1], ffReadK1, cb2);
        iRdStart += cbBuff;

        // update my pointers
        cb1         = iRdEnd - iRdStart;
        iRdStart   %= sizeof(ffRead);
        iRdEnd      = iRdStart + cb1;
    }

    *pcbRead = cbBuff;
    return(Idle);
}

int32_t JSONFT245::Read(void)
{
    if(Available() > 1)
    {
        int32_t ret = (int32_t) ffReadK1[iRdStart++];

        // update my pointers
        if(iRdStart >= (int32_t) sizeof(ffRead))
        {
            uint32_t cb = iRdEnd - iRdStart;

            iRdStart   %= sizeof(ffRead);
            iRdEnd      = iRdStart + cb;
         }

        return(ret);
    }

    return(-1);
}

STATE JSONFT245::Write(void const * const  pBuff, uint32_t cbBuff, int32_t& cbWritten, uint32_t msTimeout)
{
    static STATE stateTimeout = Idle;
    static uint32_t tTimeout = 0;
    
    uint32_t irq = FIFO245DMA.DCHxECON.CHSIRQ;
    
    // See if we are ready for a write
    if(irq == irqDMANone && PMADDRbits.CS1 == 0 && GetGPIO(FIFO245_PIN_TXE) == 0)
    {
        // the CS should be high, so the comparator should be ready
        ASSERT(IsDMAInUse(FIFO245DMA) == false);
        ASSERT(PMADDRbits.CS2 == 0);                // we are not reading
        ASSERT(CM2CONbits.COUT == 1);               // the FTDI is ready for characters
        
        if(((void *) pBuff) != KVA_2_KSEG1(pBuff)) CACHE_2_PHY(pBuff, cbBuff);

        // FIFO245 DMA
        FIFO245DMA.DCHxCONClr       = 0xFFFFFFFF;       // clear the DMA control register
        FIFO245DMA.DCHxECONClr      = 0xFFFFFFFF;       // clear extended control register
        FIFO245DMA.DCHxINTClr       = 0xFFFFFFFF;       // clear all interrupts
        FIFO245DMA.DCHxCSIZ         = 1;                // 1 bytes transferred per event (cell transfer)
        FIFO245DMA.DCHxCON.CHPRI    = FIFO245DMAPRI;                // run at priority 2                   
        FIFO245DMA.DCHxECON.SIRQEN  = 1;                // enable IRQ trigger
        FIFO245DMA.DCHxCON.CHAEN    = 0;                // run until full  

        FIFO245DMA.DCHxECON.CHSIRQ  = _COMPARATOR_2_VECTOR;
        FIFO245DMA.DCHxSSA          = KVA_2_PA(pBuff);    // transfer source physical address
        FIFO245DMA.DCHxSSIZ         = cbBuff;
        FIFO245DMA.DCHxDSA          = KVA_2_PA(&PMDIN);     // our input buffer
        FIFO245DMA.DCHxDSIZ         = 1;      
        ASSERT(PMADDRbits.CS2 == 0);
        PMADDRbits.CS1              = 1;                    // turn on PMA14 for CS write
        PMADDRbits.CS2              = 0;                    // turn off PMA15 

        FIFO245DMA.DCHxCON.CHEN    = 1;                     // turn on the DMA module                                                          
        FIFO245DMA.DCHxECON.CFORCE = 1;                     // start the DMA

        // say we are working
        stateTimeout = Working;
        tTimeout = ReadCoreTimer();
        return(stateTimeout);
    }

    // if we are writing, see if we are done.
    else if(irq == _COMPARATOR_2_VECTOR && !IsDMAInUse(FIFO245DMA))
    {
        ASSERT(PMADDRbits.CS1 == 1);
        PMADDRbits.CS1 = 0;
        FIFO245DMA.DCHxECON.CHSIRQ = irqDMANone;
        cbWritten =  cbBuff;
        stateTimeout = Idle;
        return(Idle);
    }

    else if(stateTimeout != Idle)
    {
        uint32_t tcoreTick = (msTimeout > (0x80000000 / CORE_TMR_TICKS_PER_MSEC)) ? 0x80000000 : (msTimeout * CORE_TMR_TICKS_PER_MSEC);

        if(msTimeout != FOREVERTO && (ReadCoreTimer() - tTimeout) >= tcoreTick)
        {
            // if we were in the process of doing a write and we timed-out
            if(PMADDRbits.CS1 == 1)
            {
                // suspend the DMA
                FIFO245DMA.DCHxCON.CHEN = 0;

                // wait for any transfer to complete.
                while(FIFO245DMA.DCHxCON.CHEDET);

                PMADDRbits.CS1 = 0;
                cbWritten =  (FIFO245DMA.DCHxINT.CHBCIF) ? cbBuff : FIFO245DMA.DCHxDPTR;
                FIFO245DMA.DCHxECON.CHSIRQ = irqDMANone;
            }

            stateTimeout = Idle;
            if(FIFO245DMA.DCHxINT.CHBCIF) return(Idle);
            return(TimeoutErr);
        }

        // keep waiting
        else
        {
            return(stateTimeout);
        }
    }
  
    //  otherwise we need to wait for the DMA
    else
    {
        stateTimeout = WaitingForResources;
        tTimeout = ReadCoreTimer();
        return(WaitingForResources);
    }

    ASSERT(NEVER_SHOULD_GET_HERE);
    return(Unimplemented);
}

void JSONFT245::Purge(void)
{
    iRdEnd %= sizeof(ffRead);
    iRdStart = iRdEnd;
}
