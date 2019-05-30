/************************************************************************/
/*                                                                      */
/*    OpenLogger.h                                                      */
/*                                                                      */
/*    Header file for OpenLogger                                        */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/17/2018 (KeithV): Created                                       */
/************************************************************************/
#ifndef OpenLogger_h
#define OpenLogger_h

// uncomment this to allow some time to recieve the full JSON command for debugging
// particularly useful for the serial input stream
// #define WAITINPUT500        // wait 500 to collect input from the JSON input stream

#include "p32xxxx.h"
#include <./libraries/WINC1500/utility/driver/include/m2m_wifi.h>
#include <./libraries/WINC1500/utility/driver/include/m2m_types.h>
#include <./libraries/WINC1500/utility/driver/source/nmasic.h>

#ifdef __cplusplus 
extern "C" {
#endif
#include <./libraries/WINC1500/utility/spi_flash/include/spi_flash.h>
#ifdef __cplusplus 
}
#endif

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

#include <PinAssignments.h>
#include <Threads.h>

typedef uint32_t STATE;

// C++ libraries, should be independent of the OpenLogger code 
// but will need PinAssignments.h
#ifdef __cplusplus
#include <./OSSerial.h>
#include <./libraries/DEIPcK/DEIPcK.h>
#include <./libraries/DFATFS/DFATFS.h>         
#include <./libraries/HTTPServer/HTTPServer.h>
#include <./libraries/DSDVOL/DSDVOL.h>
#include <./libraries/FLASHVOL/FLASHVOL.h> 
#endif


/* ------------------------------------------------------------ */
/*					A/D Converter Declarations					*/
/* ------------------------------------------------------------ */
#define ADCRANGE        4096ul              // analog read will return a max number of ADCRANGE-1
#define ADCTADFREQ      50000000ul          // How fast to run the TAD ADC clock
#define VREFMV          3000                // we are using a 3v reference voltage

#define ADCCLASS1       5                   // adc 0-5 are class 1
#define ADCCLASS2       12                  // adc 0-11 are class 2
#define ADCCLASS3       45                  // adc 0-44 are class 3
#define ADCALT          50                  // adc 45-49 are alt class 1

#define ADCTADSH        87ul                // How many TADs the Sample and Hold will charge for the shared ADCs, total conversion is this +13 TADs (12 bits)
#define ADCTADDC        ADCTADSH            // For the dedicated ADC, make the same as shared in the OpenLogger case
// #define ADCTADDC        3ul                 // How many TADs the Sample and Hold will charge the analog input ADCs (Dedicated)

// we are using 12 bit resoluiton, or 13 TADs, the TAD clock is 50MHz
// each shared conversion is 100 TADs or 2us per conversion
// each dedicated conversion is 16 TADs or 320ns

//**************************************************************************
//**************************************************************************
//******************* ADC Macros       *********************************
//**************************************************************************
//**************************************************************************

// ADC resolution
#define RES6BITS        0b00        // 6 bit resolution
#define RES8BITS        0b01        // 8 bit resolution
#define RES10BITS       0b10        // 10 bit resolution
#define RES12BITS       0b11        // 12 bit resolution

// ADC reference soruces
#define VREFPWR         0b000       // internal 3.3v ref
#define VREFHEXT        0b001       // external high ref 
#define VREFLEXT        0b010       // external low ref

// ADC clock sources
#define CLKSRCFRC       0b11        // Internal 8 MHz FRC clock source
#define CLKSRCREFCLK3   0b10        // External Clk 3 clock source
#define CLKSRCSYSCLK    0b01        // System clock source
#define CLKSRCPBCLK3    0b00        // PB Bus 3 as clock source

// PB 3, ADC, OC, Timers 
#define PBUS3DIV    2               // divide system clock by (1-128); the default is 2 and that is what we are using
#define PB3FREQ     (F_CPU / PBUS3DIV)

// TQ CLOCK
#define TQCLKDIV    1                               // we want to run the TQ at F_CPU == 200MHz
// Global ADC TQ Clock prescaler 0 - 63; Divide by (CONCLKDIV*2) However, the value 0 means divide by 1
#define TQCONCLKDIV (TQCLKDIV >> 1)            
#define TQ          (F_CPU / TQCLKDIV)              // ADC TQ clock frequency

// TAD = TQ / (2 * ADCTADDIV), ADCTADDIV may not be zero
#define ADCTADDIV   ((TQ / ADCTADFREQ) / 2)
#if (ADCTADDIV == 0)
    #error ADCTADFREQ is too high or TQ is too low
#endif

#define CBITSRES        ((2ul * RES12BITS) + 6ul)
#define CTADCONV        (CBITSRES + 1ul)
#define SHCONVFREQSH    (ADCTADFREQ / (CTADCONV + ADCTADSH))        // how fast we can turn around and do the next sample, must be faster than sample rate
#define SHCONVFREQDC    (ADCTADFREQ / (CTADCONV + ADCTADDC))        // how fast we can turn around and do the next sample, must be faster than sample rate

//**************************************************************************
//**************************************************************************
//******************* Calibration Versions *********************************
//**************************************************************************
//**************************************************************************
#define CALVER  1   // calibration file format version
#define WFVER   2   // wifi file format version
#define LOGFMT  3   // log file format version
#define LOGREV  1   // log header revision number

//#define SERIALBAUDRATE 9600
//#define SERIALBAUDRATE 115200
//#define SERIALBAUDRATE 460800

//#define SERIALBAUDRATE 625000
//#define SERIALBAUDRATE 500000
//#define SERIALBAUDRATE 1000000
//#define SERIALBAUDRATE 1250000
//#define SERIALBAUDRATE 2500000
//#define SERIALBAUDRATE 3750000
//#define SERIALBAUDRATE 5000000
//#define SERIALBAUDRATE 6250000
//#define SERIALBAUDRATE 12500000

#define SERIALBAUDRATE 1250000

/************************************************************************/
/************************************************************************/
/********************** FAT FILE VOL and NAMES **************************/
/************************************************************************/
/************************************************************************/
// volumes to use.
// except for VOLNONE, these index into
// DFATFS::szFatFsVols[vol] for vol strings
typedef enum
{
    VOLNONE     = 0,
    VOLSD       = 1,
    VOLFLASH    = 2,
    VOLRAM      = 3,    // this has no associated vol in the fat file system, but could
    VOLCLOUD    = 4,    // this has no associated vol in the fat file system
    VOLEND      = 5
} VOLTYPE;

typedef enum
{
    CSEVNONE        = 0,
    CSEVTHINGSPEAK  = 1,
    CSEVEND      
} CLOUDSERVICE;

/************************************************************************/ 
/************************************************************************/
/********************** Defines *****************************************/
/************************************************************************/
/************************************************************************/
#define RESOURCETIMOUT  (10*CORE_TMR_TICKS_PER_SEC)     // how long to wait for a resource to free up

// the PB bus runs at 100MHz, the output driver can only run at 50MHz
// so we need at least 2 steps for the output driver to respond.
// but it is a sloppy signal not making full VCC at 50MHz
// we really want something much more, so set the min at...
#define PWMMINPULSE     10
#define PWMHIGHLIMIT    (PWMPERIOD - PWMMINPULSE)   
#define PWMLOWLIMIT     PWMMINPULSE   

#define PWMPRESCALER    0       // this is 2^^T3PRESCALER (0 - 7, however 7 == 256 not 128) off of PBCLK3 (100 MHz)
#define PWMPERIOD       330     // currently we allow 330 resolution on the PWM, so 3.3v / 330 = 10mv resolution; at 100 Mhz / 330 = 303 KHz freq
#define PWMIDEALCENTER  175      // this is ideal, not actual; this should only be used as a starting point for calibration

#define PWM_SETTLING_TIME   (500*CORE_TMR_TICKS_PER_MSEC)         // code will break if ALL PWM settling times don't use this value

/************************************************************************/
/********************** GPIO ********************************************/
/************************************************************************/
#define NBRGPIO 10
#define SDWAITTIME 500      // how long to wait after sensing an sd insertion and mounting
 
/************************************************************************/
/********************** Common Timer Values *****************************/
/************************************************************************/
#define TMRPBCLK                100000000               // PB CLK, ticks per second
#define MAXTMRPRX               0x10000
#define MAXTMRPRESCALER         256
#define PeriodToPRx(_Period)    ((uint16_t) ((_Period)-1))  // converting a Tmr value to a PRx match register value    
#define TMROCPULSE              4                           // how many ticks the timer or OC needs to be at its current state to ensure an INT when hit.

#define PSPERTMRPBCLKTICK       10000                   // there are 10^^12 ps/sec and 10^^8 ticks/sec or 10^^12 / 10^^8 == 10^^4 ps/tick

#define GetTmrTicksFromPS(ps)   ((((uint64_t) ps) + PSPERTMRPBCLKTICK/2) / PSPERTMRPBCLKTICK)
#define GetPSFromTmrTicks(tk)   (((uint64_t) (tk)) * PSPERTMRPBCLKTICK)

#define TMRMINSPS               6   // 100,000,000 / 256 / 65536 ~=> 5.960464 Hz

// WARNING: Interleaving only works if the timer prescalar is set to 1:1 with the PB clock.
// The timer event will occur 1 PB and 2 sysclocks after the PR match. With a PBCLK3 at 2:1, and a timer prescalar of 1:1, that
// works that the interrupt event occurs on timer value 1 (2 ticks after PR)
// WARNING: the ADC is triggered off of the rising edge of the OC, not the interrupt event, this means the ADC will trigger on a match to R
// WARNING: however, the DMA will trigger off of the OC interrupt event, which is a match to RS
// As documented the interrupt for the OC should occur one PB after the R match for the ADC and 1 PB after the RS match for the DMA
// however there must be extra logic for the ADC as the ADC fires 3 PB after the R match, determined experimentally.
#define INTERLEAVEPR(_Period)         PeriodToPRx(_Period)                 
#define INTERLEAVEOC(_Period)         ((uint16_t) (((_Period)/2)-2))  

/************************************************************************/
/********************** DC Output ***************************************/
/************************************************************************/
#define mVDCChg     40                                  // The input can change by about 40mv
#define DadcDCChg   (((40l * 1365l) + 500) / 1000l)     // The input can change by about 40mv
#define DadcLimit   (2l * DadcDCChg)                    // stop looking when we get this close  

/************************************************************************/
/********************** AWG Parameters **********************************/
/********************** Max Freq 200kHz *********************************/
/********************** Min Freq 46.58uHz *******************************/
/********************** Max sample freq 2MHz ****************************/
/********************** Min Sample freq  1,526 Hz ***********************/
/************************************************************************/
#define AWGBUFFSIZE     0x8000                          // # of elements in the buffer array (each element is 2 bytes)  
#define AWGPRESCALER    0                               // this is 2^^T3PRESCALER (0 - 7, however 7 == 256 not 128) off of PBCLK3 (100 MHz)
#define AWGPBCLK        (AWGPRESCALER == 7 ? (F_CPU) >> (AWGPRESCALER+2) : (F_CPU) >> (AWGPRESCALER+1))
//#define AWGMAXDMA       10000000l                     // How fast the DMA can run without missing events.
//#define AWGMAXDMA       5000000l                      // How fast the DMA can run without missing events.
//#define AWGMAXDMA       4000000l                      // How fast the DMA can run without missing events.
//#define AWGMAXDMA       2500000l                      // How fast the DMA can run without missing events.
#define AWGMAXDMA       2000000l                        // How fast the DMA can run without missing events.
//#define AWGMAXDMA       1000000l                      // How fast the DMA can run without missing events.
#define AWGMAXSPS       (AWGPBCLK > AWGMAXDMA ? AWGMAXDMA : AWGPBCLK)
#define AWGMINTMRCNT    (AWGPBCLK / AWGMAXSPS)          // AWGPBCLK % AWGMAXSPS == 0 
#define AWGMAXFREQ      ((int32_t) (AWGMAXSPS / 10))    // Need at least 10 points to make a waveform
#define AWGMAXP2P       3000l
#define AWGMAXOFFSET    1500l

#if(AWGPBCLK % AWGMAXSPS != 0)
#error max awg sample rate must be a mult of the timer pbclk
#endif

// in the custom waveform generator we have
// tmr = pbx*scalar/freqHz/AWGMAXBUF
// and tmr >= pbx / AWGMAXDMA
// pbx1000/AWGMAXBUF should be an even number say 100,000,000 / 25,000 = 4,000
#define AWGMAXBUF       25000l       // we want (pbx1000 / AWGMAXBUF) to divide evenly. -> 100,000,000,000 / 25,000 = 4,000,000; this is in awg.c
#define AWGDMABUF       32760l       // once we have or sps and tmr values, we can allow the buffer to grow up to what the DMA can take (64K/sizeof(uint16_t) - 2), 4 bytes less

#define AWG_SETTLING_TIME   (2*CORE_TMR_TICKS_PER_MSEC)          // 2ms is min (35us simulated to stablize)
#define HWDACSIZE   1024l

// supported waveforms
typedef enum
{
    waveNone,
    waveDC,
    waveSine,
    waveSquare,
    waveTriangle,
    waveSawtooth,
    waveArbitrary,
} WAVEFORM;

typedef struct
{
    STATE       state;
    WAVEFORM    waveform;
    uint32_t    actualSamples;
    uint32_t    actualSps;
    uint32_t    actualmHz;
    int32_t     actualVpp;
    int32_t     actualVOffset;
    uint16_t    tmrPeriod;
    uint16_t    tmrPrescalar;
    uint16_t    pwmRS;
    union
    {
        int16_t     rgVWave[AWGDMABUF];  
        uint16_t    rgDMAWave[AWGDMABUF];  
    } PHY;      // PHY is actually just attributes, this is a anonymous union
} AWGPARAM;

extern AWGPARAM paramAWG; 
extern AWGPARAM paramIAWG;

/************************************************************************/
/********************** LOG Parameters **********************************/
/************************************************************************/
#define NBRLOGCH 8
#define LOGMAXSAMPLEFREQ    50000
#define LOGMAXEXADCFREQ     500000
//#define LOGMAXSAMPINFILE    400000
//#define LOGMAXSAMPINFILE    15000000
//#define LOGMAXSAMPINFILE    250000000

// we want to pick a number that is both divisible by 3*5*7*8 = 840
// and our cluster size of 2^^15 = 32,768
// and with 8 channls is close to 2^^32 = 4,294,967,296 bytes or 2,147,483,648 samples
// or for 8 channels 2,147,483,648 / 8 = 268,435,456 maximum
// 1,720,320 samples is  3,440,640 which is a multiple of 840 and 32,768
// 268,435,456 / 1,720,320 = 156.038 so say a max of 156 to 
// but we really want to be under 250,000,000 samples
// so 250,000,000 / 1,720,320 = 145.322
// 145 * 1,720,320 = 249,446,400 (2*8*249,446,400 = 3,991,142,400 < 2^^32 (4,294,967,296)
// 3,991,142,400 / 840 = 4,751,360
// 3,991,142,400 / 32,768 = 121,800
//#define LOGMAXSAMPINFILE    249446400
#define LOGMAXSAMPINFILE    1995571200
#if((LOGMAXSAMPINFILE % 840) != 0 || (LOGMAXSAMPINFILE % 16384) != 0)
#error Log Files must be a mult sample count of 1,2,3,4,5,6,7 samples and a mult of SD clusters of 32,768 bytes or 16,384 samples
#endif

/************************************************************************/
/************************************************************************/
/********************** Instrument handle indexs ************************/
/********************** Must map to indexes in rgInstr ******************/
/************************************************************************/
/************************************************************************/

typedef enum
{
    NULL_ID     = 0,
    DCVOLT1_ID,
    DCVOLT2_ID,
    AWG1_ID,
    AWG2_ID,
    ALOG1_ID,
    ALOG2_ID,
    DLOG1_ID,
    INSTR_END_ID,
    SERIAL_INPUT_ID,
    FIFO245_ID,
    WIFI_ID,
    SD_ID,
    END_ID
} INSTR_ID;

typedef enum
{
    STCDNormal = 0,
    STCDForce,
    STCDError,
    STCDOverflow,
    STCDUnknown,
} STCD;

#ifdef __cplusplus 

typedef struct _DAQHDR
{
    struct _DHDR
    {
        const uint8_t   endian;             // 0 - little endian 1 - big endian
        const uint8_t   cbSampleEntry;      // how many bytes per sample, OpenScope = 2
        const uint16_t  cbHeader;           // how long is this header structure
        const uint16_t  cbHeaderInFile;     // how much space is taken in the file for the header, sector aligned (512)
        const uint16_t  format;             // General format of the header and potential data
        const uint32_t  revision;           // specific header revision (within the general format)
        const uint64_t  voltageUnits;       // divide the voltage of each sample by this to get volts.
              uint32_t  stopReason;         // reason for logging stopping; 0 = Normal, 1 = Forced, 2 = Error, 3 = Overflow, 4 = unknown
              uint64_t  iStart;             // what sample index is the first index in the file, usually 0
              uint64_t  actualCount;        // how many samples in the file.
        const uint64_t  sampleFreqUnits;    // divide uSPS by sampleFreqUnits to get samples per second
              uint64_t  xSPS;               // sample rate in sample freq units samples / second
        const uint64_t  delayUnits;         // divide psDelay by delayUnits to get the delay in seconds.
              int64_t   xDelay;             // delay from the start of sampling until the first sample was taken, usually 0, in delay units
              uint32_t  cChannels;          // how many channels in per sample
              uint8_t   rgChannels[8];      // the channel order   
    } __attribute__((packed)) dHdr;
    const uint8_t rgSize[512-sizeof(struct _DHDR)];

    _DAQHDR() : dHdr({0, sizeof(uint16_t), sizeof(struct _DHDR), sizeof(struct _DAQHDR), LOGFMT, LOGREV, 1000ul, STCDNormal, 0, 0, 1000000ull, 1000000000, 1000000000000ull, 0}), rgSize{0}
    {     
    }
} __attribute__((packed)) DAQHDR;

typedef struct
{
    STATE           state;                  // the current state of the target
    STATE           nextState;              // when done with the current state, where we go
    STCD            stcd;                   // stop reason
    VOLTYPE         vol;                    // where this goes, RAM, SD, Cloud
    char            szUri[256];             // filename or cloud URL
    int64_t         csProcessed;            // how many samples have been processed (aggregate)
    uint32_t        csBackLog;              // what is the backlog for the channel (aggregate)
    uint8_t *       pbWrite;                // buffer to write
    uint32_t        cbWrite;                // how many bytes to write
    uint32_t        cbWritten;              // how many bytes have been written
    union
    {
        struct
        {
            uint32_t        iSeek;                  // where to seek too when opening a file
            uint32_t        iNbr;                   // where in the filename the start number goes
            uint32_t        iLOB;                   // where in the filename the log on boot number goes, this is _xxx
            int64_t         csEOF;                  // where this file must end in it sample count (aggregate)
            char            szExt[32];              // file ext
        };
        struct
        {
            CLOUDSERVICE    cloudService;           // what cloud service is vol == cloud
            uint32_t        tLastPost;              // when the last post was done
            uint32_t        tTimeout;               // The wait for response timeout value
            bool            fFinishTime;            // finish time is set
            uint32_t        tFinishTime;            // finish time
            uint32_t        cSampleWait;            // To sync on sample ticks, what the last sample count was
            uint32_t        secPerSampl;            // how many seconds between each sample
            int16_t *       psVCur;                 // pointer in to the voltage buffer to save
            uint32_t        csVSave;                // how many sampes to save this time 
            uint32_t        csVSaved;               // how many samples got saved
            uint32_t        cbMinConLen;            // the min size of a block write update
            uint32_t        cbSampleConLen;         // how long each additional sample group will add
            uint32_t        cbDelta;                // how long the delta packet is
            uint32_t        iContentLen;            // where the content lenght goes in the szPostBuf
            uint32_t        iUpdate;                // where the update packet begins in the szPostBuf
            uint32_t        iDelta;                 // where the delta packet starts
            uint32_t        iFirstValue;            // the first value location
            char            szPOSTBuf[512];         // Post command
            char            szResponse[4096];       // the response buffer
       };
    };
} ADCTARGET;

// ThingSpeak, MathWorks cloud logging
#define minSecBetweenPosts                  20ul        // actually 15, but lets slow it down
#define TSiDelta                            12          // how far in is the delta value from the start of delta string
#define TSdiFValue2Value                    21          // how many bytes between value fields
#define TScbValue                           7           // how many bytes in the value
#define TScbDelta                           8           // delta length can only be 7 char, need room for the null on the last space; we can go back ~115 days on the delta time

typedef struct
{
    uint32_t        cAve;           // if average, how much, must be a power of 2
    uint32_t        pwr2Ave;        // How much to shift to get our average
    uint32_t        iCh;            // the channel number, also the index for the calibration info for this channel
    uint32_t        iADCValue;      // In the adcDDR memory, this is the index of the data for that channel
    uint32_t        u2A;            // u2A = 1024*A/1000000
    uint32_t        u2C;            // u2C = 2^^25 - 1024*C/1000000 + 512
    uint32_t        sumRaw;         // the ave sum of the ADC raw value
} LOGCHANNEL;

#define TWOto25     (1<<25)

typedef struct
{
    STATE           state;
    STCD            stcd;                   // stop reason
    int64_t         csMax;                  // max 9,223,372,036,854,775,808 samples ~584,542 years (aggregate)
    uint64_t        uHz;                    // 500,000,000,000/nChan to 11.574 days uSamples/sec
    uint16_t        smpTmrPrescalar;
    uint16_t        smpTmrPeriod;
    uint32_t        smpTmrISRCnt;
    uint64_t        startpsDelay;           // 1 ps to 213.5 days
    uint16_t        dlyTmrPrescalar;
    uint16_t        dlyTmrPeriod;
    uint32_t        dlyTmrISRCnt;
    uint32_t        sCh;                    // Usually 1, unles nCh == 1.
    uint32_t        nCh;                    // How many channels the ADC is configured for, some may be unused (gaps in channel numbers)
    bool            fAdcFreqMax;            // true if 500kHz, false if 250kHz
    bool            fLogOnBoot;             // Does this parameter do a log on boot?
    uint16_t *      padcRawStart;           // where valid raw ADC data is
    uint16_t *      padcRawEnd;             // where the end of the valid ADC value is, NOT modulo the buffer always padcRawEnd >= padcRawStart
    uint32_t        cActiveCh;              // How many channels of the ADC provide requested data, that go to the Voltage buffer
    int64_t         csTotal;                // How many total samples have made it to the voltage buffer (aggregate)
    int16_t *       padcVoltNext;           // the next place to write a voltage padcVoltNext = padcVoltEnd % CSVBUFF
    ADCTARGET       adcTrg;                 // this is the target info, this will be an array when we support multiple targets.
    LOGCHANNEL      rgLogCh[NBRLOGCH];      // ADC channel information, for use to the adcRaw buffer
} LOGPARAM;

extern LOGPARAM logIParam;

typedef struct 
{
    uint16_t        wifiVer;
    uint16_t        cbHeader;
    SECURITY        wifiType;
    bool            fAutoConnect;
    int32_t         keyIndex;
    union
    {
        WPA2KEY     wpa2Key;
        WEP40KEY    wep40Key;
        WEP104KEY   wep104Key;
    } key;
    STATE           lastErrState;
    char            ssid[DEWF_MAX_SSID_LENGTH+1];       // Network SSID value
    char            passphrase[DEWF_MAX_PASS_PHRASE+1]; // PassPhrase
} WIFINFO;

typedef struct
{
    STATE       scanState;
    STATE       errState;
    uint32_t    cNetworks;
    uint32_t    iNetwork;
} WFSCANSTATE;

extern WIFINFO      wifiActive;
extern WIFINFO      wifiWorking;
extern WFSCANSTATE  wifiScanState;
#endif

// looking for 16MB of data, we must also maintain the 1-8 channel modulo
// we already know that 4193280 is a good number for 8MB, so double it
#define CSMINBUF    840     // 1,2,3,4,5,6,7,8 must divide in this evenly

// must be a multiple of 16, the cache line size
#define CBADCDMA 65520
#if((CBADCDMA % 16) != 0)
#error The DMA buffer must be a mult of the data cache lines
#endif

// must be a multiple of 840 samples: 1,2,3,4,5,6,7,8 must divide in this evenly
#define CSADCDMA (CBADCDMA / 2)
#if((CSADCDMA % CSMINBUF) != 0)
#error Count of samples in the DMA buffer must be a multiple of 840 samples so 1-8 divide evenly
#endif

// must be a multiple of 2*CSADCDMA
// we have alternating DMAs, and we want them to roll on the same addresses
#define CSRBUFF 4193280
#if((CSRBUFF % (2*CSADCDMA)) != 0)
#error The Raw DDR buffer must be a multiple 2 the the DMA sample sizes
#endif

#define CSVBUFF (2*CSRBUFF)
#if( (CSVBUFF % CSRBUFF) != 0 )
#error The voltage DDR buffer must be a multiple of the Raw DDR buffer
#endif

#if(CSVBUFF % CSMINBUF != 0)
#error The voltage buffer must be a mult 840
#endif

// this is where the ADC free running conversions DMA too
extern uint16_t adcHoldingCell[NBRLOGCH];

// these are in DDR memory
extern uint16_t adcRawBuffer[CSRBUFF];
extern int16_t  adcVoltageBuffer[CSVBUFF];

/************************************************************************/
/********************** Other Defines  **********************************/
/************************************************************************/

// Macros to identify warning and error states
#define IsStateAnError(s)   (STATEError <= (s))
#define IsStateAWarning(s)  (STATEWarning <= (s) && (s) < STATEError)

#define MkStateAnError(s)   (STATEError | ((s) & ~STATEWarning))
#define MkStateAWarning(s)  (STATEWarning | ((s) & ~STATEError))

 
/************************************************************************/
/************************************************************************/
/********************** TESTS THAT WE CAN RUN ***************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
typedef enum
{
    TESTNone,
    TESTCalPSK,
    TESTFat32,
    TESTGPIO,
    TESTGPIOEnd,
    TESTEncoder,
    TESTEncoderEnd,
    TESTComplete,
    TESTDDR,
    TESTFIFO245,
    TESTFIFO245End,
} TESTS;
extern TESTS runTest;

/************************************************************************/
/********************** state Machine states  ***************************/
/************************************************************************/
/************************************************************************/
typedef enum
{
    // common states
    Idle = 0,               // standing by, doing nothing, or completed
    Done,                   // To denote completion usually before going to the Idle state
    Waiting,                // in a suspended state, waiting for something else
    WaitingForResources,    // specifically suspended waiting for a resource
    Working,                // in the process of doing the task (generic)
    Finishing,              // cleaning up
    Pending,                // like waiting, only pending something else
    Acquiring,              // Acquring data from a resource, such as an ADC
    Running,                // instrument is running
    Stopped,                // instrument is stoped
    Overflow,               // weare are in overflow
            
    // calibration states
    Calibrated,
    Calibrating,
    Uncalibrated,
    FailedCalibration,       
    EndOfCommonStates,      // end of states we have strings for

    // Buffer lock states
    LOCKAvailable,
    LOCKAcq,
    LOCKOutput,
    LOCKInput,

    // main loop states
    MAINPrtHdr,
    MAIN3V0,
    MAIN1V5,
    MAIN1V8,
    MAIN5V0,
    MAIN3V3,
    MAIN5V0N,
    MAINPrtVolts,
    MAINIntFlashFileVol,
    MAINInitCalibrationData,
    MAINInitADC,
    MAINInitAWG,
    MAINInitWiFi,
    MAINRunUI,
    MAINUILoop,

    // Network stack
    INITAdaptor,
    INITWebServer,

    // LED states
    LEDInit,
    LEDReady,
    LEDPause,
    LEDHold,
    LEDResume,

    // File system
    FILEVerify,
    FILEmkfs,
    FILEmount,
    FILEunmount,
    FILEchdrive,
    FILEchdir,
    FILEexist,
    FILEopen,
    FILEopening,
    FILEclose,
    FILEgetsize,
    FILEread,
    FILEwrite,
    FILEdelete,
    FILEseek,
    FILEopendir,
    FILEreaddir,
    FILEclosedir,

    // SD states
    SDMounting,
    SDInserted,
    SDReadCalibration,

    // WiFi states
    WIFInitWaitForMacAddress,
    WIFICalPSK,
            
    WIFIScan,
    WIFIScanListAP,
            
    WIFIConnect,
    WIFIConnectSetupHTTP,
    WIFIConnectLinked,
            
    WIFIFlashEnterDownloadMode,
    WIFIFlashGetFirstFlashSector,
    WIFIFlashFlushingUART,
    WIFIFlashWaitForDownload,
    WIFIFlashReadUARTData,
    WIFIFlashEraseFlash,
    WIFIFlashProgramFlash,
    WIFIFlashStats,

    WIFIDisconnect,
    WIFIUpdateFlash,
    WIFIUpdateCert,
    WIFIReDisconnect,
    WIFIReInit,

    // HTTP States
    HTTPEnabled,

    JSONListening,
    JSONReadFile,
    JSONWriteFile,
    JSONClose,
    JSONSeekFile,
    JSONWait0,
 
    // JSON Parsing states
    JSONSkipWhite,
    JSONToken,
    JSONNextToken,
    JSONfalse,
    JSONnull,
    JSONtrue,
    JSONString,
    JSONNumber,
    JSONCallOSLex,
    JSONSyntaxError,
    JSONTokenLexingError,
    JSONNestingError,
            
    OSJBSkipWhite,
    OSJBSkipWhiteToEndOfChunk,
    OSJBReadCount,
    OSJBReadJSON,
    OSJBReadNextBinary,
    OSJBReadNextChunk,
    OSJBCarrageReturn,
    OSJBNewLine,
    OSJBParseJSON,
    OSJBReadBinary,
    OSJBOutputJSON,
    OSJBWriteChunkSize,
    OSJBWriteOData,

    DCWaitHigh,
    DCReadHigh,
    DCWaitLow,
    DCReadLow,

    AWGWaitPWMLow,
    AWGReadPWMLow,
    AWGWaitPWMHigh,
    AWGReadPWMHigh,
    AWGWaitPWM,
    AWGWaitHW,
    AWGReadHW,
    AWGSort,
    AWGFindCenter,
    AWGWaitPWMZero,
    AWGAdjZero,

    AWGDC,
    AWGSquare,
    AWGSawtooth,
    AWGTriangle,
    AWGSine,
    AWGWaitCustomWaveform,
    AWGMakeDMABuffer,
    AWGSetWaveform,

    ADCWaitPWM,
    ADCSamplingComplete,
    ADCCalAverage,
    ADCReadDC,

    CLDResolveEP,
    CLDConnect,
    CLDWrite,
    CLDWriteUpdate,
    CLDWriteField,
    CLDNextDelta,
    CLDEndDelta,
    CLDRead,
    CLDDisconnect,

    LOBAuto,
    LOBWaitDelete,
    LOBWaitWrite,
            
    /************************************************************************/
    /********************** START OF WARNING/ERROR **************************/
    /************************************************************************/
    STATEWarningErrors = 0x00010000,
                      
    /************************************************************************/
    /**************** Compound STATE/ERROR/WARNINGS have this bit set *******/
    /************************************************************************/
    STATECompound       = 0x10000000,
            
    // Manufacturing states 
    MFGSTATE            = STATECompound | 0x00100000,  

    // GCMD states 
    GCMDSTATE           = STATECompound | 0x01000000,     

    // FILE states
    FILECSTATE          = STATECompound | 0x02000000,            
            
    // WINC states
    WIFICBSTATE         = STATECompound | 0x04000000,            
            
    OSPARSyntaxError    = STATECompound | 0x08000000,      
    OSPAREnd,
            
    /************************************************************************/
    /********************** PREDEFINED ERROR/WARNINGS ***********************/
    /************************************************************************/
    STATEPredefined = 0x20000000,
    
    /************************************************************************/
    /********************** WARNINGS ****************************************/
    /************************************************************************/
    STATEWarning = 0x40000000,
    STATEWarningStart = STATEWarning | STATEWarningErrors,
            
    UINoNetworksFound,
    GPIODirectionMissMatch,
            
    /************************************************************************/
    /********************** ERRORS ******************************************/
    /************************************************************************/
    STATEError = 0x80000000,
    STATEErrorStart = STATEError | STATEWarningErrors,
 
    IOVolError,

    // Initialization Errors        
    WIFInitUnableToSetNetworkAdaptor,
    WIFInitMACFailedToResolve,
    INITHTTPSetupFailure,

    // WiFi Errors
    WiFiNoMatchingSSID,
    WiFiUnsuporttedSecurity,
    
    // AWG Errors        
    AWGBufferTooBig,
    AWGCurrentlyRunning,
    AWGExceedsMaxSamplPerSec,
    AWGValueOutOfRange,
    AWGWaveformNotSet,
    AWGWaveformNotSupported,

    // OSC Errors        
    OSCPWMOutOfRange,
    OSCGainOutOfRange,
    OSCDcNotHookedToOSC,

    //TRG errors
    TRGUnableToSetTrigger,
    TRGAcqCountOutTooLow,

    // GPIO Errors
    GPIOInvalidDirection,
    GPIONotRunning,

    // TEST Errors
    AnotherTestInUse,

    // compound errors
    // errors OR'ed with underlying error codes 
    // we can have 255 of these from 0x101xxxxx - 0x1FFxxxxx
    // OR'd in errors can be from 0x00000000 -  0x000FFFFF
    FILESystemError                 = (STATEError | FILECSTATE),
    FILEMountError                  = (FILESystemError | 0x00001000),
    WiFiError                       = (STATEError | WIFICBSTATE),

    // common predefined errors
    InvalidState                    = (STATEError | STATEPredefined | 0x00000001),  // 0xA0000001, --> 2684354561
    InvalidFileName                 = (STATEError | STATEPredefined | 0x00000002),  // 0xA0000002, --> 2684354562
    InvalidCommand                  = (STATEError | STATEPredefined | 0x00000003),  // 0xA0000003, --> 2684354563
    InvalidChannel                  = (STATEError | STATEPredefined | 0x00000004),  // 0xA0000004, --> 2684354564
    NotEnoughMemory                 = (STATEError | STATEPredefined | 0x00000005),  // 0xA0000005, --> 2684354565
    FileInUse                       = (STATEError | STATEPredefined | 0x00000006),  // 0xA0000006, --> 2684354566
    InvalidSyntax                   = (STATEError | STATEPredefined | 0x00000007),  // 0xA0000007, --> 2684354567
    UnsupportedSyntax               = (STATEError | STATEPredefined | 0x00000008),  // 0xA0000008, --> 2684354568
    Unimplemented                   = (STATEError | STATEPredefined | 0x00000009),  // 0xA0000009, --> 2684354569
    ValueOutOfRange                 = (STATEError | STATEPredefined | 0x0000000A),  // 0xA000000A, --> 2684354570
    InstrumentArmed                 = (STATEError | STATEPredefined | 0x0000000B),  // 0xA000000B, --> 2684354571
    AcqCountTooOld                  = (STATEError | STATEPredefined | 0x0000000C),  // 0xA000000C, --> 2684354572
    InstrumentInUse                 = (STATEError | STATEPredefined | 0x0000000D),  // 0xA000000D, --> 2684354573

    JSONLexingError                 = (STATEError | STATEPredefined | 0x0000000E),  // 0xA000000E, --> 2684354574
    JSONObjArrayNestingError        = (STATEError | STATEPredefined | 0x0000000F),  // 0xA000000F, --> 2684354575
    OSInvalidSyntax                 = (STATEError | STATEPredefined | 0x00000010),  // 0xA0000010, --> 2684354576
    EndOfStream                     = (STATEError | STATEPredefined | 0x00000011),  // 0xA0000011, --> 2684354577

    UnableToSaveData                = (STATEError | STATEPredefined | 0x00000013),  // 0xA0000013, --> 2684354579
    InvalidAdapter                  = (STATEError | STATEPredefined | 0x00000014),  // 0xA0000014, --> 2684354580
    NoSSIDConfigured                = (STATEError | STATEPredefined | 0x00000015),  // 0xA0000015, --> 2684354581
    MustBeDisconnected              = (STATEError | STATEPredefined | 0x00000016),  // 0xA0000016, --> 2684354582
    NoScanDataAvailable             = (STATEError | STATEPredefined | 0x00000017),  // 0xA0000017, --> 2684354583
    UnableToGenKey                  = (STATEError | STATEPredefined | 0x00000018),  // 0xA0000018, --> 2684354584
    WiFiIsRunning                   = (STATEError | STATEPredefined | 0x00000019),  // 0xA0000019, --> 2684354585
    WiFiUnableToStartHTTPServer     = (STATEError | STATEPredefined | 0x0000001A),  // 0xA000001A, --> 2684354586
    WiFiNoNetworksFound             = (STATEError | STATEPredefined | 0x0000001B),  // 0xA000001B, --> 2684354587
    FileUnableToReadFile            = (STATEError | STATEPredefined | 0x0000001C),  // 0xA000001C, --> 2684354588
    InstrumentNotArmed              = (STATEError | STATEPredefined | 0x0000001D),  // 0xA000001D, --> 2684354589
    InstrumentNotArmedYet           = (STATEError | STATEPredefined | 0x0000001E),  // 0xA000001E, --> 2684354590
    NoSDCard                        = (STATEError | STATEPredefined | 0x0000001F),  // 0xA000001F, --> 2684354591
    InstrumentNotConfigured         = (STATEError | STATEPredefined | 0x00000020),  // 0xA0000020, --> 2684354592
    NoDataAvailable                 = (STATEError | STATEPredefined | 0x00000021),  // 0xA0000021, --> 2684354593
    DirectoryDoesNotExist           = (STATEError | STATEPredefined | 0x00000022),  // 0xA0000022, --> 2684354594
    StartIndexDoesNotExist          = (STATEError | STATEPredefined | 0x00000023),  // 0xA0000023, --> 2684354595
    TimeoutErr                      = (STATEError | STATEPredefined | 0x00000024),  // 0xA0000024, --> 2684354596
    InvalidParameter                = (STATEError | STATEPredefined | 0x00000025),  // 0xA0000025, --> 2684354597
    WeakReferenceNotPresent         = (STATEError | STATEPredefined | 0x00000026),  // 0xA0000026, --> 2684354598
    InvalidVersion                  = (STATEError | STATEPredefined | 0x00000027),  // 0xA0000027, --> 2684354599
    WiFiNoScanData                  = (STATEError | STATEPredefined | 0x00000028),  // 0xA0000028, --> 2684354600
    GPIONotOutput                   = (STATEError | STATEPredefined | 0x00000029),  // 0xA0000029, --> 2684354601
} OPEN_LOGGER_STATES;

/************************************************************************/
/************************************************************************/
/********************** Calibration File Structures  ********************/
/************************************************************************/
/************************************************************************/
typedef struct 
{
    union
    {
        uint32_t    version;
        uint32_t    time;
        VOLTYPE     vol;
    };
    STATE       state;
    int64_t     A;
    int64_t     B;
    int64_t     C;
} POLYCAL;      // polynomial calibration data; Aadc + Bpwm + Coff; in uV (AWG & DC) nV (Ain)

typedef struct
{
    POLYCAL     polyCal;
    uint32_t    iCenter;
    uint32_t    pwmCenter;
    int32_t     rguVdac[HWDACSIZE];
    uint16_t    rgDdac[HWDACSIZE];
} AWGCAL;

#ifdef __cplusplus 

#include <./DeferredTasks.h>
#include <./LoopStats.h>
#include <./LexJSON.h>
#include <./JsonIO.h>

/************************************************************************/
/************************************************************************/
/********************** External Variables  *****************************/
/************************************************************************/
/************************************************************************/

extern MACADDR              macMyMacAddr;           // my MAC address
extern IPv4                 ipOpenLogger;           // my IP address
extern const char           szDefaultPage[];        // default home page string
                                                    // the Power supply and reference voltages.   
extern bool OSAdd(uint8_t m1[], uint32_t cm1, uint8_t m2[], uint32_t cm2, uint8_t r[], uint32_t cr);
extern bool OSMakeNeg(uint8_t m1[], uint32_t cm1);
extern bool OSUMult(uint8_t m1[], uint32_t cm1, uint8_t m2[], uint32_t cm2, uint8_t r[], uint32_t cr);
extern bool OSMult(int8_t m1[], uint32_t cm1, int8_t m2[], uint32_t cm2, int8_t r[], uint32_t cr);
extern bool OSDivide(int8_t m1[], uint32_t cm1, int64_t d1, int8_t r[], uint32_t cr);

extern char * ulltoa(uint64_t val, char * buf, uint32_t base);
extern char * illtoa(int64_t val, char * buf, uint32_t base);
extern char * GetPercent(int64_t actual, int64_t ideal, int32_t cbD, char * pchOut, int32_t cbOut);

extern uint64_t SPSPreScalarAndPeriod(uint64_t xsps, uint32_t scaleSPS, uint32_t const pbClk, uint16_t * pPreScalar, uint16_t * pPeriod, uint32_t * pCnt);
extern uint64_t DelayPreScalarAndPeriod(uint64_t delay, uint64_t scaleDelay, uint32_t const pbClk, uint16_t * pPreScalar, uint16_t * pPeriod, uint32_t * pCnt);

extern bool TOStart(void);
extern bool TOInstrumentAdd(uint64_t ps, INSTR_ID id);
extern void TONullInstrument(INSTR_ID id);
 
extern bool fIsCalibrated(void);
extern STATE FileOpenFlash(void);
extern uint32_t FileCreateFileName(char const * const szPrefix, char const * const szSuffix, char const * const szExt, char * sz, uint32_t cb);
extern STATE SaveCalibrationData(VOLTYPE vol);
extern STATE ReadCalibrationData(VOLTYPE vol);

extern int64_t GetSamples(int64_t psec, int64_t xsps, uint32_t scaleSPS);
extern int64_t GetPicoSec(int64_t samp, int64_t msps, uint32_t scaleSPS);

// bookkeeping
extern STATE LEDTask(STATE ledCMD);
extern STATE SdHotSwapTask(void);
extern STATE AutoLogOnBootTask(void);

// ADC 
extern STATE    InitADC(void);
extern STATE    ADCTask(void);
extern STATE    ADCTargetsTask(void);
extern STATE    ADCRun(void);
extern uint32_t ADCStopSampling(void);
extern STATE    DeferredLogRun(void);

// Cloud Logging
// extern STATE CloudTask(void);
extern STATE ADCCloudTask(ADCTARGET& adcTrg, bool fFinish);

// HTTP
extern STATE    HTTPSetup(void);
extern void     HTTPTask(void);
extern bool     HTTPEnable(bool fEnable);
#define IsHTTPRunning() (HTTPState == HTTPEnabled && deIPcK.isIPReady())

// FT245
extern void     FIFO245Task(void);
extern uint32_t FIFO245Avaliable(void);
extern uint32_t FIFO245Read(void * pBuff, uint32_t cbBuff);
extern STATE    FIFO245Write(void * pBuff, uint32_t cbBuff, uint32_t& cbWritten, uint32_t msTimeout);

// WiFi calls
extern STATE    WiFiInit(void);
extern STATE    WiFiScan(void);
extern STATE    WiFiCalPSK(void);
extern STATE    WiFiConnect(WIFINFO& wifi);
extern STATE    WiFiAutoConnect(void);
extern STATE    WiFiDisconnect(void);
extern STATE    WiFiUpdateFW(void);
extern STATE    WiFiUpdateCert(void);

extern STATE    WiFiUpdateFirmware(void);
extern STATE    WiFiUpdateCertificate(void);

extern int32_t WiFiCreateName(char const * const szSsid, bool fAutoConnect, char * szFileName, int32_t cbFileName);

/************************************************************************/
/************************************************************************/
/******************** Tests and weak references  ************************/
/************************************************************************/
/************************************************************************/
// the task that will run all of the tests
extern STATE __attribute__((weak)) TestTasks(void);

extern STATE __attribute__((weak)) TestDDR(void);
extern STATE __attribute__((weak)) FATTest(void);
extern STATE __attribute__((weak)) TestFIFO245(void);
extern STATE __attribute__((weak)) FIFOEcho(void);

// undocumented call, aid to manufacturing test.
extern STATE __attribute__((weak)) MfgTest(int32_t testNbr, STATE& statusCode, uint32_t& msWait);

/************************************************************************/
/************************************************************************/
/*************************** DC functions  ******************************/
/************************************************************************/
/************************************************************************/
extern POLYCAL  arCalIDC[3];
extern STATE    DCCalibrate(uint32_t ch);
extern int32_t  DCPWM2mV(uint32_t iChannel, uint16_t pwm);
extern uint16_t DCmV2PWM(uint32_t iChannel, int32_t mVout);
extern void     DCSetvoltage(uint32_t iChannel, int32_t mVout);

/************************************************************************/
/************************************************************************/
/************************** AWG functions  ******************************/
/************************************************************************/
/************************************************************************/
extern AWGCAL   calIAWG;
extern STATE    AWGInit(void);
extern STATE    AWGCalibrate(void);
extern int32_t  AWGPWM2mV(uint16_t pwm);
extern uint16_t AWGmV2PWM(int32_t mVout);
extern uint32_t AWGuV2iDdac(int32_t uVFind, int32_t const rguVdac[], uint32_t cDac);
extern STATE    AWGSetRegularWaveform(AWGPARAM& awg, WAVEFORM waveform , uint32_t freqmHz, int32_t mvP2P, int32_t mvDCOffset);
extern STATE    AWGRun(AWGPARAM& awg);
extern STATE    AWGStop(void);

/************************************************************************/
/************************************************************************/
/************************** DAQ functions  ******************************/
/************************************************************************/
/************************************************************************/
extern POLYCAL  arCalIDAQ[9];
extern STATE    DAQCalibrate(uint32_t ch, uint32_t chDC);

/************************************************************************/
/************************************************************************/
/********************** External Variables  *****************************/
/************************************************************************/
/************************************************************************/

extern FILETask         fileTask;
// extern DFILE            dWiFiFile;
extern OSSerial         Serial;
extern JSONSerial       jsonSerial;
extern JSONPost         jsonPost;
extern JSONFT245        jsonFT245;
extern FLASHVOL         flashVol;
extern DSDVOL           dSDVol;
extern STATE            HTTPState;

extern DMASerial        WINCSerial;
extern DEFTask          defTask;

extern OSPAR            oslex;

extern const char       szTSTermHdr[];
extern const char       szTSNextDelta[];
extern const char       szTSEndUpdate[];
extern const char  szLogOnBoot[];


#define __PIC32MZXX__

// non C++ extern    
extern "C" {
#endif
    extern const char           szEnumVersion[] ;
    extern const char           szProgVersion[];
    extern void __attribute__((noreturn)) _softwareReset(void);
    extern void MainInit(void);

    extern STATE FBUSB5V0uV(uint32_t * puVolts);
    extern STATE FBVCC3V3uV(uint32_t * puVolts);
    extern STATE FBREF3V0uV(uint32_t * puVolts);
    extern STATE FBREF1V5uV(uint32_t * puVolts);
    extern STATE FBREF1V8uV(uint32_t * puVolts);
    extern STATE FBNUSB5V0uV(uint32_t * puVolts);
    extern STATE FBAWGorDCuV(uint32_t channelFB, int32_t * puVolts);  
       
#ifdef __cplusplus
            }
#endif

extern char const * const rgVOLNames[VOLEND];
extern uint32_t uVUSB;
extern uint32_t uV3V3;
extern uint32_t uVRef3V0;
extern uint32_t uVRef1V5;
extern uint32_t uVRef1V8;
extern uint32_t uVNUSB;
extern bool     fCalibrated;
extern bool     fSd0;
extern bool     fSd0Trans;
extern bool     fWiFiInit;

// stack variables
extern uint32_t const           _min_stack_size;
//#define in PinAssignments.h   pTopOfStack;
extern uint8_t  const *         pStackLowWater;
extern uint8_t  const * const   pBottomOfStack;

extern uint8_t  const * const   pTopOfThreadStack; 
extern uint8_t  const *         pThreadStackLowWater;
extern uint8_t  const * const   pBottomOfThreadStack;



#endif // OpenLogger_h
