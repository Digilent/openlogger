/************************************************************************/
/*                                                                      */
/*    PinAssignments.h                                                  */
/*                                                                      */
/*    Header file for OpenLogger pins peripherals                       */
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
#ifndef PinAssignments_h
#define PinAssignments_h

#include "p32xxxx.h"
#include <inttypes.h>

#define SYSUNLOCK           \
{	                        \
    SYSKEY = 0x00000000;    \
    SYSKEY = 0xAA996655;    \
    SYSKEY = 0x556699AA;    \
}                           

#define SYSLOCK             \
{                           \
    SYSKEY = 0x33333333;    \
}                           

//#define hash #
#define RESOLVE(x) x
//#define CORS(a) CLR

// concatinate macros
#define CATAB2(a,b) a##b
#define CATAB(a,b) CATAB2(a,b)

#define CATABC2(a,b,c) a##b##c
#define CATABC(a,b,c) CATABC2(a,b,c)

#define CATABCD2(a,b,c,d) a##b##c##d
#define CATABCD(a,b,c,d) CATABCD2(a,b,c,e)

#define CATABCDE2(a,b,c,d,e) a##b##c##d##e
#define CATABCDE(a,b,c,d,e) CATABCDE2(a,b,c,d,e)

// gpio macros
#define SetGPIO(a,c)  SetGPIO2(a,c)
#define SetGPIO2(LorP,reg,mask,val) {if(val==0) LorP##reg &= ~(mask); else LorP##reg |= mask;}

#define GetGPIO(a)  GetGPIO2(a)
#define GetGPIO2(LorP,reg,mask) ((LorP##reg & mask) != 0)

// For use to set the TRIS value for the PIN
#define GPIO(a,b)  GPIO2(a,b)
#define GPIO2(LorP,reg,mask,CorS) LorP##reg##CorS = mask

// For use to set the TRIS value for the PIN
#define INITRIS(a,b)  INITRIS2(a,b)
#define INITRIS2(LorP,reg,mask,CorS) TRIS##reg##CorS = mask

// For use to set the ANSEL value for the PIN
#define INITANSEL(a,b)  INITANSEL2(a,b)
#define INITANSEL2(LorP,reg,mask,CorS) ANSEL##reg##CorS = mask

// For use to set the CNPUx pull up resistors for the PIN
#define INITCNPU(a,b)  INITCNPU2(a,b)
#define INITCNPU2(LorP,reg,mask,CorS) CNPU##reg##CorS = mask

// For use to set the CNPDx pull down resistors for the PIN
#define INITCNPD(a,b)  INITCNPD2(a,b)
#define INITCNPD2(LorP,reg,mask,CorS) CNPD##reg##CorS = mask

// virtual address mappings
#define KVA_2_PA(v)             (((uint32_t) (v)) & 0x1fffffff)
#define KVA_2_KSEG0(v)          ((void *) (KVA_2_PA(v) | 0x80000000))
#define KVA_2_KSEG1(v)          ((void *) (KVA_2_PA(v) | 0xA0000000))
#define KVA_2_KSEG2(v)          ((void *) (KVA_2_PA(v) | 0xC0000000))
#define KVA_2_KSEG3(v)          ((void *) (KVA_2_PA(v) | 0xE0000000))
#define IS_KVA_FLASH(v)         ((((uint32_t) (v)) & 0x10000000) != 0)    

// #define DDR __attribute__((section(".ddr")))
// #define DDR __attribute__((section(".ddr"), persistent))
// #define DDR __attribute__((section(".ddr"), space(prog), noload))
// we must put NOLOAD in the linker script. space(prog) will put the ddr
// section as code, so we don't initialize it, then the noload says
// not to load it, or put it in the .hex file 
// Net result an uninitialize data section.
#define DDR __attribute__((section(".ddr"), space(prog)))
#define PHY __attribute__((aligned(16)))
#define ISR(vec, ipl) __attribute__((nomips16, at_vector(vec),interrupt(ipl)))

// we can not go from PHY to CACHE because as we write to the cache
// it is possible the line is in use by a later address, but yet still in our copy
// and the later address will write back the cache to the phy, overwritting the phy
// before we read the phy and put back in the cache, so we will just write into the 
// cache the original cached value, and not the phy value; thus corrupting data.
//#define PHY_2_CACHE(_ptr, _size) {if(!IS_KVA_FLASH(_ptr)) memcpy(KVA_2_KSEG0(_ptr), KVA_2_KSEG1(_ptr), _size);}

// this will work because if a later write back occurs, it will just do what we are doing.
#define CACHE_2_PHY(_ptr, _size) {if(!IS_KVA_FLASH(_ptr)) memcpy(KVA_2_KSEG1(_ptr), KVA_2_KSEG0(_ptr), _size);}  

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

// string macros
#define MKSTR2(a) #a
#define MKSTR(a) MKSTR2(a)

// min, max
#ifndef max
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif

/************************************************************************/
/********************** Core Timer **************************************/
/************************************************************************/
#define CORE_TMR_TICKS_PER_SEC (F_CPU / 2ul)
#define CORE_TMR_TICKS_PER_MSEC (CORE_TMR_TICKS_PER_SEC / 1000ul)
#define CORE_TMR_TICKS_PER_USEC (CORE_TMR_TICKS_PER_SEC / 1000000ul)
#define CORE_TMR_TICKS_PER_10_NSEC (2)

static inline uint32_t ReadCoreTimer(void)
{
    uint32_t coreTimerCount;
    __asm__ __volatile__("mfc0 %0,$9" : "=r" (coreTimerCount));
    return(coreTimerCount);
}

/************************************************************************/
/********************** Interrupts  **************************************/
/************************************************************************/
static inline uint32_t __attribute__((nomips16)) OSEnableInterrupts(void)
{
    uint32_t status = 0;
    asm volatile("ei    %0" : "=r"(status));
    return status;
}

static inline uint32_t __attribute__((nomips16)) OSDisableInterrupts(void)
{
    uint32_t status = 0;
    asm volatile("di    %0" : "=r"(status));
    return status;
}

static inline void __attribute__((nomips16))  OSRestoreInterrupts(uint32_t st)
{
    if (st & 0x00000001) asm volatile("ei");
    else asm volatile("di");
}

/************************************************************************/
/********************** OFF Vector Macros *******************************/
/************************************************************************/
// we should really be taking isr - ebase, but ebase is 0x9D000000, 
// so just mask to the lower 18 bits, which is all the OFFxxx reg will pay attention to
#define MakeVector(isr) (((uint32_t) (isr)) - _ebase_address)    
#define SetVector(vec, isr)  (&OFF000)[vec] = MakeVector(isr)  

/************************************************************************/
/**************** Top of the stack is the top of SRAM *******************/
/************************************************************************/
#define pTopOfStack ((uint8_t const * const) 0x800A0000)

/************************************************************************/
/****************** Internal ADC Feedback channels **********************/
/************************************************************************/
#define	CH_NULL_FB          0	
#define	CH_DC1_FB           45	
#define	CH_DC2_FB           1	
#define	CH_AWG1_FB          47	
#define	CH_VREF1V5_FB       48	
#define	CH_VREF1V8_FB       2	
#define	CH_VREF3V0_FB       49
#define	CH_VCC3V3_FB        5	
#define	CH_USB5V0_FB        6	
#define	CH_VSS5V0_FB        7

#define	FB_PIN_DC1           PORT,   B,  (1 << 5)
#define	FB_PIN_DC2           PORT,   B,  (1 << 2)
#define	FB_PIN_AWG1          PORT,   B,  (1 << 9)
#define	FB_PIN_VREF1V5       PORT,   B,  (1 << 13)	
#define	FB_PIN_VREF1V8       PORT,   B,  (1 << 4)	
#define	FB_PIN_VREF3V0       PORT,   B,  (1 << 11)
#define	FB_PIN_VCC3V3        PORT,   B,  (1 << 10)	
#define	FB_PIN_USB5V0        PORT,   B,  (1 << 12)	
#define	FB_PIN_VSS5V0        PORT,   A,  (1 << 5)

/************************************************************************/
/************************ LEDs and BTNs *********************************/
/************************************************************************/

// some gpio pins
#define LED_PIN_1               LAT,    H,  (1 << 3)    // RH3
#define LED_PIN_2               LAT,    H,  (1 << 4)    // RH4
#define LED_PIN_3               LAT,    H,  (1 << 5)    // RH5
#define LED_PIN_4               LAT,    H,  (1 << 6)    // RH6

#define LED_RED                 LED_PIN_1      
#define LED_ORANGE              LED_PIN_2      
#define LED_GREEN               LED_PIN_3      
#define LED_BLUE                LED_PIN_4      

#define BTN_PIN_1               PORT,   H,  (1 << 0)    // RH0

#define ENC_PIN_DIR             PORT,   F,  (1 << 12)   // RF12

#define GPIO_0                  PORT,   F,  (1 << 0)    // RF0
#define GPIO_1                  PORT,   F,  (1 << 1)    // RF1
#define GPIO_2                  PORT,   F,  (1 << 2)    // RF2
#define GPIO_3                  PORT,   F,  (1 << 3)    // RF3
#define GPIO_4                  PORT,   F,  (1 << 4)    // RF4
#define GPIO_5                  PORT,   F,  (1 << 5)    // RF5
#define GPIO_6                  PORT,   F,  (1 << 8)    // RF8
#define GPIO_7                  PORT,   F,  (1 << 13)   // RF13

/************************************************************************/
/************************** DMA ASSIGNMENTS *****************************/
/************************************************************************/
#define SHARED_DMA              1
#define SHARED_DMA_IE_IF        4   // The IECx IFSx flag for the DMA interrupt
#define SHARED_DMA_IPC          33  // IPCx reg for the DMA interrupt

// Serial IO
#define SERIAL_RX_DMA           0
#define SERIAL_TX_DMA           SHARED_DMA
#define SERIAL_RX_ISR_PRI       4                   // ISR Priority
#define SERIAL_DMAPRI           0                   // run the DMAs at the lowest DMA priority
// #define SERIAL_TX_DMA_PRI       4                   // ISR Priority

// FIFO 245 IO
#define FIFO245_DMA             SHARED_DMA
#define FIFO245DMAPRI           0                   // DMA priority withing the DMA controller
#define FIFO245ISRDMAPRI        2                   // Priority to run the FIFO DMA ISR

// AWG
#define AWG1_DMA                2
#define AWG1_DMAPRI             3       // AWG is the only one with a highest pri

// ADC
#define ADC_REQ_DATA_DMA            3
#define ADC_READ_DATA_DMA           4
#define ADC_REQ_READ_DMAPRI         2

#define ADC_SAMP_DATA_DMA1          5 
#define ADC_SAMP_DATA_DMA2          6      
#define ADC_SAMP_DATA_DMAPRI        1       // DMA priority withing the DMA controller

#define ADC_SAMP_DATA_DMA_ISRPRI    1       // Priority to run the DMA ISR, we have 64ms to get to this; the half filled toggle ISR
#define ADC_SAMP_DATA_DMA1_IE_IF    4       // The IECx IFSx flag for the DMA
#define ADC_SAMP_DATA_DMA2_IE_IF    4       // The IECx IFSx flag for the DMA
#define ADC_SAMP_DATA_DMA1_IPC      34      // The IPCx reg for the DMA 
#define ADC_SAMP_DATA_DMA2_IPC      35      // The IPCx reg for the DMA 

// NOTE: we are not using any DMA buffers for the WiFi, we just do the SPI
// WIFI
//#define WIFI_SPI_RECV_DMA       6                       // the SPI to use to receive data
//#define WIFI_SPI_SEND_DMA       7                       // the SPI to use to send data

// WIFI
//#define WIFI_SPI_RECV_DMA       7                       // the SPI to use to receive data
//#define WIFI_SPI_RECV_DMAPRI    0                       // communication is the lowest

/************************************************************************/
/************************** Shared DMA MACROs *****************************/
/************************************************************************/
// DMA in use 
#define IsDMAInUse(dma)     ((dma).DCHxCON.CHEN == 1)
#define irqDMANone          0           // prohibits you from using the coretime as a trigger
#define SHAREDDMA          (*((DMACH *) &CATABC(DCH,SHARED_DMA,CON)))

/************************************************************************/
/************************** Serial IO  **********************************/
/************************************************************************/
// Serial UART I/O
#define SERIAL_UART             2

#define SERIAL_PPS_URX          U2RXR   = 0b1101        // RX2 RE9
#define SERIAL_PPS_UTX          RPC2R   = 0b0010        // TX2 RC2

/************************************************************************/
/************************** Serial IO Macros ****************************/
/************************************************************************/
#define SERIAL_OUTPUT_DMA       CATABC(DCH,SERIAL_TX_DMA,CON)  // DCHxCON

/************************************************************************/
/************************** FIFO 245  ***********************************/
/************************************************************************/
#define FIFO245_PIN_RXF         PORT,   B,  (1 << 1)    // RB1
#define FIFO245_PIN_TXE         PORT,   G,  (1 << 15)   // RG15
#define FIFO245_PIN_CMPP        PORT,   B,  (1 << 15)   // CMP+ B15
#define FIFO245_PIN_CMPM        PORT,   G,  (1 << 9)    // CMP1 G9

#define FIFO245_RXF_INT         4                   // FTDI RXF# is on INT4
#define FIFO245_PPS_RXF_INT     INT4R = 0b0101      // RB1 to INT4
#define FIFO245_DMA_IE_IF       SHARED_DMA_IE_IF    // The IECx IFSx flag for the FIFO DMA
#define FIFO245_DMA_IPC         SHARED_DMA_IPC      // The IPCx reg for the FIFO DMA 

/************************************************************************/
/************************** FIFO 245 MACROs *****************************/
/************************************************************************/
#define FIFO245DMA          (*((DMACH *) &CATABC(DCH,FIFO245_DMA,CON)))
#define FIFO245DMAIE        CATABCDE(IEC,FIFO245_DMA_IE_IF, bits.DMA,FIFO245_DMA,IE)
#define FIFO245DMAIF        CATABCDE(IFS,FIFO245_DMA_IE_IF, bits.DMA,FIFO245_DMA,IF)
#define FIFO245DMAIPCP      CATABCDE(IPC,FIFO245_DMA_IPC, bits.DMA,FIFO245_DMA,IP)
#define FIFO245DMAIPCS      CATABCDE(IPC,FIFO245_DMA_IPC, bits.DMA,FIFO245_DMA,IS)
#define FIFO245RXFINT       CATABC(_EXTERNAL_,FIFO245_RXF_INT,_VECTOR)
#define FIFO245DMAVECTOR    CATABC(_DMA,FIFO245_DMA,_VECTOR)
#define FIFO245IPLxSRS      CATABC(IPL,FIFO245ISRDMAPRI,SRS)
/************************************************************************/
/************************** DC Outputs **********************************/
/************************************************************************/
// DC output Timer to drive PWMs
#define DCOUT_TMR       2   // T2
#define DCOUT_OCTSEL    0   // T2 selection for OC4&5 clocks

// DCOUT PWM/OC mapping
#define DC1_PWM         4
#define DC1_PPS_OC      RPB3R = 0b1011  // map OC4 to RB3

#define DC2_PWM         5
#define DC2_PPS_OC      RPB8R = 0b1011  // map OC5 to RB8

/************************************************************************/
/************************** DC Outputs MACROS ***************************/
/************************************************************************/
#define DCTMR       (*((TMRCH *) &(CATABC(T,DCOUT_TMR,CON))))
#define DC1_OC      (*((OCCH *) &(CATABC(OC,DC1_PWM,CON))))
#define DC2_OC      (*((OCCH *) &(CATABC(OC,DC2_PWM,CON))))

/************************************************************************/
/********************************** AWG *********************************/
/************************************************************************/

// AWG offset timer -- same as DC TMRs
// #define AWG_TMR_OFF     6               // T6, AWG Offset PWM

// AWG 1
#define AWG1_TRG_TMR    8               // T8, for DMA trigger
#define AWG1_LAT        J
#define AWG1_OFF        6               // Offset PWM
#define AWG_OFF_OCTSEL  0               // T2 selection for OC6 offset clocks
#define AWG1_PPS_OC     RPD14R = 0b1100 // map OC6 to RD14

/************************************************************************/
/**************************** AWG MACROs ********************************/
/************************************************************************/
// #define AWGTMROFF       (*((TMRCH *) &(CATABC(T,AWG_TMR_OFF,CON))))
#define AWG1OFF         (*((OCCH *) &(CATABC(OC,AWG1_OFF,CON))))
#define AWG1LAT         CATAB(LAT,AWG1_LAT)
#define AWG1TRIS        CATAB(TRIS,AWG1_LAT)

#define AWG1TRGTMR      (*((TMRCH *) &(CATABC(T,AWG1_TRG_TMR,CON))))
#define AWG1TRGVECTOR   CATABC(_TIMER_,AWG1_TRG_TMR,_VECTOR)
#define AWG1DMA         (*((DMACH *) &CATABC(DCH,AWG1_DMA,CON)))

#define DACDATAJ(a) ((uint16_t) (~((((a) & 0b0000011111) << 3) | (((a) & 0b0001000000) << 4) | (((a) & 0b1110000000) << 6) | (((a) & 0b0000100000) << 7))))
#define DACDATA1(a) DACDATAJ(a)

/************************************************************************/
/********************************** SD **********************************/
/************************************************************************/
#define SD_SPI                  6
#define SD_SPI_PRI              3
#define SD_PIN_CS               LAT,    D,  (1 << 6)    // RD6
#define SD_PIN_DET              PORT,   A,  (1 << 0)    // RA0

#define SD_PPS_MOSI            RPD4R   = 0b1010        // MOSI/SDO6,    RD4
#define SD_PPS_MISO            SDI6R   = 0b0110        // MISO/SDI6,    RD5

/************************************************************************/
/***************************** External ADC *****************************/
/************************************************************************/
#define ADC_SPI                 2
#define ADC_INT                 3

#define ADC_SAMP_TMR            5
#define ADC_SAMP_TMR_ISRPRI     7       // Priority to run the SAMPLE TMR ISR, this trigger DMA
#define ADC_SAMP_TMR_IE_IF      0       // The IECx IFSx flag for the SAMPLE TMR
#define ADC_SAMP_TMR_IPC        6       // The IPCx reg for the SAMPLE TMR 


#define ADC_PIN_CS              LAT,    D,  (1 << 0)    // RD0
#define ADC_PIN_MOSI            LAT,    G,  (1 << 8)    // RG8
#define ADC_PIN_MISO            PORT,   G,  (1 << 7)    // RG7
#define ADC_PIN_SCK             PORT,   G,  (1 << 6)    // RG6

#define ADC_PPS_SS              RPD0R   = 0b0110        // SS2,          RD0
#define ADC_PPS_MOSI            RPG8R   = 0b0110        // MOSI/SDO2,    RG8
#define ADC_PPS_MISO            SDI2R   = 0b0001        // MISO/SDI2,    RG7

#define ADC_PIN_INT             PORT,   G,  (1 << 1)    // RG1
#define ADC_PPS_INT             INT3R = 0b1100          // IN3 to RG1

#define ADC_PIN_RESET           LAT,    A,  (1 << 2)    // RA2
#define ADC_PIN_CONVST          LAT,    A,  (1 << 3)    // RA3

/************************************************************************/
/************************* External ADC Macros **************************/
/************************************************************************/
#define ADCSPI          (*((SPICH *) &CATABC(SPI,ADC_SPI,CON)))
#define ADCSPIRXVECTOR  CATABC(_SPI,ADC_SPI,_RX_VECTOR)
#define ADCINTVECTOR    CATABC(_EXTERNAL_,ADC_INT,_VECTOR)
#define ADCINTEP3(a)    INT##a##EP          // "INT" resolves to int32_t, so we have to hard code it
#define ADCINTEP2(a)    ADCINTEP3(a)
#define ADCINTEP        ADCINTEP2(ADC_INT)

#define ADCSAMPTMR          (*((TMRCH *) &(CATABC(T,ADC_SAMP_TMR,CON))))
#define ADCSAMPTMRIE        CATABCDE(IEC,ADC_SAMP_TMR_IE_IF, bits.T,ADC_SAMP_TMR,IE)
#define ADCSAMPTMRIF        CATABCDE(IFS,ADC_SAMP_TMR_IE_IF, bits.T,ADC_SAMP_TMR,IF)
#define ADCSAMPTMRIPCP      CATABCDE(IPC,ADC_SAMP_TMR_IPC, bits.T,ADC_SAMP_TMR,IP)
#define ADCSAMPTMRIPCS      CATABCDE(IPC,ADC_SAMP_TMR_IPC, bits.T,ADC_SAMP_TMR,IS)
#define ADCSAMPTMRVECTOR    CATABC(_TIMER_,ADC_SAMP_TMR,_VECTOR)                // when sampling from the timer
#define ADCSAMPTMRIPLxSRS   CATABC(IPL,ADC_SAMP_TMR_ISRPRI,SRS)

// #define ADCREADVECTOR   CATABC(_DMA,ADC_READ_DATA_DMA,_VECTOR)              // when sampling from the read complete (calibration)
//#define ADCSAMP2VECTOR  CATABC(_DMA,ADC_SAMP_DATA_DMA1,_VECTOR)              // The 2 DMA vector for reading samples

#define ADCREQDMA       (*((DMACH *) &CATABC(DCH,ADC_REQ_DATA_DMA,CON)))
#define ADCREADDMA      (*((DMACH *) &CATABC(DCH,ADC_READ_DATA_DMA,CON)))

#define ADCSAMPDMA1         (*((DMACH *) &CATABC(DCH,ADC_SAMP_DATA_DMA1,CON)))
#define ADCSAMPDMA1IE       CATABCDE(IEC,ADC_SAMP_DATA_DMA1_IE_IF, bits.DMA,ADC_SAMP_DATA_DMA1,IE)
#define ADCSAMPDMA1IF       CATABCDE(IFS,ADC_SAMP_DATA_DMA1_IE_IF, bits.DMA,ADC_SAMP_DATA_DMA1,IF)
#define ADCSAMPDMA1IPCP     CATABCDE(IPC,ADC_SAMP_DATA_DMA1_IPC, bits.DMA,ADC_SAMP_DATA_DMA1,IP)
#define ADCSAMPDMA1IPCS     CATABCDE(IPC,ADC_SAMP_DATA_DMA1_IPC, bits.DMA,ADC_SAMP_DATA_DMA1,IS)
#define ADCSAMPDMA1VECTOR   CATABC(_DMA,ADC_SAMP_DATA_DMA1,_VECTOR)              
#define ADCSAMPDMA1IPLxSRS  CATABC(IPL,ADC_SAMP_DATA_DMA_ISRPRI,SRS)

#define ADCSAMPDMA2         (*((DMACH *) &CATABC(DCH,ADC_SAMP_DATA_DMA2,CON)))
#define ADCSAMPDMA2IE       CATABCDE(IEC,ADC_SAMP_DATA_DMA2_IE_IF, bits.DMA,ADC_SAMP_DATA_DMA2,IE)
#define ADCSAMPDMA2IF       CATABCDE(IFS,ADC_SAMP_DATA_DMA2_IE_IF, bits.DMA,ADC_SAMP_DATA_DMA2,IF)
#define ADCSAMPDMA2IPCP     CATABCDE(IPC,ADC_SAMP_DATA_DMA2_IPC, bits.DMA,ADC_SAMP_DATA_DMA2,IP)
#define ADCSAMPDMA2IPCS     CATABCDE(IPC,ADC_SAMP_DATA_DMA2_IPC, bits.DMA,ADC_SAMP_DATA_DMA2,IS)
#define ADCSAMPDMA2VECTOR   CATABC(_DMA,ADC_SAMP_DATA_DMA2,_VECTOR)              
#define ADCSAMPDMA2IPLxSRS  CATABC(IPL,ADC_SAMP_DATA_DMA_ISRPRI,SRS)

/************************************************************************/
/************************** WIFI RADIO **********************************/
/************************************************************************/

#define WIFI_PIN_RST            LAT,    H,  (1 << 8)    // RH8
#define WIFI_PIN_CE             LAT,    H,  (1 << 7)    // RH7
#define WIFI_PIN_WAKE           LAT,    H,  (1 << 9)    // RH9

// #define WIFI_INT                2                       // WiFi Radio Interrupt
#define WIFI_PIN_INT            PORT,   D,  (1 << 9)    // RD9
// #define WIFI_PPS_INT            INT2R   = 0b0000        // PPS Mapping of INT2 to RD9, don't use interrupts
// #define WIFI_INT_IE_IF          0                       // The IECx IFSx flag for the interrupt
// #define WIFI_INT_PRI            1                       // WiFi Radio Interrupt priority
// #define WIFI_INT_IPC            3                       // The IPCx for the Interrupt priority

#define WIFI_SPI                4
#define WIFI_PIN_SCK            LAT,    D,  (1 << 10)   // RD10
#define WIFI_PIN_CS             LAT,    D,  (1 << 12)   // RD12
#define WIFI_PIN_MOSI           LAT,    D,  (1 << 7)    // RD7
#define WIFI_PIN_MISO           PORT,   D,  (1 << 11)   // RD11
#define WIFI_PPS_SS             RPD12R  = 0b1000        // PPS mapping SS to RD12
#define WIFI_PPS_MOSI           RPD7R   = 0b1000        // PPS mapping MOSI/SDO to RD7
#define WIFI_PPS_MISO           SDI4R   = 0b0011        // PPS mapping MISO/SDI to RD11
#define WIFI_SPI_IE_IF          5                       // The IECx IFSx flag for the interrupt; we don't need PRI or IPC because there is no ISR

#define WIFI_UART               4
#define WIFI_PIN_MTSR           LAT,    B,  (1 << 0)    // RB0
#define WIFI_PIN_MRST           PORT,   E,  (1 << 8)    // RE8
#define WIFI_PPS_URX            U4RXR   = 0b1101        // RX4 RE8
#define WIFI_PPS_UTX            RPB0R   = 0b0010        // TX4 RB0
#define WIFI_DBG_UART_RX_DMA    AWG1_DMA
#define WIFI_DBG_UART_ISR_PRI   4

/************************************************************************/
/************************** WiFi Macros *********************************/
/************************************************************************/
// #define WIFISNDDMA          (*((DMACH *) &CATABC(DCH,WIFI_SPI_SEND_DMA,CON)))
// #define WIFIRCVDMA          (*((DMACH *) &CATABC(DCH,WIFI_SPI_RECV_DMA,CON)))

// #define WIFIINTCON          CATABC(INTCONbits.INT,WIFI_INT,EP)
// #define WIFIIE              CATABCDE(IEC,WIFI_INT_IE_IF, bits.INT,WIFI_INT,IE)
// #define WIFIIF              CATABCDE(IFS,WIFI_INT_IE_IF, bits.INT,WIFI_INT,IF)
// #define WIFIIPCP            CATABCDE(IPC,WIFI_INT_IPC, bits.INT,WIFI_INT,IP)
// #define WIFIIPCS            CATABCDE(IPC,WIFI_INT_IPC, bits.INT,WIFI_INT,IS)

#define WIFISPI          (*((SPICH *) &CATABC(SPI,WIFI_SPI,CON)))

// #define WIFISPITXVECTOR     CATABCDE(_SPI,WIFI_SPI,_,TX,_VECTOR)
// #define WIFISPIRXVECTOR     CATABCDE(_SPI,WIFI_SPI,_,RX,_VECTOR)

// #define WIFISPIEIE          CATABCDE(IEC,WIFI_SPI_IE_IF,bits.SPI,WIFI_SPI,EIE)
// #define WIFISPIEIF          CATABCDE(IFS,WIFI_SPI_IE_IF,bits.SPI,WIFI_SPI,EIF)
// #define WIFISPIRXIE         CATABCDE(IEC,WIFI_SPI_IE_IF,bits.SPI,WIFI_SPI,RXIE)
// #define WIFISPIRXIF         CATABCDE(IFS,WIFI_SPI_IE_IF,bits.SPI,WIFI_SPI,RXIF)
// #define WIFISPITXIE         CATABCDE(IEC,WIFI_SPI_IE_IF,bits.SPI,WIFI_SPI,TXIE)
// #define WIFISPITXIF         CATABCDE(IFS,WIFI_SPI_IE_IF,bits.SPI,WIFI_SPI,TXIF)

/************************************************************************/
/************************** peripheral registers ************************/
/************************************************************************/

#if 0
typedef struct _RCSIReg
{
    uint32_t    reg;
    uint32_t    clr;
    uint32_t    set;
    uint32_t    inv;
} __attribute__((packed, aligned(4))) RCSIReg;

typedef struct _PORTCH
{
    uint32_t ANSEL;
    uint32_t ANSELCLR;
    uint32_t ANSELSET;
    uint32_t ANSELINV;
    uint32_t TRIS;
    uint32_t TRISCLR;
    uint32_t TRISSET;
    uint32_t TRISINV;
    uint32_t PORT;
    uint32_t PORTCLR;
    uint32_t PORTSET;
    uint32_t PORTINV;
    uint32_t LAT;
    uint32_t LATCLR;
    uint32_t LATSET;
    uint32_t LATINV;
    uint32_t ODC;
    uint32_t ODCCLR;
    uint32_t ODCSET;
    uint32_t ODCINV;
    uint32_t CNPU;
    uint32_t CNPUCLR;
    uint32_t CNPUSET;
    uint32_t CNPUINV;
    uint32_t CNPD;
    uint32_t CNPDCLR;
    uint32_t CNPDSET;
    uint32_t CNPDINV;
    uint32_t CNCON;
    uint32_t CNCONCLR;
    uint32_t CNCONSET;
    uint32_t CNCONINV;
    uint32_t CNEN;
    uint32_t CNENCLR;
    uint32_t CNENSET;
    uint32_t CNENINV;
    uint32_t CNSTAT;
    uint32_t CNSTATCLR;
    uint32_t CNSTATSET;
    uint32_t CNSTATINV;
    uint32_t CNNE;
    uint32_t CNNECLR;
    uint32_t CNNESET;
    uint32_t CNNEINV;
    uint32_t CNF;
    uint32_t CNFCLR;
    uint32_t CNFSET;
    uint32_t CNFINV;
    uint32_t SRCON0;
    uint32_t SRCON0CLR;
    uint32_t SRCON0SET;
    uint32_t SRCON0INV;
    uint32_t SRCON1;
    uint32_t SRCON1CLR;
    uint32_t SRCON1SET;
    uint32_t SRCON1INV;
} __attribute__((packed, aligned(4))) PORTCH;
#endif 

typedef struct _TMRCH
{
    union
    {
        volatile __T2CONbits_t      TxCON;  // careful, T1CON is type A timer, and we are type B timers
        volatile uint32_t           TxCONu32;
    };
    volatile uint32_t        TxCONClr;
    volatile uint32_t        TxCONSet;
    volatile uint32_t        TxCONInv;
    volatile uint32_t        TMRx;
    volatile uint32_t        TMRxClr;
    volatile uint32_t        TMRxSet;
    volatile uint32_t        TMRxInv;
    volatile uint32_t        PRx;
    volatile uint32_t        PRxClr;
    volatile uint32_t        PRxSet;
    volatile uint32_t        PRxInv;
} __attribute__((packed, aligned(4))) TMRCH;

typedef struct _OCCH
{
    union
    {
        volatile __OC1CONbits_t     OCxCON;
        volatile uint32_t           OCxCONu32;
    };
    volatile uint32_t        OCxCONClr;
    volatile uint32_t        OCxCONSet;
    volatile uint32_t        OCxCONInv;
    volatile uint32_t        OCxR;
    volatile uint32_t        OCxRClr;
    volatile uint32_t        OCxRSet;
    volatile uint32_t        OCxRInv;
    volatile uint32_t        OCxRS;
    volatile uint32_t        OCxRSClr;
    volatile uint32_t        OCxRSSet;
    volatile uint32_t        OCxRSInv;
} __attribute__((packed, aligned(4))) OCCH;

typedef struct _DMACH
{
    union
    {
        volatile __DCH0CONbits_t    DCHxCON;
        volatile uint32_t           DCHxCONu32;
    };
    volatile uint32_t DCHxCONClr;
    volatile uint32_t DCHxCONSet;
    volatile uint32_t DCHxCONInv;

    union
    {
        volatile __DCH0ECONbits_t   DCHxECON;
        volatile uint32_t           DCHxECONu32;
    };
    volatile uint32_t DCHxECONClr;
    volatile uint32_t DCHxECONSet;
    volatile uint32_t DCHxECONInv;

    union
    {
        volatile __DCH0INTbits_t    DCHxINT;
        volatile uint32_t           DCHxINTu32;
    };
    volatile uint32_t DCHxINTClr;
    volatile uint32_t DCHxINTSet;
    volatile uint32_t DCHxINTInv;

    volatile uint32_t DCHxSSA;
    volatile uint32_t DCHxSSAClr;
    volatile uint32_t DCHxSSASet;
    volatile uint32_t DCHxSSAInv;

    volatile uint32_t DCHxDSA;
    volatile uint32_t DCHxDSAClr;
    volatile uint32_t DCHxDSASet;
    volatile uint32_t DCHxDSAInv;

    volatile uint32_t DCHxSSIZ;
    volatile uint32_t DCHxSSIZClr;
    volatile uint32_t DCHxSSIZSet;
    volatile uint32_t DCHxSSIZInv;

    volatile uint32_t DCHxDSIZ;
    volatile uint32_t DCHxDSIZClr;
    volatile uint32_t DCHxDSIZSet;
    volatile uint32_t DCHxDSIZInv;

    volatile uint32_t DCHxSPTR;
    volatile uint32_t DCHxSPTRClr;
    volatile uint32_t DCHxSPTRSet;
    volatile uint32_t DCHxSPTRInv;

    volatile uint32_t DCHxDPTR;
    volatile uint32_t DCHxDPTRClr;
    volatile uint32_t DCHxDPTRSet;
    volatile uint32_t DCHxDPTRInv;

    volatile uint32_t DCHxCSIZ;
    volatile uint32_t DCHxCSIZClr;
    volatile uint32_t DCHxCSIZSet;
    volatile uint32_t DCHxCSIZInv;

    volatile uint32_t DCHxCPTR;
    volatile uint32_t DCHxCPTRClr;
    volatile uint32_t DCHxCPTRSet;
    volatile uint32_t DCHxCPTRInv;

    volatile uint32_t DCHxDAT;
    volatile uint32_t DCHxDATClr;
    volatile uint32_t DCHxDATSet;
    volatile uint32_t DCHxDATInv;
} __attribute__((packed, aligned(4))) DMACH;

typedef struct _SPICH
{
    union
    {
        volatile __SPI1CONbits_t    SPIxCON;
        volatile uint32_t           SPIxCONu32;
    };
    volatile uint32_t            SPIxCONClr;
    volatile uint32_t            SPIxCONSet;
    volatile uint32_t            SPIxCONInv;

    union
    {
        volatile __SPI1STATbits_t   SPIxSTAT;
        volatile uint32_t           SPIxSTATu32;
    };
    volatile uint32_t            SPIxSTATClr;
    volatile uint32_t            SPIxSTATSet;
    volatile uint32_t            SPIxSTATInv;

    volatile uint32_t            SPIxBUF;
    volatile uint32_t            SPIxBUFPad1;
    volatile uint32_t            SPIxBUFPad2;
    volatile uint32_t            SPIxBUFPad3;

    volatile uint32_t            SPIxBRG;
    volatile uint32_t            SPIxBRGClr;
    volatile uint32_t            SPIxBRGSet;
    volatile uint32_t            SPIxBRGInv;

    union
    {
        volatile __SPI1CON2bits_t   SPIxCON2;
        volatile uint32_t           SPIxCON2u32;
    };
    volatile uint32_t            SPIxCON2Clr;
    volatile uint32_t            SPIxCON2Set;
    volatile uint32_t            SPIxCON2Inv;
} __attribute__((packed, aligned(4))) SPICH;


typedef void __attribute__((far, noreturn)) (* FNUSERAPP)(void);
#define FNIMGJMP FNUSERAPP

#pragma pack(push,2)
typedef struct {
    uint32_t    cbBlRamHeader;        // the number of bytes of this header, as known by this bootloader
    uint32_t    rcon;                 // value of RCON before the bootloader clears it
    uint32_t    fVirtualReset;        // virtual reset button, if true and software reset, then stay in bootloader
} RAM_HEADER_INFO;

typedef struct {
    uint32_t  cbHeader;               // length of this structure
    uint32_t  verBootloader;          // version of the booloader that loaded the sketch, it will be 0xFFFFFFFF if the bootloader did not write the version.
    uint32_t  verMPIDE;               // the version number of MPIDE that build the sketch
    uint32_t  bootloaderCapabilities; // capabilities of the bootloader defined by the blCapXXX bits.
    uint16_t  vend;                   // vendor ID as assigned by the bootloader, 0xFF if undefined
    uint16_t  prod;                   // product ID as assigned by the bootloader, 0xFF if undefined, or 0xFE is unassigned
    uint32_t  imageType;              // see image bit field definition above
    FNIMGJMP  pJumpAddr;            // the execution address that the bootloader will jump to
    uint32_t  pProgramFlash;          // also known as base address, that is, the first byte of program flash used by the sketch
    uint32_t  cbProgramFlash;         // the number of bytes of flash used by the sketch as defined by the linker script
    uint32_t  pEEProm;                // pointer to the eeprom area, usually at the end of flash, but now can be defined by the linker script
    uint32_t  cbEEProm;               // the length of eeprom, usually 4K but can now be defined by the linker script
    uint32_t  pConfig;                // physical address pointer to the config bits
    uint32_t  cbConfig;               // length of config bits.
    RAM_HEADER_INFO * pRamHeader;   // pointer to the ram header as defined by the sketch application
    uint32_t  cbRamHeader;            // length of the ram header as specified by the linker and will be cleared/used by the bootloader
    uint32_t  cbBlPreservedRam;       // the amount RAM the bootloader will not touch, 0xA0000000 -> 0xA0000000 + cbBlPerservedRam
} IMAGE_HEADER_INFO;
#pragma pack(pop) 

extern const uint32_t _IMAGE_HEADER_ADDR;
#define imageHeader (*((IMAGE_HEADER_INFO *) (_IMAGE_HEADER_ADDR)))

#endif  // PinAssignments_h