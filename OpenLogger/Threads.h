/************************************************************************/
/*                                                                      */
/*    Threads.h                                                         */
/*                                                                      */
/*    Header file for the Virtual Thread implementation                 */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/26/2018 (KeithV): Created                                       */
/************************************************************************/

/************************************************************************/
/************************************************************************/
/********************** Virtual Thread Stuff  ***************************/
/************************************************************************/
/************************************************************************/
#ifndef Threads_h
#define Threads_h

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus 
extern "C" {
#endif

#define CBFF    32
extern uint8_t const rgFF[CBFF];

typedef enum
{
    YYield,
    YThread1,
    YContinue
} YTYPE;

extern void __attribute__((weak)) InitThread(void);
extern void Thread1(void);
extern bool Yield(YTYPE yType);
extern uint8_t const * FindLowWaterMark(uint8_t const * pBottom, uint8_t const * const pLowWater);

#ifndef CORE_TMR_TICKS_PER_SEC
/************************************************************************/
/********************** Core Timer **************************************/
/************************************************************************/
#define CORE_TMR_TICKS_PER_SEC (F_CPU / 2ul)
#define CORE_TMR_TICKS_PER_MSEC (CORE_TMR_TICKS_PER_SEC / 1000ul)
#define CORE_TMR_TICKS_PER_USEC (CORE_TMR_TICKS_PER_SEC / 1000000ul)

static inline uint32_t ReadCoreTimer(void)
{
    uint32_t coreTimerCount;
    __asm__ __volatile__("mfc0 %0,$9" : "=r" (coreTimerCount));
    return(coreTimerCount);
}

// This is a Macro so if you stop in the debugger, it will stop at
// the location in code of the assert
//static inline void ASSERT(bool f) 
#define ASSERT(f)                           \
{                                           \
    if(!(f))                                \
    {                                       \
        LATHSET = 0b01111000;               \
        while(PORTHbits.RH0 == 0);          \
    }                                       \
}                       

#define NEVER_SHOULD_GET_HERE false
#endif

#ifdef __cplusplus 
}
#endif

#endif // Threads.h 
