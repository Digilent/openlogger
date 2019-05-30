/************************************************************************/
/*                                                                      */
/*    DeferredTasks.cpp                                                 */
/*                                                                      */
/*    Manage deferred tasks                                             */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    8/28/2018(KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

bool DEFTask::QueTask(DFER dTask, void * pData, uint32_t cbData, int32_t tmsWait)
{
    taskInfo[idTaskNext].tmsWait = tmsWait;
    taskInfo[idTaskNext].dTask = dTask;

    // save away any data if there
    if(cbData > 0) 
    {
        ASSERT(cbData <= sizeof(taskInfo[0].scratch));
        taskInfo[idTaskNext].cbScratch = cbData;
        memcpy(taskInfo[idTaskNext].scratch, pData, cbData);
    }

    // move to the next task index
    idTaskNext++;
    idTaskNext %= cMaxTask;

    return(true);
}

int32_t DEFTask::GetWaitTime(void)
{
    uint32_t iEnd = idTaskNext < idTaskCur ? idTaskNext + cMaxTask : idTaskNext;
    int32_t tmsTotal = 0;
    uint32_t i = 0;

    for(i=idTaskCur; i<iEnd; i++)
    {
        int32_t tmsWait = taskInfo[i%cMaxTask].tmsWait;

        if(tmsWait < 0)
        {
            return(-1);
        }
        else
        {
            tmsTotal += tmsWait;
        }
    }

    return(tmsTotal);
}

int32_t DEFTask::GetWaitTime(DFER dTask)
{
    uint32_t iEnd = idTaskNext < idTaskCur ? idTaskNext + cMaxTask : idTaskNext;
    int32_t tmsTotal = 0;
    uint32_t i = 0;

    for(i=idTaskCur; i<iEnd; i++)
    {
        int32_t tmsWait = taskInfo[i%cMaxTask].tmsWait;
        
        if(tmsWait < 0)
        {
            tmsTotal = -1;
        }
        else if(tmsTotal >= 0)
        {
            tmsTotal += tmsWait;
        }

        // when we find it on the list return
        if(taskInfo[i%cMaxTask].dTask == dTask) return(tmsTotal);
    }

    // if we get all the way through the for loop, we never found it
    return(0);
}

STATE DEFTask::PeriodicTasks(void)
{
    STATE   retState    = Idle;
    FRESULT fr          = FR_OK;

    if(idTaskCur == idTaskNext) return(Idle);

    switch(taskInfo[idTaskCur].dTask)
    {
        case None:
            break;

        case CalDAQ1:
            retState = DAQCalibrate(1, 1);
            break;

        case CalDAQ2:
            retState = DAQCalibrate(2, 1);
            break;

        case CalDAQ3:
            retState = DAQCalibrate(3, 1);
            break;

        case CalDAQ4:
            retState = DAQCalibrate(4, 1);
            break;

        case CalDAQ5:
            retState = DAQCalibrate(5, 1);
            break;

        case CalDAQ6:
            retState = DAQCalibrate(6, 1);
            break;

        case CalDAQ7:
            retState = DAQCalibrate(7, 1);
            break;

        case CalDAQ8:
            retState = DAQCalibrate(8, 1);
            break;

        case CalGndDAQ1:
            retState = DAQCalibrate(1, 0);
            break;

        case CalGndDAQ2:
            retState = DAQCalibrate(2, 0);
            break;

        case CalGndDAQ3:
            retState = DAQCalibrate(3, 0);
            break;

        case CalGndDAQ4:
            retState = DAQCalibrate(4, 0);
            break;

        case CalGndDAQ5:
            retState = DAQCalibrate(5, 0);
            break;

        case CalGndDAQ6:
            retState = DAQCalibrate(6, 0);
            break;

        case CalGndDAQ7:
            retState = DAQCalibrate(7, 0);
            break;

        case CalGndDAQ8:
            retState = DAQCalibrate(8, 0);
            break;

        case CalDC1:
            retState = DCCalibrate(1);
            break;

        case CalDC2:
            retState = DCCalibrate(2);
            break;

        case CalAWG:
           retState = AWGCalibrate();
           break;

        case UpdWFFW:
            if(oslex.fLocked) retState = WaitingForResources;
            else retState = WiFiUpdateFirmware();
            break;

        case UpdWFCert:           
            if(oslex.fLocked) retState = WaitingForResources;
            else retState = WiFiUpdateCertificate();
            break;

        case SoftReset:
            if(oslex.fLocked) 
            {
                retState = WaitingForResources;
            }
            else 
            {
                IMAGE_HEADER_INFO imh;
                RAM_HEADER_INFO    rh;
                RAM_HEADER_INFO& ramHeader =  *imageHeader.pRamHeader;
                
                memcpy(&imh, &imageHeader, sizeof(IMAGE_HEADER_INFO));
                memcpy(&rh, imh.pRamHeader, sizeof(RAM_HEADER_INFO));
                
                // BUG BUG
                // need to wait for file system to be Idle
                // Need to consider the state of instruments... Maybe
                ((RAM_HEADER_INFO *) KVA_2_KSEG1(&ramHeader))->fVirtualReset = true;
                _softwareReset();
            }
            break;

        case SaveCalibration:
            retState = SaveCalibrationData(arCalIDC[0].vol);
            break;

        case ResetInstruments:
            // stop the works
            AWGStop();
            DCSetvoltage(1, 0);
            DCSetvoltage(2, 0);
            ADCStopSampling();
            retState = Idle;
            break;

        case FileDelete:
            if((fr = DFATFS::fsunlink(fileTask.GetPath(FILETask::DELETING, true))) == FR_WAITING_FOR_THREAD)
            {
                retState = FileDelete;
            }
            else
            {
                fileTask.ClearUsage(FILETask::DELETING);

                // the return state
                if(fr == FR_OK) retState = Idle;
                else            retState = MkStateAnError(FILECSTATE | fr);
            }
            break;

        case FileLog:
            retState = DeferredLogRun();
            break;

        case WIFIConnect:
            {
                WIFINFO&    wifi =  *(*((WIFINFO **) ((void *) taskInfo[idTaskCur].scratch)));

                // can't connect if active
                if(wifiActive.lastErrState != Working && deIPcK.isLinked())
                {
                    wifiActive.lastErrState = InstrumentInUse;
                    retState                = InstrumentInUse;
                }
                else if((retState = WiFiConnect(wifi)) == Idle)
                {
                    if(&wifi != &wifiActive) memcpy(&wifiActive, &wifi, sizeof(WIFINFO));
                    wifiActive.lastErrState = Running;
                }
                else if(IsStateAnError(retState))
                {
                    wifiActive.lastErrState = retState;
                }
                else
                {
                    wifiActive.lastErrState = Working;
                }
            }
            break;

        case WIFIDisconnect:
            if((retState = WiFiDisconnect()) == Idle)
            {
                wifiActive.lastErrState = Idle;
            }
            else if(IsStateAnError(retState))
            {
                wifiActive.lastErrState = retState;
            }
            break;

        case WIFIScan:
            retState = WiFiScan();
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // if done, go to the next deferred state
    if(retState == Idle || IsStateAnError(retState))
    {
        idTaskCur++;
        idTaskCur %= cMaxTask;
    }

    return(retState);
}