/************************************************************************/
/*                                                                      */
/*    Instrument.C                                                      */
/*                                                                      */
/*    Creates and manages Instrument Handles                            */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/25/2016 (KeithV): Created                                       */
/************************************************************************/
#include <OpenLogger.h>

extern void __pic32MZ_flush_cache(void);

// WE ARE IN _on_reset()
// NO GLOBALS HAVE BEEN INITIALIZED!
// ONLY DO SFR INITIALIZATION HERE
// DO NOT DO MIPS PROCESSOR INITIALIZATION HERE AS THE CRT WILL WIPE THEM OUT!
void InitSFRs(void) 
{
    unsigned int    val;
    uint32_t        cTADWarmUp = 0;
  
    // We assume the processor is unlocked, so let's just assert that here
    // While the processor comes up in the unlocked state after reset it is
    // possible that the rest code was jumped to after someone locked the system
    // and we need to re-assert the unlock.
    SYSUNLOCK;
    CFGCONbits.IOLOCK   = 0;    // unlock PPS
    SYSLOCK;
    
    CFGCONbits.OCACLK   = 1;    // use alternate OC timers as inputs
    
//    CFGCONbits.DMAPRI   = 1;    // give the DMA priority arbitration on the bus
//    CFGCONbits.CPUPRI   = 0;    // give the CPU Least Recently Used priority arbitration on the bus

    CFGCONbits.TROEN    = 0;    // disable trace output

    CFGCONbits.TDOEN    = 1;    // use JTAG TDO output
    CFGCONbits.JTAGEN   = 0;    // enable the JTAG port
    
    CFGCONbits.IOANCPEN = 0;    // VDD >= 2.5v set to 0 (this is the charge pump for < 2.5v)              


    // Set wait states and enable prefetch buffer 
    PRECON = 0u 
            | (2u << _PRECON_PFMWS_POSITION)  // 2 wait states 
            | (3u << _PRECON_PREFEN_POSITION); // Enable prefetch for instructions + data 

// This is setup in _startup by the CRT
//    asm volatile("mfc0   %0,$13" : "=r"(val));
//    asm volatile("mtc0   %0,$13" : "+r"(val));
   
    
    // allow for debugging, this will stop the core timer when the debugger takes control
    // the CRT kills this, move to main())
//    _CP0_BIC_DEBUG(_CP0_DEBUG_COUNTDM_MASK); 
   
    // jack up PB4 (IO Bus) to 200MHz
    while(!PB4DIVbits.PBDIVRDY);        // wait to ensure we can set it.
    PB4DIVbits.PBDIV = 0;               // set to sysclock speed
//    PB4DIVbits.PBDIV = 1;             // set to sysclock/2 speed
    while(!PB4DIVbits.PBDIVRDY);        // wait until we are done.
    
    // set up some default shadow registers for each interrupt priority level
    // the shadow register set used is the same as the priority level
    PRISS = 0x76543210;

	// Turn on multi-vectored interrupts.
    INTCONSET = _INTCON_MVEC_MASK;


    //**************************************************************************
    //*******************  Turn things OFF *************************************
    //**************************************************************************

    // timers OFF
    T1CON               = 0;                    
    T2CON               = 0;                    
    T3CON               = 0;                    
    T4CON               = 0;                    
    T5CON               = 0;                    
    T6CON               = 0;                    
    T7CON               = 0;                    
    T8CON               = 0;                    
    T9CON               = 0;  

    // OC OFF
    OC1CON              = 0; 
    OC2CON              = 0; 
    OC3CON              = 0; 
    OC4CON              = 0; 
    OC5CON              = 0; 
    OC6CON              = 0; 
    OC7CON              = 0; 
    OC8CON              = 0; 
    OC9CON              = 0; 

    // DMA OFF
    DCH0CON         = 0;
    DCH1CON         = 0;
    DCH2CON         = 0;
    DCH3CON         = 0;
    DCH4CON         = 0;
    DCH5CON         = 0;
    DCH6CON         = 0;
    DCH7CON         = 0;

    // change notice off
   // CNCONE = 0;

    //**************************************************************************
    //*******************  Start with everything mapped as a digital pin  ******
    //**************************************************************************
    ANSELA              = 0;
    ANSELB              = 0;
    ANSELC              = 0;
    ANSELD              = 0;
    ANSELE              = 0;
    ANSELF              = 0;
    ANSELG              = 0;
    ANSELH              = 0;
    ANSELJ              = 0;
    ANSELK              = 0;

    //**************************************************************************
    //**************  Virtual Thread Initalization for the SD card *************
    //**************************************************************************
    if(InitThread) InitThread();    // this is a weak reference, so check it
    
    //**************************************************************************
    //**************************  Shared DMA  **********************************
    //**************************************************************************
    SHAREDDMA.DCHxCONClr        = 0xFFFFFFFF;       // clear the DMA control register
    SHAREDDMA.DCHxECONClr       = 0xFFFFFFFF;       // clear extended control register
    SHAREDDMA.DCHxINTClr        = 0xFFFFFFFF;       // clear all interrupts
    SHAREDDMA.DCHxECON.CHSIRQ   = irqDMANone;
                    
    //**************************************************************************
    //**************************  Serial I/O UART5  ****************************
    //**************************************************************************
    //  0       RC14    SOSCO/RPC14/T1CK/RC14               RX Serial Monitor       
    //  1       RD11    EMDC/RPD11/RD11                     TX Serial Monitor    
    SERIAL_PPS_URX;
    SERIAL_PPS_UTX; 

    //**************************************************************************
    //*****************************  FIFO 245  *********************************
    //**************************************************************************
    // C2INA CMP+ as analog input
    INITANSEL(FIFO245_PIN_CMPP, SET);

    // C2INC CMP- as analog input
    INITANSEL(FIFO245_PIN_CMPM, SET);

    // interrupt 4 on RB1 as input
    FIFO245_PPS_RXF_INT;                // B1 as INT4
    INITCNPU(FIFO245_PIN_RXF, SET);     // turn on pullup for INT4

    // set up the compare module
    IFS4bits.CMP2IF = 0;
    IEC4bits.CMP2IE = 0;
    IPC32bits.CMP2IP = 4;
    IPC32bits.CMP2IS = 0;

    // set up the compare module
    CM2CON              = 0;
    CM2CONbits.EVPOL    = 0b01;         // low to high
    CM2CONbits.CCH      = 0b01;         // cmp- connected to C2INC
    CM2CONbits.COE      = 0;            // turn off compare output

                                        // set up the PMP bus
    PMCON   = 0;
    PMMODE  = 0;
    PMADDR  = 0;        // single buffer master mode only
    PMAEN   = 0;
    PMSTAT  = 0;        // slave mode only
    PMWADDR = 0;        // dual buffer master mode only
    PMRADDR = 0;        // dual buffer master mode only

    // PMDIN    input and output buffer for single buffer master mode.
    // PMRDIN   input buffer for dual buffer master mode; PMDIN is the output register for dual buffer mode
    // PMDOUT   This register is used only in Slave mode for buffered output data.

    PMCONbits.DUALBUF   = 0;        // use legacy PMADDR and PMRDIN registers
    PMCONbits.SIDL      = 0;        // run PMP in idle mode.
    PMCONbits.ADRMUX    = 0;        // data address on separate pins.
    PMCONbits.PMPTTL    = 0;        // PMP module uses Schmitt Trigger input buffer
    PMCONbits.PTWREN    = 1;        // PMWR/PMENB port is disabled
    PMCONbits.PTRDEN    = 1;        // PMRD/PMWR port is enabled
    PMCONbits.CSF       = 0b10;     // PMCS2/PMCS2A and PMCS1/PMCS1A function as Chip Select 
    PMCONbits.CS2P      = 0;        // Chip Select 2/2A Polarity Active-low    
    PMCONbits.CS1P      = 0;        // Chip Select 1/1A Polarity Active-low      
    PMCONbits.WRSP      = 0;        // Master mode 2 MODE Write strobe active-low (PMWR)
    PMCONbits.RDSP      = 0;        // Master mode 2 MODE Read strobe active-low (PMRD)

    PMMODEbits.IRQM     = 0;        // No Interrupt generated
//    PMMODEbits.IRQM     = 0b01;        // int at end of read/write
    PMMODEbits.INCM     = 0;        // No increment or decrement of address
    PMMODEbits.MODE16   = 0;        // 8-bit mode: a read or write to the data register invokes a single 8-bit transfer
    PMMODEbits.MODE     = 0b10;     // Master mode 2 (PMCSx, PMRD, PMWR, PMA<x:0>, PMD<7:0> and PMD<8:15>(3))

    PMADDR              = 0x3FFF;        // set all address lines

    PMAENbits.PTEN      = 0x0000C000;   // PMA15 and PMA14 function as either PMA<15:14> or PMCS2 and PMCS1, see CSF

    // turn on PMP and CMP modules
    CM2CONbits.ON       = 1;        // turn on the compare module
    while(PMMODEbits.BUSY);
    PMCONbits.ON        = 1;        // turn on PMP

    // wait for PMP and CMP to settle
//    tPMP = ReadCoreTimer();
//    while(ReadCoreTimer() - tPMP < CORE_TMR_TICKS_PER_USEC);

    //    PMMODEbits.WAITB    = 1;        // 2 wait states (20ns > 14ns > 5 ns)
    //    PMMODEbits.WAITM    = 3;        // 4 wait states (30ns >= 30ns > 15ns > (30-20)ns)
    PMMODEbits.WAITB    = 0;        // 1 wait state (10nsec)
    PMMODEbits.WAITM    = 2;        // 3 wait states (30ns)
    PMMODEbits.WAITE    = 0;        // 0 0nsec read, 10ns write.

    //**************************************************************************
    //**************************  WINC Serial I/O UART1  ***********************
    //**************************************************************************
    //         RB5    MZ_RX / WINC_TX         
    //         RC4    MZ_TX / WINC_RX       
    WIFI_PPS_URX;   // RX RB5
    WIFI_PPS_UTX;   // TX RC4

    //**************************************************************************
    //***************************  uSD SPI Port  *******************************
    //**************************************************************************
    //  51      RD15    AN33/RPD15/SCK6/RD15                uSD SCK6            
    //  52      RD14    AN32/AETXD0/RPD14/RD14              uSD SS6             
    //  53      RD5     SQICS1/RPD5/RD5                     uSD SDI6            
    //  54      RD12    EBID12/RPD12/PMD12/RD12             uSD SDO6            
    //          RD1                                         uSD DET             

    SD_PPS_MOSI;
    SD_PPS_MISO;

    GPIO(SD_PIN_CS, SET);      // set High, not selected
    INITRIS(SD_PIN_CS, CLR);

    // SD code assumes that if the SD card is not initialized a 1 will come
    // in on the MISO pin. However some SD cards leave their SDO (MISO) pin tristate
    // until initialized and a 1 may not come in to the MCU pre-initialization
    // So turn on the weak pullup to make sure that if everyone is tristated a 1
    // will shift in on this pin.
    // In addition, the OpenScope has a 10K pullup resistor on the board
    // so technecally we don't need to do this.
    //CNPUDbits.CNPUD5    = 1;        // turn on the pullup resistor on uSD MISO
  
    // Turn on the pullup for the SD DET
    //CNPUDbits.CNPUD1 = 1;
        
    //**************************************************************************
    //*******************  GPIO PINs  ******************************************
    //**************************************************************************
    TRISFSET           = 0x00FF;           // All input s

    // ################################### TEST ONLY TO BE REMOVED!!!! $$$$$$$$$$$$$$$$$$$$$$$$$$$
    //GPIO(GPIO_3, SET);      
    //INITRIS(GPIO_3, CLR);

    //**************************************************************************
    //*******************  LEDs  and Buttons ***********************************
    //**************************************************************************

    // LED 1
    GPIO(LED_PIN_1, CLR);      
    INITRIS(LED_PIN_1, CLR);

    // LED 2
    GPIO(LED_PIN_2, CLR);      
    INITRIS(LED_PIN_2, CLR);

    // LED 3
    GPIO(LED_PIN_3, CLR);      
    INITRIS(LED_PIN_3, CLR);

    // LED 4
    GPIO(LED_PIN_4, CLR);      
    INITRIS(LED_PIN_4, CLR);

    // BUTTON 1 as in input
    INITRIS(BTN_PIN_1, SET);

    //**************************************************************************
    //*****************************  Set Pin Usage  ****************************
    //**************************************************************************
    // Make all FB input
    INITRIS(FB_PIN_DC1, SET);
    INITRIS(FB_PIN_DC2, SET);
    INITRIS(FB_PIN_AWG1, SET);
    INITRIS(FB_PIN_VREF1V5, SET);
    INITRIS(FB_PIN_VREF1V8, SET);
    INITRIS(FB_PIN_VREF3V0, SET);
    INITRIS(FB_PIN_VCC3V3, SET);
    INITRIS(FB_PIN_USB5V0, SET);
    INITRIS(FB_PIN_VSS5V0, SET);

    // make all FB analog   
    INITANSEL(FB_PIN_DC1, SET);
    INITANSEL(FB_PIN_DC2, SET);
    INITANSEL(FB_PIN_AWG1, SET);
    INITANSEL(FB_PIN_VREF1V5, SET);
    INITANSEL(FB_PIN_VREF1V8, SET);
    INITANSEL(FB_PIN_VREF3V0, SET);
    INITANSEL(FB_PIN_VCC3V3, SET);
    INITANSEL(FB_PIN_USB5V0, SET);
    INITANSEL(FB_PIN_VSS5V0, SET);

    //**************************************************************************
    //********************  Configure and turn on the ADCs  ********************
    //**************************************************************************
    
    // Initialize MCHP ADC Calibration Data
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC7CFG = DEVADC7;

    ADCCON1     = 0; 
    ADCCON2     = 0; 
    ADCCON3     = 0; 
    ADCANCON    = 0;
    ADCTRGMODE  = 0;
    ADCIMCON1   = 0x00000000;   // signed single ended mode (not differential)
    ADCIMCON2   = 0x00000000;   // signed single ended mode (not differential)
    ADCIMCON3   = 0x00000000;   // signed single ended mode (not differential)
    ADCTRGSNS   = 0;
 
    // resolution 0 - 6bits, 1 - 8bits, 2 - 10bits, 3 - 12bits
    ADCCON1bits.SELRES  =   RES12BITS;  // shared ADC, 12 bits resolution (bits+2 TADs, 12bit resolution = 14 TAD).

    // 0 - no trigger, 1 - clearing software trigger, 2 - not clearing software trigger, the rest see datasheet
    ADCCON1bits.STRGSRC     = 1;    //Global software trigger / self clearing.

    // 0 - internal 3.3, 1 - use external VRef+, 2 - use external VRef-
    ADCCON3bits.VREFSEL     = VREFHEXT;    // use extern VREF3V0

    // these should be set if VDD <= 2.5v
    // ADCCON1bits.AICPMPEN     = 1;    // turn on the analog charge pump
    
    // set up the TQ and TAD and S&H times

    // TCLK: 00- pbClk3, 01 - SysClk, 10 - External Clk3, 11 - interal 8 MHz clk
    ADCCON3bits.ADCSEL      = CLKSRCSYSCLK;             // TCLK clk == Sys Clock == F_CPU  

    // Global ADC TQ Clock: Global ADC prescaler 0 - 63; Divide by (CONCLKDIV*2) However, the value 0 means divide by 1
    ADCCON3bits.CONCLKDIV   = TQCONCLKDIV;                // Divide by 1 == TCLK == SYSCLK == F_CPU

    // must be divisible by 2 
    ADCCON2bits.ADCDIV      = ADCTADDIV;   // run TAD at 50MHz

    ADCCON2bits.SAMC        = (ADCTADSH - 2);   // for the shared S&H this will allow source impedances < 10Kohm

    // with 50MHz TAD and 68 TAD S&H and 14 TAD for 12 bit resolution, that is 25000000 / (68+14) = 609,756 Sps or 1.64 us/sample

    // initialize the warm up timer
    // 20us or 500 TAD which ever is higher 1/20us == 50KHz
    cTADWarmUp = ((F_CPU / (ADCCON3bits.CONCLKDIV == 0 ? 1 : (ADCCON3bits.CONCLKDIV * 2))) / (F_CPU / ADCTADFREQ) / 50000ul);
    if(cTADWarmUp < 500) 
    {
        cTADWarmUp = 500;
    }

    // get the next higher power of the count
    for(val=0; val<16; val++)
    {
        if((cTADWarmUp >> val) == 0)
        {
            break;
        }
    }

    // the warm up count is 2^^X where X = 0 -15
    ADCANCONbits.WKUPCLKCNT = val; // Wakeup exponent = 2^^15 * TADx   
  
    // ADC 0
    ADC0TIMEbits.ADCDIV     = ADCCON2bits.ADCDIV;       // ADC0 TAD = 50MHz
    ADC0TIMEbits.SAMC       = (ADCTADDC-2);             // ADC0 sampling time = (SAMC+2) * TAD0
    ADC0TIMEbits.SELRES     = ADCCON1bits.SELRES;       // ADC0 resolution is 12 bits 
    ADCIMCON1bits.SIGN0     = 0;                        // unsigned data

    // ADC 1
    ADC1TIMEbits.ADCDIV     = ADCCON2bits.ADCDIV;       // ADC1 TAD = 50MHz
    ADC1TIMEbits.SAMC       = (ADCTADDC-2);             // ADC1 sampling time = (SAMC+2) * TAD0
    ADC1TIMEbits.SELRES     = ADCCON1bits.SELRES;       // ADC1 resolution is 12 bits 
    ADCIMCON1bits.SIGN1     = 0;                        // unsigned data

    // ADC 2
    ADC2TIMEbits.ADCDIV     = ADCCON2bits.ADCDIV;       // ADC2 TAD = 50MHz
    ADC2TIMEbits.SAMC       = (ADCTADDC-2);             // ADC2 sampling time = (SAMC+2) * TAD0
    ADC2TIMEbits.SELRES     = ADCCON1bits.SELRES;       // ADC2 resolution is 12 bits 
    ADCIMCON1bits.SIGN2     = 0;                        // unsigned data

    // ADC 3
    ADC3TIMEbits.ADCDIV     = ADCCON2bits.ADCDIV;       // ADC3 TAD = 50MHz
    ADC3TIMEbits.SAMC       = (ADCTADDC-2);             // ADC3 sampling time = (SAMC+2) * TAD0
    ADC3TIMEbits.SELRES     = ADCCON1bits.SELRES;       // ADC3 resolution is 12 bits 
    ADCIMCON1bits.SIGN3     = 0;                        // unsigned data

    // ADC 4
    ADC4TIMEbits.ADCDIV     = ADCCON2bits.ADCDIV;       // ADC4 TAD = 50MHz
    ADC4TIMEbits.SAMC       = ADCCON2bits.SAMC;         // ADC4 sampling time = (SAMC+2) * TAD0
    ADC4TIMEbits.SELRES     = ADCCON1bits.SELRES;       // ADC4 resolution is 12 bits 
    ADCIMCON1bits.SIGN4     = 0;                        // unsigned data

    /* Configure ADCIRQENx */
    ADCCMPEN1 = 0; // No interrupts are used
    ADCCMPEN2 = 0;
    
    /* Configure ADCCSSx */
    ADCCSS1 = 0; // No scanning is used
    ADCCSS2 = 0;
    
    /* Configure ADCCMPxCON */
    ADCCMP1 = 0; // No digital comparators are used. Setting the ADCCMPxCON
    ADCCMP2 = 0; // register to '0' ensures that the comparator is disabled.
    ADCCMP3 = 0; // Other registers are ?don't care?.
    ADCCMP4 = 0;
    ADCCMP5 = 0;
    ADCCMP6 = 0;    

    /* Configure ADCFLTRx */
    ADCFLTR1 = 0; // Clear all bits
    ADCFLTR2 = 0;
    ADCFLTR3 = 0;
    ADCFLTR4 = 0;
    ADCFLTR5 = 0;
    ADCFLTR6 = 0;
    
    // disable all global interrupts
    ADCGIRQEN1 = 0;
    ADCGIRQEN2 = 0;
    
    /* Early interrupt */
    ADCEIEN1 = 0; // No early interrupt
    ADCEIEN2 = 0;

    // turn on the ADCs
    ADCCON1bits.ON = 1;

    /* Wait for voltage reference to be stable */
    while(!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
    while(ADCCON2bits.REFFLT); // Wait if there is a fault with the reference voltage
    
    /* Enable clock to analog circuit */
    ADCANCONbits.ANEN0 = 1; // Enable the clock to analog bias and digital control
    ADCANCONbits.ANEN1 = 1; // Enable the clock to analog bias and digital control
    ADCANCONbits.ANEN2 = 1; // Enable the clock to analog bias and digital control
    ADCANCONbits.ANEN3 = 1; // Enable the clock to analog bias and digital control
    ADCANCONbits.ANEN4 = 1; // Enable the clock to analog bias and digital control
    ADCANCONbits.ANEN7 = 1; // Enable the clock to analog bias and digital control
   
    /* Wait for ADC to be ready */
    while(!ADCANCONbits.WKRDY0); // Wait until ADC0 is ready
    while(!ADCANCONbits.WKRDY1); // Wait until ADC1 is ready
    while(!ADCANCONbits.WKRDY2); // Wait until ADC2 is ready
    while(!ADCANCONbits.WKRDY3); // Wait until ADC3 is ready
    while(!ADCANCONbits.WKRDY4); // Wait until ADC4 is ready
    while(!ADCANCONbits.WKRDY7); // Wait until ADC7 is ready
        
    /* Enable the ADC module */
    ADCCON3bits.DIGEN0 = 1; // Enable ADC0
    ADCCON3bits.DIGEN1 = 1; // Enable ADC1
    ADCCON3bits.DIGEN2 = 1; // Enable ADC2
    ADCCON3bits.DIGEN3 = 1; // Enable ADC3
    ADCCON3bits.DIGEN4 = 1; // Enable ADC3
    ADCCON3bits.DIGEN7 = 1; // Enable shared ADC

    // This has conflicting documentation, this is to make
    // sure we do not have early ADC interrupts enabled.
    ADCCON2bits.ADCEIOVR = 0;           // override early interrupts

    //**************************************************************************
    //*******************  Timers to drive PWM Offset circuits  ****************
    //******************** T2 drives DC Output and AWG offset timers ***********
    //**************************************************************************
    DCTMR.TxCON.TCKPS   = PWMPRESCALER;         // timer pre scaler
    DCTMR.PRx           = (PWMPERIOD - 1);      // match count
    DCTMR.TMRx          = 0;                    // init the timer
    DCTMR.TxCON.ON      = 1;                    // Turn on the timer

    //**************************************************************************
    //*******************  DCOUT 1 PWM6  ***************************************
    //**************************************************************************
    DC1_PPS_OC;
    DC1_OC.OCxCON.OCTSEL   = DCOUT_OCTSEL;      // T2 selection
    DC1_OC.OCxCON.OCM      = 0b101;             // PWM Mode
    DC1_OC.OCxR             = 0;                // go high on counter T2 wrap
    DC1_OC.OCxRS            = PWMIDEALCENTER;   // set to about zero
    DC1_OC.OCxCON.ON       = 1;                 // turn on the output compare  

    //**************************************************************************
    //*******************  DCOUT 2 PWM4  ***************************************
    //**************************************************************************
    DC2_PPS_OC;
    DC2_OC.OCxCON.OCTSEL    = DCOUT_OCTSEL;     // T2 selection
    DC2_OC.OCxCON.OCM       = 0b101;            // PWM Mode
    DC2_OC.OCxR             = 0;                // go high on counter T2 wrap
    DC2_OC.OCxRS            = PWMIDEALCENTER;   // set to about 0
    DC2_OC.OCxCON.ON        = 1;                // turn on the output compare  

    //**************************************************************************
    //*******************  DMA  ************************************************
    //**************************************************************************
    DMACONbits.ON   = 1;    // turn on the DMA module

	// enable interrupts
    // CRT kills this, move to main()
    // asm volatile("ei    %0" : "=r"(val));
    // the CRT disables interrupts

    return;
}

// This is called from main() and we can do all of the MIPS 
// and other core stuff. Here is our chance to change what the CRT does
extern const uint8_t __pic32_init_cache_data_base_addr;
void MainInit(void)
{
    uint8_t const * pCacheCur = &__pic32_init_cache_data_base_addr; // base address of the data cache
    uint8_t const * pCacheEnd;                                      // end of the data cache
    uint32_t        Config1;                                        // config 1 register, tells us the cache values
    uint32_t        DS;                                             // data cache sets, should be 512 (0x3)                    
    uint32_t        DL;                                             // data line size, should be 16 bytes (0x3)
    uint32_t        DA;                                             // data cache associativity, should be 4-way (0x3)
    uint32_t        cbCacheLine;                                    // how many bytes in a cache line, should be 16
    uint32_t        cbDataCache;                                    // how big is the data cache, should be 32768
    register uint32_t regFP __asm__("$fp");                         // get our frame pointer
    uint32_t tStart     = ReadCoreTimer();                          // got to wait before we start reading from the Serial input

	// enable interrupts
    _CP0_BIC_DEBUG(_CP0_DEBUG_COUNTDM_MASK); 
    OSEnableInterrupts();

    // Don't wipe out my current frame and don't wipe out any potential frame memset might
    // have. So clear up to our current frame less whatever memset will use
    // FYI: does not use stack space; but we are suppose to leave a min of 16 bytes by spec
    // and it looks like memset will use 12 bytes of param, RA, FP and locals
    // so lets say 0x30 for memset
    memset((void *) pBottomOfStack, 0xFF, (regFP - ((uint32_t) pBottomOfStack) - 0x30));
    pStackLowWater = (uint8_t *) (regFP - 0x30);

    // get config1 register
    __asm__ __volatile__("mfc0 %0,$16,1" : "=r" (Config1));

    // get the cache info bits
    DS = (Config1 >> 13) & 0b111;
    DL = (Config1 >> 10) & 0b111;
    DA = (Config1 >> 7) & 0b111;

    // only cache line supported is 16 bytes
    ASSERT(DL == 3);
    cbCacheLine = 16;

    // cache size is linesize * X-Way association * 64 shifted DS (the groups).
    cbDataCache = cbCacheLine * (DA+1) * (64 << DS);
    ASSERT(cbDataCache == 32768);

    // note, if the compile would let us get to the t5 and t6 registers
    // we could.....
    // register uint32_t cbDataCache __asm__("$t5");
    // register uint32_t cbCacheLine __asm__("$t6");

    // or we can do it the hard way with an ASM instruction
    // asm volatile("move %0, $13" : "=r" (cbDataCache));
    // ASSERT(cbDataCache == 32768);
    // asm volatile("move %0, $14" : "=r" (cbCacheLine));
    // ASSERT(cbCacheLine == 16);

    // op code 0b101-01 is Data-Fill-address, data cache; recommended way to invalidate a cache line in a running cache
    // also not, we set our address 0xC bytes into the cache line; thus the + cbCacheLine-4
    for(pCacheCur += (cbCacheLine - 4), pCacheEnd = pCacheCur + cbDataCache; pCacheCur < pCacheEnd; pCacheCur += cbCacheLine) _cache(0b10101, pCacheCur);
    
//    __pic32MZ_flush_cache();

    // we should be done before our time is up.
    ASSERT(ReadCoreTimer() - tStart < (500 * CORE_TMR_TICKS_PER_MSEC));
    
    // We need to wait for the Arduino IDE to open the COM port 
    // if the Serial Monitor is open after the reset
    while(ReadCoreTimer() - tStart < (500 * CORE_TMR_TICKS_PER_MSEC)); 
}

