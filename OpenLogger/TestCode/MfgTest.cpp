/************************************************************************/
/*                                                                      */
/*    MfgTest.cpp                                                       */
/*                                                                      */
/*   Implements Manufactuing tests executed by the JSON:                */
/*                                                                      */
/*    {                                                                 */
/*        "test":[                                                      */
/*            {                                                         */
/*                "command":"run",                                      */
/*                "testNbr": <test number>                              */
/*            }                                                         */
/*        ]                                                             */
/*    }                                                                 */
/*                                                                      */
/*   command: must be "run"                                             */
/*   testNbr: Is the test number to run                                 */
/*                                                                      */
/*    The return format is:                                             */
/*                                                                      */
/*    {                                                                 */
/*        "test":[                                                      */
/*            {                                                         */
/*                "command":"run",                                      */
/*                "statusCode": 0,                                      */
/*                "returnNbr": <test number>,                           */
/*                "wait": 0,                                            */
/*            }                                                         */
/*        ]                                                             */
/*    }                                                                 */
/*                                                                      */
/*   command: will be "run"                                             */
/*   statusCode: returned status code from the test                     */
/*   returnNbr: Is the test number                                      */
/*   wait: how long to wait until the test is complete in ms            */
/*                                                                      */
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

/***    STATE MfgTest(int32_t testNbr, STATE& statusCode, uint32_t& msWait)
 *
 *    Parameters:
 *          testNbr:    The test to run
 *
 *    Output Parameters:
 *          statusCode: The return status of the test, 0 (Idle) means success.
 *                          Idle (0), means all went well, but you can return other STATEs even if the test succeeds
 *                          The compound state of MFGSTATE | <your state> (0x10100000 | 0x000xxxxx) is reserved for manufacturing tests
 *                          Test return range is 0x10100000 -> 0x101FFFFF
 *                          The return range in decimal is 269,484,032 -> 270,532,607
 *                          0x10100000 == 0d269484032
 *                          0x101FFFFF == 0d270532607
 *          msWait:     How long to wait after parsing is complete until the test is complete, usually 0
 *                          Usually you do your test while parsing; return Pending is you need multiple passes on the super loop
 *
 *    Returns:
 *          Idle:           When the test is done
 *          Pending:        Not done; means you will be called again on the next super loop pass with the same parameters.
 *                              Allows for long processing while in the parser.
 *          Other STATEs:   Not recommended: but if MkStateAnError(MFGSTATE | 2), will cause an error to be passed to the parser
 *                              It will look like you got a parsing error on the test command
 *    
 *    Description: 
 *      Implements the manufacturing tests such that they can be invoked via 
 *      a JSON test command.
 *    
 *    Note: 
 *      This is a "weak" function. If it does not exist (this module is excluded from the build)
 *      the executing the test will return an unimplemented status code
 *      Unimplemented: 0xA0000009, --> 0d2684354569
 *    
 * ------------------------------------------------------------ */
STATE __attribute__((weak)) MfgTest(int32_t testNbr, STATE& statusCode, uint32_t& msWait)
{
    // example tests, just returns various status codes.
    switch(testNbr)
    {
        case 1:
            // return manufacturing status code 1
            statusCode = MFGSTATE | 1;
            break;

        case 2:
            // return manufacturing status code 2
            statusCode = MFGSTATE | 2;
            break;

        case 3:
            // return completed successfully
            statusCode = Idle;      // Idle is a well known completion state.
            break;

        default:
            // test not implemented
            statusCode = Unimplemented;
            break;
    }

    // When parsing is done, so is the test; usually 0
    msWait = 0;

    // Idle means we are done and won't be called again by the parser
    // returning Pending means you want to be called again with the same parameters
    // effectively yielding to the super loop for another pass
    return(Idle);
}