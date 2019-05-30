/************************************************************************/
/*                                                                      */
/*	MRF24GAdaptor.h This implements a Nework adaptor                    */
/*	for the MRF24WG0MA                                                  */
/*                                                                      */
/************************************************************************/
/*	Author: 	Keith Vogel                                             */
/*	Copyright 2013, Digilent Inc.                                       */
/************************************************************************/
/* 
*
* Copyright (c) 2013-2014, Digilent <www.digilentinc.com>
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
/*	A WiFi Network adaptor using the Microchip Universal Driver         */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*	5/31/2013(KeithV): Created                                          */
/*                                                                      */
/************************************************************************/


#ifndef WINC1500ADAPTOR_H
#define	WINC1500ADAPTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <PinAssignments.h>
#include "../DEIPcK/utility/deIP.h"
#include "./utility/driver/include/m2m_wifi.h"


// 00000001 -> 0000FFFF; Adaptor status; specific to adaptor



// 10000001 -> 1000FFFF; Adaptor errors; specific to adaptor

#define WFNbrAllocFails 10

#define WINC1500_NWA_VERSION          0x01000101
#define WINC1500_NWA_MTU_RX_FRAME     1536
#define WINC1500_NWA_MTU_RX           1500
#define WINC1500_NWA_MIN_TX_MTU       TCP_EMTU_S

typedef struct WINC1500P_T
{
    FFPT                    ffptRead;
    FFPT                    ffptWrite;
    IPSTACK *               pIpStackBeingTx;
    IPSTATUS    volatile    initStatus;
    IPSTATUS    volatile    connectionStatus;
    int32_t     volatile    cScanResults;
    tstrM2mWifiscanResult   scanResult;
    bool        volatile    fMRFBusy;      // scan, key gen, connect
    uint8_t     volatile    lastEvent;
    uint32_t    volatile    connectionError;
} WINC1500P;

typedef struct WINC1500D_T
{
    NWADP           adpWINC1500;
    NWWF            wfWINC1500;
    WINC1500P       priv;
} WINC1500D;

const NWADP * GetWINC1500Adaptor(MACADDR *pUseThisMac, HRRHEAP hAdpHeap, IPSTATUS * pStatus);
const NWWF *  GetWINC1500WF(void);

extern bool WINCInitWiFi(void);
extern void WINC_Task(void);

extern          WINC1500D         wfWINC1500;

#ifdef	__cplusplus
}
#endif

#endif	/* WINC1500ADAPTOR_H */

