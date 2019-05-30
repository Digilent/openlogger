/************************************************************************/
/*                                                                      */
/*    JsonIO.h                                                          */
/*                                                                      */
/*    Header for JSON IO stream that are supported                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/* 
*
* Copyright (c) 2013-2018, Digilent <www.digilent.com>
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
/*  Revision History:                                                   */
/*    8/6/2018(KeithV): Created                                        */
/************************************************************************/


class JSONSerial : public JSONIO
{
    private:
        DMASerial&  serial;
        STATE       writeState;

        JSONSerial();           // no such constructor

    public:
        JSONSerial(DMASerial& dmaSerial) : serial(dmaSerial), writeState(Idle) {}

        uint32_t Available(void) { return(serial.available()); }
        STATE Read(void * const pBuff, uint32_t cbBuff, int32_t * const pcbRead);
        int32_t Read(void) { return(serial.read()); }
        STATE Write(void const * const pBuff, uint32_t cbWrite, int32_t * const pcbWritten);
        void Purge(void) { return(serial.purge()); }
        void Done(void) { state = stateWhenComplete; }
};

class JSONPost : public JSONIO
{
    private:

    public:
        JSONPost() {}          

        uint32_t Available(void);
        STATE Read(void * const pBuff, uint32_t cbBuff, int32_t * const pcbRead);
        int32_t Read(void);
        STATE Write(void const * const pBuff, uint32_t cbWrite, int32_t * const pcbWritten);
        void Purge(void);
        void Done(void);
};

class JSONFT245 : public JSONIO
{
    private:

        static const uint32_t  FIFO245BUFSIZ = 32768;   // must be a multiple of 16 and less than 65520
        static const uint32_t  MAXPASSCNT = 100;

        uint8_t  PHY ffRead[FIFO245BUFSIZ];     // NOTE: coherent won't work in a structure/class
        uint8_t * const   ffReadK1;             // Create a pointer that we can assign ffRead with KSEG1 addressing.

        int32_t      iRdStart;
        int32_t      iRdEnd;                    // iRdEnd >= iRdStart, always (do modulo math when indexing)
        int32_t      cbDMA;
        uint32_t     iDMACur;
        uint32_t     cPass;
        uint32_t     cStart;

    public:

        static const uint32_t  FOREVERTO = 0xFFFFFFFFul;

        JSONFT245() : ffReadK1((uint8_t *) KVA_2_KSEG1(ffRead)), iRdStart(0), iRdEnd(0), cbDMA(0), iDMACur(0), cPass(0), cStart(0) {}         

        uint32_t Available(void);
        STATE Read(void * const pBuff, uint32_t cbBuff, int32_t * const pcbRead);
        int32_t Read(void);
        STATE Write(void const * const pBuff, uint32_t cbWrite, int32_t * const pcbWritten) {return(Write(pBuff, cbWrite, *pcbWritten, FOREVERTO));}
        STATE Write(void const * const  pBuff, uint32_t cbBuff, int32_t& cbWritten, uint32_t msTimeout);
        void Purge(void);
        void Done(void) { state = stateWhenComplete; }
        void PeriodicTask(void);
};