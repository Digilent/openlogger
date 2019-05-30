/************************************************************************/
/*                                                                      */
/*    DCout.cpp                                                         */
/*                                                                      */
/*    DC output functionality                                           */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    8/29/2018 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

// calibration buffer
static POLYCAL  calDC = {0, Idle, 0, 40000, 7000000};

// uVDC = -40000*Dpwm + 7000000  --- ideally
STATE DCCalibrate(uint32_t ch) 
{
    OCCH&           ocDC    = (ch == 1) ? DC1_OC    : DC2_OC;
    uint32_t        fbCh    = (ch == 1) ? CH_DC1_FB : CH_DC2_FB;
    int32_t         uV;

    // I can always calibrate the DC, no need to check if off
    
    // valid channel?
    ASSERT(ch == 1 || ch == 2);

    if(calDC.state != Idle && calDC.A != ch)
    {
        return(WaitingForResources);
    }

    switch(calDC.state)
    {
        case Idle:
            // pause the LEDs
            LEDTask(LEDPause);
            arCalIDC[ch].state = Calibrating;

            // no AC gain, so use A as the channel indicator
            calDC.A             = ch;
            ocDC.OCxRS          = 100;          // set to +3v => 175 - 3*0.04 = 100
            calDC.time          = ReadCoreTimer();
            calDC.state         = DCWaitHigh;
            break;
            
        case DCWaitHigh:
            if(ReadCoreTimer() - calDC.time >= PWM_SETTLING_TIME)
            {
                calDC.state     = DCReadHigh;    
            }
            break;

        case DCReadHigh:
            if(FBAWGorDCuV(fbCh, &uV) == Idle)
            {
                calDC.B         = uV;
                ocDC.OCxRS      = 250;          // set to -3V
                calDC.time      = ReadCoreTimer();
                calDC.state     = DCWaitLow;    
            }
            break;
            
        case DCWaitLow:
            if(ReadCoreTimer() - calDC.time >= PWM_SETTLING_TIME)
            {
                calDC.state     = DCReadLow;    
            }
            break;
            
        case DCReadLow:
            if(FBAWGorDCuV(fbCh, &uV) == Idle)
            {
                int32_t D1 = (int32_t) calDC.B; 
                int32_t D2 = uV;

                // B ~ -40000,  C ~ 7000000 -- ideally
                // uV1 = Bpwm1 + C
                // uV2 = Bpwm2 + C
                // uV1 - uV2 = B(pwm1-pwm2) == B(100-250) == B(-150)
                // B = (uV2 - uV1) / 150 
                // or storing B as a positive number
                // (-B) = (uV1 - uV2) / 150 => (uV1 - uV2 + 75) / 150
                calDC.B = (D1 - D2 + 75) / 150;     // -B, a positive number
            
                // uV1 = Bpwm1 + C; 
                // C = uV1 - Bpwm = uV1 - B*100
                // but we store -B so, C = uV1 + (-B)pwm
                calDC.C = D1 + calDC.B * 100;

                // A is unused
                calDC.A = 0;

                // put the version in
                calDC.version = CALVER;

                // say we are calibrated
                calDC.state                     = Calibrated;

                // move it into our calibration constant for the channel
                memcpy(&arCalIDC[ch], &calDC, sizeof(POLYCAL));
                
                // now set the channel to zero volts
                ocDC.OCxRS = DCmV2PWM(ch, 0);

                // we are done
                LEDTask(LEDResume);
                fCalibrated = fIsCalibrated();
                calDC.state = Idle;
            }
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            return(STATEError);
    }
    
    return(calDC.state);  
}

uint16_t DCmV2PWM(uint32_t iChannel, int32_t mVout)
{
    int64_t uVout = mVout * 1000ll;
    uint16_t pwm = 175;

    ASSERT(1 <= iChannel && iChannel <= 2);

    if(mVout < -4000)       uVout = -4000000;
    else if(4000 < mVout)   uVout =  4000000;

    // uVout = B*pwm + C
    // uVout - C = B*pwm
    // pwm = (uVoult - C) / B;
    // pwm = (uVoult - C) / B;
    // pwm = (C - uVoult) / (-B);   Remember, we save away -B
    pwm = (uint16_t) ((arCalIDC[iChannel].C - uVout + arCalIDC[iChannel].B / 2) / arCalIDC[iChannel].B);

    return(pwm);
}

int32_t DCPWM2mV(uint32_t iChannel, uint16_t pwm)
{
    int32_t uV;
    
    if(pwm < 10)    pwm = 10;
    else if(320 < pwm) pwm = 320;

    uV = arCalIDC[iChannel].B * pwm + arCalIDC[iChannel].C;
    
    return((uV + 500) / 1000);
}

void DCSetvoltage(uint32_t iChannel, int32_t mVout)
{
    if(iChannel == 1) DC1_OC.OCxRS = DCmV2PWM(iChannel, mVout);
    else if(iChannel == 2) DC2_OC.OCxRS = DCmV2PWM(iChannel, mVout);  
}