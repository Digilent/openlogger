/************************************************************************/
/*                                                                      */
/*    LEDs.cpp                                                          */
/*                                                                      */
/*    LEDs State machine                                                */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    6/16/2016(KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

static STATE        ledState = Idle;
static STATE        ledCur = Idle;
static STATE        ledNext = LEDInit;
static uint32_t     tBlink;
static uint32_t     cPause = 0;

STATE LEDTask(STATE ledCMD)
{   
    // this must take effect right now!
    if(ledCMD == LEDPause)
    {
        SetGPIO(LED_RED, 0);
        SetGPIO(LED_ORANGE, 0);
        SetGPIO(LED_GREEN, 0);
        SetGPIO(LED_BLUE, 0);
        ledState = LEDHold;
        ledCMD = Idle;
        cPause++;
    }

    if(ledCMD != Idle && ledNext != ledCMD) ledNext = ledCMD;
    
    if(ledState == Idle && ledCur != ledNext)
    {
        ledState = ledNext;
        ledCur = ledNext;
    }

    switch(ledState)
    {
        case Idle:
            switch(ledCur)
            {
                case LEDReady:
                    if((ReadCoreTimer() - tBlink) > (500 * CORE_TMR_TICKS_PER_MSEC))
                    {
                        int64_t min, ave, max1, max2, max3, total;
                        tBlink = ReadCoreTimer(); 

                        // the Green LED, blinking ready heartbeat
                        // steady Green means logging to the RAM buffer
                        if(logIParam.state == Running)    
                        {
                            SetGPIO(LED_GREEN, 1);
                        }
                        else            
                        {
                            SetGPIO(LED_GREEN, (GetGPIO(LED_GREEN)^true));    
                        }

                        // Blue LED, are we connected to WiFi
                        // in future, blinking should mean WiFi waiting
                        // steady, WiFi connected
                        SetGPIO(LED_BLUE, deIPcK.isIPReady());

                        // the Orange status LED
                        // Steady Green and blinking orange means logging to the target has stopped
                        // Steady Green and steady orange means logging to the target; 
                        // Just Stead Orange meansthe OpenLogger is still initializing.

                        if((logIParam.state == Running || logIParam.state == Finishing) && logIParam.adcTrg.vol != VOLRAM) 
                        {
                            // if we are done logging to the target, blink
                            if(logIParam.adcTrg.state == Idle)
                            {
                                SetGPIO(LED_ORANGE, (GetGPIO(LED_ORANGE)^true));
                            }

                            // if we are still logging to the target, steady orange
                            else
                            {
                                SetGPIO(LED_ORANGE, 1);
                            }
                        }
                        else
                        {
                                SetGPIO(LED_ORANGE, 0);
                        }

                        // bad conditions turn on the RED LED
                        // solid Red means we overran
                        loopStats.GetStats(LOOPSTATS::ADCBACKLOG, min, ave, max1, total);
                        loopStats.GetStats(LOOPSTATS::ADCTRGBACKLOG, min, ave, max2, total);
                        loopStats.GetStats(LOOPSTATS::DBDLADCTRG, min, ave, max3, total);
                        if(max1 >= CSRBUFF || max2 >= CSVBUFF) 
                        {
                            SetGPIO(LED_RED, 1);
                        }
                        else if(total > ((3 * CSVBUFF)/4) || (ave > 100 && total > CSVBUFF/8)) 
                        {
                            // blinking Red means we are close to an overrun
                            SetGPIO(LED_RED, (GetGPIO(LED_RED)^true));
                        }
                        else
                        {
                            SetGPIO(LED_RED, 0);
                        }
                    }  

                    // see if the user button is to stop logging
                    if(logIParam.state == Running && GetGPIO(BTN_PIN_1))    
                    {
                        // stop sampling
                        ADCStopSampling();

                        // put in the finishing state
                        // force stopped logging
                        logIParam.state = Finishing;
                        logIParam.stcd  = STCDForce;
                    }
                    break;

                default:
                    break;
            }
            break;

        case LEDInit:
            SetGPIO(LED_RED, 0);
            SetGPIO(LED_ORANGE, 1);
            SetGPIO(LED_GREEN, 0);
            SetGPIO(LED_BLUE, 0);
            ledState = Idle;
            break;

        case LEDReady:
            SetGPIO(LED_RED, 0);
            SetGPIO(LED_ORANGE, 0);
            SetGPIO(LED_GREEN, 1);
            SetGPIO(LED_BLUE, 0);
            tBlink = ReadCoreTimer();
            ledState = Idle;
            break;

        case LEDHold:
            if(ledNext == LEDResume)
            {
                cPause--;
                if(cPause == 0)
                {
                    ledNext = ledCur;
                    ledState = ledCur;
                }
            }
            else if(ledCMD != Idle && ledCur != ledCMD) 
            {
                ledCur = ledCMD;
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }    
    
    return(ledState);
}


