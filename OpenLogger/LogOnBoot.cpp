/************************************************************************/
/*                                                                      */
/*    LoopStats.cpp                                                     */
/*                                                                      */
/*    Calculate loop statistics                                         */
/*                                                                      */
/*      Use the following command to get them                           */
/*                                                                      */
/*      {"device":[{"command":"loopStatistics"}]}                       */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2017, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    9/28/2017(KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

static LOGPARAM     logAParam;

STATE AutoLogOnBootTask(void)
{
    static FRESULT      fr          = FR_OK;
    static uint32_t     cbRead      = 0;
    static uint32_t     tTimeOut;

    // we will have an SD card already if there because if SD Mount; so if SD logging we can go right for it
    // we will be linked, but maybe not have an IP, so if we are doing auto log on WiFi, we need to wait for the IP address
    // internal logging we can go for it immediately
    // no logging we will exit.

    switch(logAParam.state)
    {
        case Idle:
            if(fileTask.SetUsage(FILETask::READING, VOLFLASH, szLogOnBoot))
            {
                fr = FR_OK;
                cbRead = 0;
                memset(&logAParam, 0, sizeof(logAParam));
                logAParam.state = FILEexist;
            }
            break;

        case FILEexist:
            fr = DFATFS::fsexists(fileTask.GetPath(FILETask::READING, true));
            if(fr == FR_OK)
            {
                logAParam.state = FILEopening; 
            }
            else
            {
                fileTask.ClearUsage(FILETask::READING);
                logAParam.state = Idle; 
            }
            break;

        case FILEopening:
            fr = FILETask::rgpdFile[FILETask::READING]->fsopen(fileTask.GetPath(FILETask::READING, true), FA_READ);
            if(fr == FR_OK && FILETask::rgpdFile[FILETask::READING]->fssize() == sizeof(logAParam))
            {
                logAParam.state = FILEread;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                // we did not read enough, it is the wrong size and is invalid
                if(fr == FR_OK) fr = FR_INVALID_OBJECT;
                logAParam.state = FILEclose;
            }
            break;

        case FILEread:
            fr = FILETask::rgpdFile[FILETask::READING]->fsread(&logAParam, sizeof(logAParam), &cbRead, 100);
            if(fr == FR_OK)
            {
                ASSERT(cbRead == sizeof(logAParam));
                logAParam.state = FILEclose;
            }            
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                logAParam.state = FILEclose;
            }
            break;

        case FILEclose:
            if(FILETask::rgpdFile[FILETask::READING]->fsclose() != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::READING);
                if(fr == FR_OK)     
                {
                    switch(logAParam.adcTrg.vol)
                    {
                        case VOLRAM:
                            logAParam.state = Done;
                            break;

                        case VOLCLOUD:
                            if(deIPcK.isLinked()) 
                            {
                                logAParam.state = HTTPEnabled;
                                tTimeOut = ReadCoreTimer();
                            }
                            break;

                        case VOLSD:
                            if(fSd0)            
                            {
                                logAParam.state = Done;
                            }
                            else if(fSd0Trans)
                            {
                                logAParam.state = SDMounting;
                                tTimeOut = ReadCoreTimer();
                            }
                            else
                            {
                                logAParam.state = Stopped;
                            }
                            break;

                        default:
                            ASSERT(NEVER_SHOULD_GET_HERE);
                            break;
                    }  
                }

                // some kind of error
                else
                {
                    logAParam.state = Stopped;
                }
            }
            break;

        case HTTPEnabled:
            if(deIPcK.isIPReady())                                           logAParam.state = Done;
            else if(ReadCoreTimer() - tTimeOut > 10*CORE_TMR_TICKS_PER_SEC) logAParam.state = Stopped;
            break;

        case SDMounting:
            if(fSd0)                                                        logAParam.state = Done;
            else if(ReadCoreTimer() - tTimeOut > CORE_TMR_TICKS_PER_SEC)    logAParam.state = Stopped;
            break;

        case Done:
            // copy over the data
            memcpy(&logIParam, &logAParam, sizeof(logIParam));
            logIParam.state = Pending;

            // put the run on the deferred task.
            defTask.QueTask(DEFTask::FileLog, -1);
            logAParam.state = Idle; 
            break;

        case Stopped:
            logAParam.state = Idle; 
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;

    }

    return(logAParam.state);
}

