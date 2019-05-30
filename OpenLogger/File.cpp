/************************************************************************/
/*                                                                      */
/*    File.cpp                                                          */
/*                                                                      */
/*    File system code                                                  */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    12/20/2017 (KeithV): Created                                       */
/************************************************************************/
#include <OpenLogger.h>

static const char  szCal[]       = "CAL";

/************************************************************************/
/*    Instrument Calibration Constants                                  */
/*    Results are in nVolts                                             */
/************************************************************************/
// uVDC = -40000pwm + 7000000
// B ~ -40000, C ~ 7000000
// but remember, we store (-B)
static POLYCAL arCalTDC[3]     =   {
                                {CALVER, Uncalibrated, 0, 40000, 7000000},   // first entry is ideal
                                {CALVER, Uncalibrated, 0, 40000, 7000000}, 
                                {CALVER, Uncalibrated, 0, 40000, 7000000}
                            };

static AWGCAL  calTAWG         =   {   {CALVER, Uncalibrated, 0, 11995, 2099125}, 511, 175, {0}, {0}  };

// uVin = (3)(5^^10)(Dadc)/((2^^10)(87)) - (3)(5^^10)(2^^5)/87
// uVin = (29,296,875)[(Dadc/89,088) - (32/87)]
// uVin = (29,296,875)(Dadc)/(89,088) - 937,500,000/87
// uVin = (328.85321)Dadc - 10,775,862
// nVin = (328,853.21)Dadc - 10,775,862,000
static POLYCAL arCalTDAQ[9]    =   {
                                {CALVER, Uncalibrated, 328853, 0, 10775862000},     // first entry is ideal  
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}
                            };

bool fIsCalibrated(void)
{
    uint32_t i;

    bool fCalibrated = calIAWG.polyCal.state == Calibrated;

    // Check that all are calibrated
    for(i=1; i<3; i++) fCalibrated &= arCalIDC[i].state == Calibrated;
    for(i=1; i<8; i++) fCalibrated &= arCalIDAQ[i].state == Calibrated;

    return(fCalibrated);
}

STATE SaveCalibrationData(VOLTYPE vol)
{
    static STATE curState = Idle;
    static STATE retState = Idle;
    static uint32_t bw;

    FRESULT     fr = FR_OK;
    uint32_t    fPos;
    char        szFileName[64];

    switch(curState)
    {
        case Idle:

            // init 
            retState = Idle;
            bw = 0;

            // get the file name to open
            if(FileCreateFileName(szCal, MKSTR(CALVER), szCal, szFileName, sizeof(szFileName)) == 0)    ASSERT(NEVER_SHOULD_GET_HERE);

            // SD card not loaded
            if(vol == VOLSD && !fSd0)                                                                   return(MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM));

            // say we are calibrating
            if(!fileTask.SetUsage(FILETask::CALIBRATING, vol, szFileName))                              return(MkStateAnError(FILECSTATE | FR_LOCKED));

            curState = FILEopen;
            // fall thru

        case FILEopen:
            fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsopen(fileTask.GetPath(FILETask::CALIBRATING, true), FA_WRITE | FA_OPEN_ALWAYS);
            if(fr == FR_OK)                         curState = FILEwrite;
            else if(fr != FR_WAITING_FOR_THREAD)    retState = MkStateAnError(FILECSTATE | fr);
            break;

        case FILEwrite:
            fPos = FILETask::rgpdFile[FILETask::CALIBRATING]->fstell();

            if(fPos < sizeof(arCalIDC))                                             fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fswrite(arCalIDC, sizeof(arCalIDC), &bw);
            else if(fPos < sizeof(calIAWG) + sizeof(arCalIDC))                      fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fswrite(&calIAWG, sizeof(calIAWG), &bw);
            else if(fPos < sizeof(arCalIDAQ) + sizeof(calIAWG) + sizeof(arCalIDC))  fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fswrite(arCalIDAQ, sizeof(arCalIDAQ), &bw);
            else
            {
                ASSERT(FILETask::rgpdFile[FILETask::CALIBRATING]->fstell() == sizeof(arCalIDAQ) + sizeof(calIAWG) + sizeof(arCalIDC));
                curState = FILEclose;
            }

            if(fr == FR_OK)                         bw = 0;                                         // all good
            else if(fr != FR_WAITING_FOR_THREAD)    retState = MkStateAnError(FILECSTATE | fr);     // a problem
            break;

        case FILEclose:
            fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsclose();
            if(fr != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::CALIBRATING);
                curState = Idle;
                return(retState);
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    if(IsStateAnError(retState))
    {
        if(*(FILETask::rgpdFile[FILETask::CALIBRATING]))
        {
            curState = FILEclose;
        }
        else
        {
            fileTask.ClearUsage(FILETask::CALIBRATING);
            curState = Idle;
            return(retState);
        }      
    }

    return(curState);
}

STATE ReadCalibrationData(VOLTYPE vol)
{
    static STATE curState = Idle;
    static STATE retState = Idle;
    static uint32_t br;

    FRESULT     fr = FR_OK;
    uint32_t    fPos;
    char        szFileName[64];

    switch(curState)
    {
        case Idle:

            // init 
            retState    = Idle;
            br          = 0;

            // get the file to open
            if(FileCreateFileName(szCal, MKSTR(CALVER), szCal, szFileName, sizeof(szFileName)) == 0)    ASSERT(NEVER_SHOULD_GET_HERE);

            // SD card not loaded
            if(vol == VOLSD && !fSd0)                                                                   return(MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM));

            // say we are calibrating
            if(!fileTask.SetUsage(FILETask::CALIBRATING, vol, szFileName))                              return(MkStateAnError(FILECSTATE | FR_LOCKED));
 
            curState = FILEopen;
            // fall thru

        case FILEopen:
            fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsopen(fileTask.GetPath(FILETask::CALIBRATING, true), FA_READ);
            if(fr == FR_OK)                         curState = FILEseek;
            else if(fr != FR_WAITING_FOR_THREAD)    retState = MkStateAnError(FILECSTATE | fr);
            break;

        case FILEseek:
            fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fslseek(0);
            if(fr == FR_OK)                         curState = FILEread;
            else if(fr != FR_WAITING_FOR_THREAD)    retState = MkStateAnError(FILECSTATE | fr);
            break;

        case FILEread:
            fPos = FILETask::rgpdFile[FILETask::CALIBRATING]->fstell();

            if(fPos < sizeof(arCalTDC))                                             fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsread(arCalTDC, sizeof(arCalTDC), &br);
            else if(fPos < sizeof(calTAWG) + sizeof(arCalTDC))                      fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsread(&calTAWG, sizeof(calTAWG), &br);
            else if(fPos < sizeof(arCalTDAQ) + sizeof(calTAWG) + sizeof(arCalTDC))  fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsread(arCalTDAQ, sizeof(arCalTDAQ), &br);
            else
            {
                ASSERT(FILETask::rgpdFile[FILETask::CALIBRATING]->fstell() == sizeof(arCalTDAQ) + sizeof(calTAWG) + sizeof(arCalTDC));
                memcpy(arCalIDC, arCalTDC, sizeof(arCalTDC));
                memcpy(&calIAWG, &calTAWG, sizeof(calTAWG));
                memcpy(arCalIDAQ, arCalTDAQ, sizeof(arCalTDAQ));

                // a little backwards compatibility, we know that only calibrated stuff is saved, 
                // we know the state is calibrated; because the enum changes with code updates, 
                // for the state to calibrated.
                calIAWG.polyCal.state   = Calibrated;
                arCalIDC[1].state       = Calibrated;
                arCalIDC[2].state       = Calibrated;
                arCalIDAQ[1].state      = Calibrated;
                arCalIDAQ[2].state      = Calibrated; 
                arCalIDAQ[3].state      = Calibrated; 
                arCalIDAQ[4].state      = Calibrated;
                arCalIDAQ[5].state      = Calibrated; 
                arCalIDAQ[6].state      = Calibrated; 
                arCalIDAQ[7].state      = Calibrated; 
                arCalIDAQ[8].state      = Calibrated;

                fCalibrated = true;
                curState = FILEclose;
            }

            // is everything working
            if(fr == FR_OK)                         br          = 0;
            else if(fr != FR_WAITING_FOR_THREAD)    retState    = MkStateAnError(FILECSTATE | fr);         
            break;

        case FILEclose:
            fr = FILETask::rgpdFile[FILETask::CALIBRATING]->fsclose();
            if(fr != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::CALIBRATING);
                curState = Idle;
                return(retState);
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // check for errors and clean up
    if(IsStateAnError(retState))
    {
        if(*(FILETask::rgpdFile[FILETask::CALIBRATING]))
        {
            curState = FILEclose;
        }
        else
        {
            fileTask.ClearUsage(FILETask::CALIBRATING);
            curState = Idle;
            return(retState);
        }      
    }

     return(curState);
}

STATE FileOpenFlash(void)
{
    FRESULT fr = FR_WAITING_FOR_THREAD;
    static STATE curState = Idle;

    switch(curState)
    {
        case Idle:
        case FILEVerify:
            if(DFATFS::fsvolmounted(DFATFS::szFatFsVols[VOLFLASH]))
            {
                curState = Idle;
                break;
            }

            curState = FILEmount;
            // fall thru

        case FILEmount:
            if((fr = DFATFS::fsmount(flashVol, DFATFS::szFatFsVols[VOLFLASH], 1)) == FR_OK) 
            {
                fr = FR_WAITING_FOR_THREAD;
                curState = FILEVerify;
            }
            else if(fr == FR_NO_FILESYSTEM)
            {
                fr = FR_WAITING_FOR_THREAD;
                curState = FILEmkfs;
            }
            break;

        case FILEmkfs:
            if((fr = DFATFS::fsmkfs(flashVol)) == FR_OK)
            {
                fr = FR_WAITING_FOR_THREAD;
                curState = FILEVerify;
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    if(fr != FR_WAITING_FOR_THREAD)
    {
        curState = Idle;
        return(MkStateAnError(FILECSTATE | fr));
    }

    return(curState);
}

char const * const FILETask::MkQualifiedPath(VOLTYPE vol, char const * const szPath)
{
    char const * const szVol = DFATFS::szFatFsVols[vol];
    uint32_t cchVol = strlen(szVol);
    uint32_t cchRoot = strlen(DFATFS::szRoot);
    uint32_t i      = 0;

    if(szPath == NULL) return(NULL);
    
    memcpy(szQualPath, szVol, cchVol);
    if(memcmp(szPath, szVol, cchVol) == 0) i += cchVol;

    memcpy(&szQualPath[cchVol], DFATFS::szRoot, cchRoot);
    if(memcmp(&szPath[i], DFATFS::szRoot, cchRoot) == 0) i += cchRoot;
    
    strcpy(&szQualPath[cchVol+cchRoot], &szPath[i]);

    return(szQualPath);
}

STATE SdHotSwapTask(void)
{
    static STATE state          = Idle;
    static STATE nextState      = Idle;
    static uint32_t tStart      = 0;
    static uint32_t cClusters   = 0;
    static uint32_t cSectors    = 0;
    FRESULT fr = FR_OK;

    switch(state)
    {
    case Idle:

        // if we see a low on the pin
        // something is stuffed in the SD card
        if(!GetGPIO(SD_PIN_DET))
        {
            fSd0Trans = true;
            state = Waiting;
            tStart = ReadCoreTimer();
        }
        else
        {
            fSd0Trans = false;
            fSd0 = false;
        }
        break;

    case Waiting:
        // if the SD card is still in after our wait time, mount it
        if(ReadCoreTimer() - tStart >= (SDWAITTIME * CORE_TMR_TICKS_PER_MSEC) && !GetGPIO(SD_PIN_DET))
        {
            // if it is mounted, unmount it.
            if(DFATFS::fsvolmounted(DFATFS::szFatFsVols[VOLSD])) 
            {
                nextState   = SDMounting;
                state       = FILEunmount;
            }
            else state      = SDMounting;
        }
        break;

    case SDMounting:

        // now mount the volume, don't care if succeeds, just try
        if((fr = DFATFS::fsmount(dSDVol, DFATFS::szFatFsVols[VOLSD], 1)) == FR_OK)
        {
            cClusters = 0;
            fSd0 = true;
            state = FILEgetsize;
        }
        else if(fr != FR_WAITING_FOR_THREAD)
        {
            Serial.print("SD card detected but unable to mount. FR = 0x");
            fSd0 = false;
            Serial.println(fr, 16);
            state = SDInserted;
        }
        fSd0Trans = false;
        break;

    case FILEgetsize:
        if((fr = DFATFS::fsgetfree(DFATFS::szFatFsVols[VOLSD], &cClusters, &cSectors)) != FR_WAITING_FOR_THREAD)
        {
            if(cClusters > 0)
            {
                Serial.println("SD card detected and mounted");
                Serial.print(cClusters, 10);
                Serial.println(" free clusters");
            }
            else
            {
                Serial.println("SD card detected, but is either full or corrupted");
                Serial.println("Please free up space on the card");
            }

            if(fCalibrated) state = SDInserted;
            else state = SDReadCalibration;
        }
        break;

    case SDReadCalibration:
        // pause until we are in the main loop
        if(jsonFT245.state == JSONListening)
        {
            STATE retState = ReadCalibrationData(VOLSD);

            if(retState == Idle)
            {
                Serial.println("Calibration data was loaded from the SD Card");
                state = SDInserted;
            }
            else if(IsStateAnError(retState))
            {
                Serial.print("No Calibration data was found on the SD Card, Error: 0x");
                Serial.println(retState, 16);
                Serial.println();
                state = SDInserted;
             }
        }
        break;

    case SDInserted:

        // if someone pulls the SD card out, unmount it
        if(GetGPIO(SD_PIN_DET))
        {
             fSd0 = false;
             fSd0Trans = true;

            // SD card removed
            Serial.println("SD card removed");

            state       = FILEunmount;
            nextState   = Idle;
        }
        break;

    case FILEunmount:
        if(DFATFS::fsunmount(DFATFS::szFatFsVols[VOLSD]) != FR_WAITING_FOR_THREAD)
        {
            state = nextState;
        }
        break;

    default:
        ASSERT(NEVER_SHOULD_GET_HERE);                      // we should never get here
        break;
    }

    return(Idle);
}

// file name is PREFIX_MAC_SUFFIX.EXT
uint32_t FileCreateFileName(char const * const szPrefix, char const * const szSuffix, char const * const szExt, char * sz, uint32_t cb)
{
    uint32_t cbPrefix       = (szPrefix != NULL) ? strlen(szPrefix) + 1 : 0;                // Any prefex string + trailing _
    uint32_t cbSuffix       = (szSuffix != NULL) ? strlen(szSuffix) + 1 : 0;                // Any suffix string + leading _
    uint32_t cbExt          = (szExt != NULL) ? strlen(szExt) + 1 : 0;                      // Any Extenstion string + leading .
    uint32_t cbStr          = cbPrefix + cbSuffix + cbExt + 2*sizeof(MACADDR) + 1;          // cbPrefix + cbSuffix + cbExt + 12 char for MAC + null terminator           
    uint32_t  i             = 0;

    // Is the provided string long enough?
    if(cb < cbStr) 
    {
        return(0);
    }

    // file name is PREFIX_MAC_SUFFIX.EXT

    // PREFIX
    if(cbPrefix) 
    {
        i += cbPrefix;
        cbPrefix--;
        memcpy(sz, szPrefix, cbPrefix);
        sz[cbPrefix] = '-';
    }

    // MAC
    for(unsigned j=0; j<sizeof(MACADDR); j++)
    {
        if(macMyMacAddr.u8[j] < 16) sz[i++] = '0';
        itoa(macMyMacAddr.u8[j], &sz[i], 16);
        i += strlen(&sz[i]);
     }

    // SUFFIX
    if(cbSuffix)
    {
        sz[i++]  = '_';          // add _
        cbSuffix--;
        memcpy(&sz[i], szSuffix, cbSuffix);
        i += cbSuffix;
    }

    // EXT
    if(cbExt)
    {
        sz[i++]  = '.';          // add .
        cbExt--;
        memcpy(&sz[i], szExt, cbExt);
        i += cbExt;
    }

    // put in null terminator.
    sz[i++] = '\0';

    ASSERT(i == cbStr);

    return(cbStr);
}

#if(FR_OK != Idle) 
#error FR_OK and Idle both need to be zero
#endif

GCMD::ACTION OSPAR::WriteFile(char const pchWrite[], int32_t cbWrite, int32_t& cbWritten)
{
    static uint32_t cbWrittenInProgress = 0;
    FRESULT fr = FR_OK;
    GCMD::ACTION retAction = GCMD::CONTINUE;

    cbWritten = 0;
    if(*(FILETask::rgpdFile[FILETask::WRITING]))
    {
        if(cbWrite == 0 || cbWrittenInProgress == ((uint32_t) -1))
        {
            fr = FILETask::rgpdFile[FILETask::WRITING]->fsclose();
        }

        else if((fr = FILETask::rgpdFile[FILETask::WRITING]->fswrite(pchWrite, cbWrite, &cbWrittenInProgress)) == FR_OK)
        {
            cbWritten = cbWrittenInProgress;
            cbWrittenInProgress = 0;
            retAction = GCMD::READ;
        }

        else if(fr != FR_WAITING_FOR_THREAD)
        {
            cbWritten = cbWrittenInProgress;
            cbWrittenInProgress = ((uint32_t) -1);
        }     
    }

    else
    {
        ASSERT(fileTask.GetPath(FILETask::WRITING, true) != NULL);
        fileTask.ClearUsage(FILETask::WRITING);
        cbWrittenInProgress = 0;
        retAction = GCMD::DONE;
    }

    return(retAction);
}

GCMD::ACTION OSPAR::ReadFile(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead)
{
    static uint32_t cbMax = 0;
    static uint32_t cbReadInProgress = 0;
    ODATA& oData = odata[iOData];
    FRESULT fr = FR_OK;
    GCMD::ACTION retAction = GCMD::CONTINUE;
    
    static_assert(sizeof(pchJSONRespBuff) >= DFILE::FS_DEFAULT_BUFF_SIZE, "pchJSONRespBuff is too small");

    // if the file is open, start returning data
    if(*(FILETask::rgpdFile[FILETask::READING]))
    {
        uint32_t fsize = FILETask::rgpdFile[FILETask::READING]->fssize();
        uint32_t ftell = FILETask::rgpdFile[FILETask::READING]->fstell();
        
        if(cbMax == 0) cbMax = min(min(fsize - ftell, sizeof(pchJSONRespBuff)), oData.cb - oData.iOut);
 
        if(cbMax > 0)
        {
            fr = FILETask::rgpdFile[FILETask::READING]->fsread(pchJSONRespBuff, cbMax, &cbReadInProgress);	
        }

        else
        {
            fr = FILETask::rgpdFile[FILETask::READING]->fsclose();
        }

        if(fr == FR_OK) 
        {
            // finished reading
            if(cbReadInProgress > 0)
            {
                ftell =  FILETask::rgpdFile[FILETask::READING]->fstell();
                pbRead = (uint8_t *) pchJSONRespBuff;
                cbRead = cbReadInProgress;
                oData.iOut += cbRead;
                retAction = GCMD::WRITE;
            }
            
            // this is a close
            else
            {
                pbRead = NULL;
                cbRead = 0;
                oData.iOut = 0;
            }
            cbReadInProgress = 0;
            cbMax = 0;
        }
        
        // error
        else if(fr != FR_WAITING_FOR_THREAD)
        {
            // read error
            if(cbMax > 0)
            {
                pbRead = (uint8_t *) pchJSONRespBuff;
                cbRead = cbReadInProgress;
                               
                // force a zero count and to close
                oData.iOut = oData.cb; 
                
                // only write if we have data
                if(cbRead > 0) retAction = GCMD::WRITE;
            }
            
            // close error
            else
            {
                pbRead = NULL;
                cbRead = 0;
                oData.iOut = 0;            
                retAction = GCMD::ERROR;
                ASSERT(NEVER_SHOULD_GET_HERE);
            }
            cbReadInProgress = 0;
            cbMax = 0;           
        }
        
        // else just continue, it is a thread yield
    }
    
    // if we are closed, we are done
    else
    {
        ASSERT(fileTask.GetPath(FILETask::READING, true) != NULL);
        fileTask.ClearUsage(FILETask::READING);

        pbRead = NULL;
        cbRead = 0;
        oData.iOut = 0;
        cbReadInProgress = 0;
        cbMax = 0;
        retAction = GCMD::DONE;
    }

    return(retAction);
 }

