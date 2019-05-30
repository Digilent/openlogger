/************************************************************************/
/*                                                                      */
/*    FATTest.cpp                                                       */
/*                                                                      */
/*    Test that FAT File system and virtual thread is working           */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent                                          */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    6/25/2018 (KeithV): Created                                       */
/************************************************************************/
#include <OpenLogger.h>

static DFILE dFileTest;

typedef enum
{
    stateFSNone = FILECSTATE,
    stateFSChgDisk,
    stateFSOpenWrite,
    stateFSWrite,
    stateFSCloseWrite,
    stateFSOpenRead,
    stateFSRead,
    stateFSCompareData,
    stateFSClose
} STATEFS;

extern "C" uint8_t const _RESET_ADDR;
#define CBMAXCMP    8192
// #define CBCOPYFW 300000
#define CBCOPYFW 100000
// #define CBRWTEST 2000000
#define CBRWTEST CBCOPYFW
static uint8_t const * const bData = (uint8_t *) &_RESET_ADDR;
static uint8_t DDR bReadBack[CBRWTEST];
//static uint8_t DDR bWritePattern[CBRWTEST];
STATE FATTest(void)
{
    static STATE        stateFT         = Idle;
    static uint32_t     cb              = 0;
    static uint32_t     cbRW            = 0;

    uint32_t cbtp = 0;
    uint32_t tStart = ReadCoreTimer();
    FRESULT fr;

    switch(stateFT)
    {
        case Idle:
            stateFT         = WaitingForResources;
            // fall thru

        case WaitingForResources:
            if(DFATFS::fsvolmounted(DFATFS::szFatFsVols[VOLSD])) 
            {
                cb = 0;
                stateFT = stateFSChgDisk; 
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSChgDisk:
            if((fr = DFATFS::fschdrive(DFATFS::szFatFsVols[VOLSD])) == FR_OK && (fr = DFATFS::fschdir(DFATFS::szRoot)) == FR_OK)
            {
                stateFT = stateFSOpenWrite; 
            }
            else
            {
                stateFT = Idle; 
                Serial.println("unable to select drive");
                return(fr | STATEError);
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSOpenWrite:
            if((fr = dFileTest.fsopen("foo.txt", FA_CREATE_ALWAYS | FA_WRITE)) == FR_OK)
            {
                cb      = 0;
                cbRW    = 0;
                stateFT = stateFSWrite; 
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                stateFT = Idle; 
                Serial.println("unable to open file for write");
                return(fr | STATEError);
            }
            tStart = ReadCoreTimer() - tStart;
            ASSERT(tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSWrite:
            if(cb == CBRWTEST)
            {
                cb = 0;
                stateFT = stateFSCloseWrite; 
            }
            else if(cb < CBRWTEST)
            {
                cbtp = (CBRWTEST - cb) > CBCOPYFW ? CBCOPYFW : (CBRWTEST - cb);

                if((fr = dFileTest.fswrite(bData, cbtp, &cbRW)) == FR_OK)
                {
                    cb += cbRW;
                    cbRW = 0;
                }
                else if(fr != FR_WAITING_FOR_THREAD)
                {
                    stateFT = stateFSClose;
                    Serial.println("Failed to write file");
                    return(fr | STATEError);
                }
            }
            else
            {
                ASSERT(NEVER_SHOULD_GET_HERE);
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSCloseWrite:
            if(dFileTest) 
            {
                if((fr = dFileTest.fsclose()) == FR_OK) stateFT = stateFSOpenRead;  
                else ASSERT(fr == FR_WAITING_FOR_THREAD);
            }
            else
            {
                stateFT = stateFSOpenRead; 
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSOpenRead:
            if((fr = dFileTest.fsopen("foo.txt", FA_READ)) == FR_OK)
            {
                cb = 0;
                cbRW = 0;
                stateFT = stateFSRead; 
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                stateFT = Idle; 
                Serial.println("unable to open file for read");
                return(fr | STATEError);
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSRead:
            if((fr = dFileTest.fsread(bReadBack, CBRWTEST, &cb)) == FR_OK)
            {
                ASSERT(cb == CBRWTEST);
                cb = 0;
                cbRW = 0;
                stateFT = stateFSCompareData;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                stateFT = stateFSClose;
                Serial.println("Failed to read file");
                return(fr | STATEError);
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSCompareData:
            if(cb == CBRWTEST)
            {
                cb = 0;
                cbRW = 0;
                stateFT = stateFSClose; 
                Serial.println("SD Card Test Passed");
            }
 
            else if(cb < CBRWTEST)
            {
                cbtp = (cb % CBCOPYFW);
                if((cbtp + CBMAXCMP) > CBCOPYFW) cbRW = CBCOPYFW - cbtp;
                else cbRW = CBMAXCMP;
                if(cb + cbRW > CBRWTEST) cbRW = CBRWTEST - cb;
                if(memcmp(&bReadBack[cb], &bData[cbtp], cbRW) == 0) 
                {
                    cb += cbRW;
                    cbRW = 0;
                }
                else
                {
                    uint32_t i = 0;
                    uint32_t j = 0;
                    uint8_t  b1[32];
                    uint8_t  b2[32];
                    
                    for(i=0; i<cbRW && bReadBack[cb+i] == bData[cbtp+i]; i++);
                    
                    j = i & 0xFFFFFFF0;
                    memcpy(b1, &bReadBack[cb+j], 32);
                    memcpy(b2, &bData[cbtp+j], 32);
                                       
                    stateFT = stateFSClose;
                    Serial.println("Invalid buffer compare");  
                }
            }

            else
            {
                ASSERT(NEVER_SHOULD_GET_HERE);
            }

            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        case stateFSClose:
            if(dFileTest) 
            {
                if((fr = dFileTest.fsclose()) == FR_OK) stateFT = Idle;  
                else ASSERT(fr == FR_WAITING_FOR_THREAD);
            }
            else
            {
                stateFT = Idle; 
            }
            ASSERT(ReadCoreTimer() - tStart < CORE_TMR_TICKS_PER_MSEC);
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(stateFT);
}