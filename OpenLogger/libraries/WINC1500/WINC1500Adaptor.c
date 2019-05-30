/************************************************************************/
/*                                                                      */
/*	WINC1500Adaptor.c This implements a Nework adaptor                  */
/*	for the WINC1500                                                    */
/*                                                                      */
/************************************************************************/
/*	Author: 	Keith Vogel                                             */
/*	Copyright 2018, Digilent Inc.                                       */
/************************************************************************/
/* 
*
* Copyright (c) 2013-2018, Digilent <www.digilentinc.com>
* Contact Digilent for the latest version.
*
* This program is free software; distributed under the terms of 
* BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*	A WiFi Network adaptor using the Microchip WINC1500 codebase        */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*	2/21/2018(KeithV): Created                                          */
/*                                                                      */
/************************************************************************/
#include "WINC1500Adaptor.h"

static bool IsInitialized(IPSTATUS * pStatus)
{
    // we ONLY DO THIS ONCE
    static bool fInitSetup = false;

    AssignStatusSafely(pStatus, ForceIPStatus(wfWINC1500.priv.initStatus));
    
    if(!fInitSetup)
    {
        // Run the task because we might not be in the Adaptor yet
        WINC_Task();

        if(wfWINC1500.priv.initStatus == M2M_WIFI_CONNECTED)
        {
    //        WF_ReconnectModeSet(3, WF_ATTEMPT_TO_RECONNECT, 40, WF_ATTEMPT_TO_RECONNECT);

            // Some interesting scan options
            // to look for hidden SSID, put them in this list
            // sint8 m2m_wifi_request_scan_ssid_list(uint8 ch,uint8 * u8Ssidlist);

            // once a scan is done, you can ask how many APs found
            // uint8 m2m_wifi_get_num_ap_found(void);

            // set scan time options if defaults don't work
            // sint8 m2m_wifi_set_scan_options(tstrM2MScanOption* ptstrM2MScanOption);

            // use NORTH_AMERICA (11 channels), could be EUROPE (13 channels) or ASIA (14 channels)
            m2m_wifi_set_scan_region(NORTH_AMERICA);

            // we want to make sure we get the NULL mac, as this is a broadcast of sorts.
            m2m_wifi_enable_mac_mcast((uint8_t *) &MACNONE, true);

            // we are done
            fInitSetup = true;
        }
    }
    
    return(wfWINC1500.priv.initStatus == M2M_WIFI_CONNECTED);
}

static bool Disconnect(void)
{   
    // if we are disconnected, we are done, 
    if(wfWINC1500.priv.connectionStatus == M2M_WIFI_DISCONNECTED)
    {
        return(true);
    }
    
    // if we are busy we need to wait
    else if(wfWINC1500.priv.fMRFBusy)
    {
        return(false);    
    }
    
    // if we are connected, start a disconnect
    else if(wfWINC1500.priv.connectionStatus == M2M_WIFI_CONNECTED)
    {
        wfWINC1500.priv.fMRFBusy = (m2m_wifi_disconnect() == M2M_SUCCESS);
        return(false);
    }
    
    // I suppose if we are in monitor mode or something, this could happen
    else
    {   
        ASSERT(NEVER_SHOULD_GET_HERE);
        return(false);
        
//        wfWINC1500.priv.connectionStatus = M2M_WIFI_DISCONNECTED;
//        wfWINC1500.priv.connectionError = M2M_WIFI_DISCONNECTED;
    }
}

// Linked means, connected to the network
// just being initialized is NOT being linked!
// we must be connected to the AP to be linked
static bool IsLinked(IPSTATUS * pStatus)
{
    if(wfWINC1500.priv.fMRFBusy)
    {
        if(wfWINC1500.priv.connectionError == M2M_ERR_CONN_INPROGRESS) 
        {
            AssignStatusSafely(pStatus, ForceIPStatus(M2M_ERR_CONN_INPROGRESS));
        }
        else 
        {
            AssignStatusSafely(pStatus, ForceIPError(M2M_ERR_CONN_INPROGRESS));
        }
        
        return(false);        
    }
    else if(wfWINC1500.priv.connectionStatus == M2M_WIFI_CONNECTED)
    {
        AssignStatusSafely(pStatus, ipsSuccess);
        return(true);
    }
    
    else if(wfWINC1500.priv.connectionError == M2M_ERR_ASSOC_FAIL)
    {
        AssignStatusSafely(pStatus, ForceIPStatus(M2M_ERR_ASSOC_FAIL));
        return(false);       
    }

    else if(!IsInitialized(pStatus))
    {
        return(false);
    }

    // not connected, not working on it, we have some kind of error
    AssignStatusSafely(pStatus, ForceIPError(M2M_WIFI_DISCONNECTED));
    return(false);
}

// we want to be initialized, but not linked and not in progress
// this is so we can start a connection process.
static bool IsInitNotLinked(IPSTATUS * pStatus)
{
    if(wfWINC1500.priv.fMRFBusy)
    {
        AssignStatusSafely(pStatus, ipsInUseW);
        return(false);
    }
    else if(wfWINC1500.priv.connectionStatus != M2M_WIFI_DISCONNECTED)
    {
        AssignStatusSafely(pStatus, ipsInUse);
        return(false);

    }
        
    // we are not busy and we are in the initialize state and we have
    // not connected. However, in the past, we may have attempted a disconnect while
    // during a reconnect, so lets just make sure we are disconnected
    return(Disconnect());
}

static bool Send(IPSTACK * pIpStack, IPSTATUS * pStatus)
{
    AssignStatusSafely(pStatus, ipsSuccess);
    pIpStack->fOwnedByAdp = true;
    FFInPacket(&wfWINC1500.priv.ffptWrite, pIpStack);
    return(true);
}

static bool SendNextIpStack(void)
{
    sint8   ret;
    
    // get the next stack to send if we have one
    if(wfWINC1500.priv.pIpStackBeingTx == NULL)
    {
        wfWINC1500.priv.pIpStackBeingTx = FFOutPacket(&wfWINC1500.priv.ffptWrite);
    }

    // try and send it.
    if(wfWINC1500.priv.pIpStackBeingTx != NULL)
    {
        IPSTACK *   pIPStack = wfWINC1500.priv.pIpStackBeingTx;
        int16_t     cbTotal = pIPStack->cbFrame + pIPStack->cbIPHeader + pIPStack->cbTranportHeader + pIPStack->cbPayload;
        

       if(cbTotal)
        {
            uint8_t *   pEthernetPkt    = alloca(cbTotal);
            uint8_t *   pb              = pEthernetPkt;

            // always have a frame, alway FRAME II (we don't support 802.3 outgoing frames; this is typical)
            memcpy(pb, (uint8_t *) pIPStack->pFrameII, pIPStack->cbFrame);
            pb += pIPStack->cbFrame;

            // IP Header
            if(pIPStack->cbIPHeader > 0)
            {
                memcpy(pb, (uint8_t *) pIPStack->pIPHeader, pIPStack->cbIPHeader);
                pb += pIPStack->cbIPHeader;
            }

            // Transport Header (TCP/UDP)
            if(pIPStack->cbTranportHeader > 0)
            {
                memcpy(pb, (uint8_t *) pIPStack->pTransportHeader, pIPStack->cbTranportHeader);
                pb += pIPStack->cbTranportHeader;
            }

            // Payload / ARP / ICMP
            if(pIPStack->cbPayload > 0)
            {
                memcpy(pb, (uint8_t *) pIPStack->pPayload, pIPStack->cbPayload);
                pb += pIPStack->cbPayload;
            }

            // transmit
            ret = m2m_wifi_send_ethernet_pkt(pEthernetPkt, cbTotal);

            // we sent it, clean up
            pIPStack->fOwnedByAdp = false;
            IPSRelease(pIPStack);
            wfWINC1500.priv.pIpStackBeingTx = NULL;

            return(ret == M2M_SUCCESS);
        }
        else
        {
            return(false);
        }
    }

    return(true);
}

static IPSTACK * Read(IPSTATUS * pStatus)
{
    IPSTACK *   pIpStack = FFOutPacket(&wfWINC1500.priv.ffptRead);

    if(pIpStack != NULL)
    {
        pIpStack->fOwnedByAdp = false;
    }

    AssignStatusSafely(pStatus, ipsSuccess);
    return(pIpStack);
}
// this code is not complete or tested; 
// The problem is, we can only handle calling MRF init once, if
// we lose the fundamental connection to the MRF at this time we must MCLR.
//
// At some point in the future we may wish to make close diconnect the MRF as well
// we would then want to reset the fInitSetup in Initialize()
// we would also have to loop until the MRF successfully disconnected from the AP so we
// can safely drop the connection to the MRF.
static bool Close(void)
{
    // Disconnect will not break the connection to the MRF
    // it will only drop the connection to the AP
    Disconnect();
    return(true);
}

static bool StartScan(WFSCANMODE filter, IPSTATUS * pStatus)
{
    // if we are working on a scan
    if(wfWINC1500.priv.scanResult.u8ch == 0xFF)
    {
        if(wfWINC1500.priv.fMRFBusy)
        {
            AssignStatusSafely(pStatus, ipsPending);
        }
        else
        {
            wfWINC1500.priv.scanResult.u8ch = 0;    // done scanning
            AssignStatusSafely(pStatus, ipsSuccess);
            return(true);
        }
    }
    else if(!IsInitialized(pStatus))
    {
        AssignStatusSafely(pStatus, ipsNotInitialized);
    }
    else if(wfWINC1500.priv.connectionStatus != M2M_WIFI_DISCONNECTED || wfWINC1500.priv.fMRFBusy)
    {
        AssignStatusSafely(pStatus, ipsInUse);
    }
    else
    {        
        sint8 ret = M2M_ERR_INVALID_ARG;
    
        if(filter == DEWF_PASSIVE_SCAN) 
        {
            ret = m2m_wifi_request_scan_passive(M2M_WIFI_CH_ALL, 0);  // time in ms, 0 is default time
        }
        else // DEWF_ACTIVE_SCAN
        {
            // m2m_wifi_set_scan_options(4, 100);
            ret = m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
        }
        
        if(ret == M2M_SUCCESS)
        {
            wfWINC1500.priv.fMRFBusy = true;
            wfWINC1500.priv.cScanResults = -1;              // indicate scanning
            wfWINC1500.priv.scanResult.u8index = 0xFF;      // to init first time to no scan data
            wfWINC1500.priv.scanResult.u8ch = 0xFF;         // means we are doing a scan
            AssignStatusSafely(pStatus, ipsPending);
        }
        else
        {
            AssignStatusSafely(pStatus, ForceIPError(ret));    
        }
    }
    return(false);
}

static bool GetScanResult(int32_t index, SCANINFO *pScanResult, IPSTATUS * pStatus)
{
    // currently working, 0xFE means we are still doing a scan result
    if(wfWINC1500.priv.scanResult.u8ch == 0xFE)
    {
            AssignStatusSafely(pStatus, ipsPending);
    }
    
    // out of range
    else if(index < 0 || wfWINC1500.priv.cScanResults <= index)
    {
        AssignStatusSafely(pStatus, ispInvalidArgument);
    }
     
    // got it, return the results
    else if(index == wfWINC1500.priv.scanResult.u8index)
    {        
        memcpy(pScanResult->bssid, wfWINC1500.priv.scanResult.au8BSSID, DEWF_BSSID_LENGTH);
        strcpy((char *) pScanResult->ssid, (char *) wfWINC1500.priv.scanResult.au8SSID);
        pScanResult->ssidLen        = strlen((char *) wfWINC1500.priv.scanResult.au8SSID);
        pScanResult->rssi           = wfWINC1500.priv.scanResult.s8rssi;
        pScanResult->channel        = wfWINC1500.priv.scanResult.u8ch;
        pScanResult->bssType        = DEWF_INFRASTRUCTURE;

        // this is different than the MRF24 that gets the funky bitfield 
        switch(wfWINC1500.priv.scanResult.u8AuthType)
        {
            case M2M_WIFI_SEC_802_1X:                   
                pScanResult->apConfig = DEWF_SECURITY_ENTERPRISE;
                break;

            case M2M_WIFI_SEC_WPA_PSK:
                pScanResult->apConfig = DEWF_SECURITY_WPA;
                break;

            case M2M_WIFI_SEC_WEP:                      
                pScanResult->apConfig = DEWF_SECURITY_WEP;
                break;

            default:
                pScanResult->apConfig = DEWF_SECURITY_OPEN;
                break;
        }

       // TODO, need to find out these values     
        pScanResult->beaconPeriod   = 0;
        pScanResult->atimWindow     = 0;
        pScanResult->dtimPeriod     = 0;
    //        pScanResult->basicRateSet   = 0;
        pScanResult->cBasicRates    = 0;
        
        AssignStatusSafely(pStatus, ipsSuccess);        
        return(true);
    }
    
    // start a new request
    else 
    {
        sint8 ret = m2m_wifi_req_scan_result(index);
        
        if(ret == M2M_SUCCESS)
        {
            wfWINC1500.priv.fMRFBusy = true;
            AssignStatusSafely(pStatus, ipsPending);
            wfWINC1500.priv.scanResult.u8index = index;
            wfWINC1500.priv.scanResult.u8ch = 0xFE;         // means we are doing a get scan result
        }
        
        // something went wrong
        else
        {
            AssignStatusSafely(pStatus, ForceIPError(ret));    
        }
    }
       
    // by default, not done
    return(false);
}

// must loop on connect until a connect
static bool Connect(SECURITY security, const uint8_t * szSsid, const void * pvPkt, int iKey, bool fPICKeyCalc, IPSTATUS * pStatus)
{    
    if(!IsInitialized(pStatus))
    {
        return(false);
    }
    else if(szSsid == NULL || (security != DEWF_SECURITY_OPEN && pvPkt == NULL))
    {
        AssignStatusSafely(pStatus, ispInvalidArgument);
    }   
    else if(!wfWINC1500.priv.fMRFBusy && wfWINC1500.priv.connectionError != M2M_SUCCESS)
    {
        AssignStatusSafely(((IPSTATUS *) pStatus), ForceIPError(wfWINC1500.priv.connectionError));
        wfWINC1500.priv.connectionStatus = M2M_WIFI_DISCONNECTED;
        wfWINC1500.priv.connectionError = M2M_SUCCESS;
        return(false);
    }   
    else if(IsInitNotLinked(pStatus))
    {       
        sint8 ret = M2M_ERR_INIT;
        
        switch(security)
        {
            case DEWF_SECURITY_OPEN:
                ret = m2m_wifi_connect((char *) szSsid, strlen((char *) szSsid), M2M_WIFI_SEC_OPEN, NULL, M2M_WIFI_CH_ALL);
                break;

            case DEWF_SECURITY_WPA_WITH_PASS_PHRASE:
            case DEWF_SECURITY_WPA2_WITH_PASS_PHRASE:
            case DEWF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
                ret = m2m_wifi_connect((char *) szSsid, strlen((char *) szSsid), M2M_WIFI_SEC_WPA_PSK, (void *) pvPkt, M2M_WIFI_CH_ALL);
                break;
                         
            case DEWF_SECURITY_WPA_WITH_KEY:
            case DEWF_SECURITY_WPA2_WITH_KEY:
            case DEWF_SECURITY_WPA_AUTO_WITH_KEY:
                {
                    WPA2KEY * pWpaKey = (WPA2KEY *) pvPkt; 
                    char szWPAKey[2*DEWF_PSK_LENGTH+1];
                    int i,j;

                    // WINC wants an ascii string of the key
                    for(i=0, j=0; i<DEWF_PSK_LENGTH; i++)
                    {
                        uint8_t high                    = pWpaKey->rgbKey[i] >> 4;
                        uint8_t low                     = pWpaKey->rgbKey[i] & 0xF;
                        if(high < 10)   szWPAKey[j++]   = high + '0';
                        else            szWPAKey[j++]   = high - 10 + 'A';
                        if(low < 10)    szWPAKey[j++]   = low + '0';
                        else            szWPAKey[j++]   = low - 10 + 'A';
                    }  
                    szWPAKey[j] = '\0';

                    ret = m2m_wifi_connect((char *) szSsid, strlen((char *) szSsid), M2M_WIFI_SEC_WPA_PSK, (void *) szWPAKey, M2M_WIFI_CH_ALL);
                }
                break;   

            case DEWF_SECURITY_WEP_40:
                {
                    WEP40KEY * pWepKey = (WEP40KEY *) pvPkt; 
                    tstrM2mWifiWepParams wepParams;
                    int i,j;

                    wepParams.u8KeySz   = WEP_40_KEY_STRING_SIZE+1;
                    wepParams.u8KeyIndx = iKey;

                    // WINC wants an ascii string of the key
                    for(i=0, j=0; i<sizeof(pWepKey->key[0].rgbKey); i++)
                    {
                        uint8_t high                                = pWepKey->key[iKey-1].rgbKey[i] >> 4;
                        uint8_t low                                 = pWepKey->key[iKey-1].rgbKey[i] & 0xF;
                        if(high < 10)   wepParams.au8WepKey[j++]    = high + '0';
                        else            wepParams.au8WepKey[j++]    = high - 10 + 'A';
                        if(low < 10)    wepParams.au8WepKey[j++]    = low + '0';
                        else            wepParams.au8WepKey[j++]    = low - 10 + 'A';
                    }  
                    wepParams.au8WepKey[j] = '\0';

                    ret = m2m_wifi_connect((char *) szSsid, strlen((char *) szSsid), M2M_WIFI_SEC_WEP, (void *) &wepParams, M2M_WIFI_CH_ALL);
                }
                break;   

            case DEWF_SECURITY_WEP_104:
            {
                WEP104KEY * pWepKey = (WEP104KEY *) pvPkt; 
                tstrM2mWifiWepParams wepParams;
                int i,j;

                wepParams.u8KeySz   = WEP_104_KEY_STRING_SIZE+1;
                wepParams.u8KeyIndx = iKey;   

                // WINC wants an ascii string of the key
                for(i=0, j=0; i<sizeof(pWepKey->key[0].rgbKey); i++)
                {
                    uint8_t high                                = pWepKey->key[iKey-1].rgbKey[i] >> 4;
                    uint8_t low                                 = pWepKey->key[iKey-1].rgbKey[i] & 0xF;
                    if(high < 10)   wepParams.au8WepKey[j++]    = high + '0';
                    else            wepParams.au8WepKey[j++]    = high - 10 + 'A';
                    if(low < 10)    wepParams.au8WepKey[j++]    = low + '0';
                    else            wepParams.au8WepKey[j++]    = low - 10 + 'A';
                }  
                wepParams.au8WepKey[j] = '\0';

                ret = m2m_wifi_connect((char *) szSsid, strlen((char *) szSsid), M2M_WIFI_SEC_WEP, (void *) &wepParams, M2M_WIFI_CH_ALL);
            }
            break;   

            case DEWF_SECURITY_ENTERPRISE:   
                {
                    ENTPRCRED *             pEntAuth = (ENTPRCRED *) pvPkt;
                    tstr1xAuthCredentials   authCred;
                    
                    strcpy((char *) authCred.au8UserName, pEntAuth->szUserName);
                    strcpy((char *) authCred.au8Passwd, pEntAuth->szPasswd);
                    
                    ret = m2m_wifi_connect((char *) szSsid, strlen((char *) szSsid), M2M_WIFI_SEC_802_1X, (void *) &authCred, M2M_WIFI_CH_ALL);
                }
                break;
                
            // TODO
            case DEWF_SECURITY_WPS_PUSH_BUTTON:
            case DEWF_SECURITY_WPS_PIN:
                AssignStatusSafely(((IPSTATUS *) pStatus), ipsNotSupported);
                return(false);
                break;

            default:
                AssignStatusSafely(((IPSTATUS *) pStatus), ispInvalidArgument);
                return(false);
                break;
        }
        
        // not able to connect, return the underlying error
        if(ret != M2M_SUCCESS)
        {
            AssignStatusSafely(((IPSTATUS *) pStatus), ForceIPError(ret));
            return(false);
        }
        
        // go and connect
        wfWINC1500.priv.connectionError = M2M_ERR_CONN_INPROGRESS;
        wfWINC1500.priv.fMRFBusy = true;
    }
     
    // when we are linked, we are connected to a WiFi network
    return(IsLinked(pStatus));
}

static void WINC1500PeriodicTasks(void)
{
    IPSTATUS status;
    
    SendNextIpStack();
    
    // if initialization has started, we want to finish
    IsInitialized(&status);     
    WINC_Task();
}

WINC1500D wfWINC1500 =
{
    {
        WINC1500_NWA_VERSION,
        false,
        WINC1500_NWA_MTU_RX,
        WINC1500_NWA_MIN_TX_MTU,
        {.u8 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        NULL,
        WINC1500PeriodicTasks,
        IsLinked,
        IsLinked,
        Send,
        Read,
        Close,
    },
    {
        IsInitialized,
        Connect,
        Disconnect,
        StartScan,
        GetScanResult,
    },
    {
        {NULL, NULL},
        {NULL, NULL},
        NULL,
        M2M_WIFI_DISCONNECTED,
        M2M_WIFI_DISCONNECTED,
        0,
        {0},
        false,
        0x00,
        M2M_WIFI_DISCONNECTED,
    },
};

const NWADP * GetWINC1500Adaptor(MACADDR *pUseThisMac, HRRHEAP hAdpHeap, IPSTATUS * pStatus)
{
    if(hAdpHeap == NULL)
    {
        AssignStatusSafely(pStatus, ipsNoHeapGiven);
        return(NULL);
    }

    AssignStatusSafely(pStatus, ipsSuccess);
    wfWINC1500.priv.initStatus = M2M_WIFI_DISCONNECTED;
    wfWINC1500.priv.connectionStatus = M2M_WIFI_DISCONNECTED;
    wfWINC1500.priv.cScanResults = -1;
    wfWINC1500.adpWINC1500.hAdpHeap = hAdpHeap;
    wfWINC1500.priv.pIpStackBeingTx = NULL;
    memset(&wfWINC1500.priv.ffptRead, 0, sizeof(FFPT));
    memset(&wfWINC1500.priv.ffptWrite, 0, sizeof(FFPT));

    if(WINCInitWiFi())
    {
        sint8 retStatus = M2M_SUCCESS;
        
        // fix up the MAC
        if(pUseThisMac != NULL)
        {
            memcpy(&wfWINC1500.adpWINC1500.mac, pUseThisMac, sizeof(MACADDR));
            retStatus = m2m_wifi_set_mac_address((uint8_t *) &wfWINC1500.adpWINC1500.mac);
        }
        else
        {
            retStatus = m2m_wifi_get_mac_address((uint8_t *) &wfWINC1500.adpWINC1500.mac);
        }
        ASSERT(retStatus == M2M_SUCCESS);
        
        wfWINC1500.priv.initStatus = M2M_WIFI_CONNECTED;
        return(&wfWINC1500.adpWINC1500);
    }
    
    return(NULL);
}

const NWWF * GetWINC1500WF(void)
{
    return(&wfWINC1500.wfWINC1500);
}

