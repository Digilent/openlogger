/************************************************************************/
/*                                                                      */
/*    HTMLReboot.cpp                                                    */
/*                                                                      */
/*    Renders the HTML reboot page                                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2013, Digilent Inc.                                     */
/************************************************************************/
/************************************************************************/
/*  Revision History:                                                   */
/*    7/24/2013(KeithV): Created                                         */
/************************************************************************/
#include <OpenLogger.h>

/************************************************************************/
/*    State machine states                                              */
/************************************************************************/
typedef enum {
    CONTLEN,
    ENDHDR,
    JSONLEX,
    WRITEHTTP,
    WRITEJSON,
    JMPFILENOTFOUND,
    DONE
} HSTATE;

typedef enum {
    LEXNONE,
    LEXAVAILABLE,
    LEXLISTEN,
    LEXWAIT,
} LEXSTATE;

/************************************************************************/
/*    HTML Strings                                                      */
/************************************************************************/
static const char szContentLength[] = "Content-Length: ";

/************************************************************************/
/*    Static local variables                                            */
/************************************************************************/
static CLIENTINFO *     pClientMutex        = NULL;
static uint32_t         cbContentLength     = 0;
static bool             fFirstWrite         = true;

static uint32_t         cbAvailable         = 0;
static uint32_t         cbWriting          = 0;
static LEXSTATE         lexState            = LEXNONE;
static LEXSTATE         nextState           = LEXNONE;
static GCMD::ACTION     nextCMD             = GCMD::CONTINUE;

JSONPost jsonPost = JSONPost();

/***    GCMD::ACTION ComposeHTMLPostCmd(CLIENTINFO * pClientInfo)
 *
 *    Parameters:
 *          pClientInfo - the client info representing this connection and web page
 *              
 *    Return Values:
 *          GCMD::ACTION    - GCMD::CONTINUE, just return with no outside action
 *                          - GCMD::READ, non-blocking read of input data into the rgbIn buffer appended to the end of cbRead
 *                          - GCMD::GETLINE, blocking read until a line of input is read or until the rgbIn buffer is full, always the line starts at the beginnig of the rgbIn
 *                          - GCMD::WRITE, loop writing until all cbWrite bytes are written from the pbOut buffer
 *                          - GCMD::DONE, we are done processing and the connection can be closed
 *
 *    Description: 
 *    
 *      Renders the server restart HTML page 
 *    
 * ------------------------------------------------------------ */
GCMD::ACTION ComposeHTMLPostCmd(CLIENTINFO * pClientInfo)
{
   GCMD::ACTION retCMD = GCMD::CONTINUE;

    switch(pClientInfo->htmlState)
    {
         case HTTPSTART:
                         
            // absolutely critical that we don't process 2 JSON commands concurrently
            // so much interaction between the states and the datastructures that new
            // information can not be added inbetween the states.
            if(pClientMutex != NULL)
            {
                break;
            }
            pClientMutex = pClientInfo;
            
            // make sure available does not set cbRead to 0
            nextState    = LEXNONE;

            Serial.println("JSON Post Detected");
            pClientInfo->htmlState = CONTLEN;

            // read off the URL until the body
            retCMD = GCMD::GETLINE;
            break;

        case CONTLEN:

            // assumeing we will be getting the next line
            retCMD = GCMD::GETLINE;

            // if we hit the end of the header then there was no content length
            // and we don't know how to handle that, so exit with an error
            // File not found is probably the wrong error, but it does get out out
            // Fortunately all major browsers put in the content lenght, so this
            // will almost never fail.
            if(strlen((char *) pClientInfo->rgbIn) == 0)    // cbRead may be longer than just the line, so do a strlen()
            {
                pClientInfo->htmlState = JMPFILENOTFOUND;
                retCMD = GCMD::CONTINUE;
            }

            // found the content lengths
            else if(strncasecmp(szContentLength, (char *) pClientInfo->rgbIn, sizeof(szContentLength)-1) == 0)
            {
                cbContentLength = atoi((char *) &pClientInfo->rgbIn[sizeof(szContentLength)-1]);
                pClientInfo->htmlState = ENDHDR;
            }
            break;

        case ENDHDR:

            // the header is ended with a double \r\n\r\n, so I will get
            // a zero length line. Just keep reading lines until we get to the blank line
            if(strlen((char *) pClientInfo->rgbIn) == 0)    // cbRead may be longer than just the line, so do a strlen()
            {
                uint32_t i = 0;

                // go to beyond the \0
                for(i = 0; i < pClientInfo->cbRead && pClientInfo->rgbIn[i] == '\0'; i++);

                // move the buffer to the front
                pClientInfo->cbRead -= i;
                if(pClientInfo->cbRead > 0)
                {
                    memcpy(pClientInfo->rgbIn, &pClientInfo->rgbIn[i], pClientInfo->cbRead);
                }

                // if there is nothing left in the buffer, read some in
                else retCMD = GCMD::READ;

                // no content, just exit
                if(cbContentLength == 0)
                {
                    pClientInfo->htmlState = HTTPDISCONNECT;
                }

                else
                {
                    cbAvailable         = 0;
                    lexState            = LEXAVAILABLE;
                    fFirstWrite = true;
                    pClientInfo->cbWrite = 0;
                    pClientInfo->htmlState = JSONLEX;
                }
            }
            else
            {
                retCMD = GCMD::GETLINE;
            }
            break;

        case JSONLEX:

            switch(lexState)
            {
                case LEXAVAILABLE:

                    cbAvailable += pClientInfo->cbRead;

                    // fall thru

                case LEXLISTEN:
                    nextState   = LEXWAIT;
                    nextCMD     = GCMD::CONTINUE;

                    // fall thru

                case LEXWAIT:
                    retCMD      = nextCMD;
                    lexState    = nextState;
                    break;

                default:
                    ASSERT(NEVER_SHOULD_GET_HERE);
                    break;
            }

            break;

        case WRITEHTTP:

            fFirstWrite = false;

            // if there is binary to write out.
            if(oslex.cOData > 1)
            {
                uint32_t cbT = 0;
                uint32_t cb = 0;
                int32_t i = 0;

                // we need to caluculate the content length
                // it will be the sum of the output plus
                // the chunk sizes (in hex), plus \r\n

                // sum up all of the blocks
                for(i=0; i<oslex.cOData; i++) cbT += oslex.odata[i].cb;

                // The first chunk is the JSON, the second chunk is the rest of the binary
                // do the binary first, because cbT has the sum of all parts
                cb = cbT -  oslex.odata[0].cb;

                // now see how many digits it has, base 16
                for(i=0; cb>0; i++,cb>>=4);
                if(i==0) i = 1; // this should never happen
                cbT += i;

                // now do for the first chunk, the JSON chunk
                cb = oslex.odata[0].cb;
                for(i=0; cb>0; i++,cb>>=4);
                if(i==0) i = 1; // this should never happen
                cbT += i;

                // now we have to add all of the \r\n
                // each chunk is terminated with  a \r\n (there are 2 of them)
                // each count is terminate with a \r\n (there are 2 of them)
                // there is a zero lenght chunk at the end (1 zero char, and 2 \r\n)
                // in effect there are 3 chunks each with 2 \r\n (or 12 bytes) + the zero char (13 bytes)
                cbT += 13;

                // create the header
                pClientInfo->cbWrite  = BuildHTTPOKStr(true, cbT, ".osjb", (char *) pClientInfo->rgbScratch, sizeof(pClientInfo->rgbScratch));

                // push this out on the network the header and JSON count
                pClientInfo->pbOut = pClientInfo->rgbScratch;
            }

            // no binary, just JSON
            else
            {
                // we add a \r\n at the end of the JSON
                // big problem; our content length must match the actual length exactly, if we add extra char on
                // the browser may close the connection before we write all of the data
                uint32_t cb = oslex.odata[0].cb + 2;        
                
                pClientInfo->cbWrite = BuildHTTPOKStr(true, cb, ".json", (char *) pClientInfo->rgbScratch, sizeof(pClientInfo->rgbScratch));
                pClientInfo->pbOut = pClientInfo->rgbScratch;
            }

            pClientInfo->htmlState = WRITEJSON;
            retCMD = GCMD::WRITE;
            break;

        case WRITEJSON:

            nextState   = LEXLISTEN;
            nextCMD     = GCMD::CONTINUE;
            pClientInfo->htmlState = JSONLEX;
            retCMD = GCMD::CONTINUE;

            // let the next call to Write push things out
            break;

        case JMPFILENOTFOUND:

            Serial.println("Jumping to HTTP File Not Found page");
            pClientMutex = NULL;
            oslex.Init(OSPAR::ICDEnd);
            return(JumpToComposeHTMLPage(pClientInfo, ComposeHTTP404Error));
            break;

        case HTTPTIMEOUT:

            Serial.println("Timeout error occurred, closing the session");

            // fall thru to close

        case HTTPDISCONNECT:

            if(pClientMutex == pClientInfo)
            {
                Serial.print("Closing Client ID: 0x");
                Serial.println((uint32_t) pClientMutex, 16);
                pClientMutex = NULL;
//                ASSERT(cbWriting == 0);
            }
            // fall thru Done

        case DONE:
        default:

            cbAvailable             = 0;
            lexState                = LEXNONE;
            nextState               = LEXNONE;
            pClientInfo->cbWrite    = 0;
            cbWriting               = 0;
            retCMD                  = GCMD::DONE;
            break;
    }

    return(retCMD);
}

/************************************************************************/
/************************************************************************/
/************************  JSON Post  ***********************************/
/************************************************************************/
/************************************************************************/
uint32_t JSONPost::Available(void)
{
    if(pClientMutex == NULL) return(0);

    // start a read if we need it
    if(cbAvailable == 0 && nextState == LEXWAIT)
    {
        nextState = LEXAVAILABLE;
        nextCMD = GCMD::READ;
        
        // clear the current buffer
        pClientMutex->cbRead = 0;
    }

    return(cbAvailable);
};

STATE JSONPost::Read(void * const pBuff, uint32_t cbBuff, int32_t * const pcbRead)
{
   if(pClientMutex == NULL) return(MustBeDisconnected);

   *pcbRead = (cbBuff > cbAvailable) ? cbAvailable : cbBuff;

    // we have data, return it
    if(cbAvailable != 0)
    {
        memcpy(pBuff, &pClientMutex->rgbIn[pClientMutex->cbRead - cbAvailable], *pcbRead);
        cbAvailable -= *pcbRead;
    }

    return(Idle);
};

int32_t JSONPost::Read(void)
{
    if(pClientMutex == NULL) return(-1);
    
    if(cbAvailable > 0)
    {
        int32_t bt = pClientMutex->rgbIn[pClientMutex->cbRead - cbAvailable];
        cbAvailable--;
        return(bt);
    }

    return(-1);
};

STATE JSONPost::Write(void const * const pBuff, uint32_t cbWrite, int32_t * const pcbWritten)
{
    STATE retState = Idle;

   if(pClientMutex == NULL) return(MustBeDisconnected);

    if(nextState == LEXWAIT)
    {
        if(cbWriting > 0)
        {
            *pcbWritten         = cbWriting;
            cbWriting           = 0;
            retState            = Idle;
        }

        else if(fFirstWrite)
        {
            nextState               = LEXLISTEN;
            pClientMutex->htmlState = WRITEHTTP;
            retState                = Waiting;
        }

        else
        {
            nextState               = LEXLISTEN;
            nextCMD                 = GCMD::WRITE;
            cbWriting               = cbWrite;
            pClientMutex->cbWrite   = cbWrite;
            pClientMutex->pbOut     = (uint8_t *) pBuff;
            retState                = Working;
        }
    }
    else if(cbWriting > 0)      retState = Working;
    else                        retState = Waiting;

    return(retState);
};

void JSONPost::Purge(void)
{
    if(pClientMutex == NULL) return;

    pClientMutex->htmlState = JMPFILENOTFOUND;
};

void JSONPost::Done(void)
{
    state       = stateWhenComplete;
    if(pClientMutex != NULL)
    {
        pClientMutex->htmlState = HTTPDISCONNECT;
    }
}
