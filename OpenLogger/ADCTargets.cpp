/************************************************************************/
/*                                                                      */
/*    ADCTargets.cpp                                                    */
/*                                                                      */
/*    Manages the ADC targets such as SD or Cloud                       */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    1/23/2019 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>
#include <math.h>

STATE ADCSDTask(ADCTARGET& adcTrg, bool fFinish)
{
    static  DAQHDR  daqHdr;
    FRESULT         fr              = FR_OK;
    uint32_t        csBacklog;
    uint32_t        cClusters;
    uint32_t        cSecClust;

    switch(adcTrg.state)
    {
        case Running:

            // file better be open and ready
            ASSERT(*(FILETask::rgpdFile[FILETask::LOGGING])); 

            // if we have something to write, lets write it
            csBacklog = (uint32_t) (logIParam.csTotal - adcTrg.csProcessed);
 
            if(csBacklog > 0)
            {
                int16_t * pStart = logIParam.padcVoltNext - csBacklog;  

                // get loop stats
                loopStats.RecordCount(LOOPSTATS::ADCTRGBACKLOG, csBacklog);
                loopStats.RecordCount(LOOPSTATS::DBDLADCTRG, csBacklog - adcTrg.csBackLog);
                adcTrg.csBackLog = csBacklog;

                // if we blew over the top
                // we need to leave room to allow for incoming while we write out
                // but that should be calculated at setparam time as the sample rate and channel count
                // should be included in the calculations.
                // For now, if we blow over we know the last CSVBUFF is corrupt
                if(csBacklog > CSVBUFF)
                {
                    adcTrg.state    = Finishing;
                    adcTrg.stcd     = STCDOverflow;
                    break;
                }

                // note, the voltage buffer is way up in the DDR address space, so we can subtract a ton
                // and still not wrap, so we can just do unsigned math and compare pointers and it will all work out.
                if(pStart < adcVoltageBuffer) 
                {
                    csBacklog = adcVoltageBuffer - pStart;
                    pStart += CSVBUFF;
                }

                if((adcTrg.csProcessed + csBacklog) > adcTrg.csEOF) csBacklog = adcTrg.csEOF - adcTrg.csProcessed;
                adcTrg.pbWrite  = (uint8_t *) pStart;
                adcTrg.cbWrite  = csBacklog * sizeof(int16_t);
                adcTrg.cbWritten = 0;
 
                adcTrg.state    = FILEwrite;
                adcTrg.nextState = Running;
            }

            // close the file and get out
            else if(fFinish)
            {
                adcTrg.state        = Finishing;
                adcTrg.stcd         = STCDNormal;
            }
            break;

        case FILEwrite:
            // we use 1024 with 512 byte sectors to make a 512kB RU size for one write on a class 10 card
            // this would be about a 250ms write at 2MB/s
            if((fr = FILETask::rgpdFile[FILETask::LOGGING]->fswrite(adcTrg.pbWrite, adcTrg.cbWrite, &adcTrg.cbWritten, 1024)) == FR_OK)
            {
                // how many we wrote out
                if(adcTrg.nextState == Running) 
                {
                    adcTrg.csProcessed += adcTrg.cbWritten/sizeof(int16_t);
                    
                    // This can happen when the SD card if full
                    // Which can happen if the free cluster count is out of sync with actual free clusters
                    // or WFL writes to the SD after the remaining file space was calculated.
                    if(adcTrg.cbWritten < adcTrg.cbWrite)
                    {
                        adcTrg.state = Finishing;
                    }
                    else if(adcTrg.csProcessed >= adcTrg.csEOF && (logIParam.csMax < 0 || adcTrg.csProcessed < logIParam.csMax))
                    {
                        ASSERT(adcTrg.csProcessed == adcTrg.csEOF);
                        adcTrg.iSeek = 0;       
                        adcTrg.state = FILEclose;
                    }
                    else
                    {
                        adcTrg.state = Running;
                    }
                }

                // if we just wrote the header
                else if(adcTrg.nextState == Done) 
                {
                    adcTrg.state = FILEclose;
                }

                else
                {
                    ASSERT(NEVER_SHOULD_GET_HERE);
                    // go to the next state, where ever that may be
                    adcTrg.state = adcTrg.nextState;
                }
            }

            // got an error
            else if(fr != FR_WAITING_FOR_THREAD)
            { 
                adcTrg.stcd  = STCDError;
                if(adcTrg.nextState == Running) 
                {
                    adcTrg.csProcessed += adcTrg.cbWritten/sizeof(int16_t);
                    adcTrg.state = Finishing;
                }
                else
                {
                    adcTrg.nextState = Done;
                    adcTrg.state = FILEclose;
                }
            }
            break;

       // closing a file
       // could be a logging file
       // could be our header
        case FILEclose:
            if(FILETask::rgpdFile[FILETask::LOGGING]->fsclose() != FR_WAITING_FOR_THREAD)
            {
                // pretty much always going to open another file
                // except when we are done
                ASSERT(fileTask.GetPath(FILETask::LOGGING, true) != NULL);
                fileTask.ClearUsage(FILETask::LOGGING);

                if(adcTrg.nextState == Done)            adcTrg.state = Done;
                else if(adcTrg.nextState == Pending)    adcTrg.state = FILEopen;
                else                                    adcTrg.state = FILEgetsize;
            }
            break;

        case FILEgetsize:
            if((fr = DFATFS::fsgetfree(adcTrg.szUri, &cClusters, &cSecClust)) != FR_WAITING_FOR_THREAD)
            {
                int64_t csMax = ((((int64_t) cClusters)-2) * ((int64_t) cSecClust) * 512) / sizeof(uint16_t);

                // I need my extra cluster, and 1 cluster for padding
                if(csMax < logIParam.cActiveCh)
                {
                    adcTrg.state = Finishing;
                    break;
                }

                // can we do it all>
                else if(csMax >= LOGMAXSAMPINFILE)
                {
                    csMax = LOGMAXSAMPINFILE;
                }

                // make mult of channel
                csMax -= (csMax % logIParam.cActiveCh);

                // make the next file name
                ulltoa((adcTrg.csEOF/logIParam.cActiveCh), &adcTrg.szUri[adcTrg.iNbr], 10); 
                strcat(&adcTrg.szUri[adcTrg.iNbr], adcTrg.szExt);

                // update for the next file
                adcTrg.csEOF += csMax;
                ASSERT((logIParam.adcTrg.csEOF % logIParam.cActiveCh) == 0);

                // try to open the new file
                adcTrg.state = FILEopen;                
            }
            break;

        // opening a new file
        // could be to write the header
        // could be a new logging file
        case FILEopen:
            ASSERT(fileTask.GetPath(FILETask::LOGGING, true) == NULL);

            // something is seriously wrong, cleanup, get out!
            if(!fileTask.SetUsage(FILETask::LOGGING, adcTrg.vol, adcTrg.szUri))
            {
                fr =  FR_LOCKED;
                break;
            }

            adcTrg.state = FILEopening;
            // fall thru

        case FILEopening:
           if((fr = FILETask::rgpdFile[FILETask::LOGGING]->fsopen(logIParam.adcTrg.szUri, ((adcTrg.nextState == Pending) ? (FA_OPEN_EXISTING | FA_WRITE) : (FA_CREATE_ALWAYS | FA_READ | FA_WRITE)))) == FR_OK)
            {
                // got it, lets go to the seek
                adcTrg.state = FILEseek;
            }
            break;

        // seek to our location to write
        // could be our header
        // could be a new file
        case FILEseek:
            if((fr = FILETask::rgpdFile[FILETask::LOGGING]->fslseek(adcTrg.iSeek)) == FR_OK)
            {
               adcTrg.state = adcTrg.nextState;
            }
            break;

        // time to update the header and close all files
        case Finishing:

            // if we are on the first file, we don't have to open a new file to write the header
            // we only need to seek to the file position we need to write the header.
            if(adcTrg.szUri[adcTrg.iNbr] == '0')
            {
                // if the file is open, we only need to seek to the start
                if(*(FILETask::rgpdFile[FILETask::LOGGING]))   adcTrg.state = FILEseek;

                // but it may already be closed then we need to reopen the file
                else                adcTrg.state = FILEopen; 
            }

            // put the file name in to open
            // close the current file
            // open the first file
            else
            {
                // go to the first file
                adcTrg.szUri[adcTrg.iNbr] = '0';
                strcpy(&adcTrg.szUri[adcTrg.iNbr+1], adcTrg.szExt);

                // close the current file
                if(*(FILETask::rgpdFile[FILETask::LOGGING]))   adcTrg.state = FILEclose;

                // but it may already be closed then just open the header
                else                adcTrg.state = FILEopen; 
            }

            // the write the headder
            adcTrg.nextState    = Pending;
            adcTrg.iSeek        = offsetof(DAQHDR, dHdr.stopReason);
            break;

        // setting up to write the header info out
        case Pending:
            {
                daqHdr.dHdr.stopReason   = adcTrg.stcd;
                daqHdr.dHdr.iStart       = 0;  
                daqHdr.dHdr.actualCount  = adcTrg.csProcessed/logIParam.cActiveCh;

                adcTrg.pbWrite  = (uint8_t *) &daqHdr.dHdr.stopReason;
                adcTrg.cbWrite  = sizeof(daqHdr.dHdr.stopReason) + sizeof(daqHdr.dHdr.iStart) + sizeof(daqHdr.dHdr.actualCount);
                adcTrg.cbWritten = 0;

                adcTrg.state        = FILEwrite;
                adcTrg.nextState    = Done;
            }
            break;

        // we are done
        case Done:
            {
                ASSERT(!(*(FILETask::rgpdFile[FILETask::LOGGING]))); 
                ASSERT(fileTask.GetPath(FILETask::LOGGING, true) == NULL);
                adcTrg.state = Idle;
            }
            break;

        // do nothing
        case Idle:
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // file IO error condition
    if(!(fr ==  FR_OK || fr == FR_WAITING_FOR_THREAD))
    {
        // if we are trying to write the header
        // we are screwed, just get out.
        if(adcTrg.nextState == Pending)
        {
            // if the file is open, try to close it
            if(*(FILETask::rgpdFile[FILETask::LOGGING]))
            {
                adcTrg.nextState    = Done;
                adcTrg.state        = FILEclose;
            }

            // otherwise, just get out!
            else adcTrg.state        = Done;
        }

        // otherwise write out the header and quit
        else
        {
            adcTrg.state        = Finishing;
        }
        adcTrg.stcd         = STCDError;
    }

    return(adcTrg.state);
}

STATE ADCTargetsTask(void)
{

    if(logIParam.adcTrg.state != Idle)
    {
        bool fFinish = (logIParam.state == Finishing && logIParam.padcRawEnd == logIParam.padcRawStart);

        switch(logIParam.adcTrg.vol)
        {
            case VOLSD:
                ADCSDTask(logIParam.adcTrg, fFinish);
                break;

            case VOLCLOUD:
                ADCCloudTask(logIParam.adcTrg, fFinish);
                break;

            default:
                if(fFinish) logIParam.adcTrg.state = Idle;
                break;
        }
    }

    // when all targets are idle, we can say logging has stopped
    // this is important to set because sometimes we just issue a stop when
    // things aren't running; and we just want to stop
    else if(logIParam.state == Finishing) 
    {
        logIParam.state = Stopped;
    }

    return(Idle);
}