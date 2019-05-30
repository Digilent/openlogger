/************************************************************************/
/*                                                                      */
/*    LexJSON.h                                                         */
/*                                                                      */
/*    Header for the JSON parser                                        */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/* 
*
* Copyright (c) 2013-2016, Digilent <www.digilentinc.com>
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
/*    7/11/2016(KeithV): Created                                        */
/************************************************************************/

#ifndef JSONLex_h
#define JSONLex_h

#ifdef __cplusplus

// forward reference of the typedef
typedef struct _JSONENTRY   JSONENTRY;
typedef struct _JSONLIST    JSONLIST;

#define JListNone       (*((JSONLIST * const) 0x00000000))

#define JListPop0       (*((JSONLIST * const) 0xFFFFFFFF))
#define JListPop1       (*((JSONLIST * const) 0xFFFFFFFE))
#define JListPop2       (*((JSONLIST * const) 0xFFFFFFFD))
#define JListPop3       (*((JSONLIST * const) 0xFFFFFFFC))
#define JListPop4       (*((JSONLIST * const) 0xFFFFFFFB))
#define JListPop5       (*((JSONLIST * const) 0xFFFFFFFA))
#define JListPop6       (*((JSONLIST * const) 0xFFFFFFF9))
#define JListPopBase    (*((JSONLIST * const) 0xFFFFFFF8))

#define JListPush0      (*((JSONLIST * const) 0xFFFFFFF7))
#define JListPush1      (*((JSONLIST * const) 0xFFFFFFF6))
#define JListPush2      (*((JSONLIST * const) 0xFFFFFFF5))
#define JListPush3      (*((JSONLIST * const) 0xFFFFFFF4))
#define JListPush4      (*((JSONLIST * const) 0xFFFFFFF3))
#define JListPush5      (*((JSONLIST * const) 0xFFFFFFF2))
#define JListPush6      (*((JSONLIST * const) 0xFFFFFFF1))
#define JListPushBase   (*((JSONLIST * const) 0xFFFFFFF0))

#define JListPop        JListPop1
#define JListPush       JListPush1

#define CBJSONRESPBUFF 0x4000
extern char PHY __attribute__((coherent)) pchJSONRespBuff[CBJSONRESPBUFF];

class JSON 
{

public:

//     static const JSONLIST *   JListPop   = (JSONLIST *) 0xFFFFFFFF;

    typedef enum
    {
        tokNone = 0,
        tokNull,       
        tokFalse,         
        tokTrue,
        tokObject,
        tokEndObject,
        tokArray,
        tokEndArray,
        tokNumber,
        tokMemberName,
        tokStringValue,
        tokNameSep,
        tokValueSep,
        tokAny,
        tokJSONSyntaxError,
        tokLexingError,
        tokEndOfJSON
    } JSONTOKEN;

    typedef enum
    {
        jvalINT32,
        jvalUINT32,
        jvalINT64,
        jvalUINT64,
        jvalFUNC,
        jvalPTR
    } JSONVALUE;

    // this is a regular non-method function pointer.
    typedef STATE (* FNJPARSE)(JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry);

private:

    static const uint32_t maxAggregate      = 80;   // depth is 1/2 == 40 
    static const uint32_t maxObjArray       = 127;  //int8_t positive number
    static const uint32_t cAny              = 0xFF;
    static const uint32_t cjList            = 64;

    typedef enum
    {
        parValue,          
        parName,           
        parValueSep,       
        parNameSep,        
        parArray          
    } PARCD;

    STATE           state;
    STATE           tokenLexState;
    FNJPARSE        FNCurrent;
    JSONENTRY const * pjListCurrent;
    PARCD           parState;
    JSONTOKEN       jsonToken;
    int32_t         cbToken;
    int32_t         cZero;
    uint32_t        iAggregate;    // even object, odd array 
    int8_t          aggregate[maxAggregate]; // even object cnt, odd array cnt
    uint8_t         fNegative;
    uint8_t         fFractional;
    uint8_t         fExponent;
    uint8_t         fNegativeExponent;

    int32_t         iInput;
    int32_t         cbTokenBuff;
    int32_t         iTokenBuff;
    char            szTokenBuff[0x100];         // for tokens

    char const *    SkipWhite(char const * sz, uint32_t cbsz);

    uint32_t        ijList;
    JSONLIST const * rgpjList[cjList];

protected:
    bool            IsWhite(char const ch);
    STATE           tokenErrorState;
    char const *    szMoveInput;

public:

    JSON(const JSONLIST& jList) 
    {
        rgpjList[0] = &jList;
        szMoveInput = NULL;
        cbToken     = 0;
        Init();
    }

    void Init(void)
    {
        state               = Idle;
        tokenLexState       = Idle;
        tokenErrorState     = Idle;
        FNCurrent           = NULL;
        pjListCurrent       = NULL;
        parState            = parValue;
        jsonToken           = tokNone;
        cbToken             = 0;
        cZero               = 0;
        iAggregate          = 0;
        fNegative           = false;
        fFractional         = false;
        fExponent           = false;
        fNegativeExponent   = false;
        memset(aggregate, 0, sizeof(aggregate));

        iInput = 0;
        cbTokenBuff = 0;
        iTokenBuff = 0;

        // this is so the HTTP code can move past the token
        // even when done parsing the JSON
        szMoveInput         += cbToken;
        cbToken             = 0;

        ijList              = 0;
    }

    GCMD::ACTION LexJSON(char const * szInput, int32_t cbInput, int32_t& cbConsumed);
    STATE ParseToken(char const * szToken, uint32_t cbToken, JSONTOKEN jsonToken); 

};

// I wish I could put this in the JSON class, but the forward typedef does not resolve 
// properly in the class namespace.
// this structures can only be initialized, they are tables in flash and are constant.
typedef struct _JSONLIST
{
    uint32_t const          cEntries;
    JSONENTRY const * const rgEntries;
} JSONLIST;

typedef struct _JSONENTRY
{
    JSON::JSONTOKEN const   token;
    char const * const      szEToken;
    JSON::FNJPARSE const    fnParse;
    const JSONLIST&         nextList;
} JSONENTRY;


class JSONIO 
{

    public:

        STATE  state;
        STATE  stateWhenComplete;

        JSONIO() : state(Idle), stateWhenComplete(Idle) {}

        bool EnableIO(bool fEnable) 
        { 
            STATE stateOld = state;
            
            stateWhenComplete = fEnable ? JSONListening : Idle; 
            
            if(stateOld == JSONListening || stateOld == Idle) state = stateWhenComplete;
            
            return(stateOld == JSONListening);
        }

        virtual uint32_t Available(void) = 0;
        virtual STATE Read(void * const pBuff, uint32_t cbBuff, int32_t * pcbRead) = 0;
        virtual int32_t Read(void) = 0;
        virtual STATE Write(void const * const pBuff, uint32_t cbWrite, int32_t * pcbWritten) = 0;
        virtual void Purge(void) = 0;
        virtual void Done(void) = 0;
};

/************************************************************************/
/************************************************************************/
/********************** OSPAR CLASS  ************************************/
/************************************************************************/
/************************************************************************/

class OSPAR : public JSON
{
public:

    typedef GCMD::ACTION (OSPAR::* FNWRITEDATA)(char const pchWrite[], int32_t cbWrite, int32_t& cWritten);
    typedef GCMD::ACTION (OSPAR::* FNREADDATA)(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead);

    typedef struct _STRU32
    {
        char const * const  szToken;
        const uint32_t      u32;
    } STRU32;

    typedef struct _ODATA
    {
        INSTR_ID    id;
        STATE *     pLockState;
        FNREADDATA  ReadData;
        uint32_t    iOut;
        uint32_t    cb;
        uint8_t  *  pbOut;
    } ODATA;

    typedef struct _IDATA
    {
        STATE *     pLockState;
        FNWRITEDATA WriteData;       
        int32_t     cb;
        uint32_t    iBinary;
    } IDATA;

    typedef enum
    {
        NONE,
        JSON,
        OSJB
    } OSCMD;

    typedef enum
    {
        ICDNone,
        ICDStart,
        ICDEnd
    } ICD;

private:
    STATE           stateOSJB;
    STATE           stateOSJBNextWhite;
    STATE           stateOSJBNextNewLine;
    STATE           stateOSJBNextChunk;

    STATE           state;
    STATE           stateNameSep;
    STATE           stateArray;
    STATE           stateValueSep;
    STATE           stateEndArray;
    STATE           stateEndObject;
    STATE           stateOutLock;
    
    STRU32 const *  rgStrU32;
    uint32_t        cStrU32;

    bool            fError;
    bool            fDoneReadingJSON;
    bool            fWrite;

    uint32_t        tStartCmd;

    int32_t         cbStreamInception;
    int32_t         iStream;
    int32_t         cbConsumed;

    int32_t         iBinary;
    int32_t         iBinaryDone;
    FNWRITEDATA     WriteData; 

    int32_t         cbChunk;
    int32_t         iChunkStart;
    int32_t         iChunk;

    int32_t         iOSJBCount;
    char            szOSJBCount[128];
 
    char            szInput[1024];
    uint32_t        cbInput;


                                                // JSON callback routine; Must be supplied
//    STATE ParseToken(char const * szToken, uint32_t cbToken, JSONTOKEN jsonToken); 

    uint32_t Uint32FromStr(STRU32 const * const rgStrU32L, uint32_t cStrU32L, char const * const sz, uint32_t cb, STATE defaultState=OSPARSyntaxError);
    GCMD::ACTION ReadJSONResp(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead);
 
public:
    uint32_t        tLastCmd;
    bool            fLocked;
    int32_t         iOData;
    int32_t         cOData;
    int32_t         cIData;
    ODATA           odata[4];
    IDATA           idata[4];

    uint8_t const * pbOutput;
    int32_t         cbOutput;

    OSPAR(const JSONLIST& jList) :  JSON::JSON(jList), tStartCmd(0), tLastCmd(0)
    {
        Init(ICDNone);
    }

    void Init(ICD icd)
    {
        JSON::Init();    
        stateOSJB           = Idle;
        stateOSJBNextWhite  = Idle;
        stateOSJBNextChunk  = Idle;
        stateOSJBNextNewLine = Idle;

        state               = Idle;
        stateNameSep        = OSPARSyntaxError;
        stateArray          = OSPARSyntaxError;
        stateValueSep       = OSPARSyntaxError;
        stateEndArray       = OSPARSyntaxError;
        stateEndObject      = OSPARSyntaxError;
        
        cbInput             = 0;
                
        rgStrU32            = NULL;
        cStrU32             = 0;

        fError              = false;
        fWrite              = false;

        cbStreamInception   = 0;
        iStream             = 0;
        cbConsumed          = 0;

        iBinary             = 0;
        iBinaryDone         = 0;
        WriteData           = NULL;

        cbChunk             = 0;
        iChunkStart         = 0;
        iChunk              = 0;
        iOSJBCount          = 0;

        fLocked             = false;
        iOData              = 0;
        cOData              = 1;
        cIData              = 0;

        pbOutput            = NULL;
        cbOutput            = 0;

        memset(idata, 0, sizeof(idata));
        memset(odata, 0, sizeof(odata));

        stateOutLock        = LOCKAvailable;
        odata[0].id         = NULL_ID;
        odata[0].pbOut      = (uint8_t *) KVA_2_KSEG1(pchJSONRespBuff);
        odata[0].cb         = 0;
        odata[0].pLockState = &stateOutLock;
        odata[0].ReadData   = &OSPAR::ReadJSONResp;

        switch(icd)
        {
        case ICDStart:
             tStartCmd = ReadCoreTimer();
            break;

        case ICDEnd:
            tLastCmd = ReadCoreTimer() - tStartCmd;
            loopStats.UpdateEntry(LOOPSTATS::JSONCMD, tLastCmd);
            break;

        default:
        case ICDNone:
            break;
        }
    }

    OSCMD IsOSCmdStart(char ch)
    {
        if(ch == '{') return(OSPAR::JSON);
        else if(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F')) return(OSPAR::OSJB);

        return(OSPAR::NONE);
    }

    GCMD::ACTION StreamOS(char const * szStream, int32_t cbStream);
//    void ReadSerial(DMASerial& JSerial);
    void ProcessJSON(JSONIO& jsonIO);
    GCMD::ACTION WriteFile(char const pchWrite[], int32_t cbWrite, int32_t& cbWritten);
    
    GCMD::ACTION ReadFile(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead);
    GCMD::ACTION ReadVoltageLog(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead);

};

#endif

#endif