/************************************************************************/
/*                                                                      */
/*    JsonIO.cpp                                                        */
/*                                                                      */
/*    The JSON IO objects for JSON processing                           */
/*    Includes:                                                         */
/*      Serial                                                          */
/*      FIFO245                                                         */
/*      HTTP Post                                                       */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent                                          */
/************************************************************************/
/************************************************************************/
/*  Revision History:                                                   */
/*    8/6/2018(KeithV): Created                                         */
/************************************************************************/
#include <OpenLogger.h>

/************************************************************************/
/************************************************************************/
/**********************  JSON Serial  ***********************************/
/************************************************************************/
/************************************************************************/
JSONSerial jsonSerial = JSONSerial(Serial);

STATE JSONSerial::Write(void const * const pBuff, uint32_t cbWrite, int32_t * pcbWritten) 
{
    switch(writeState)
    {
        case Idle:
            writeState = Pending;

            // fall thru

        case Pending:
            if(serial.writeBuffer((uint8_t  *) pBuff, cbWrite, &SERIAL_OUTPUT_DMA)) writeState = Waiting;
            break;

        case Waiting:
            if(serial.isDMATxDone())
            {
                *pcbWritten = cbWrite; 
                writeState = Idle;
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(writeState); 
}

STATE JSONSerial::Read(void * const pBuff, uint32_t cbBuff, int32_t * pcbRead) 
{ 
    uint32_t cbMax = serial.available();

    // we do NOT want to wait on the DMA, so only copy data out of the existing buffer
    if(cbBuff > cbMax) cbBuff = cbMax;

    for(uint32_t i=0; i<cbBuff; i++) ((uint8_t *) pBuff)[i] = (uint8_t) serial.read();

    *pcbRead = cbBuff; 

    return(Idle); 
}

