/************************************************************************/
/*                                                                      */
/*    LoopStats.h                                                       */
/*                                                                      */
/*    Header for Loop statistics support                                */
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
/*    9/13/2018(KeithV): Created                                        */
/************************************************************************/
#ifndef _LOOP_STAT_INCLUDE_
#define _LOOP_STAT_INCLUDE_

#ifdef __cplusplus

class LOOPSTATS 
{
public:

    typedef enum
    {
        SUPERLOOP,
        INITFLSHVOL,
        INITCALDATA,
        WIFIINIT,
        WIFISCAN,
        WIFICONN,
        SWITCH,
        ADC,
        ADCBACKLOG,
        ADCTRG,
        ADCTRGBACKLOG,
        DBDLADCTRG,
        CLOUDTRG,
        TEST,
        DEFERRED,
        HTTP,
        HTTPRECONNECT,
        DEIP,
        JSON,
        JSONCMD,
        SERIAL,
        FT245,
        LED,
        STATUPD,
        SDSWAP,
        SDBUSYTIME,
        SDBUSYCNT,
        THREADCNT,
        MANTIME,
        STEND
    } STATID;

   static char const * const agszStats[STEND];

private:

    typedef struct
    {
        STATID      statID;
        uint32_t    tEnd;
    } SENRY;

    typedef struct
    {
        int64_t     cEntry;
        int64_t     min;
        int64_t     max;
        int64_t     total;
        bool        fCnt;
    } STATINFO;

    static const int32_t MAXVALUE = 0x7FFFFFFFl;
    static const int32_t MINVALUE = 0x80000000l;
    static const uint32_t PWR2AVE = 10;
    static const uint32_t  MAXENTRY = STEND;

    uint32_t    iNext;
    uint32_t    tStart;
    SENRY       agSENRY[MAXENTRY];
    STATINFO    agSTATINFO[STEND];

    void UpdateEntry(STATINFO& statInfo, int32_t value);
    uint32_t UpdateTime(SENRY& sEntry, uint32_t tLast);
 
public:

    LOOPSTATS();
    bool GetStats(STATID stID, int64_t& min, int64_t& ave, int64_t& max, int64_t& total);

    void RecordTime(STATID stID)
    {
        ASSERT(iNext < MAXENTRY);
        agSENRY[iNext].statID = stID;
        agSENRY[iNext++].tEnd = ReadCoreTimer();
    }

    void ClearEntry(STATID stID)
    {
        bool fCnt = agSTATINFO[stID].fCnt;

        memset(&agSTATINFO[stID], 0, sizeof(STATINFO));
        agSTATINFO[stID].min    = MAXVALUE;
        agSTATINFO[stID].max    = MINVALUE;
        agSTATINFO[stID].fCnt   = fCnt;
    }

    void SetAsCount(STATID stID)
    {
        agSTATINFO[stID].fCnt = true;
    }

    bool IsCount(STATID stID)
    {
        return(agSTATINFO[stID].fCnt);
    }

    void RecordCount(STATID stID, int32_t count)
    {
        if(agSTATINFO[stID].fCnt) UpdateEntry(agSTATINFO[stID], count);
    }

    void UpdateEntry(STATID stID, int32_t value)
    {
        UpdateEntry(agSTATINFO[stID], value);
    }

    void UpdateStats(void);
};

extern LOOPSTATS    loopStats;

extern "C"
{
#endif

// Loop statistic variables

extern int64_t    tMinInit;
extern int64_t    tAveInit;
extern int64_t    tMaxInit;

extern uint32_t    tManual;     // For manually placing in the code for time checks

void RecordThreadCnt(uint32_t cCurThreadYield);

#ifdef __cplusplus
}
#endif

#endif // LOOP STATS
