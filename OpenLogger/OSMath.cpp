/************************************************************************/
/*                                                                      */
/*    Helper.cpp                                                        */
/*                                                                      */
/*    Helper functions to deal with date and time                       */
/*    and MAC and Number printing                                       */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2013, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    2/1/2013(KeithV): Created                                         */
/************************************************************************/
#include <OpenLogger.h>

bool OSAdd(uint8_t m1[], uint32_t cm1, uint8_t m2[], uint32_t cm2, uint8_t r[], uint32_t cr)
{
    uint32_t i;
    uint16_t sum = 0;
    
    // do not want to clear the result
    // because the result may be the same buffer as m1 or m2

    // add them up until we run out the result
    for(i=0; i<cr; i++)
    {
        if(i<cm1) sum += (unsigned short) m1[i];
        if(i<cm2) sum += (unsigned short) m2[i];
        r[i] = (unsigned char) (sum & 0xFF);
        sum >>= 8;
    }

    return(true);
}

bool OSMakeNeg(uint8_t m1[], uint32_t cm1)
{
    uint32_t i;
    uint8_t one = 1;

    for(i=0; i<cm1; i++) m1[i] = ~m1[i];
    OSAdd(m1, cm1, &one, 1, m1, cm1); 

    return(true);
}

bool OSUMult(uint8_t m1[], uint32_t cm1, uint8_t m2[], uint32_t cm2, uint8_t r[], uint32_t cr)
{
    uint32_t im1, im2, ir;

    // clear the result
    memset(r, 0, cr);

    if(cr < (cm1 + cm2))
        return(false);
 
    for(im2 = 0; im2 < cm2; im2++)
    {
        for(im1 = 0; im1 < cm1; im1++)
        {
            unsigned long sr = (unsigned long) m1[im1] * (unsigned long) m2[im2];

            for(ir = im1+im2; sr != 0 && ir < cr; ir++)
            {
                sr += ((unsigned long) r[ir]);
                r[ir] = sr & 0xFF;
                sr >>= 8;
            }
        }
    }

    return(true);
}

bool OSMult(int8_t m1[], uint32_t cm1, int8_t m2[], uint32_t cm2, int8_t r[], uint32_t cr)
{
    bool negative = false;
    uint8_t lm1[cm1];
    uint8_t lm2[cm2];

    if(m1[cm1-1] < 0)
    {
        negative ^= true;
        OSMakeNeg(lm1, cm1);
    }
    else
    {
        memcpy(lm1, m1, cm1);
    }

    if(m2[cm2-1] < 0)
    {
        negative ^= true;
        OSMakeNeg(lm2, cm2);
    }
    else
    {
        memcpy(lm2, m2, cm2);
    }

    OSUMult(lm1, cm1, lm2, cm2, (uint8_t *) r, cr);

    if(negative)
    {
        OSMakeNeg((uint8_t *) r, cr);
    }

    return(true);
}

// WARNING, the largest dl can be is 2^^56!! => 72,057,594,037,927,936
bool OSDivide(int8_t m1[], uint32_t cm1, int64_t d1, int8_t r[], uint32_t cr)
{
    bool negative = false;
    int i = 0;
    uint8_t lm1[cm1];

    // clear the result
    memset(r, 0, cr);

    // can't divide by zero
    if(d1 == 0)
        return(false);

    // just do int64 math if small enough
    else if(cm1 <= sizeof(int64_t))
    {
        int64_t r1 = 0;

        // sign extend if negative
        if(m1[cm1-1] < 0) memset(&r1, 0xFF, sizeof(r1));

        // put in r1 and divide as int64
        memcpy(&r1, m1, cm1);
        r1 /= d1;
           
        // return the result
        // if r1 is sign extend
        if(r1 < 0) memset(r, 0xFF, cr);

        // now put in the result
        if(sizeof(r1) < cr) cr = sizeof(r1);
        memcpy(r, &r1, cr);

        // done
        return(true);
    }

    // make positive
    else if(d1 < 0)
    {
        negative ^= true;
        d1 = -d1;
    }

    // make positive
    if(m1[cm1-1] < 0)
    {
        negative ^= true;
        OSMakeNeg((uint8_t *) lm1, cm1);
    }
    else
    {
        memcpy(lm1, m1, cm1);
    }

    // long division
    for(i = cm1 - sizeof(uint64_t); i >= 0; i--)
    {
        uint64_t m2, q2, r2;

        // get the upper bits for division
        memcpy(&m2, &lm1[i], sizeof(uint64_t));

        // add to the result
        // overflow could occur if the result won't fit in a int64_t
        q2 = m2 / d1;
        if(i < (int32_t) cr) OSAdd((uint8_t *) &r[i], cr - i, (uint8_t *) &q2, sizeof(q2), (uint8_t *) &r[i], cr - i);

        // get the remainder
        r2 = m2 % d1;

        // put it back and get ready for the next division.
        memcpy(&lm1[i], &r2, sizeof(uint64_t));
    }

    if(negative)
    {
        OSMakeNeg((uint8_t *) r, cr);
    }

    return(true);
}

char * ulltoa(uint64_t val, char * buf, uint32_t base)
{
	uint64_t	v;
	char		c;

	v = val;
	do {
		v /= base;
		buf++;
	} while(v != 0);
	*buf-- = 0;
	do {
		c = val % base;
		val /= base;
		if(c >= 10)
			c += 'A'-'0'-10;
		c += '0';
		*buf-- = c;
	} while(val != 0);
	return ++buf;
}

char * illtoa(int64_t val, char * buf, uint32_t base)
{
	char *	cp = buf;

	if(val < 0) {
		*buf++ = '-';
		val = -val;
	}
	ulltoa(val, buf, base);
	return cp;
}

int64_t GetSamples(int64_t psec, int64_t xsps, uint32_t scaleSPS)
{
    int32_t     np      = 1; 
    int64_t     samp    = 0;
    uint64_t    half    = 500000000000ull;
    uint8_t     r[2*sizeof(uint64_t)];
    uint8_t     r2[2*sizeof(uint64_t)];

    // The math.
    // samples = samples/sec * sec
    // samples = (xSamples/sec) * (Samples/xSamples)  * psec * (sec/1000000000000psec)
    // samples = (xsps / scaleSPS) * (psec / 1000000000000)
    // samples = (xsps * psec) / (scaleSPS * 1000000000000)

    // want to do positive math, for rounding
    if(psec < 0) 
    {
        psec *= -1;
        np = -1;
    }

    // xSamples/sec * psec
    OSUMult((uint8_t *) &psec, sizeof(psec), (uint8_t *) &xsps, sizeof(xsps), r, sizeof(r));

    // Division is hard because we will exceed our maximum of 2^^56, so we do it in 2 part
    // the first part is the smaller divisor so we get the biggest quotient. That we do can skip averaging
    // normally scaleSPS is 1000, sometimes 1000000, but 1000000000000 >> 1000000, so we don't have to round the first
    // divide because the second one will swamp it. We may be slightly off on rounding around the 50% mark, but close enough

    // (mSamples/sec * psec) / scaleSPS
    OSDivide((int8_t *) r, sizeof(r), scaleSPS, (int8_t *) r2, sizeof(r2));

    // add for rounding
    // ((mSamples/sec * psec) / scaleSPS) + ps/2        // for averaging
    OSAdd(r2, sizeof(r2), (uint8_t *) &half, sizeof(half), r2, sizeof(r2)); 

    // ps = 10^^12 = 1000000000000ull
    // (((mSamples/sec * psec) / scaleSPS) + ps/2) / ps        
    OSDivide((int8_t *) r2, sizeof(r2), 1000000000000ull, (int8_t *) &samp, sizeof(samp));

    // get our sign correct
    samp *= np;
    
    return(samp);
}

// max xsps = 1,000,000 ==  59.8 bits (still fits in int64_t)
// pratically we can go to uSPS or 1,000,000
int64_t GetPicoSec(int64_t samp, int64_t xsps, uint32_t scaleSPS)
{
    int32_t np = 1;
    uint8_t r[2*sizeof(uint64_t)];
    int64_t psec = 0;
    uint64_t psScaleSPS = 1000000000000ull * scaleSPS;
    uint64_t half = (uint64_t) (xsps/2);

    // The math.
    // sec = samples / (samples / sec)
    // psec = 10^^12 * samples / (samples / sec)
    // psec = (10^^12 * scaleSPS) * samples / ((scaleSPS * samples) / sec)

    // go positive
    if(samp < 0)
    {
        samp *= -1;
        np = -1;
    }

    OSUMult((uint8_t *) &samp, sizeof(samp), (uint8_t *) &psScaleSPS, sizeof(psScaleSPS), r, sizeof(r));
    OSAdd(r, sizeof(r), (uint8_t *) &half, sizeof(half), r, sizeof(r));
    OSDivide((int8_t *) r, sizeof(r), xsps, (int8_t *) &psec, sizeof(psec));

    // get our sign back
    psec *= np;

    return(psec);
}



#define MAX_D 7
// max number of digits, cbD <= 7
char * GetPercent(int64_t actual, int64_t ideal, int32_t cbD, char * pchOut, int32_t cbOut)
{
    bool    fNeg        = (ideal > actual);
    int64_t diffp       = fNeg ? (ideal - actual) : (actual - ideal);
    char    szI2A[20];
    char    szR[20];
    int32_t cbI2A;
    int32_t i;
    char *  pchS;

    ASSERT(cbOut > (cbD + 4));  // leave room for "+/-", ".", leading 0, and NULL
    ASSERT(cbD <= MAX_D);           // can only  have 7 digits

    // this will round to our digits
    diffp *= 100;
    for(i=0; i < cbD; i++) diffp *= 10;
    diffp = (diffp + ideal/2) / ideal;
    for(; i < MAX_D; i++) diffp *= 10;

    // overflow condition. We will only allow 1,000,000,000 %
    if(diffp > 10000000000000000ll)
    {
        memset(pchOut, 'E', cbOut-1);
        pchOut[cbOut-1] = 0;
        return(pchOut);
    }

    illtoa(diffp, szI2A, 10);
    cbI2A = strlen(szI2A);

    memset(szR, '0', sizeof(szR)-1);
    szR[sizeof(szR)-1] = 0;

    pchS = szR + sizeof(szR) - cbI2A - 1;
    memcpy(pchS, szI2A, cbI2A);

    // make room for the "."
    memcpy(&szR[1], &szR[2], sizeof(szR) - MAX_D - 3);  // 7 behind the ".", the NULL, the +/-, and shift up one. sizeof(szR) - 7 - 1 - 1 - 1;

    // put the "." in
    szR[sizeof(szR) - MAX_D - 2] = '.';

    // find the top of the string
    if(cbI2A < (MAX_D + 1)) i = sizeof(szR) - MAX_D - 3;
    else i = sizeof(szR) - cbI2A - 2;

    pchS = &szR[i];

    if(fNeg)
    {
        pchS--;
        *pchS = '-';
        i--;
    }

    // how many char in the output buffer
    i = sizeof(szR) - i - MAX_D + cbD;

    if(i > cbOut) i = cbOut;
    memcpy(pchOut, pchS, i-1);
    pchOut[i-1] = 0;

    return(pchOut);
}

// returns actual sps scaled by scaleSPS
// xsps = SPS at the scaling factor
// scaleSPS SPS == 1, mSPS == 1000, uSPS == 1000000
// pbclk in ticks per second
// output prescalar for tmr
// output pPeriod for tmr
// output pCnt, how many times the tmr must roll
uint64_t TmrPreScalarAndPeriod(uint64_t tmr, uint16_t * pPreScalar, uint16_t * pPeriod, uint32_t * pCnt)
{
    uint8_t preScalar   = 0;
    uint32_t preDivide  = 1;
    uint32_t cnt        = 1;

    ASSERT(tmr > 0);

    // if we are too slow to use just the timer, also add outside counts
    // set prescalar to 256
    if(tmr > (MAXTMRPRX * 256))
    {
        ASSERT(pCnt != NULL);

        tmr = (tmr + 128) / 256;
        cnt = tmr / MAXTMRPRX;

        if((tmr % MAXTMRPRX) > 0) cnt++;
        if(cnt == 0) cnt = 1;
        tmr = (tmr + cnt/2) / cnt;

        preScalar = 7;                  // divide by 256
        preDivide = 256;
    }

    else
    {
        for(preScalar=0; preScalar<9; preScalar++)
        {
            uint64_t tmrT = (tmr + (preDivide / 2)) / preDivide;

            if(tmrT <= MAXTMRPRX)     // we wil always substract 1 before putting in the PRx register
            {
                // we are done
                tmr = tmrT;
                break;
            }

            // go to next preScalar
            preDivide *= 2;
        }

        // there is no 8, but we are with a preDivide of 256
        if(preScalar == 8)
        {
            preScalar = 7;
        }

        // there is no preDivide of 128, so bump up to 256
        else if(preScalar == 7)
        {
            tmr = (tmr + 1) / 2;
            preDivide *= 2;
        }

        // error condition
        if(tmr == 0 || preScalar >= 9)
        {
            preDivide   = 0;        // cause return value to be zero
            tmr         = 1;        // we can not divide by zero
            preScalar   = 0;
        }
    }

    if(pPeriod != NULL)     *pPeriod    = ((uint16_t) tmr);
    if(pPreScalar != NULL)  *pPreScalar =  preScalar;
    if(pCnt != NULL)        *pCnt       = cnt;

    // actual tmr count
    return(tmr * preDivide * cnt);
}

uint64_t SPSPreScalarAndPeriod(uint64_t xsps, uint32_t scaleSPS, uint32_t const pbClk, uint16_t * pPreScalar, uint16_t * pPeriod, uint32_t * pCnt)
{
    uint64_t    pbXScale;
    uint64_t    tmr;

    ASSERT(scaleSPS > 0 && xsps > 0 && pbClk > 0);

    pbXScale = pbClk * ((uint64_t) scaleSPS);
    tmr = TmrPreScalarAndPeriod(((pbXScale + (xsps/2)) / xsps), pPreScalar, pPeriod, pCnt);

    if(tmr == 0) return(0);

    return((pbXScale + tmr/2) / tmr);
}

uint64_t DelayPreScalarAndPeriod(uint64_t delay, uint64_t scaleDelay, uint32_t const pbClk, uint16_t * pPreScalar, uint16_t * pPeriod, uint32_t * pCnt)
{
    uint64_t    half = (scaleDelay+1)/2;
    uint8_t     r[2*sizeof(uint64_t)];
    uint64_t    tmr;

    ASSERT(scaleDelay > 0 && pbClk > 0);
    if(delay == 0)
    {
        if(pPeriod != NULL)     *pPeriod    = 0;
        if(pPreScalar != NULL)  *pPreScalar = 0;
        if(pCnt != NULL)        *pCnt       = 0;
        return(0);
    }
 
    OSUMult((uint8_t *) &delay, sizeof(delay), (uint8_t *) &pbClk, sizeof(pbClk), r, sizeof(r));
    OSAdd(r, sizeof(r), (uint8_t *) &half, sizeof(half), r, sizeof(r));
    OSDivide((int8_t *) r, sizeof(r), scaleDelay, (int8_t *) &tmr, sizeof(tmr));

    tmr = TmrPreScalarAndPeriod(tmr, pPreScalar, pPeriod, pCnt);

    half = pbClk / 2;
    OSUMult((uint8_t *) &tmr, sizeof(tmr), (uint8_t *) &scaleDelay, sizeof(scaleDelay), r, sizeof(r));
    OSAdd(r, sizeof(r), (uint8_t *) &half, sizeof(half), r, sizeof(r));
    OSDivide((int8_t *) r, sizeof(r), pbClk, (int8_t *) &delay, sizeof(delay));

    return(delay);
}

#if 0
// returns actual sps scaled by scaleSPS
// xsps = SPS at the scaling factor
// scaleSPS SPS == 1, mSPS == 1000, uSPS == 1000000
// pbclk in ticks per second
// output prescalar for tmr
// output pPeriod for tmr
// output pCnt, how many times the tmr must roll
uint64_t CalculatePreScalarAndPeriod(uint64_t xsps, uint32_t scaleSPS, uint32_t const pbClk, uint16_t * pPreScalar, uint16_t * pPeriod, uint32_t * pCnt)
{
    uint64_t pbXScale   = pbClk * ((uint64_t) scaleSPS);
    uint64_t tmr        = (pbXScale + (xsps/2)) / xsps;
    uint8_t preScalar   = 0;
    uint32_t preDivide  = 1;
    uint32_t cnt        = 1;

    // if we are too slow to use just the timer, also add outside counts
    // set prescalar to 256
    if(xsps < (TMRMINSPS * scaleSPS))
    {
        ASSERT(pCnt != NULL);

        tmr = (tmr + 128) / 256;
        cnt = tmr / MAXTMRPRX;

        if((tmr % MAXTMRPRX) > 0) cnt++;
        if(cnt == 0) cnt = 1;
        tmr = (tmr + cnt/2) / cnt;

        preScalar = 7;                  // divide by 256
        preDivide = 256;
    }

    else
    {
        for(preScalar=0; preScalar<9; preScalar++)
        {
            uint64_t tmrT = (tmr + (preDivide / 2)) / preDivide;

            if(tmrT <= MAXTMRPRX)     // we wil always substract 1 before putting in the PRx register
            {
                // we are done
                tmr = tmrT;
                break;
            }

            // go to next preScalar
            preDivide *= 2;
        }

        // there is no 8, but we are with a preDivide of 256
        if(preScalar == 8)
        {
            preScalar = 7;
        }

        // there is no preDivide of 128, so bump up to 256
        else if(preScalar == 7)
        {
            tmr = (tmr + 1) / 2;
            preDivide *= 2;
        }

        // error condition
        if(tmr == 0 || preScalar == 9)
        {
            pbXScale    = 0;        // cause return value to be zero
            tmr         = 1;        // we can not divide by zero
            preScalar   = 0;
        }
    }

    if(pPeriod != NULL)     *pPeriod    = ((uint16_t) tmr);
    if(pPreScalar != NULL)  *pPreScalar =  preScalar;
    if(pCnt != NULL)        *pCnt       = cnt;

    // pbXScale == 100,000,000 * scale which 1,2,4,8,16,32,64,256 all divide evenly into
    // so we only need to round the cnt and tmr values
    return(((pbXScale / preDivide) + (cnt*tmr/2)) / (cnt*tmr));
}
#endif

