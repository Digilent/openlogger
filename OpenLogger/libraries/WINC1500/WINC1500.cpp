/************************************************************************/
/*                                                                      */
/*  WINC.cpp    The Digilent WIFI static class For the chipKIT          */
/*                  software compatible product line. This includes the */
/*                  Arduino compatible chipKIT boards as well as        */
/*                  the Digilent Cerebot cK boards.                     */
/*                                                                      */
/************************************************************************/
/*  Author: Keith Vogel                                                 */
/*          Copyright 2018, Digilent Inc.                               */
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
/*      This the Static DEWFcK Class implementation file                */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  2/21/2018 (KeithV): Created                                        */
/*                                                                      */
/************************************************************************/
#include <OpenLogger.h>

/************************************************************************/
/*    WINC Serial Debug Object                                          */
/************************************************************************/
#if (M2M_LOG_LEVEL != M2M_LOG_NONE)
// the serial buffer
uint8_t PHY uartWINCBuff[65520]; // keep at least 16 bytes below 65536

// the serial object (AND ISR)
DMASerialOBJ(WINCSerial, ((uint8_t *) KVA_2_KSEG1(uartWINCBuff)), sizeof(uartWINCBuff), WIFI_UART, WIFI_DBG_UART_RX_DMA, WIFI_DBG_UART_ISR_PRI, WIFI_DBG_UART_ISR_PRI);
#endif

const NWADP * WINC1500::deIPGetAdaptor(void)
{

    return(GetWINC1500Adaptor(NULL, hRRAdpHeap, NULL));
}

const NWWF * WINC1500::deWFGetWF()
{
    return(GetWINC1500WF());
}

extern "C" void CONF_WINC_PRINTF(char * sz, ...)
{
    (void) sz;
#if (M2M_LOG_LEVEL != M2M_LOG_NONE)
    char szT[2048]; 
    va_list args;
    va_start(args, sz);
    vsprintf(szT, sz, args); 
    Serial.print(szT);
}

extern "C" void WINCSerilTask(void)
{
    while(WINCSerial.available()) 
    {
        char chWINC = (char) WINCSerial.read();
        Serial.print(chWINC);
    }
#endif
}
