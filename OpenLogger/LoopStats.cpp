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

int64_t    tMinInit             = 0;
int64_t    tAveInit             = 0;
int64_t    tMaxInit             = 0;

uint32_t   tManual              = 0;

LOOPSTATS loopStats = LOOPSTATS();

char const * const LOOPSTATS::agszStats[STEND] = { 
                                                    "SUPER LOOP",
                                                    "INIT FLASH VOL",
                                                    "INIT CALIBRATION DATA",
                                                    "INIT WIFI",
                                                    "WIFI SCAN",
                                                    "WIFI CONN",
                                                    "SWITCH",
                                                    "ADC",
                                                    "ADC RAW TO VOLTAGE BACKLOG",
                                                    "ADC TARGET",
                                                    "ADC TARGET BACKLOG",
                                                    "ADC TARGET BACKLOG DERIVATIVE",
                                                    "CLOUD TARGET",
                                                    "TEST",
                                                    "DEFERED",
                                                    "HTTP",
                                                    "HTTP RECONNECT",
                                                    "DEIP",
                                                    "JSON",
                                                    "JSON COMMANDS",
                                                    "SERIAL",
                                                    "FT245",
                                                    "LED",
                                                    "STAT UPDATE",
                                                    "SD SWAP", 
                                                    "SD BUSY TIME",                                             
                                                    "SD BUSY IN A ROW",
                                                    "THREAD YIELDS",
                                                    "MANUALLY PLACED TIME"
                                                };


LOOPSTATS::LOOPSTATS()
{
    int stID;

    memset(agSTATINFO, 0, sizeof(agSTATINFO));
    memset(agSENRY, 0, sizeof(agSENRY));

    iNext = 0;
    tStart = ReadCoreTimer();

    for(stID = SUPERLOOP; stID < STEND; stID++) ClearEntry((STATID) stID);
}

void LOOPSTATS::UpdateEntry(STATINFO& statInfo, int32_t value)
{
    if(value > statInfo.max) statInfo.max = value;
    if(value < statInfo.min) statInfo.min = value;
    statInfo.total += value;
    statInfo.cEntry++;
}

uint32_t LOOPSTATS::UpdateTime(SENRY& sEntry, uint32_t tLast)
{
    if(!agSTATINFO[sEntry.statID].fCnt) UpdateEntry(agSTATINFO[sEntry.statID], sEntry.tEnd - tLast);
    return(sEntry.tEnd);
}

void LOOPSTATS::UpdateStats(void)
{
    uint32_t    tLast = tStart;
    uint32_t    tStartUpd = ReadCoreTimer();
    uint32_t    i;
    SENRY       sEntryT;

    for(i = 0; i < iNext; i++) tLast = UpdateTime(agSENRY[i], tLast);

    memset(agSENRY, 0, sizeof(agSENRY));
    iNext = 0;

    // this time will actually go into the next loop
    sEntryT.statID = STATUPD;
    sEntryT.tEnd = ReadCoreTimer();     // where the next loop time starts
    UpdateTime(sEntryT, tStartUpd);    // update loop stats

    sEntryT.statID = SUPERLOOP;         // update the superloop
    UpdateTime(sEntryT, tStart);

    tStart = sEntryT.tEnd;              // put it in tStart
}

bool LOOPSTATS::GetStats(STATID stID, int64_t& min, int64_t& ave, int64_t& max, int64_t& total)
{
    min = agSTATINFO[stID].min;
    if(min == MAXVALUE) min = 0;
        
    if(agSTATINFO[stID].cEntry == 0) ave = 0;
    else ave = agSTATINFO[stID].total / agSTATINFO[stID].cEntry;
        
    max = agSTATINFO[stID].max;
    if(max == MINVALUE) max = 0;

    total = agSTATINFO[stID].total;

    return(agSTATINFO[stID].fCnt);
}

extern "C"
{
    void RecordThreadCnt(uint32_t cCurThreadYield)
    {
        loopStats.RecordCount(LOOPSTATS::THREADCNT, cCurThreadYield);
    }
}