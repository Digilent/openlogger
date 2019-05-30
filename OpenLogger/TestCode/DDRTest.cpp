/************************************************************************/
/*                                                                      */
/*    DDRTest.cpp                                                       */
/*                                                                      */
/*    Test that the DDR memory is working properly                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent                                          */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    6/15/2018 (KeithV): Created                                       */
/************************************************************************/
#include <OpenLogger.h>

// just some data to compare against
extern "C" void InitSFRs(void);
extern bool HTTPEnable(bool fEnable);

#define DELAY       for(int dj=0; dj<4096; dj++)
#define CBDDRTOTAL  64000
#define CSELEMENTS  (CBDDRTOTAL/sizeof(uint16_t))

uint8_t DDR     ddrMem[CBDDRTOTAL]; // DDR MEMORY
uint8_t         rBuf[CBDDRTOTAL];   // result after reading DDR

#define CBEOMT    0x2000
uint8_t *                                               pddr = (uint8_t *) (0xA9FFFFFF - CBEOMT);

STATE TestDDR(void)
{    
    uint16_t    sBuf[CSELEMENTS];   // the source buffer to fill with sequential data
    int         fRet = false;
    
    // test memcpy to non-cached DDR memory
    memset(sBuf, 0, sizeof(sBuf));
    memset(KVA_2_KSEG1(ddrMem), 0, sizeof(ddrMem));
    memset(rBuf, 0, sizeof(rBuf));
    for(unsigned i=0; i<CSELEMENTS; i++) sBuf[i] = i;
    memcpy(KVA_2_KSEG1(ddrMem), sBuf, sizeof(sBuf));
    memcpy(rBuf, KVA_2_KSEG1(ddrMem), sizeof(ddrMem));
    fRet = (memcmp(rBuf, sBuf, sizeof(sBuf)) == 0);
   
    // test to cached DDR memory
    memset(ddrMem, 0, sizeof(ddrMem));
    memcpy(ddrMem, (void *) HTTPEnable, sizeof(ddrMem));  
    fRet = fRet && (memcmp(ddrMem, (void *) HTTPEnable, sizeof(ddrMem)) == 0);
  
    // test end of DDR memory
    memset(pddr, 0, CBEOMT);
    memcpy(pddr, (void *) InitSFRs, CBEOMT);  
    fRet = fRet && (memcmp(pddr, (void *) InitSFRs, CBEOMT) == 0);
   
    // Test DMA to DDR
    DCH2CON             = 0;                            // clear the DMA control register
    DCH2ECON            = 0;                            // clear extended control register
    DCH2INT             = 0;                            // clear all interrupts
    DCH2CONbits.CHPRI   = 2;                            // run at priority 2                   
    DCH2ECONbits.SIRQEN = 0;                            // no IRQ trigger
    DCH2CONbits.CHAEN   = 0;                            // run until full               
    DCH2CSIZ            = sizeof(rBuf);                 // 1 bytes transferred per event (cell transfer)
    DCH2SSA             = KVA_2_PA(InitSFRs);           // read, and start a read cycle
    DCH2SSIZ            = sizeof(rBuf);                 // 1 byte PMP bus
    DCH2DSA             = KVA_2_PA(ddrMem);             // put in our output buffer
    DCH2DSIZ            = sizeof(rBuf);                 // size is one less than the full size (leave room for another read)
    
    DCH2CONbits.CHEN    = 1;                             
    DCH2ECONbits.CFORCE = 1;
          
    while(DCH2CONbits.CHEN) DELAY;
    
    // Test DMA from DDR
    DCH2CON             = 0;                            // clear the DMA control register
    DCH2ECON            = 0;                            // clear extended control register
    DCH2INT             = 0;                            // clear all interrupts
    DCH2CONbits.CHPRI   = 2;                            // run at priority 2                   
    DCH2ECONbits.SIRQEN = 0;                            // no IRQ trigger
    DCH2CONbits.CHAEN   = 0;                            // run until full               
    DCH2CSIZ            = sizeof(rBuf);                 // 1 bytes transferred per event (cell transfer)
    DCH2SSA             = KVA_2_PA(ddrMem);             // read, and start a read cycle
    DCH2SSIZ            = sizeof(rBuf);                 // 1 byte PMP bus
    DCH2DSA             = KVA_2_PA(rBuf);               // put in our output buffer
    DCH2DSIZ            = sizeof(rBuf);                 // size is one less than the full size (leave room for another read)
    
    DCH2CONbits.CHEN    = 1;                             
    DCH2ECONbits.CFORCE = 1;
  
    while(DCH2CONbits.CHEN) DELAY;

    // get it back into cached memory
    PHY_2_CACHE((void *) rBuf, sizeof(rBuf));
    
    // Clear the DMA
    DCH2CON         = 0;                                // clear the DMA control register
    DCH2ECON        = 0;                                // clear extended control register
    DCH2INT         = 0;                                // clear all interrupts  

    // return if all tests passed
    fRet = (fRet && (memcmp((void *) InitSFRs, (void *) rBuf, sizeof(rBuf)) == 0));

    if(fRet)        Serial.println("DDR Memory Test Passed");
    else            Serial.println("DDR Memory Test Failed");

    return(Idle);
}
    