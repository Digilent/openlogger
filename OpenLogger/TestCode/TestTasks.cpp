/************************************************************************/
/*                                                                      */
/*    TestTasks.cpp                                                     */
/*                                                                      */
/*    Manage our Test Tasks                                             */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    7/23/2018(KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

STATE TestTasks(void)
{   
    static STATE retState = Idle;
    static uint16 gpioCnt   = 0;

    switch(runTest)
    {
        case TESTComplete:
            Serial.println("Test Finished");
            runTest = TESTNone;
            break;

        case TESTNone:
            break;

        case TESTCalPSK:
            if((retState = WiFiCalPSK()) == Idle || IsStateAnError(retState)) runTest = TESTComplete;
            break;

        case TESTFat32:
            if(FATTest() == Idle) runTest = TESTComplete;
            break;

        case TESTGPIO:
            TRISFCLR    = 0b0010000100111111;
            LATF = gpioCnt++;
            break;

        case TESTGPIOEnd:
            TRISFSET    = 0b0010000100111111;
            runTest = TESTComplete;
            break;

        case TESTEncoder:
            SetGPIO(LED_PIN_2, GetGPIO(ENC_PIN_DIR));
            SetGPIO(LED_PIN_3, !GetGPIO(ENC_PIN_DIR));
            break;

        case TESTEncoderEnd:
            SetGPIO(LED_PIN_2, 0);
            SetGPIO(LED_PIN_3, 0);
            runTest = TESTComplete;
            break;

        case TESTDDR:
            if((retState = TestDDR()) == Idle || IsStateAnError(retState)) runTest = TESTComplete;
            break;
 
        case TESTFIFO245:
            FIFOEcho();
            break;

        case TESTFIFO245End:
            if((retState = FIFOEcho()) == Idle || IsStateAnError(retState))
            {
                runTest = TESTComplete;
                jsonFT245.EnableIO(true);
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }
                
    return(retState);
}
