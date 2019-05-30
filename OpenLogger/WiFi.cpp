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

WPA2KEY wpaKey;

#define TIMOUTTOOLONG (5*CORE_TMR_TICKS_PER_SEC)
STATE WiFiInit(void)
{
    static STATE state = Idle;
    static uint32_t tStart = 0;

    switch (state) 
    {
        case Idle:
            
            if(deIPcK.deIPInit())
            {
                tStart = ReadCoreTimer();
                state = WIFInitWaitForMacAddress;
            }
            else 
            {
                // hard error, just get stuck at this state
                state = WIFInitUnableToSetNetworkAdaptor;
            }
            break;

        case WIFInitWaitForMacAddress:              
            if(deIPcK.getMyMac(macMyMacAddr))
            {   
                char sz[32];
                char szMac[32];
                tstrM2mRev wifiVer;

                Serial.println("WiFi Info: ");
                Serial.println();
                nm_get_firmware_full_info(&wifiVer);    // don't check error return, it will fail is miss-match firmware
                Serial.print("WiFi ID:        ");
                utoa(wifiVer.u32Chipid, sz, 16);    
                Serial.println(sz);

                Serial.print("FW Version:     ");
                utoa(wifiVer.u8FirmwareMajor, sz, 10);    
                Serial.print(sz);
                Serial.print('.');
                utoa(wifiVer.u8FirmwareMinor, sz, 10);    
                Serial.print(sz);
                Serial.print('.');
                utoa(wifiVer.u8FirmwarePatch, sz, 10);    
                Serial.println(sz);

                Serial.print("Driver Version: ");
                utoa(wifiVer.u8DriverMajor, sz, 10);    
                Serial.print(sz);
                Serial.print('.');
                utoa(wifiVer.u8DriverMinor, sz, 10);    
                Serial.print(sz);
                Serial.print('.');
                utoa(wifiVer.u8DriverPatch, sz, 10);    
                Serial.println(sz);

                Serial.print("Build Date:     ");
                Serial.println((char *) wifiVer.BuildDate);

                Serial.print("Build Time:     ");
                Serial.println((char *) wifiVer.BuildTime);
                Serial.println();
                
                Serial.print("My              ");
                GetMAC(macMyMacAddr, szMac);
                Serial.println(szMac); 
                
                tStart = ReadCoreTimer();
                fWiFiInit = true;
                state = Idle;
            }
            else if((SYSGetMilliSecond() - tStart) > TIMOUTTOOLONG)
            {
                // Hard error, just get stuck at this state
                Serial.println("Unable to Initialize WiFi");
                state = WIFInitMACFailedToResolve;               
            }
            break;

        default:
            state = Idle;
            break;
    }    
    
    if(IsStateAnError(state))
    {
        STATE retState = state;
        state = Idle;
        return(retState);
    }
    else
    {
        return(state);
    }
}

STATE WiFiAutoConnect(void)
{
    static STATE        curState    = Idle;
    static STATE        statusCode  = Idle;
    static uint32_t     cNetworks   = 0;
    static uint32_t     iNetwork    = 0;
    static int8_t       rssi1       = 0;
    static int8_t       rssi2       = 0;
    static char const * szQualPath  = NULL;
    static VOLTYPE      vol         = VOLFLASH;
    static WIFINFO *    pWifi       =  NULL;
    static uint32_t     cbRead      = 0;
    static SCANINFO     scanInfo;
    char                szFileName[DEWF_MAX_SSID_LENGTH+5];
    char                szT[DEWF_MAX_SSID_LENGTH+5];

    IPSTATUS            status      = ipsSuccess;
    FRESULT             fr          = FR_OK;
    bool                fDone       = false;

    switch(curState)
    {
        case Idle:

            // the WiFi module did not init
            if(!fWiFiInit) return(WiFiNoNetworksFound);

            // init everything
            statusCode  = Idle;
            cbRead      = 0;
            cNetworks   = 0;
            iNetwork    = 0;
            rssi1       = -128;
            rssi2       = -128;
            szQualPath  = NULL;
            pWifi       =  NULL;
            vol         = VOLFLASH;
            memset(&scanInfo, 0, sizeof(scanInfo));
            memset(&wifiScanState, 0, sizeof(WFSCANSTATE));
            curState = WIFIScan;

            // fall thru

        case WIFIScan:
            // DEWF_PASSIVE_SCAN
            if((fDone = deIPcK.wfScan(DEWF_ACTIVE_SCAN, (int *) &cNetworks, &status)) && cNetworks > 0) curState    = WIFIScanListAP;
            else if(IsIPStatusAnError(status))                                                          statusCode  = MkStateAnError(WIFICBSTATE | status);
            else if(fDone)                                                                              statusCode = WiFiNoNetworksFound;

            if(statusCode != Idle) curState = Done;
            break;

        case WIFIScanListAP:

            // alway check flash first
            vol         = VOLFLASH;

            // get the next SSID
            if((fDone = deIPcK.getScanInfo(iNetwork, &scanInfo, &status)) && scanInfo.rssi > rssi2) curState    = FILEVerify;
            else if(IsIPStatusAnError(status))                                                      statusCode  = MkStateAnError(WIFICBSTATE | status);
            else if(fDone && ++iNetwork >= cNetworks)                                               curState    = Working;

            if(statusCode != Idle) curState = Done;
            break;

        case FILEVerify:
            // make the file name
            memcpy(szT, scanInfo.ssid, scanInfo.ssidLen);
            szT[scanInfo.ssidLen] = '\0';

            // see if the file exists
            if(scanInfo.ssidLen == 0)
            {
                if(++iNetwork >= cNetworks) curState    = Working;          // we are done scanning
                else                        curState    = WIFIScanListAP;   // try the next scan entry
            }
            else if(WiFiCreateName(szT, true, szFileName, sizeof(szFileName)) && (szQualPath  = fileTask.MkQualifiedPath(vol, szFileName)) != NULL) curState    = FILEexist;
            else if(++iNetwork >= cNetworks)                                                                                                        curState    = Working;
            else                                                                                                                                    curState    = WIFIScanListAP;
            break;

        case FILEexist:
            if((fr = DFATFS::fsexists(szQualPath)) != FR_WAITING_FOR_THREAD)
            {
                // if the file exists, update
                if(fr == FR_OK)
                {
                    if(scanInfo.rssi > rssi1)
                    {
                        memcpy(wifiActive.ssid, scanInfo.ssid, scanInfo.ssidLen);
                        wifiActive.ssid[scanInfo.ssidLen] = '\0';
                        strcpy(wifiActive.passphrase, szQualPath);
                        wifiActive.keyIndex =  vol;
                        rssi1 = scanInfo.rssi;
                    }

                    // always give FLASH priority, so if the file exists both
                    // in flash and sd, use the flash one. The rssi would be set
                    // and the greater will not be met, so the flash file would stay
                    else if(scanInfo.rssi > rssi2)
                    {
                        memcpy(wifiWorking.ssid, scanInfo.ssid, scanInfo.ssidLen);
                        wifiWorking.ssid[scanInfo.ssidLen] = '\0';
                        strcpy(wifiWorking.passphrase, szQualPath);                   
                        wifiWorking.keyIndex =  vol;
                        rssi2 = scanInfo.rssi;
                    }

                    // this is just so we don't do this for the SD if we found it in flash
                    vol = VOLSD;
                }

                // got to the next state
                fr = FR_OK;
                curState = FILEVerify;
                if(fSd0 && vol == VOLFLASH)         vol         = VOLSD;            // try the SD card
                else if(++iNetwork >= cNetworks)    curState    = Working;          // we are done scanning
                else                                curState    = WIFIScanListAP;   // try the next scan entry
            }
            break;

        case Working:            
            if(pWifi == NULL && rssi1 > -128)               pWifi = &wifiActive;
            else if(pWifi == &wifiActive && rssi2 > -128)   pWifi = &wifiWorking;
            else                                            pWifi = NULL;

            curState    = Done;
            if(statusCode == Idle) statusCode = WiFiNoNetworksFound;
            if(pWifi != NULL)
            {
                statusCode = Idle;
                vol = (VOLTYPE) pWifi->keyIndex;
                if(fileTask.SetUsage(FILETask::WRITING, vol, pWifi->passphrase))   curState = FILEopen;
                else statusCode = MkStateAnError(FILECSTATE | FR_LOCKED);               // this should never happen.
                ASSERT(statusCode == Idle);
                pWifi->passphrase[0] = '\0';        // never keep a passphrase, eve if it is just a filename
            }
            break;

        case FILEopen:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsopen(fileTask.GetPath(FILETask::WRITING, true), FA_READ)) == FR_OK)
            {
                curState = FILEseek;
            }
            break;

        case FILEseek:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fslseek(0)) == FR_OK)
            {
                cbRead      = 0;
                curState    = FILEread;
            }
            break;

        // write out the header and then leave the file posistion after the header.
        case FILEread:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsread(&wifiActive, offsetof(WIFINFO, lastErrState), &cbRead)) == FR_OK)
            {
                if(cbRead != offsetof(WIFINFO, lastErrState) || wifiActive.wifiVer < WFVER)     statusCode = InvalidVersion; 
                curState = FILEclose;
            }            
            break;

        // on an error we will close
        case FILEclose:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsclose()) == FR_OK)
            {
                fileTask.ClearUsage(FILETask::WRITING);

                if(statusCode == Idle)  curState = WIFIConnect;
                else                    curState = Working;
            }
            break;

        case WIFIConnect:
            {
                STATE conState = WiFiConnect(*pWifi);

                if(conState == Idle)                curState = Done;
                else if(IsStateAnError(conState))   curState = Working;
            }
            break;

        case Done:
            curState = Idle;

            // the working set should be empty
            memset(&wifiWorking, 0, sizeof(wifiWorking));

            // it worked, we are done
            if(!IsStateAnError(statusCode) && deIPcK.isLinked())
            {
                ASSERT(wifiActive.passphrase[0] == 0);
                Serial.print("Auto Connected to WiFi network: \"");
                Serial.print(wifiActive.ssid);                  // if connected, it should come from the active set
                Serial.print("\" from ");
                Serial.println(rgVOLNames[vol]);
            }

            // did not work, clean up
            else
            {
                memset(&wifiActive, 0, sizeof(wifiActive));
                    
                Serial.print("Failed to AutoConnect, error: ");
                Serial.println(statusCode, 16);
                return(statusCode);
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }
    
    // file error conditions
    if(fr != FR_OK && fr != FR_WAITING_FOR_THREAD)
    {
        statusCode = MkStateAnError(FILECSTATE | fr);

        switch(curState)
        {
            case Idle:
                break;

            case WIFIScan:
            case WIFIScanListAP:
            case FILEVerify:
            case FILEexist:
            case Done:
            case Working:
            case WIFIConnect:
                curState = Done;
                break;

            case FILEopen:
            case FILEclose:
                fileTask.ClearUsage(FILETask::WRITING);
                curState = Done;
                break;

            case FILEseek:
            case FILEread:
                curState = FILEclose;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    return(curState);
}

STATE WiFiScan(void)
{
    static STATE        curState    = Idle;
    IPSTATUS            status      = ipsSuccess;
    
    switch(curState)
    {
        case Idle:
            if(deIPcK.isLinked())
            {
                wifiScanState.scanState = Stopped;
                wifiScanState.errState  = MustBeDisconnected;
            }
            else
            {
                wifiScanState.cNetworks = 0;
                wifiScanState.iNetwork  = 0;
                wifiScanState.scanState = Working;
                curState                = WIFIScan;
            }
            break;
            
        case WIFIScan:
            // DEWF_PASSIVE_SCAN
            if(deIPcK.wfScan(DEWF_ACTIVE_SCAN, (int *) &wifiScanState.cNetworks, &status))
            {
                wifiScanState.scanState = Done;
                wifiScanState.errState  = Idle;
                wifiScanState.iNetwork  = 0;
                curState = Idle;
            }
            else if(IsIPStatusAnError(status))
            {
                wifiScanState.scanState = Stopped;
                wifiScanState.errState  = MkStateAnError(WIFICBSTATE | status);
                curState = Idle;
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(curState);
}


STATE WiFiCalPSK(void)
{
    static STATE        state       = Idle;
    static uint32_t     tTotal      = 0;  
    static uint32_t     tTotalLoop  = 0;
    static uint32_t     cpass       = 0;
   
    switch(state)
    {
        case Idle:
            tTotal = 0;
            tTotalLoop = ReadCoreTimer();
            cpass = 0;
            state = Working;
           
        case Working:
            {
                uint32_t tTime = ReadCoreTimer();

                cpass++;
                if(deIPcK.convertSSIDandPassPhraseToPSK("SSID Name", "passphrase", wpaKey))
                {
                    char szWPAKey[2*DEWF_PSK_LENGTH+1];
                    int i,j;

                    tTotal += (ReadCoreTimer() - tTime) / CORE_TMR_TICKS_PER_USEC;               
                    tTotalLoop = (ReadCoreTimer() - tTotalLoop) / CORE_TMR_TICKS_PER_MSEC;
                    

                    // convert to ascii
                    for(i=0, j=0; i<DEWF_PSK_LENGTH; i++)
                    {
                        uint8_t high                    = wpaKey.rgbKey[i] >> 4;
                        uint8_t low                     = wpaKey.rgbKey[i] & 0xF;
                        if(high < 10)   szWPAKey[j++]   = high + '0';
                        else            szWPAKey[j++]   = high - 10 + 'A';
                        if(low < 10)    szWPAKey[j++]   = low + '0';
                        else            szWPAKey[j++]   = low - 10 + 'A';
                    }  
                    szWPAKey[j] = '\0';

                    Serial.print("PSK: ");
                    Serial.println(szWPAKey);

                    Serial.print("Total Main Loop Time: ");
                    Serial.print(tTotalLoop, 10);
                    Serial.println(" msec");

                    Serial.print("Total Conversion Time: ");
                    Serial.print(tTotal / 1000, 10);
                    Serial.println(" msec");

                    Serial.print("Number of Passes: ");
                    Serial.println(cpass, 10);

                    Serial.print("Average Pass Time: ");
                    Serial.print(tTotal / cpass, 10);
                    Serial.println(" usec");

                    state = Idle;
                }

                tTotal += (ReadCoreTimer() - tTime) / CORE_TMR_TICKS_PER_USEC;               
            }
            break;
            
        default:
            state = Idle;
            break;
    } 
    
    if(IsStateAnError(state))
    {
        STATE retState = state;

        Serial.print("Unable to create PSK key: 0x");
        Serial.println(retState, 16);
        state = Idle;
        return(retState);
    }
    else
    {
        return(state);
    }
}

STATE WiFiConnect(WIFINFO& wifi)
{
    static STATE        curState   = Idle;
    static STATE        retState   = Idle;
    IPSTATUS            status  = ipsSuccess;
  
    switch(curState)
    {
        case Idle:
            retState   = Idle;
            wifiScanState.scanState = Stopped;
            wifiScanState.errState  = MustBeDisconnected;
            if(wifi.wifiType == DEWF_SECURITY_WPA_WITH_PASS_PHRASE || wifi.wifiType == DEWF_SECURITY_WPA2_WITH_PASS_PHRASE) 
            {
                ASSERT(wifi.passphrase[0] != '\0');
                curState = WIFICalPSK;
            }
            else 
            {
                curState = WIFIConnect;
            }
            break;
            
        case WIFICalPSK:
            if(deIPcK.convertSSIDandPassPhraseToPSK(wifi.ssid, wifi.passphrase, wifi.key.wpa2Key, &status))
            {
                // we don't want the passphrass in the clear
                memset(wifi.passphrase, 0, sizeof(wifi.passphrase));

                if(wifi.wifiType == DEWF_SECURITY_WPA_WITH_PASS_PHRASE)     wifi.wifiType = DEWF_SECURITY_WPA_WITH_KEY;
                else                                                        wifi.wifiType = DEWF_SECURITY_WPA2_WITH_KEY;

                curState = WIFIConnect;
            }
            break;

        case WIFIConnect:  
            {
                bool    fDone = false;

                switch(wifi.wifiType)
                {
                    case DEWF_SECURITY_OPEN:
                        fDone = deIPcK.wfConnect(wifi.ssid, &status);
                        break;

                    case DEWF_SECURITY_WPA_WITH_KEY:
                    case DEWF_SECURITY_WPA2_WITH_KEY:
                        fDone = deIPcK.wfConnect(wifi.ssid, wifi.key.wpa2Key, &status);
                        break;

                    case DEWF_SECURITY_WEP_40:
                        fDone = deIPcK.wfConnect(wifi.ssid, wifi.key.wep40Key, wifi.keyIndex, &status);
                        break;

                    case DEWF_SECURITY_WEP_104:
                        fDone = deIPcK.wfConnect(wifi.ssid, wifi.key.wep104Key, wifi.keyIndex, &status);
                        break;

                    default:
                        ASSERT(NEVER_SHOULD_GET_HERE);
                        break;
                }

                // if we are done, continue on
                if(fDone)
                {
                    curState = WIFIConnectLinked;
                }
            }
            break;

        case WIFIConnectLinked:
                if(deIPcK.isLinked(&status))
                {
                    curState = WIFIConnectSetupHTTP;
                }
                break;
                    
        case WIFIConnectSetupHTTP:
            if(HTTPSetup() == Idle)
            {
                HTTPEnable(true);
                curState = Idle;
            }
            break;

        // only called on an error
        case WIFIDisconnect:
            if(WiFiDisconnect() == Idle)
            {
                curState = Idle;
                return(retState);
            }
            break;
              
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }   
    
    // if we got an error in the process
    if(IsIPStatusAnError(status))
    {
        retState = MkStateAnError(WIFICBSTATE | status);

        Serial.print("WiFi Connection Error:: 0x");
        Serial.println(retState, 16);

        wifiScanState.scanState = Idle;
        wifiScanState.errState  = Idle;

        // do we need to disconnect
        if(curState == WIFIConnectLinked || curState == WIFIConnectSetupHTTP)
        {
            curState = WIFIDisconnect;
        }

        // just get out
        else
        {
            curState = Idle;
            return(retState);
        }
            
    }

    return(curState);
}

STATE WiFiDisconnect(void)
{
    HTTPEnable(false);
    if(deIPcK.wfDisconnect())
    {
        deIPcK.end();
        wifiScanState.scanState = Idle;
        wifiScanState.errState  = Idle;
        return(Idle);
    }
    
    return(Waiting);
}

STATE WiFiUpdateFirmware(void)
{
    static STATE state = Idle;
    STATE retState = Idle;

    // First make sure we are disconnected from any networks
    switch(state)
    {
        case Idle:
            state = WIFIDisconnect;
            // fall thru

        case WIFIDisconnect:
            if((retState = WiFiDisconnect()) == Idle) 
            {
                state = WIFIUpdateFlash;
                retState = WIFIUpdateFlash;
            }
            else if(IsStateAnError(retState)) 
            {
                state = Idle;
            }
            break;

        case WIFIUpdateFlash:
            if((retState = WiFiUpdateFW()) == Idle)
            {
                Serial.println("Flash Program Done.");
                state = WIFIReDisconnect;
                retState = WIFIReDisconnect;
            }
            else if(IsStateAnError(retState)) 
            {
                state = Idle;
            }
            break;

        case WIFIReDisconnect:
            if((retState = WiFiDisconnect()) == Idle) 
            {
                state = WIFIReInit;
                retState = WIFIReInit;
            }
            else if(IsStateAnError(retState)) 
            {
                state = Idle;
            }
            break;

        case WIFIReInit:
            if((retState = WiFiInit()) == Idle || IsStateAnError(retState))
            {
               state = Idle;
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }
    
    return(retState);
}

STATE WiFiUpdateCertificate(void)
{
    static STATE state = Idle;
    STATE retState = Idle;

    // First make sure we are disconnected from any networks
    switch(state)
    {
        case Idle:
            state = WIFIDisconnect;
            // fall thru

        case WIFIDisconnect:
            if((retState = WiFiDisconnect()) == Idle) 
            {
                state = WIFIUpdateCert;
                retState = WIFIUpdateCert;
            }
            else if(IsStateAnError(retState)) 
            {
                state = Idle;
            }
            break;

        case WIFIUpdateCert:
            if((retState = WiFiUpdateCert()) == Idle)
            {
                Serial.println("Cert Update Done.");
                state = WIFIReDisconnect;
                retState = WIFIReDisconnect;
            }
            else if(IsStateAnError(retState)) 
            {
                state = Idle;
            }
            break;

        case WIFIReDisconnect:
            if((retState = WiFiDisconnect()) == Idle) 
            {
                state = WIFIReInit;
                retState = WIFIReInit;
            }
            else if(IsStateAnError(retState)) 
            {
                state = Idle;
            }
            break;

        case WIFIReInit:
            if((retState = WiFiInit()) == Idle || IsStateAnError(retState))
            {
               state = Idle;
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }
    
    return(retState);
}

int32_t WiFiCreateName(char const * const szSsid, bool fAutoConnect, char * szFileName, int32_t cbFileName)
{
    int32_t cbSsid = strlen(szSsid);        

    if(cbFileName > 0) szFileName[0] = '\0';

    // + ".WF[0|1]\0"
    if((cbSsid + 5) > cbFileName) return(0);

    strcpy(szFileName, szSsid);
    strcpy(&szFileName[cbSsid], ".WF");
    cbSsid += 3;
    if(fAutoConnect)  strcat(&szFileName[cbSsid], "1");
    else strcat(&szFileName[cbSsid], "0");
    cbSsid++;

    return(cbSsid);
}
