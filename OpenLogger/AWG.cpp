/************************************************************************/
/*                                                                      */
/*    AWG.cpp                                                           */
/*                                                                      */
/*    Function Generator code                                           */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/17/2016 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>
#include <math.h>

// move to global
// AWGPARAM __attribute__((coherent)) paramIAWG;
AWGPARAM paramIAWG = {Idle, waveNone, 175, 0, 0, 0, 0};
AWGPARAM paramAWG = {Idle, waveNone, 175, 0, 0, 0, 0}; 

// calibration buffer
// uVoffset = -11995pwm + 2099125
// A = 0, B ~ -11995, C ~ 2099125   
// we save -B => 11995
static AWGCAL  calAWG = {{0, Idle, 0, 11995, 2099125}, 511, 175, {0}, {0}};

STATE AWGInit(void) {
    //**************************************************************************
    //*******************  Timers to drive PWM Offset circuits  ****************
    //******************** T2 drives AWG  **************************************
    //********************Same as DC OFFSET TMR ********************************
    //**************************************************************************
    //**************************************************************************
    //*******************  AWG/DAC PWM7  ***************************************
    //**************************************************************************
    // turn everything OFF
    AWG1TRGTMR.TxCONu32     = 0;                            // initialize timer8, used of DMA2/AWG trigger
    AWG1OFF.OCxCONu32       = 0;                            // AWG1 offset
    AWG1DMA.DCHxCONu32      = 0;                            // clear the con reg
    AWG1DMA.DCHxECONu32     = 0;                            // clear the econ reg
   
    // setup the offset PWM, AWG1 on RD3
    AWG1_PPS_OC;                                            // map OC6 to RD14
    AWG1OFF.OCxCON.OCTSEL   = AWG_OFF_OCTSEL;               // T2 selection
    AWG1OFF.OCxCON.OCM      = 0b101;                        // PWM Mode
    AWG1OFF.OCxR            = 0;                            // go high on counter wrap
    AWG1OFF.OCxRS           = PWMIDEALCENTER;               // set to midpoint

    AWG1LAT                 = DACDATA1(511);                // about the mid point
    AWG1TRIS                = 0;                            // set DAC port J to output

    AWG1OFF.OCxCON.ON       = 1;                            // turn on the OC, so we can center the PWM; timer already running
    
    // set up the DMA trigger
    AWG1TRGTMR.TxCON.TCKPS  = 0;                            // timer prescaler
    AWG1TRGTMR.TMRx         = 0;                            // clear timer value
    AWG1TRGTMR.PRx          = PeriodToPRx(AWGPBCLK/AWGMAXSPS); // Now how often the DMA is triggered (10MHz = 100MHz/10)

    // set up the DMA
    paramIAWG.state         = Idle;
    paramIAWG.rgDMAWave[0]  = DACDATA1(511);                // about half through the R2R
    AWG1DMA.DCHxCON.CHPRI   = AWG1_DMAPRI;                  // Give the DAC the highest priority
    AWG1DMA.DCHxCON.CHAEN   = 1;                            // auto enable, keep cycling on block transfer 
    AWG1DMA.DCHxECON.CHSIRQ = AWG1TRGVECTOR;                // event start IRQ Timer 8
    AWG1DMA.DCHxECON.SIRQEN = 1;                            // enable start IRQ
    AWG1DMA.DCHxDSA         = KVA_2_PA(&AWG1LAT);           // Latch J address for destination
    AWG1DMA.DCHxDSIZ        = 2;                            // destination size 2 byte
    AWG1DMA.DCHxCSIZ        = 2;                            // cell transfer size 2 byte
    AWG1DMA.DCHxSSA         = KVA_2_PA(paramIAWG.rgDMAWave); // source buffer
    AWG1DMA.DCHxSSIZ        = 2;                            // source size

    // do not start DMA or DMA trigger

    return(Idle);
}

STATE AWGStop(void) 
{
    
    if(paramIAWG.state == Pending) return(Pending);
    
    // turn off DMA and DMA TRIGGER
    AWG1TRGTMR.TxCON.ON     = 0;
    AWG1DMA.DCHxCON.CHEN    = 0;   
    if(paramIAWG.state ==  Running) paramIAWG.state =  Stopped;

    // center everything
    AWG1OFF.OCxRS           = calIAWG.pwmCenter;                
    AWG1LAT                 = calIAWG.rgDdac[calIAWG.iCenter];  
 
    return(Idle);
}


STATE AWGRun(AWGPARAM& awg) 
{
    // T2 Run always for offset control
    // T8 is the DMA TRIGGER

    if(paramIAWG.state == Calibrating) return(Calibrating);
    
    // turn off DMA and DMA TRIGGER
    AWG1TRGTMR.TxCON.ON     = 0;
    AWG1DMA.DCHxCON.CHEN    = 0;   
    paramIAWG.state         = Pending;

    // Do not turn off offset control; but set it to the new values
    AWG1OFF.OCxRS           = awg.pwmRS;                    // Set offset control

    memcpy(&paramIAWG, &awg, offsetof(AWGPARAM, rgDMAWave));
    memcpy(KVA_2_KSEG1(paramIAWG.rgDMAWave), awg.rgDMAWave, awg.actualSamples*sizeof(uint16_t));

    // configure the trigger timer
    AWG1TRGTMR.TxCON.TCKPS  = awg.tmrPrescalar;             // timer prescaler
    AWG1TRGTMR.TMRx         = 0;                            // clear timer value
    AWG1TRGTMR.PRx          = PeriodToPRx(awg.tmrPeriod);   // Now how often the DMA is triggered (10MHz = 100MHz/10)

    // configure the DMA
    AWG1DMA.DCHxSSA         = KVA_2_PA(paramIAWG.rgDMAWave); // source buffer
    AWG1DMA.DCHxSSIZ        = awg.actualSamples*sizeof(uint16_t);                            // source size

    paramIAWG.state         = Running;              // it is now running

    // get everything going!
    AWG1DMA.DCHxCON.CHEN    = 1;                    // turn on AWG1 DMA

    // turn the trigger on
    AWG1TRGTMR.TxCON.ON     = 1;                    // turn on DMA triggers

    return(Idle);
}

int32_t AWGPWM2mV(uint16_t pwm)
{
    // uVout = B*pwm + C
    // remember we store (-B)
    // uVout = C - (-B)*pwm
    int32_t uV = (int32_t) (calIAWG.polyCal.C - (calIAWG.polyCal.B * pwm));
    
    return((uV + 500) / 1000);
}

uint16_t AWGmV2PWM(int32_t mVout)
{
    int64_t uVout = mVout * 1000ll;

    if(mVout < -1500)   uVout = -1500000;
    if(mVout > 1500)    uVout =  1500000;

    // uVout = B*pwm + C
    // uVout - C = B*pwm
    // pwm = (uVoult - C) / B;
    // pwm = (uVoult - C) / B;
    // pwm = (C - uVoult) / (-B);   Remember, we save away -B
    return((uint16_t) ((calIAWG.polyCal.C - uVout + calIAWG.polyCal.B / 2) / calIAWG.polyCal.B));
}

uint32_t AWGuV2iDdac(int32_t uVFind, int32_t const rguVdac[], uint32_t cDac)
{
    uint32_t    iDacTop     = cDac-1;           // point to the top element
    uint32_t    iDac        = iDacTop / 2;      // always round down!
    uint32_t    iDacBot     = 0;                // point to the bottom element
    uint32_t iDacPass       = iDac;

    ASSERT(cDac >= 2);

    // binary search with bounds checks
    do
    {
        iDacPass = iDac;
        ASSERT(iDac < cDac);

        if(uVFind < rguVdac[iDac])
        {
            iDacTop = iDac;
            iDac = (iDac + iDacBot) / 2;        // round down
        }
 
        else if(uVFind > rguVdac[iDac+1])
        {
            iDacBot = iDac+1;
            iDac = (iDac + iDacTop) / 2;        // round down; iDac never resolve to iDacTop
        }

    // try again if we moved the pointer
    } while(iDacPass != iDac);

    if(iDac == 0 && uVFind < rguVdac[0]) return(0);
    if(iDac == (cDac-1) && uVFind > rguVdac[cDac-1]) return(cDac-1);
    if((uVFind - rguVdac[iDac]) <= (rguVdac[iDac+1] - uVFind)) return(iDac);

    ASSERT(iDac < (cDac-1));
    return(iDac+1);
}

STATE AWGCalibrate(void) {
    int32_t     uV;

    switch (calAWG.polyCal.state) 
    {
        case Idle:
            
            if(AWGStop() != Idle) return(Pending);

            // pause the LEDs
            LEDTask(LEDPause);
            calIAWG.polyCal.state = Calibrating;

            // set our PWM
            AWG1OFF.OCxRS = 175;            // assume something close to zero on the PWM
            calAWG.rgDdac[0] = DACDATA1(0);
            AWG1LAT = DACDATA1(0);          // set the DAC value (0)
            calAWG.iCenter  = 0;
            calAWG.polyCal.time = ReadCoreTimer();
            calAWG.polyCal.state = AWGWaitPWM;
            break;

        case AWGWaitPWM:
            if(ReadCoreTimer() - calAWG.polyCal.time >= PWM_SETTLING_TIME) 
            {
                // we can jump directly to the read as
                // we waited the longer PWM settling time.

                calAWG.polyCal.state = AWGReadHW;
            }
            break;

        case AWGWaitHW:
            if(ReadCoreTimer() - calAWG.polyCal.time >= AWG_SETTLING_TIME) 
            {
                calAWG.polyCal.state = AWGReadHW;
            }

        case AWGReadHW:
            if(FBAWGorDCuV(CH_AWG1_FB, &uV) == Idle) 
            {
                // save away the voltage
                calAWG.rguVdac[calAWG.iCenter] = uV;

                calAWG.iCenter++;
                if (calAWG.iCenter < HWDACSIZE)
                {
                    // set up for the next read
                    calAWG.rgDdac[calAWG.iCenter] = DACDATA1(calAWG.iCenter);
                    AWG1LAT = calAWG.rgDdac[calAWG.iCenter];

                    calAWG.polyCal.time = ReadCoreTimer();
                    calAWG.polyCal.state = AWGWaitHW;
                } 
                else 
                {
                    // done, get out.
                    calAWG.polyCal.A = 0;                 // we can use this as a pass count for now
                    calAWG.polyCal.state = AWGSort;
                }
            }
            break;

        case AWGSort:
            {
                bool        fStillSorting = false;
                int         i;

                // pass count
                calAWG.polyCal.A++;     

                // bubble sort
                // run through the list once
                for (i = 0; i < (HWDACSIZE - 1); i++) 
                {

                    // exchange if out of order
                    if (calAWG.rguVdac[i] > calAWG.rguVdac[i + 1]) 
                    {
                        int32_t     uVdac   = calAWG.rguVdac[i];
                        uint16_t    dacD    = calAWG.rgDdac[i];

                        calAWG.rguVdac[i]   = calAWG.rguVdac[i+1];
                        calAWG.rgDdac[i]    = calAWG.rgDdac[i+1];

                        calAWG.rguVdac[i+1] = uVdac;
                        calAWG.rgDdac[i+1]  = dacD;

                        fStillSorting = true;
                    }
                }

                // if done, go to the next state
                // otherwise do another pass
                if(!fStillSorting) calAWG.polyCal.state = AWGFindCenter;
            }
            break;

        // find the center point so we know how to do offset
        case AWGFindCenter:
            {  
                int32_t uVDynamicRange  = calAWG.rguVdac[HWDACSIZE-1] - calAWG.rguVdac[0];
                int32_t uVCenter        = (calAWG.rguVdac[HWDACSIZE-1] + calAWG.rguVdac[0]) / 2;

                // make sure we have a 3v P2P dynamic range;
                // actually to account for the PWM swing, PWM moves 12mV per
                // half of that is 6mV and we will find the closest PWM value
                ASSERT(uVDynamicRange > 3006000l);

                // now that the voltages are sorted, we can know the dynamic range of the list
                calAWG.iCenter = AWGuV2iDdac(uVCenter, calAWG.rguVdac, HWDACSIZE);

                // set the center of the ladder, now lets find the best PWM values
                AWG1LAT = calAWG.rgDdac[calAWG.iCenter];
                AWG1OFF.OCxRS = 300;         // set low output (high PWM value)

                // wait for PWM to settle
                calAWG.polyCal.state = AWGWaitPWMLow;
            }
            break;

        case AWGWaitPWMLow:
            if (ReadCoreTimer() - calAWG.polyCal.time >= PWM_SETTLING_TIME) 
            {
                calAWG.polyCal.state = AWGReadPWMLow;
            }
            break;

        case AWGReadPWMLow:
            if(FBAWGorDCuV(CH_AWG1_FB, &uV) == Idle) 
            {
                calAWG.polyCal.A = uV;
                AWG1OFF.OCxRS = 50;
                calAWG.polyCal.time = ReadCoreTimer();
                calAWG.polyCal.state = AWGWaitPWMHigh;
            }
            break;

       case AWGWaitPWMHigh:
            if(ReadCoreTimer() - calAWG.polyCal.time >= PWM_SETTLING_TIME) 
            {
                calAWG.polyCal.state = AWGReadPWMHigh;
            }
            break;

       case AWGReadPWMHigh:
            if(FBAWGorDCuV(CH_AWG1_FB, &uV) == Idle) 
            {
                // uV = Bpwm + C
                // uV1 - uV2 = B(pwm1 - pwm2)  but B will be negative
                // uV2 - uV1 = (-B)(pwm1 - pwm2)
                // uV2 - uV1 = (-B)(300 - 50)
                // uV2 - uV1 = (-B)250
                // (-B) = (uV2 - uV1) / 250 => (uV2 - uV1 + 125) / 250
                calAWG.polyCal.B = (uV - calAWG.polyCal.A + 125) / 250;   // ~ 11995

                // our full equation is: 
                // uV = ADadc + BDpwm + C
                // we did our table with Dpwm == 175
                // so (uV at iCenter) == 175B + [ADadc + C]
                // if we want ADadc == 0 at the center, then make ADadc = 0, remeber we store -B
                // (uV at iCenter) = -175(-B) + C
                // C = (uV at iCenter) + 175(-B)
                // and now we centered the table on zero
                calAWG.polyCal.C = calAWG.rguVdac[calAWG.iCenter] + 175 * calAWG.polyCal.B;

                // now center the PWM
                // uV = Bpwm + C
                // pwm = (uV - C)/B 
                // pwm = (C - uV)/(-B)
                // uV == 0;
                // pwm = (C)/(-B) => pwm = (C + (-B)/2)/(-B)
                calAWG.pwmCenter  = (uint16_t) ((calAWG.polyCal.C + calAWG.polyCal.B / 2) / calAWG.polyCal.B);
                calAWG.polyCal.A = calAWG.polyCal.C - calAWG.polyCal.B * calAWG.pwmCenter;

                AWG1OFF.OCxRS = calAWG.pwmCenter;

                calAWG.polyCal.time = ReadCoreTimer();
                calAWG.polyCal.state = AWGWaitPWMZero;
            }
            break;

        // both PWM and ladder are at best zero, wait to settle to zero
        case AWGWaitPWMZero:
            if(ReadCoreTimer() - calAWG.polyCal.time >= PWM_SETTLING_TIME) 
            {
                calAWG.polyCal.state = AWGAdjZero;
            }
            break;

        // get our new best zero value; this will be close, but not our center value
        case AWGAdjZero:
            if(FBAWGorDCuV(CH_AWG1_FB, &uV) == Idle) 
            {
                int32_t duV = calAWG.rguVdac[calAWG.iCenter];               
                int32_t duVC = calAWG.polyCal.A - uV;               
                int     i;

                // at this point our C is very close; but we still have some minor variations
                // in the hardware, so we can tweak things just a little better to hit zero the best.
                // uV = C - (-B)Dpwm
                // uV = C - calAWG.polyCal.B * calAWG.pwmCenter
                // C = uV + calAWG.polyCal.B * calAWG.pwmCenter
                calAWG.polyCal.C = uV + calAWG.polyCal.B * calAWG.pwmCenter;

                // adjust the table for zero at the center;
                // BEWARE: just because the PWM is centered, do
                for (i = 0; i < HWDACSIZE; i++) calAWG.rguVdac[i] -= duV;

                // do some checks on some assumptions; these may need tweaking for limits
                ASSERT(-3000 < duVC && duVC < 3000);            // our initial guess should be very close 3mV (maybe too tight of tolerance)
                ASSERT(1500000 < calAWG.rguVdac[HWDACSIZE-1]);  // make sure we can drive 1.5v from the tables maybe false assumption)
                ASSERT(calAWG.rguVdac[0] < -1500000);           // make sure we can drive -1.5v from our tables (maybe false assumption)
 
                // At this point, to do wave gen; first set the offset with the PWM value, find out what the actual offset is
                // subtract that from you original wave pattern, and then do the lookups in the table.
                calAWG.polyCal.state = Done;
            }

        case Done:

            // A is unused
            calAWG.polyCal.A = 0;

            // put the version in
            calAWG.polyCal.version = CALVER;

            // say we are calibrated
            calAWG.polyCal.state = Calibrated;

            // move it into our calibration constant for the channel
            // this will take some time
            memcpy(&calIAWG, &calAWG, sizeof(AWGCAL));

            // say we are done
            calAWG.polyCal.state = Idle;
            LEDTask(LEDResume);
            fCalibrated = fIsCalibrated();
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return (calAWG.polyCal.state);
}


STATE AWGSetCustomWaveform(AWGPARAM& awg, int16_t mvDCOffset) 
{

    switch (awg.state) {
        
        case Idle: 
        case AWGWaitCustomWaveform:
            {
                uint32_t i = 0;
                int32_t mvMax = -AWGMAXP2P;
                int32_t mvMin = AWGMAXP2P;
                int16_t pwmMvOffset = mvDCOffset;
                                   
                if(awg.actualSps > AWGMAXSPS)
                {
                    awg.state = Idle;
                    awg.waveform = waveNone;
                    return(AWGExceedsMaxSamplPerSec);                
                }
                
                // find max and min of waveform
                for(i = 0; i < awg.actualSamples; i++) 
                {
                    int32_t mvEntry = awg.rgVWave[i] + mvDCOffset;

                    if(mvEntry > mvMax) mvMax = mvEntry;
                    if(mvEntry < mvMin) mvMin = mvEntry;
                }

                if((mvMax - mvMin) > AWGMAXP2P || mvMin < -AWGMAXP2P || AWGMAXP2P < mvMax)
                {
                    awg.state = Idle;
                    awg.waveform = waveNone;
                    return(AWGValueOutOfRange);
                }

                // adjust for the actual offset we can provide by PWM, the rest has to be
                // be done by the table.
                awg.pwmRS = AWGmV2PWM((mvMax + mvMin + 1) / 2);
                pwmMvOffset = AWGPWM2mV(awg.pwmRS);
                    
                for(i = 0; i < awg.actualSamples; i++) awg.rgVWave[i] += (mvDCOffset - pwmMvOffset);

                for(i = 0; i < awg.actualSamples; i++) awg.rgDMAWave[i] = calIAWG.rgDdac[AWGuV2iDdac(awg.rgVWave[i]*1000, calIAWG.rguVdac, HWDACSIZE)];

                awg.state = Idle;
            }
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(awg.state);
}

// take freq in mHz, returns in mHz
// Highest freq we can support is 1MHz, or 1,000,000,000 mHz (still an int32_t)
// Highest buffer size AWGMAXBUF, which must be less than 64K (still an int32_t))
// Highest AWGMAXSPS we support is 10MS/s (still an int32_t)
// so while we take uint32_t as inputs, you can cast from an int32_t just fine.
// if we did 1S/s and had a size of 25,000 we could support a waveform as slow as 1/25,000 Hz or 40 uHz
uint32_t AWGCalculateBuffAndSps(uint32_t reqFreqmHz, uint32_t * pcBuff, uint32_t * pSps)
{
    // calculate the timer and buffer size
    if(reqFreqmHz == 0)             // just a DC Value
    {
        *pSps = AWGMAXSPS;
        *pcBuff = 1;
        return(0);
    }

    // PB = (buff)(tmr)(freq)
    // you can't have a bigger buffer because we can't push data out faster
    // with a buffer size of 25,000 and sample rate of 10,000,000 
    // we can use slower sample rate at 400 Hz
    else if((*pcBuff = (uint32_t) ((((uint64_t) AWGMAXSPS) * 1000ull + (reqFreqmHz/2)) / reqFreqmHz)) <= ((uint32_t) AWGMAXBUF))
    {
        // the cutoff freq for here is 400 Hz
        *pSps = AWGMAXSPS;                                       // samples per sec
//        *pcBuff = (uint32_t) ((((uint64_t) AWGMAXSPS) * 1000ull + (reqFreqmHz/2)) / reqFreqmHz);             // size of buffer              
    }
    // PB = (buff)(tmr)(freq)
    // use the maximum buffer size to use the fastest clock
    // this is from 400Hz down to 40uHz
    else
    {
        const uint64_t pbx1000              = AWGPBCLK * 1000ull;
        uint32_t tmr                        = (uint32_t) (((pbx1000 / AWGMAXBUF) + (reqFreqmHz/2)) / reqFreqmHz); 

        // if we have a tmr overflow, put it at the max
        if(tmr > 65536)         tmr         = 65536;

        *pSps                               = (AWGPBCLK + (tmr/2)) / tmr;
        if(*pSps > AWGMAXSPS)   *pSps       = AWGMAXSPS;

        *pcBuff                             = (uint32_t) ((((uint64_t) *pSps) * 1000ll + (reqFreqmHz/2)) / reqFreqmHz);
        if(*pcBuff > AWGDMABUF) *pcBuff     = AWGDMABUF;

    }

    // sps = freq * cbuff
    // freq = sps/cbuff
    // return in mHz
    return((uint32_t) ((((int64_t) (*pSps) * 1000ll) + (*pcBuff)/2) / (*pcBuff)));         
}

// mvP2P are always centered on zero, the waveform generated is centered on zero
// the offset must contain the full offset.
STATE AWGSetRegularWaveform(AWGPARAM& awg, WAVEFORM waveform , uint32_t freqmHz, int32_t mvP2P, int32_t mvDCOffset) 
{
    uint32_t cnt;
    uint64_t mSPS;

    switch (awg.state) 
    {
        
        case Idle: 
            
            if( mvP2P > AWGMAXP2P || mvP2P < 0  || 
                (mvDCOffset - mvP2P/2)  < -AWGMAXP2P || AWGMAXP2P < (mvP2P/2 + mvDCOffset) ||
                freqmHz > (AWGMAXFREQ * 1000) 
                ) return(AWGValueOutOfRange);

            switch(waveform)
            {
                case waveDC:
                    awg.state = AWGDC;
                    freqmHz = 0;
                    break;
                    
                case waveSine:
                    awg.state = AWGSine;
                    break;
                    
                case waveSquare:
                    awg.state = AWGSquare;
                    break;
                    
                case waveTriangle:
                    awg.state = AWGTriangle;
                    break;
                    
                case waveSawtooth:
                    awg.state = AWGSawtooth;
                    break;

                default:
                    return(AWGWaveformNotSupported);
            }
            
            awg.actualmHz = AWGCalculateBuffAndSps(freqmHz, &awg.actualSamples, &awg.actualSps);
            mSPS = SPSPreScalarAndPeriod(awg.actualSps, 1, AWGPBCLK, &awg.tmrPrescalar, &awg.tmrPeriod, &cnt);
            ASSERT(mSPS == awg.actualSps && cnt == 1); 
            awg.waveform = waveform;

            // we are going to use this as index counters
            // we will set this after the wave is created
            awg.actualVOffset   = 0;
            awg.actualVpp       = 0;
            break;
 
        case AWGDC:
        case AWGSquare:
        case AWGSawtooth:
        case AWGTriangle:
        case AWGSine:
            {
                uint32_t tStart = ReadCoreTimer();
                uint32_t i = (uint32_t) awg.actualVOffset;

                do
                {
                    uint32_t j = i + 10;

                    switch(awg.state)
                    {
                        case AWGDC:
                            {
                                i                   = 1;
                                awg.actualSps       = AWGMAXSPS;
                                awg.rgVWave[0]      =  0;                       // DC value centered on zero              
                                awg.state           = AWGWaitCustomWaveform;
                            }
                            break;

                        case AWGSquare:
                            {
                                int16_t halfMag = ((int16_t) mvP2P) / 2;
                                int16_t nhalfMag = -halfMag;
                
                                for (; i < awg.actualSamples/2 && i < j; i++) {
                                    awg.rgVWave[i] =  nhalfMag;                   
                                }
                
                                for (; i < awg.actualSamples && i < j; i++) {
                                    awg.rgVWave[i] =  halfMag;                   
                                }                
                            }
                            break;
            
                        case AWGSawtooth:
                            {
                                double dblBuffSize      = ((double) awg.actualSamples);
                                double dblMagnitude     = (double) mvP2P;
                                double dblHalfMagnitude = dblMagnitude / 2;
                
                                for (; i < awg.actualSamples && i < j; i++) {
                                    awg.rgVWave[i] = ((int16_t) ((dblMagnitude * (((double) i) / dblBuffSize)) - dblHalfMagnitude));                   
                                }
                            }
                            break;
            
                        case AWGTriangle:
                            {
                                uint32_t buffDown       = (awg.actualSamples / 2);           // truncates odd numbers low
                                uint32_t buffUp         = (awg.actualSamples - buffDown);    // may be one longer than buffDown
                                double dblBuffSizeUp    = ((double) buffUp);
                                double dblBuffSizeDown  = ((double) buffDown);
                                double dblMagnitude     = (double) mvP2P;
                                double dblHalfMagnitude = dblMagnitude / 2;
                
                                // create up slope
                                for (; i < buffUp && i < j; i++) {
                                    awg.rgVWave[i] = ((int16_t) ((dblMagnitude * (((double) i) / dblBuffSizeUp)) - dblHalfMagnitude));                   
                                }
                
                                // create down slope
                                for (; i < awg.actualSamples && i < j; i++) {
                                    awg.rgVWave[i] = ((int16_t) ((dblMagnitude * ((dblBuffSizeDown - ((double) (i - buffUp))) / dblBuffSizeDown)) - dblHalfMagnitude));                   
                                }
                            }
                            break;
        
                        case AWGSine:
                            {
                                double dblBuffSize = ((double) awg.actualSamples);
                                double dblMagnitude = ((double) mvP2P) / 2;

                                for (; i < awg.actualSamples && i < j; i++) {
                                    awg.rgVWave[i] = ((int16_t) (dblMagnitude * sin((((double) i) / dblBuffSize) * M_TWOPI)));                   
                                }               
                            }
                            break;
                    }

                } while(ReadCoreTimer() - tStart < 500 * CORE_TMR_TICKS_PER_USEC);

                // are we done?
                if(i == awg.actualSamples)
                {
                    awg.actualVOffset   = mvDCOffset;
                    awg.actualVpp       = mvP2P;
                    awg.state           = AWGWaitCustomWaveform;
                }

                // go again
                else
                {
                    // save i away for the next pass
                    awg.actualVOffset   = i;
                }
            }
            break;

        case AWGWaitCustomWaveform:
            return(AWGSetCustomWaveform(awg, mvDCOffset));
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return (awg.state);
}

