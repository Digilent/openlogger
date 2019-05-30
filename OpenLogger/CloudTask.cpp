/************************************************************************/
/*                                                                      */
/*    CloudTast.cpp                                                     */
/*                                                                      */
/*    ThingSpeak.com MathWorks Cloud Logging                            */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    4/1/2019 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>


static TCPSocket tcpSocketCloud;
static const char  szSuccess[] = "{\"success\":true}";
static const char  szNetworkFailure[] = "{\"failed\": ";

#if 0
static const char  szThingSpeak[]       = "http://api.thingspeak.com/channels/745598/bulk_update.json";
static IPEndPoint epRemote;
static uint32_t cbWritten = 0;
static uint32_t cbRead = 0;
// static char szSend[12288];
// static uint32_t iSend = 0; 

static const char  szThingSpeakPost[] = "POST /channels/745598/bulk_update.json HTTP/1.1\r\n\
Host: api.thingspeak.com\r\n\
Content-Type: application/json\r\n\
Connection: close\r\n\
content-length: 122\r\n\r\n\
{\r\n\
\"write_api_key\": \"OTKLRTYT18TG3XA9\",\r\n\
\"updates\": [{\r\n\
\"delta_t\": 0     ,\r\n\
\"field1\": 100    ,\r\n\
\"field2\": 100    \r\n\
}\r\n\
]\r\n\
}";

STATE CloudTask(void)
{

//    static STATE curState = Idle;
    static STATE curState = Done;

    IPSTATUS status = ipsSuccess;

    switch(curState)
    {
        case Idle:
            if(deIPcK.isIPReady())
            {
                memset(logIParam.adcTrg.szResponse, 0, sizeof(logIParam.adcTrg.szResponse));
                curState = CLDConnect;
            }
            break;

        case CLDResolveEP:
            if(deIPcK.resolveEndPoint(szThingSpeak, 80, epRemote, &status))
            {
                char szIp[32];

                GetNumb(epRemote.ip.ipv4.u8, 4, '.', szIp);
                Serial.print(szThingSpeak);
                Serial.print(" IP: ");
                Serial.println(szIp);
                curState = CLDConnect;
            }
            else if(IsIPStatusAnError(status))
            {
                Serial.print("Unable to get IP for ");
                Serial.println(szThingSpeak);
                curState = Done;
            }  
            break;

        case CLDConnect:
            if(deIPcK.tcpConnect(szThingSpeak, tcpSocketCloud, &status))
            {
                cbWritten = 0;
                curState = CLDWrite;
            }
            else if(IsIPStatusAnError(status))
            {
                Serial.print("Unable to connect to ");
                Serial.println(szThingSpeak);
                curState = Done;
            }
            break;

        case CLDWrite:
            cbWritten += tcpSocketCloud.writeStream((uint8_t *) &szThingSpeakPost[cbWritten], sizeof(szThingSpeakPost) - 1 - cbWritten, &status);

            if(IsIPStatusAnError(status))
            {
                tcpSocketCloud.close();
                Serial.println("Unable to write data");
                curState = Done;
            }

            else if(cbWritten == sizeof(szThingSpeakPost) - 1)
            {
                Serial.println("Successfully sent: ");
                Serial.println(szThingSpeakPost);
                curState = CLDRead;
            }   
            break;

        case CLDRead:
        {
            int32_t cbReadA = tcpSocketCloud.available();

            if(cbReadA > 0)
            {
                cbRead += tcpSocketCloud.readStream((uint8_t *) &logIParam.adcTrg.szResponse[cbRead], sizeof(logIParam.adcTrg.szResponse) - cbRead);
            }

            if(!tcpSocketCloud.isConnected())
            {
                curState = CLDDisconnect;
            }
         }
        break;

        case CLDDisconnect:
            tcpSocketCloud.close();
            curState = Finishing;
            // fall thru

        case Finishing:
            // wait for the disconnect
            if(!tcpSocketCloud.isConnected(&status))
            {
                Serial.println("Response: ");
                Serial.println(logIParam.adcTrg.szResponse);
                curState = Done;
            }
            else if(IsIPStatusAnError(status))
            {
                Serial.print("Is connect error: ");
                Serial.println(status, 10);
                curState = Done;
            }
            break;

        case Done:
            // hold at this state
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(Idle);
}
#endif

STATE ADCCloudTask(ADCTARGET& adcTrg, bool fFinish)
{
    IPSTATUS    status      = ipsSuccess;
    uint32_t    csBacklog;


    switch(adcTrg.state)
    {
        case Running:

            // check to see if we are done, get out
            // we must do this before checking for the time to post
            if(fFinish && logIParam.csTotal == adcTrg.csProcessed)
            {
                adcTrg.state        = Finishing;
                adcTrg.stcd         = STCDNormal;
                return(adcTrg.state);
            }

            // wait for the min post times, 
            else if(adcTrg.secPerSampl < minSecBetweenPosts)
            {   
                // not ready to post
                if(ReadCoreTimer() - adcTrg.tLastPost <= CORE_TMR_TICKS_PER_SEC * minSecBetweenPosts) 
                {
                    // if we are finishing, take the time of the last sample taken, if waiting
                    if(fFinish && !adcTrg.fFinishTime)
                    {
                        adcTrg.tFinishTime = ReadCoreTimer();
                        adcTrg.fFinishTime = true;
                    }

                    adcTrg.cSampleWait = logIParam.csTotal;
                    return(Waiting);
                }

                // wait for a sample boundary
                else if(!fFinish && logIParam.csTotal <= adcTrg.cSampleWait)
                {
                    return(Waiting);
                }

                // set the timeout period for the next samples
                adcTrg.tLastPost    = ReadCoreTimer();                  
            }
            
            // otherwise process the data.

            // if we have something to post, lets post it
            csBacklog = (uint32_t) (logIParam.csTotal - adcTrg.csProcessed);
 
            if(csBacklog > 0)
            {
                int32_t cbExtra;
                
                // please note that padcVoltNext is in the DDR buffer range
                // so we don't need to worry about blowing over the top of the DDR buffer
                int16_t * pStart = logIParam.padcVoltNext - csBacklog;  

                // get loop stats
                loopStats.RecordCount(LOOPSTATS::ADCTRGBACKLOG, csBacklog);
                loopStats.RecordCount(LOOPSTATS::DBDLADCTRG, csBacklog - adcTrg.csBackLog);

                // because ThingSpeak only allows 960 messages per post
                // if we backlog more than 960, we are in trouble
                // because the last message is always the current time, I can not push that back
                // in time to do more than the 960 posts
                // This will kick us out if the server goes down. Ultimately we will overflow.
                if(csBacklog > 960)
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

                // the aggregate sample count should to into the voltage buffer in blocks of active channels
                ASSERT(csBacklog > 0 && (csBacklog % logIParam.cActiveCh) == 0);

                adcTrg.psVCur   =  pStart;
                adcTrg.csVSave  = csBacklog;
                adcTrg.csVSaved = 0;
                adcTrg.csBackLog = csBacklog;

                // how long are the extras in the content length
                cbExtra = ((csBacklog/logIParam.cActiveCh)-1) * adcTrg.cbSampleConLen;

                // if we are finishing up and this is the last packet to go out and we have extra time to post
                if(adcTrg.fFinishTime && logIParam.csTotal == (adcTrg.csProcessed + adcTrg.csBackLog)) cbExtra += adcTrg.cbSampleConLen;
                
                // put out the POST and HEADER
                utoa(adcTrg.cbMinConLen + cbExtra, &adcTrg.szPOSTBuf[adcTrg.iContentLen], 10);
                strcat(&adcTrg.szPOSTBuf[adcTrg.iContentLen], szTSTermHdr);

                adcTrg.state    = CLDConnect;
            }
            break;

        case CLDConnect:
            if(deIPcK.tcpConnect(adcTrg.szUri, tcpSocketCloud, &status))
            {
                adcTrg.pbWrite      = (uint8_t *) adcTrg.szPOSTBuf;
                adcTrg.cbWrite      = strlen(adcTrg.szPOSTBuf);
                adcTrg.cbWritten    = 0;
                adcTrg.nextState    = CLDWriteUpdate;

//                iSend = 0;
                adcTrg.state    = CLDWrite;
            }

            // on connect error we may be okay, maybe the server is just down, just try again later
            // however, we don't want to infinit loop if we are finishing; we need to kick out.
            else if(IsIPStatusAnError(status) && !fFinish)
            {
                status              = ipsSuccess;           // just let the error go
                adcTrg.state        = Running;              // go back to the running state
            }
            break;

        case CLDWriteUpdate:
            adcTrg.pbWrite      = (uint8_t *) &adcTrg.szPOSTBuf[adcTrg.iUpdate];
            adcTrg.cbWrite      = adcTrg.iDelta - adcTrg.iUpdate;
            adcTrg.cbWritten    = 0;
            adcTrg.nextState    = CLDWriteField;

            adcTrg.state        = CLDWrite;
            break;

        case CLDWriteField:
            {
                char *      szCurValue = &adcTrg.szPOSTBuf[adcTrg.iFirstValue];
                uint32_t cbT, cbS, i, j;
                char * pch;

                // put in the delta
                pch = &adcTrg.szPOSTBuf[adcTrg.iDelta + TSiDelta];
                
                // for now we assume we are on the most current data point
                if(adcTrg.csVSaved == 0) 
                {
                    utoa(0, pch, 10);
                }

                // if this is padding at the end
                else if(adcTrg.csVSaved == adcTrg.csVSave)
                {
                    ASSERT(adcTrg.fFinishTime && adcTrg.csVSaved >= logIParam.cActiveCh && logIParam.csTotal == (adcTrg.csProcessed + adcTrg.csBackLog));
                    utoa(((ReadCoreTimer() - adcTrg.tFinishTime + CORE_TMR_TICKS_PER_SEC/2) / CORE_TMR_TICKS_PER_SEC), pch, 10);
                    adcTrg.fFinishTime = false;

                    adcTrg.psVCur   -= logIParam.cActiveCh;
                    adcTrg.csVSaved -= logIParam.cActiveCh;
                }
                else     
                {
                    utoa(adcTrg.secPerSampl, pch, 10);
                }
                
                cbT = strlen(pch);
                cbS = TScbDelta - cbT;
                pch = &pch[cbT];
                for(j=0; j<cbS; j++, pch++) *pch = ' ';

                // put in the fields
                for(i=0; i<logIParam.cActiveCh; i++) 
                {
                    itoa(*adcTrg.psVCur, szCurValue, 10);
                    cbT = strlen(szCurValue);
                    cbS = TScbValue - cbT;
                    pch = &szCurValue[cbT];
                    for(j=0; j<cbS; j++, pch++) *pch = ' ';

                    szCurValue += TSdiFValue2Value;
                    adcTrg.psVCur++;
                    adcTrg.csVSaved++;
                }

                // if we have a finish time, and this is the last time to write, then go around again to post the padding time
                if(adcTrg.fFinishTime && logIParam.csTotal == (adcTrg.csProcessed + adcTrg.csVSaved))
                {
                    ASSERT(adcTrg.csVSaved == adcTrg.csVSave);
                    adcTrg.nextState = CLDNextDelta;
                }
                else if(adcTrg.csVSaved == adcTrg.csVSave)   
                {
                    adcTrg.nextState = CLDEndDelta;
                }
                else
                {
                    adcTrg.nextState = CLDNextDelta;
                }

                adcTrg.pbWrite      = (uint8_t *) &adcTrg.szPOSTBuf[adcTrg.iDelta];
                adcTrg.cbWrite      = adcTrg.cbDelta;
                adcTrg.cbWritten    = 0;

                adcTrg.state        = CLDWrite;           
            }
            break;

        case CLDNextDelta:
            adcTrg.pbWrite      = (uint8_t *) szTSNextDelta;
            adcTrg.cbWrite      = strlen(szTSNextDelta);
            adcTrg.cbWritten    = 0;
            adcTrg.nextState    = CLDWriteField;
            adcTrg.state        = CLDWrite;            
            break;

        case CLDEndDelta:       
            adcTrg.pbWrite      = (uint8_t *) szTSEndUpdate;
            adcTrg.cbWrite      = strlen(szTSEndUpdate);
            adcTrg.cbWritten    = 0;
            adcTrg.nextState    = CLDRead;
            adcTrg.state        = CLDWrite;
            break;

        case CLDWrite:
            adcTrg.cbWritten += tcpSocketCloud.writeStream(&adcTrg.pbWrite[adcTrg.cbWritten], adcTrg.cbWrite - adcTrg.cbWritten, &status);

            if(adcTrg.cbWritten == adcTrg.cbWrite)
            {

//                memcpy(&szSend[iSend], adcTrg.pbWrite, adcTrg.cbWritten);
//                iSend += adcTrg.cbWritten;

                adcTrg.state = adcTrg.nextState;
                adcTrg.cbWritten    = 0;

                // get our timeout value set
                if(adcTrg.nextState == CLDRead) adcTrg.tTimeout = ReadCoreTimer();
            }   
            break;

        case CLDRead:
            {
                int32_t cbReadA = tcpSocketCloud.available();

                // get all of the returned data
                if(cbReadA > 0 && adcTrg.cbWritten < sizeof(adcTrg.szResponse))
                {
                    adcTrg.cbWritten += tcpSocketCloud.readStream((uint8_t *) &adcTrg.szResponse[adcTrg.cbWritten], sizeof(adcTrg.szResponse) - adcTrg.cbWritten);
                    adcTrg.tTimeout = ReadCoreTimer();     // reset the timeout value
                }

                // we need to check our timeout value, if the server did not close
                // and we timed out, then just close
                // set this to half of our post time
                else if(!tcpSocketCloud.isConnected() || (ReadCoreTimer() - adcTrg.tTimeout) > (CORE_TMR_TICKS_PER_SEC * (minSecBetweenPosts/2)))
                {
                    tcpSocketCloud.close();
                    adcTrg.nextState = Working;
                    adcTrg.state = CLDDisconnect;
                }
             }
            break;

        case CLDDisconnect:
             // wait for the disconnect
            if(!tcpSocketCloud.isConnected())
            {
                adcTrg.state = adcTrg.nextState;
            }           
            break;

        case Working:
 //           szSend[iSend] = '\0'; 

            // put a null terminator in the response string
            if(adcTrg.cbWritten < sizeof(adcTrg.szResponse))    adcTrg.szResponse[adcTrg.cbWritten] = '\0';
            else                                                adcTrg.szResponse[adcTrg.cbWritten-1] = '\0';

            // make sure we succeeded
            if(strstr(adcTrg.szResponse, szSuccess) != NULL)
            {
                // put in how much we processed
                adcTrg.csProcessed += adcTrg.csVSaved;
                adcTrg.state        = Running;
                adcTrg.csBackLog    = 0;
            }

            // something went wrong
            else
            {
                adcTrg.stcd     = STCDError;
                adcTrg.state    = Finishing;
            }
            break;

        case Finishing:
            adcTrg.state        = Idle;
            adcTrg.nextState    = Idle;
            break;

        // do nothing
        case Idle:
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    if(IsIPStatusAnError(status))
    {
        uint32_t cb;

        if(tcpSocketCloud.isConnected())
        {
            tcpSocketCloud.close();
            adcTrg.nextState = Finishing;
            adcTrg.state = CLDDisconnect; 
        }
        else
        {
            adcTrg.state = Finishing; 
        }

        adcTrg.stcd     = STCDError;
        memcpy(adcTrg.szResponse, szNetworkFailure, sizeof(szNetworkFailure)-1);
        utoa(status, &adcTrg.szResponse[sizeof(szNetworkFailure)-1], 10);
        cb = strlen(adcTrg.szResponse); 
        adcTrg.szResponse[cb++] = '}';
        adcTrg.szResponse[cb] = '\0';
    }

    return(adcTrg.state);
}
