/************************************************************************/
/*                                                                      */
/*    Threads.cpp                                                       */
/*                                                                      */
/*    Provides basic pseudo thread functionality                        */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent                                          */
/************************************************************************/
/*  Revision History:                                                   */
/*    6/25/2018(KeithV): Created                                        */
/************************************************************************/
#include "p32xxxx.h"
#include "Threads.h"
#include <string.h>
#include <LoopStats.h>

#define CBTHREADSTACK       8192
#define CU64 (CBTHREADSTACK/sizeof(uint64_t))

// stack must be 8 byte aligned!!!
// this is so uint64_t parameters will be aligned (used in  sprintf())
static uint64_t __attribute__((aligned(8))) mystack[CU64] = {[0 ... CU64-1] = 0xFFFFFFFFFFFFFFFFull};
uint8_t const rgFF[CBFF] = {[0 ... CBFF-1] = 0xFF};

// Stacks grow down, go to the end of the stack less a uint64_t
// by GCC spec, there must be at least 16 bytes at the starting SP address
// so set the starting SP 16 bytes from the end
static const uint32_t thread1SP = ((uint32_t) &mystack[CU64]) - 16;

uint8_t const * const   pBottomOfThreadStack  = (uint8_t *) mystack;
uint8_t const * const   pTopOfThreadStack     = ((uint8_t *) mystack) + sizeof(mystack);
uint8_t const *         pThreadStackLowWater  = (uint8_t const *) (((uint32_t) &mystack[CU64]) - 16);

// some frame pointers to save
static volatile uint32_t topFP   = 0;       // the top level frame pointer
static volatile uint32_t yieldFP = 0;       // the return after yield frame pointer
static volatile uint32_t yieldSP = 0;       // the return after yield frame pointer

static uint32_t cCurThreadYield = 0;

uint8_t const * FindLowWaterMark(uint8_t const * pBottom, uint8_t const * const pLowWater)
{
    int32_t cbRemaining = pLowWater - pBottom - sizeof(rgFF);
    uint8_t const * pNewLowWaterMark = pBottom + cbRemaining;
    uint8_t const * pHalfWay = pBottom;
    uint32_t i;

    // we blew the stack!
    if(pLowWater < pBottom) return(pBottom);

    if(cbRemaining > 0)
    {
        // see if the stack moved at all, if not return the current location.
        if(memcmp(pBottom+cbRemaining, rgFF, sizeof(rgFF)) == 0) return(pLowWater);

        // now try and find the new location. Do a binary search
        // 2^^17 == 131,072 so 17 tries will only be 17*32 = 544 byte compare, when is not very long
        // we can do the search without worry of taking too much time.
        do
        {
            pHalfWay = pBottom + (pNewLowWaterMark - pBottom) / 2;

            // we are above this point
            if(memcmp(pHalfWay, rgFF, sizeof(rgFF)) == 0) pBottom = pHalfWay;

            // other wise we are below
            else pNewLowWaterMark = pHalfWay;

        } while(pBottom + sizeof(rgFF) <= pNewLowWaterMark);

    }
   
    // look up for them bottom for the first non-zero
    // this could be with sizeof(rgFF) OR sizeof(rgFF) of the pNewLowWaterMark
    for(i=0; i<2*sizeof(rgFF) && pBottom[i] == 0xFF; i++);

    return(&pBottom[i]);
}

#pragma GCC diagnostic ignored "-Wvolatile-register-var"

// Yield functionality
// This is the function that does the magic of yielding between the main loop and thread 1
bool __attribute__((optimize("-O0"))) Yield(YTYPE yType)
{
    // The Frame pointer should be one of the first things saved
    // when entering the function. Because we call another function 
    // and because we have stack variables, the frame pointer should always
    // be present; even with optimizations.
    volatile register uint32_t regFP __asm__("$fp");
    volatile register uint32_t curSP __asm__("$sp");
    (void) curSP;       // suppress warning
    
    switch(yType)
    {
        case YThread1:

            // CAUTION!!!
            // We are depending that at the end of this function the SP is restored from the FP
            // In unoptimized code, the GCC compiler does this, but is not specifically speicified in the spec
            // if this changes our SP and FP will get out of sync in the Main Loop Code.

            // if we are yielding, change the FP and return it where we yielded
            if(yieldFP != 0) 
            {
                // change to the thread stack
                // this is very trick, we only have to do this because we might take in interrupt
                // and the interrupt is going to run on the current stack, and that might be the thread stack
                // we want to make sure it is below anything of value so the interrupt routine does not walk 
                // on our returning active stack.
                curSP = yieldSP;

                // regFP = yieldFP; will cause a compiler error, use the MIPS
                // instruction set directly to assign the FP register.
                // this will return us back to our thread code where we yielded
                asm volatile("move $30, %0" :  : "r" (yieldFP));
                yieldFP = 0;        // done with the yeild
            }
            
            // if we are not yielding, it must be a new call
            else
            {
                uint32_t volatile fpT = regFP;
                (void) fpT;
                
                ASSERT(topFP == 0);
                cCurThreadYield = 0;    // initialize for max and min thread yields
                
                // call the thread for the first time
                topFP = regFP;          // save away the top frame pointer for yielding the thread

                // change to the thread stack
                curSP = thread1SP;

                Thread1();              // call the thread code

                topFP = 0;              // done, can't return here

                // we will only return from Thread1(), when it completes
                // at this point we are back to normally executing code

                // deal with the average counts
                RecordThreadCnt(cCurThreadYield);
            }
            break;
            
        case YYield:
            
            // only if we can yield
            if(topFP != 0)
            {
                cCurThreadYield++;
                yieldFP = regFP;        // save away the thread frame pointer so we can return to the thread
                yieldSP = curSP;        // this is so we can restore it and not trash the thread stack.

                // regFP = topFP; will cause a compiler error, use the MIPS
                // instruction set directly to assign the FP register.
                // this will return us back to the ISR to return to the main loop
                asm volatile("move $30, %0" :  : "r" (topFP));
            }
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }
    
    return(yieldFP != 0);
}

#pragma GCC diagnostic pop


/************************************************************************/
/* The following is for documentation purposes only                     */
/*    This shows how to just to an ISR if we need to save registers     */
/************************************************************************/
#if PREEMPTIVE       // Preemptive implementation, use an ISR to save the register set
volatile bool fSpin =  true;

// Yield functionality
// This is the function that does the magic of yielding between the main loop and thread 1
// compile unoptimized so no surprises come in with optimizations.
bool __attribute__((optimize("-O0"))) Yield(YTYPE yType)
{
    // The Frame pointer should be one of the first things saved
    // when entering the function. Because we call another function 
    // and because we have stack variables, the frame pointer should always
    // be present; even with optimizations.
    volatile register uint32_t regFP __asm__("$fp");
   
    switch(yType)
    {
        case YContinue:
            // if the thread is running, return to the thread
            if(fYielding) 
            {
                // regFP = yieldFP; will cause a compiler error, use the MIPS
                // instruction set directly to assign the FP register.
                // this will return us back to our thread code where we yielded
                asm volatile("move $30, %0" :  : "r" (yieldFP));
                break; 
            }
            
            // if the thread is not running, enter the thread code
            isrFP = regFP;          // save away the ISR frame pointer for yielding the thread
            fYielding = true;       // we are in the thread
            cCurThreadYield = 0;    // initialize for max and min thread yields
            Thread1();              // call the thread code

            // deal with the average counts
            if(cCurThreadYield < cMinThreadYield) cMinThreadYield = cCurThreadYield;
            cNBRThreadCalls++;
            sumThreadYield += cCurThreadYield;
            cAveThreadYield = sumThreadYield / cNBRThreadCalls;
            if(cNBRThreadCalls >= cNBRAveThread)
            {
                cNBRThreadCalls--;
                sumThreadYield -= cAveThreadYield;
            }
            if(cCurThreadYield > cMaxThreadYield) cMaxThreadYield = cCurThreadYield;

            fYielding = false;      // we are done with the thread
            fSpin = false;  // get us out of the while(fSpin) on return from the ISR
            break;
            
        case YYield:
            cCurThreadYield++;
            yieldFP = regFP;        // save away the thread frame pointer so we can return to the thread
            // regFP = isrFP; will cause a compiler error, use the MIPS
            // instruction set directly to assign the FP register.
            // this will return us back to the ISR to return to the main loop
            asm volatile("move $30, %0" :  : "r" (isrFP));
            fSpin = false;  // get us out of the while(fSpin) on return from the ISR
            break;
            
        case YThread1:
            fSpin = true;       // so we will spin until the ISR is done
            IFS0bits.IC1IF = 1; // raise the interrupt, call the ISR; do a context switch
            while(fSpin);       // spin until the ISR is entered; don't move the IP
            break;
            
        default:
            break;
    }
    
    return(fYielding);
}

// Thread 1 ISR
void __attribute__((optimize("-O0"), nomips16, at_vector(_INPUT_CAPTURE_1_VECTOR),interrupt(IPL1SRS))) ISRThread1(void)
{    
    volatile register uint32_t curSP __asm__("$sp");
    (void) curSP;       // suppress warning
    
    // change to my stack
    curSP = isrSP;
    
    IFS0CLR = _IFS0_IC1IF_MASK;
    
    // very important to understand, I do not need to save and restore the 
    // original SP because I did not change the FP here.
    // when Yield() is called, the current FP (which points to the Main Stack)
    // is stored on my thread stack. When Yield() returns to Main FP is restored
    // and then SP is set from the Main FP, so both are on the Main Stack
    // by the time we return from the ISR
    
    // IT IS VERY IMPORTATNT that NEITHER the ISR or Yield() get optimized!
    
    Yield(YContinue);

    return;
}

// Make sure the thread has the lowest priority so that 
// anything will interrupt it except of course, the CPU
void InitThread(void)
{
    // thread ISR
    IPC1bits.IC1IP = 1;     // lowest priority.
    IPC1bits.IC1IS = 0;
    IFS0bits.IC1IF = 0;
    IEC0bits.IC1IE = 1;
};

#endif

   



