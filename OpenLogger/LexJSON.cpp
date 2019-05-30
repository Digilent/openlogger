/************************************************************************/
/*                                                                      */
/*    LexJSON.cpp                                                       */
/*                                                                      */
/*    JSON Lexer / Parser                                               */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    7/11/2016(KeithV): Created                                        */
/************************************************************************/
#include    <OpenLogger.h>

/************************************************************************/
/********************** Strings used for OSJB ***************************/
/************************************************************************/
static const char szStatusCode[] = "{\"statusCode\":";
static const char szCharLocation[] = ",\"Char Location\":";
static const char szEndError[] = "}";
static const char szTerminateChunk[] = "\r\n0\r\n\r\n";

static const char szInvalidSyntax[] = "{\"statusCode\":2684354567,\"Char Location\":0}\r\n";

char PHY __attribute__((coherent)) pchJSONRespBuff[CBJSONRESPBUFF];    // 16384 bytes for output OSJB

/************************************************************************/
/*    RFC 7159 say JSON-text = ws value ws                              */
/*    however JSON-text = ws object ws                                  */
/*    is a valid subset that we are going to adhear to.                 */
/************************************************************************/

bool JSON::IsWhite(char const ch)
{
    switch(ch)
    {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return(true);
            break;

        default:
            return(false);
            break;
    }

    return(false);
}

char const *  JSON::SkipWhite(char const * sz, uint32_t cbsz)
{
    uint32_t i = 0;

    for(i=0; i<cbsz && IsWhite(sz[i]); i++);
    return(sz + i);
}

GCMD::ACTION  JSON::LexJSON(char const * szInput, int32_t cbInput, int32_t& cbConsumed)
{

    while(true)
    {
        szMoveInput = szTokenBuff;

        if(cbTokenBuff < (int32_t) sizeof(szTokenBuff) && iInput < cbInput)
        {
            uint32_t cbCopy = min((int32_t) sizeof(szTokenBuff) - cbTokenBuff, cbInput - iInput);
            memcpy(&szTokenBuff[cbTokenBuff], &szInput[iInput], cbCopy);
            iInput += cbCopy;
            cbTokenBuff += cbCopy;
        }

        // this should never happen on a state of Idle 
        // as cbToken == 0 at Idle
        if(cbToken > cbTokenBuff) 
        {
            iInput = 0;
            return(GCMD::READ);
        }

        switch(state)
        {
            case Idle:
                parState = parValue;
                state = JSONSkipWhite;
                // fall thru

            case JSONSkipWhite:
                {
                    cbToken = 0;
                    szMoveInput = SkipWhite(szMoveInput, cbTokenBuff - (szMoveInput - szTokenBuff));

                    // Depleated our buffer, so continue to look for white space
                    if(cbTokenBuff == 0)
                    {
                        iInput = 0;
                        return(GCMD::READ);                    
                    }
                    
                    // found the start of a token
                    else if(szMoveInput == szTokenBuff)
                    {
                        state = JSONToken;
                        break;
                    }
                 
                    // otherwise shift and continue
                 }
                break;

            case JSONToken:
                switch(szTokenBuff[0])
                {

                    //VALUES

                    case 'f':
                        if(parState != parValue)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        jsonToken = tokFalse;
                        cbToken = 5;
                        state = JSONfalse;
                        break;

                    case 'n':
                        if(parState != parValue)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        jsonToken = tokNull;
                        cbToken = 4;
                        state = JSONnull;
                        break;

                    case 't':
                        if(parState != parValue)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        jsonToken = tokTrue;
                        cbToken = 4;
                        state = JSONtrue;
                        break;

                    case '{':
                        if(parState != parValue)
                        {
                            state = JSONSyntaxError;
                            break;
                        }

                        // we are in an object, up the object count
                        // nesting more in an object.
                        if((iAggregate % 2) == 0)
                        {
                            if(aggregate[iAggregate] == maxObjArray)
                            {
                                state = JSONNestingError;
                                break;
                            }
                            else
                            {
                                aggregate[iAggregate]++;
                            }
                        }

                        // was in an array, now in an object
                        // make sure we have room in our aggregate array
                        else if(iAggregate+1 >= maxAggregate)
                        {
                            state = JSONNestingError;
                            break;
                        }

                        // was in an array, now in an object
                        else
                        {
                            iAggregate++;
                            aggregate[iAggregate] = 1;
                        }

                        jsonToken = tokObject;
                        cbToken = 1;
                        state = JSONCallOSLex;
                        break;

                    case '[':
                        if(parState != parValue)
                        {
                            state = JSONSyntaxError;
                            break;
                        }

                        // we are in an array, up the array count
                        // nesting more in an array.
                        if((iAggregate % 2) == 1)
                        {
                            if(aggregate[iAggregate] == maxObjArray)
                            {
                                state = JSONNestingError;
                                break;
                            }
                            else
                            {
                                aggregate[iAggregate]++;
                            }
                        }

                        // was in an object, now in an array
                        // don't have to worry about overshooting
                        // as the aggregate array has an even value
                        // and that will always occur on an object bump
                        else
                        {
                            iAggregate++;
                            aggregate[iAggregate] = 1;
                        }

                        jsonToken = tokArray;
                        cbToken = 1;
                        state = JSONCallOSLex;
                        break;

                    case '"':
                        cbToken = 2;        // I can ask for another char as there must be an end "
                        state = JSONString;
                        
                        switch(parState)
                        {
                            case parName:
                                jsonToken = tokMemberName;
                                break;

                            case parValue:
                                jsonToken = tokStringValue;
                                break;

                            default:
                                jsonToken = tokNone;
                                state = JSONSyntaxError;
                                break;
                        }
                        break;

                    case '-':
                        fNegative = true;
                        // fall thru

                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if(parState != parValue)
                        {
                            state = JSONSyntaxError;
                            break;
                        }

                        if(szTokenBuff[0] == '0') cZero = 0;
                        else cZero = cAny;

                        // normal case
                        jsonToken = tokNumber;
                        cbToken = 2;  
                        state = JSONNumber;

                        break;

                    // NAME SEPARATORS
                    case ':':
                        if(parState != parNameSep)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        jsonToken = tokNameSep;
                        cbToken = 1;
                        state = JSONCallOSLex;
                        break;

                    // VALUE SEPARATORS
                    case ',':
                        if(parState != parValueSep)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        jsonToken = tokValueSep;
                        cbToken = 1;
                        state = JSONCallOSLex;
                        break;

                    case ']':
                        if(parState != parValueSep || (iAggregate % 2) != 1 || aggregate[iAggregate] < 1)
                        {
                            state = JSONSyntaxError;
                            break;
                        }

                        // comming out of an array
                        aggregate[iAggregate]--;

                        // if this is the closing array bump down to the object
                        if(aggregate[iAggregate] == 0) iAggregate--;

                        jsonToken = tokEndArray;
                        parState = parValueSep;
                        cbToken = 1;
                        state = JSONCallOSLex;
                        break;

                    case '}':
                        if(parState != parValueSep || (iAggregate % 2) != 0 || aggregate[iAggregate] < 1)
                        {
                            state = JSONSyntaxError;
                            break;
                        }

                        // coming out of an object
                        aggregate[iAggregate]--;

                        // if this is the closing object, bump down to an array
                        // unless this is the bottom and there is nothing below.
                        if(aggregate[iAggregate] == 0 && iAggregate > 0) iAggregate--;

                        jsonToken = tokEndObject;
                        parState = parValueSep;
                        cbToken = 1;
                        state = JSONCallOSLex;
                        break;


                    default:
                        state = JSONSyntaxError;
                        break;
                }
                break;

            case JSONfalse:
                if(memcmp(szTokenBuff, "false", 5) == 0) state = JSONCallOSLex;
                else state = JSONSyntaxError;
                break;

            case JSONnull:
                if(memcmp(szTokenBuff, "null", 4) == 0) state = JSONCallOSLex;
                else state = JSONSyntaxError;
                break;

            case JSONtrue:
                if(memcmp(szTokenBuff, "true", 4) == 0) state = JSONCallOSLex;
                else state = JSONSyntaxError;
                break;

            case JSONString:
                for(; cbToken <= cbTokenBuff; cbToken++)
                {
                    if(szTokenBuff[cbToken-1] == '"' && szTokenBuff[cbToken-2] != '\\')
                    {
                        // the question is, should the underlying code
                        // get escape sequences, or should they somehow be stripped
                        // here. My conclusion is, since I don't know the character set
                        // let the underlying code process the escaped characters.
                        state = JSONCallOSLex;
                        break;
                    }
                }           
                break;

                // what if the number is the last in the file
            case JSONNumber:
                for(; cbToken <= cbTokenBuff; cbToken++)
                {
                    char cch = szTokenBuff[cbToken-1];

                    if(cch == '0')
                    {
                        if(cZero == cAny)
                        {
                            continue;
                        }
                        else if(cZero == 0)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        else
                        {
                            cZero--;
                            continue;
                        }
                        break;
                    }
                    else if(isdigit(cch))
                    {
                        cZero = cAny;
                        continue;
                    }
                    else if(cch == '.')
                    {
                        if(fFractional || fExponent)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        else
                        {
                            cZero = cAny;
                            fFractional = true;
                        }
                    }
                    else if(cch == 'e' || cch == 'E')
                    {
                        if(fExponent)
                        {
                            state = JSONSyntaxError;
                            break;
                        }
                        else
                        {
                            fExponent = true;
                            cZero = 0;
                        }
                    }
                    else if(cch == '+' && fExponent && (szTokenBuff[cbToken-2] == 'e' || szTokenBuff[cbToken-2] == 'E'))
                    {
                            cZero = 0;
                    }
                    else if(cch == '-' && fExponent && (szTokenBuff[cbToken-2] == 'e' || szTokenBuff[cbToken-2] == 'E'))
                    {
                            fNegativeExponent = true;
                            cZero = 0;
                    }
                    else if(IsWhite(cch) || cch == ',' || cch == ']' || cch == '}')
                    {
                        cbToken--;
                        state = JSONCallOSLex;
                        break;
                    }
                    else
                    {
                        state = JSONSyntaxError;
                        break;
                    }
                } 

                // done processing this number, reset the flags for the next number
                if(state != JSONNumber)
                {
                    fNegative           = false;
                    fFractional         = false;
                    fExponent           = false;
                    fNegativeExponent   = false;
                }
                break;

            case JSONCallOSLex:

                // call the OpenScope lexer here
                // LexOpenScope(char const * szJSON, uint32_t cbJSON);
                if((tokenLexState = ParseToken(szTokenBuff, cbToken, jsonToken)) == Idle)
                {
                    state = JSONNextToken;

                }
                else if(IsStateAnError(tokenLexState))
                {
                    state = JSONTokenLexingError;
                }

                // we want to give up the time on each token processing
                // This could take a lot of time.
                return(GCMD::CONTINUE);
                break;
              
           case JSONNextToken:

                // consume the token
                szMoveInput =  szTokenBuff + cbToken;

                if(jsonToken == tokEndOfJSON)
                {
                    state = Done;
                    break;
                }

                if(iAggregate == 0 && aggregate[iAggregate] == 0)
                {
                    jsonToken = tokEndOfJSON;
                    cbToken = 0;
                    state = JSONCallOSLex;
                    break;
                }

                // go to skip more white space
                state = JSONSkipWhite;

                // put in what the next parsing state is
                switch(jsonToken)
                {
                    case tokFalse:
                    case tokNull:
                    case tokTrue:
                    case tokEndObject:
                    case tokEndArray:
                    case tokNumber:
                    case tokStringValue:
                         parState = parValueSep;
                        break;

                    case tokMemberName:
                        parState = parNameSep;
                        break;

                    case tokObject:
                        parState = parName;
                        break;

                    case tokValueSep:
                        // if we are at the object level
                        // value separators separate members
                        // so we would be looking for a member name
                        if((iAggregate % 2) == 0) parState = parName;

                        // otherwise we are in an array looking for a value
                        else parState = parValue;
                        break;

                    case tokNameSep:
                    case tokArray:
                        parState = parValue;
                        break;

                    case tokJSONSyntaxError:
                    default:
                        state = JSONSyntaxError;
                        break;
                }
                break;

            case JSONTokenLexingError:
                tokenErrorState = tokenLexState;
                state = Done;
                break; 

             case JSONSyntaxError:
                tokenErrorState = JSONLexingError;
                state = Done;
                break;

             case JSONNestingError:
                tokenErrorState = JSONObjArrayNestingError;
                state = Done;
                break;

            case Done:
                // all good, we are done.
                if(tokenErrorState == Idle) return(GCMD::DONE);
                else return(GCMD::ERROR);
                break;
        }

        // shift the token buffer if needed
        if(szMoveInput > szTokenBuff)
        {
            int32_t cbMove = (szMoveInput - szTokenBuff);
            cbTokenBuff -= cbMove;
            cbConsumed  += cbMove;
            memcpy(szTokenBuff, szMoveInput, cbTokenBuff);
            cbToken = 0;
        }
    }

    ASSERT(NEVER_SHOULD_GET_HERE);
    return(GCMD::CONTINUE);
}

/*  Token number codes
*   1:  tokNull       8:    tokNumber,
*   2:  tokFalse      9:    tokMemberName,  
*   3:  tokTrue       10/A: tokStringValue,
*   4:  tokObject     11/B: tokNameSep,
*   5:  tokEndObject  12/C: tokValueSep,
*   6:  tokArray      13/C: tokAny,
*   7:  tokEndArray
*/
STATE JSON::ParseToken(char const * szToken, uint32_t cbToken, JSONTOKEN jsonToken)
{

    // fixup / check incoming parameters
    switch(jsonToken)
    {
        // on strings, take off the quotes
    case tokMemberName:
    case tokStringValue:
        szToken++;
        cbToken -= 2;
        break;

    case tokNone:
    case tokJSONSyntaxError:
    case tokLexingError:
    case tokAny:
        ASSERT(NEVER_SHOULD_GET_HERE);
        return(tokJSONSyntaxError);
        break;

    case tokEndOfJSON:
        ASSERT(ijList == 0);
        break;

    default:
        break;

    }

    if(FNCurrent == NULL)
    {
        uint32_t    i = 0;

        // walk our current list looking for the token
        for(i=0; i<rgpjList[ijList]->cEntries; i++)
        {
            if(jsonToken == rgpjList[ijList]->rgEntries[i].token || rgpjList[ijList]->rgEntries[i].token == tokAny)
            {
                char const * const szEToken = rgpjList[ijList]->rgEntries[i].szEToken;
 
                // if we found the token
//                if(rgpjList[ijList]->rgEntries[i].szEToken == NULL || strncmp(szToken, rgpjList[ijList]->rgEntries[i].szEToken, cbToken) == 0)
                if(szEToken == NULL || (strncmp(szToken, szEToken, cbToken) == 0 && szEToken[cbToken] == '\0'))
                {
                    break;
                }
            }
        }

        // if we have an entry, process it
        if(i < rgpjList[ijList]->cEntries)
        {
            FNCurrent       = rgpjList[ijList]->rgEntries[i].fnParse;
            pjListCurrent   = &rgpjList[ijList]->rgEntries[i];

            // pop up the list
            if(((uint32) &(rgpjList[ijList]->rgEntries[i].nextList)) > ((uint32) &JListPopBase))
            {
                uint32_t dPop = ((uint32) &JListPop0) - ((uint32) &rgpjList[ijList]->rgEntries[i].nextList);

                ASSERT(ijList >= dPop);
                ijList -= dPop;
            }

            // push down the list copying my jList
            else if(((uint32) &(rgpjList[ijList]->rgEntries[i].nextList)) > ((uint32) &JListPushBase))
            {
                uint32_t dPush = ((uint32) &JListPush0) - ((uint32) &rgpjList[ijList]->rgEntries[i].nextList);

                ASSERT((ijList + dPush) < cjList);

                for(i=0; i<dPush; i++, ijList++) rgpjList[ijList+1] = rgpjList[ijList];
            }

            // go to a new list
            else if(&rgpjList[ijList]->rgEntries[i].nextList != NULL)
            {
                ijList++;
                ASSERT(ijList < cjList);
                rgpjList[ijList] = &pjListCurrent->nextList;
            }
        }

        else
        {
            return(InvalidSyntax);
        }
    }

    // if we have a function to call, call it
    if(FNCurrent != NULL)
    {
        STATE retState = FNCurrent(jsonToken, szToken, cbToken, *pjListCurrent);

        // if we are done with the function, clear the pointer
        if(retState == Idle || IsStateAnError(retState)) FNCurrent = NULL;

        return(retState);
    }
 
    return(Idle);
}

/************************************************************************/
/************************************************************************/
/********************** OSPAR CLASS  ************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/*    Parser helper methods                                             */
/************************************************************************/

uint32_t OSPAR::Uint32FromStr(STRU32 const * const rgStrU32L, uint32_t cStrU32L, char const * const sz, uint32_t cb, STATE defaultState)
{
    uint32_t i = 0;

    for(i = 0; i < cStrU32L; i++)
    {
        if((strlen(rgStrU32L[i].szToken) == cb) && memcmp(sz, rgStrU32L[i].szToken, cb) == 0)
        {
            return(rgStrU32L[i].u32);
        }
    }

    return(defaultState);
}

GCMD::ACTION OSPAR::ReadJSONResp(int32_t iOData, uint8_t const *& pbRead, int32_t& cbRead)
{
    ODATA& oData = odata[iOData];

    if(*oData.pLockState != LOCKOutput)
    {
        return(GCMD::ERROR);
    }
    else if(oData.iOut == (uint32_t) oData.cb)
    {
        pbRead = NULL;
        cbRead = 0;
        oData.iOut = 0;
        *oData.pLockState = LOCKAvailable;
        return(GCMD::DONE);
    }
    else if(oData.iOut == 0)
    {
        oData.iOut = oData.cb;
        pbRead = oData.pbOut;
        cbRead = oData.cb;
        return(GCMD::WRITE);
    }

    pbRead = NULL;
    cbRead = 0;
    oData.iOut = 0;
    *oData.pLockState = LOCKAvailable;
    return(GCMD::ERROR);
}



/************************************************************************/
/*    RFC 7159 say JSON-text = ws value ws                              */
/*    however JSON-text = ws object ws                                  */
/*    is a valid subset that we are going to adhear to.                 */
/*    If doing chunks, we will parse ws to the end of the chunk         */
/************************************************************************/

GCMD::ACTION OSPAR::StreamOS(char const * szStream, int32_t cbStream)
{
    bool fYield     = false;
    bool fNewError  = false;
    bool fContinue  = false;

    do
    {
        fContinue   = false;
        fYield      = false;

        switch(stateOSJB)
        {
        case Idle:
            //            Init(OSPAR::ICDNone);

            // lock us for JSON input parsing
            *odata[0].pLockState = LOCKInput;

            stateOSJB           = OSJBSkipWhite;
            stateOSJBNextWhite  = OSJBReadJSON;
            // fall thru

        case OSJBSkipWhite:
            if(IsWhite(szStream[iStream])) iStream++;
            else stateOSJB = stateOSJBNextWhite;   
            fContinue = true;
            break;

        case OSJBSkipWhiteToEndOfChunk:

            if(cbChunk > 0 && cbChunk-iChunk > 0)
            {
                if(IsWhite(szStream[iStream])) 
                {
                    iStream++;

                    // The stream will advance iChunk, see if we are done
                    // we have to do this here because chunking will just silently advance to the next chunk
                    if(cbChunk-iChunk == 1) stateOSJB = stateOSJBNextWhite;  
                }
                else fNewError = true; 

            }

            else ASSERT(NEVER_SHOULD_GET_HERE); 

            fContinue = true;
            break;

        case OSJBReadJSON:
            // if this is JSON, go parse it
            if(IsOSCmdStart(szStream[iStream]) == OSPAR::JSON) stateOSJB = OSJBParseJSON;

            // if this is OSJB, prepare to read binary
            else if(IsOSCmdStart(szStream[iStream]) == OSPAR::OSJB)
            {
                stateOSJBNextChunk      = OSJBParseJSON;
                stateOSJB               = OSJBReadCount;
            }

            // some thing unexpected is happening
            else fNewError = true;

            // we did very little here, so lets just do the next step immediately
            fContinue = true;
            break;

            // if we get here, we are expecting OSJB
        case OSJBReadNextChunk:
            if(szStream[iStream] == '\r')
            {
                cbChunk         = 0;
                iChunk          = -1;   // so we don't loop trying to read the next chunk
                iChunkStart     = -1;
                stateOSJB       = OSJBNewLine;
            }
            else
            {
                fNewError = true;
            }
            break;

        case OSJBReadCount:
            if(IsOSCmdStart(szStream[iStream]) == OSPAR::OSJB)
            {
                szOSJBCount[iOSJBCount++]   = szStream[iStream++];
                fContinue = true;
            }
            else if(szStream[iStream] == '\r')
            {
                iStream++;
                szOSJBCount[iOSJBCount]     = '\0';
                cbChunk = strtol(szOSJBCount, NULL, 16);
                iChunk          = 0;
                iChunkStart     = iStream + 1;      // add the new line

                iOSJBCount   = 0;
                stateOSJB = OSJBNewLine;
                if(cbChunk == 0) stateOSJBNextNewLine = OSJBCarrageReturn;
                else stateOSJBNextNewLine = stateOSJBNextChunk;
            }
            else 
            {
                fNewError = true;
            }
            break;

        case OSJBCarrageReturn:
            if(szStream[iStream] == '\r')
            {
                iStream++;
                stateOSJB = OSJBNewLine;
                if(cbChunk == 0) stateOSJBNextNewLine =  OSJBOutputJSON;
                fContinue = true;
            }
            else 
            {
                fNewError = true;
            }
            break;

        case OSJBNewLine:
            if(szStream[iStream] == '\n')
            {
                iStream++;
                stateOSJB = stateOSJBNextNewLine;
                fContinue = true;
            }
            else 
            {
                fNewError = true;
            }
            break;

        case OSJBParseJSON:
        {
            int32_t cbDataStream = cbStream - iStream;
            
            if(cbChunk > 0) cbDataStream = min(cbDataStream, cbChunk-iChunk);

            // call the lexer
            switch(LexJSON(&szStream[iStream], cbDataStream, cbConsumed))
            {
            case GCMD::CONTINUE:
                fYield = true; 
                break;

            // this is only called when all of the bytes in cbDataStream are consumed
            case GCMD::READ:                                                           
                cbConsumed -= cbDataStream; // this can go neg if the token is not fully parsed
                iStream    += cbDataStream;
                break;

            case GCMD::ERROR:
                fNewError = true;
                break;

            case GCMD::DONE:
                iStream += cbConsumed;
                cbConsumed = 0;
                if(cbChunk > 0) 
                {
                    // if we are already at the end of the chunk
                    if((iStream - iChunkStart) == cbChunk)
                    {
                        stateOSJB = OSJBReadNextBinary;
                    }

                    // otherwise skip white spaces until the end of the chunk
                    else
                    {
                        stateOSJB           = OSJBSkipWhiteToEndOfChunk;
                        stateOSJBNextWhite  = OSJBReadNextBinary;
                    }
                }

                // if we are not in chunks, we are done.
                else 
                {
                    stateOSJB       = OSJBOutputJSON;
                }
                break;

            default:
                break;

            }
        }
        break;

        case OSJBReadNextBinary:
        {
            int32_t     i;
            int32_t     deltaBinary = 0x7FFFFFFF;

            // look for the binary section for where we are at.
            iBinaryDone = -1;
            WriteData   = NULL;
            for(i=0; i<cIData; i++)
            {
                int32_t deltaBinaryCur = idata[i].iBinary - iBinary;
                if(deltaBinaryCur >= 0 && deltaBinaryCur < deltaBinary) 
                {
                    deltaBinary = deltaBinaryCur;
                    if(deltaBinary == 0)
                    {
                        WriteData   = idata[i].WriteData;
                        iBinaryDone = idata[i].iBinary + idata[i].cb;
                        break;
                    }
                    else iBinaryDone = iBinary + deltaBinary;
                }
            }

            if(iBinaryDone == -1) fNewError = true;
            else stateOSJB = OSJBReadBinary; 
        }
        break;

        case OSJBReadBinary:
        {
            int32_t cbWritten = 0;
            int32_t cbWrite = min(min(cbStream-iStream, cbChunk-iChunk), iBinaryDone-iBinary);

            // not a good thing to have happen
            if(cbWrite < 0)
            {
                cbWrite = 0;
                fNewError = true;
            }

            if(WriteData != NULL)
            {

                switch((this->*WriteData)(&szStream[iStream], cbWrite, cbWritten))
                {
                case GCMD::DONE:
                    ASSERT(cbWrite == 0 && cbWritten == 0);
                    if(iBinary != iBinaryDone) fNewError = true;
                    stateOSJB = OSJBReadNextBinary;          
                    break;

                case GCMD::CONTINUE:
                    fYield = true; 
                    // fall thru

                    // need more data
                case GCMD::READ:

                    // update pointers
                    iStream += cbWritten;
                    iBinary += cbWritten;

                    // iBinary could == iBinaryDone
                    // also we could be done with the chunk and that will
                    // automatically cause a chunk read, and to look at the next chunk
                    // we don't want to go to the next chunk if this is the end of the binary
                    // we want to come through again and send a zero to the Write
                    // so yield if this is the end of the binary
                    fYield |= (iBinary >= iBinaryDone);

                    // in general we don't have to skip the potential READ if 
                    // iStream == cbStream because we know there is a cbChunk of 0 and 2 /cr an /n comming.
                    // basically, we know the input stream is not done.

                    break;

                case GCMD::ERROR:
                default:
                    // try to close the file, zero count will do that.
                    (this->*WriteData)(&szStream[iStream], 0, cbWritten);
                    fNewError = true;
                    break;
                }

            }


            // skipping binary
            else
            {
                // update pointers
                iStream += cbWrite;
                iBinary += cbWrite;

                // when we have skipped it, go to the next Binary
                if(iBinary == iBinaryDone) stateOSJB = OSJBReadNextBinary;                     
            }
        }
        break;

        case OSJBOutputJSON:

            // clean up reading.
            cbStreamInception += iStream;
            iStream = 0;
            fWrite = true;

            // if we are doing chunks
            iOData = 0;
            *odata[0].pLockState = LOCKOutput;
            iBinary = 0;

            if(cOData > 1) stateOSJB = OSJBWriteChunkSize;
            else stateOSJB = OSJBWriteOData;

            fYield = true;
            break;

        case OSJBWriteChunkSize:

            // assume we have data to write out.
            stateOSJB = OSJBWriteOData;

            iChunk = 0;     // where are we in the psudo chunk (OData).

                            // only writing JSON, no chunks, and we are done
            if(cOData == 1)
            {
                // put a \r\n after the JSON
                cbOutput = 0;
                szOSJBCount[cbOutput++] = '\r';
                szOSJBCount[cbOutput++] = '\n';
                pbOutput = (uint8_t *) szOSJBCount;

                // uncomment and the \r\n will not go out
                //                    fYield = true;      // skip reading and writing
                //                    fContinue = true;   // this was quick, so just go there

                stateOSJB = Done;   // we are done, there are no chunks
            }

            // if this is the first chunk, it is the JSON chunk
            else if(iOData == 0)
            {
                // create the length of the JSON part of the message
                itoa(odata[0].cb, szOSJBCount, 16);
                cbOutput = strlen(szOSJBCount);
                szOSJBCount[cbOutput++] = '\r';
                szOSJBCount[cbOutput++] = '\n';
                pbOutput = (uint8_t *) szOSJBCount;
            }

            // the binary chunk
            else if(iOData == 1)
            {
                int32_t i;
                int32_t cb = 0;

                // we just merge all chunks into one.
                for(i=1; i<cOData; i++) cb += odata[i].cb;

                // put a \r\n after the JSON
                cbOutput = 0;
                szOSJBCount[cbOutput++] = '\r';
                szOSJBCount[cbOutput++] = '\n';

                // put out the binary length
                itoa(cb, &szOSJBCount[cbOutput], 16);
                cbOutput += strlen(&szOSJBCount[cbOutput]);
                szOSJBCount[cbOutput++] = '\r';
                szOSJBCount[cbOutput++] = '\n';
                pbOutput = (uint8_t *) szOSJBCount;
            }

            // the last chunk
            else if(cOData == iOData)
            {
                // put our the chunk terminator
                pbOutput = (uint8_t *) szTerminateChunk;
                cbOutput = sizeof(szTerminateChunk) - 1;
                stateOSJB = Done;
            }

            // all the intermediate chunks are merged into the 2nd chunk, so just skip
            else
            {
                fYield = true;      // skip reading and writing
                fContinue = true;   // this was quick, so just go there
            }
            break;

        case OSJBWriteOData:

            if(odata[iOData].ReadData != NULL)
            {
                switch((this->*odata[iOData].ReadData)(iOData, pbOutput, cbOutput))
                {
                case GCMD::WRITE:
                    iBinary += cbOutput;
                    iChunk  += cbOutput;
                    break;

                case GCMD::CONTINUE:
                    fYield = true;      // skip reading and writing
                    break;

                 case GCMD::ERROR:
                    ASSERT(NEVER_SHOULD_GET_HERE); 
                    
               case GCMD::DONE:
                    stateOSJB = OSJBWriteChunkSize;
                    iOData++;
                    fYield = true;      // skip reading and writing
                    break;
                    
                default:
                    ASSERT(NEVER_SHOULD_GET_HERE); 
                    break;
                }
            }
            else ASSERT(NEVER_SHOULD_GET_HERE); 
            break;

        case Done:
            stateOSJB = Idle;
            return(GCMD::DONE);

        }

        if(fNewError)
        {
            stateOSJB   = Idle;
            fError      = true;

            // Error Code
            strcpy(pchJSONRespBuff, szStatusCode);
            odata[0].cb = sizeof(szStatusCode)-1;
            utoa(InvalidSyntax, &pchJSONRespBuff[odata[0].cb], 10);
            odata[0].cb = strlen(pchJSONRespBuff);

            // location
            memcpy(&pchJSONRespBuff[odata[0].cb], szCharLocation, sizeof(szCharLocation)-1);
            odata[0].cb += sizeof(szCharLocation)-1;
            itoa(cbStreamInception+iStream+cbConsumed, &pchJSONRespBuff[odata[0].cb], 10);
            odata[0].cb += strlen(&pchJSONRespBuff[odata[0].cb]);

            // end the error code
            memcpy(&pchJSONRespBuff[odata[0].cb], szEndError, sizeof(szEndError)-1);
            odata[0].cb += sizeof(szEndError)-1;

            // write out the error
            stateOSJB = OSJBOutputJSON;
            cOData = 1;
        }

        // update the chunk count
        if(!fYield)
        {
            if(fWrite)
            {
                return(GCMD::WRITE);
            }

            else
            {
                if(cbChunk > 0)
                {
                    iChunk = iStream - iChunkStart;

                    if(iChunk == cbChunk)
                    {
                        iChunk                  = -2;
                        cbChunk                 = -1;   // so /cr does not take us to done
                        stateOSJBNextChunk      = stateOSJB;
                        stateOSJBNextNewLine    = OSJBReadCount;
                        stateOSJB               = OSJBCarrageReturn;                
                    }
                }

                // see if we need to read more data
                if(iStream == cbStream) 
                {
                    // adjust for moving iStream
                    if(cbChunk > 0) iChunkStart -= iStream;

                    cbStreamInception += iStream;
                    iStream = 0;

                    if(stateOSJB != OSJBOutputJSON)
                    {
                        // need more data
                        return(GCMD::READ);
                    }
                }
            }
        }

    } while(fContinue);

    return(GCMD::CONTINUE);
}

void OSPAR::ProcessJSON(JSONIO& jsonIO)
{
    int32_t cbWritten;
    STATE retState;

    switch(jsonIO.state)
    {
        /************************************************************************/
        /************************************************************************/
        /********************** JSON PARSING  ***********************************/
        /************************************************************************/
        /************************************************************************/

    // Don't read until we are ready! 
    case Idle:
        jsonIO.Done();
        break;

    case JSONListening:
        if(!fLocked && jsonIO.Available() > 0) 
        {
            char chInput = jsonIO.Read();

            // if white space just skip it.
            if(IsWhite(chInput) || IsOSCmdStart(chInput) == OSPAR::NONE) 
            {
                break;
            }

            // we have something real to parse
            Init(OSPAR::ICDStart);
            fLocked = true;
            szInput[0] = chInput;
            cbInput = 1;

#ifdef WAITINPUT500       // debug to collect more char before proceeding
            {
                uint32_t tStart = ReadCoreTimer();
                while(ReadCoreTimer() - tStart < 500 * CORE_TMR_TICKS_PER_MSEC);
            }
#endif

            // read the rest of the data
//            while(jsonIO.Available() > 0 && cbInput < sizeof(szInput)) szInput[cbInput++] = jsonIO.Read();

            // go process it
            jsonIO.state = GCMD::CONTINUE | GCMDSTATE;
        }
        break;

    case (GCMD::CONTINUE | GCMDSTATE):
        jsonIO.state = StreamOS(szInput, cbInput) | GCMDSTATE;
        break;

    case (GCMD::READ | GCMDSTATE):
        {
            cbInput = 0;
            if(jsonIO.Available() > 0)
            {
                int32_t cbRead;

                if((retState = jsonIO.Read(szInput, sizeof(szInput), &cbRead)) == Idle)
                {
                    ASSERT(cbRead <= (int32_t) sizeof(szInput));

                    // we are done
                    if(cbRead > 0)
                    {
                        cbInput = cbRead;
                        jsonIO.state = GCMD::CONTINUE | GCMDSTATE;
                    }

                }
                else if(IsStateAnError(retState))
                {
                    jsonIO.state = GCMD::ERROR | GCMDSTATE;
                }
            }
        }
        break;

    case (GCMD::WRITE | GCMDSTATE):

        // we can't DMA more than 65536-2 (the -2 is to prevent freakout mode in the DMA)
        ASSERT(cbOutput < 65534);

        if((retState = jsonIO.Write(pbOutput, cbOutput, &cbWritten)) == Idle)
        {
            ASSERT(cbWritten <= cbOutput);

            // we are done
            if(cbWritten == cbOutput)
            {
                jsonIO.state = GCMD::CONTINUE | GCMDSTATE;
            }

            // keep writing
            else
            {
                pbOutput = &pbOutput[cbWritten];
                cbOutput -= cbOutput;
            }
        }
        else if(IsStateAnError(retState))
        {
            jsonIO.state = GCMD::ERROR | GCMDSTATE;
        }
        break;

    case (GCMD::ERROR | GCMDSTATE):
        // clear the input stream, we lost sync
        // so wait for a new command.
        jsonIO.Purge();

        // fall thru

    case (GCMD::DONE | GCMDSTATE):

        // Unlock all of the buffers
        for(int32_t i=0; i<cOData; i++)
        {
            if(odata[i].pLockState != NULL && *odata[i].pLockState == LOCKOutput)
            {
                *odata[i].pLockState = LOCKAvailable;
            }
        }

        // we are no longer parsing JSON
        jsonIO.Done();
        Init(OSPAR::ICDEnd);
        break;

    default:
        ASSERT(NEVER_SHOULD_GET_HERE); 
        break;
    }
}

