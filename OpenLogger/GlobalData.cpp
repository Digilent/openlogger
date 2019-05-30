/************************************************************************/
/*                                                                      */
/*    GlobalData.cpp                                                    */
/*                                                                      */
/*   This file contains the Global data declarations for the            */
/*    OpenScope project                                                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    12/20/2016 (KeithV): Created                                       */
/************************************************************************/
#include <OpenLogger.h>

/************************************************************************/
/*    Stack Pointers                                                    */
/************************************************************************/
//#define in PinAssignments.h   pTopOfStack;
uint8_t const * const   pBottomOfStack  = (pTopOfStack - ((uint32_t) &_min_stack_size));
uint8_t const *         pStackLowWater  = pTopOfStack;

/************************************************************************/
/*    Power Supply voltages                                             */
/************************************************************************/
uint32_t uVUSB       = 5000000;
uint32_t uV3V3       = 3300000;
uint32_t uVRef3V0    = 3000000;
uint32_t uVRef1V5    = 1500000;
uint32_t uVRef1V8    = 1800000;
uint32_t uVNUSB      = 5000000;
bool     fCalibrated = false;
bool     fSd0        = false;
bool     fSd0Trans   = true;
bool     fWiFiInit   = false;

/************************************************************************/
/*    Power Supply voltages                                             */
/************************************************************************/
MACADDR             macMyMacAddr    = MACNONE;
IPv4                ipOpenLogger    = IPv4NONE;
        
/************************************************************************/
/*    Deferred Tasks                                                    */
/************************************************************************/
DEFTask defTask;

/************************************************************************/
/*    Enum for a test to run                                            */
/************************************************************************/
TESTS runTest = TESTNone;

/************************************************************************/
/*    Serial Object                                                     */
/************************************************************************/
// the serial buffer
uint8_t PHY __attribute__((coherent)) uartBuff[65520]; // keep at least 16 bytes below 65536

// the serial object (AND ISR)
OSSerialOBJ(Serial, ((uint8_t *) KVA_2_KSEG1(uartBuff)), sizeof(uartBuff), SERIAL_UART, SERIAL_RX_DMA, SERIAL_RX_ISR_PRI, SERIAL_DMAPRI);

/************************************************************************/
/*    FT245 public class                                                */
/************************************************************************/
// at risk as there is a KSEG1 buffer that will be init by the CRT in KSEG0
// we depend on the cache being flushed in MainInit() to insure the CRT does not hose us
JSONFT245 jsonFT245 = JSONFT245();

/************************************************************************/
/*   WINC1500 Buffer                                                    */
/************************************************************************/
uint8_t PHY __attribute__((coherent)) gmgmt[4096]; // must be a multiple of 16 and < 65520

/************************************************************************/
/*   FAT32 - Flash / SD Card Reader variables                           */
/************************************************************************/
char const * const rgVOLNames[VOLEND] = {"NONE", "sd0", "flash", "ram", "cloud"};

DFILE   dGFile;                     // general use
DFILE   dCFile;                     // reading/writing calibration data
DFILE   dLFile;                     // for logging
FILETask fileTask;                  // file task object
char const * const FILETask::rgszDoing[ENDDOING] = {"nothing", "logging", "deleting", "calibrating", "writing", "reading", "directory"};
DFILE * FILETask::rgpdFile[ENDDOING] = {NULL, &dLFile, NULL, &dCFile, &dGFile, &dGFile, NULL};   
        
DSPIPOBJ(dSDSpi, SD_SPI, SD_SPI_PRI, SD_PIN_CS);
DSDVOL dSDVol(dSDSpi);

// create the FLASH volume memory space in flash
const uint8_t __attribute__((aligned(BYTE_PAGE_SIZE), section(".flashfat"))) flashVolMem[_FLASHVOL_CBFLASH(0x20000)] = { 0xFF };
FLASHVOL    flashVol(flashVolMem, sizeof(flashVolMem));

/************************************************************************/
/*  WiFi data                                                           */
/************************************************************************/
WIFINFO     wifiActive;
WIFINFO     wifiWorking;
WFSCANSTATE wifiScanState;

/************************************************************************/
/*   Flash based eeprom                                                 */
/************************************************************************/
// get the eeprom area to place sensitive data
const uint8_t __attribute__((aligned(BYTE_PAGE_SIZE), section(".eeprom_pic32"))) eepromMem[0x10000] = { 0xFF };

/************************************************************************/
/*    Instrument Calibration Constants                                  */
/*    Results are in nVolts                                             */
/************************************************************************/
// uVDC = -40000pwm + 7000000
// B ~ -40000, C ~ 7000000
// but remember, we store (-B)
POLYCAL arCalIDC[3]     =   {
                                {CALVER, Uncalibrated, 0, 40000, 7000000},   // first entry is ideal
                                {CALVER, Uncalibrated, 0, 40000, 7000000}, 
                                {CALVER, Uncalibrated, 0, 40000, 7000000}
                            };

AWGCAL  calIAWG         =   {   {CALVER, Uncalibrated, 0, 11995, 2099125}, 511, 175, {0}, {0}  };

// uVin = (3)(5^^10)(Dadc)/((2^^10)(87)) - (3)(5^^10)(2^^5)/87
// uVin = (29,296,875)[(Dadc/89,088) - (32/87)]
// uVin = (29,296,875)(Dadc)/(89,088) - 937,500,000/87
// uVin = (328.85321)Dadc - 10,775,862
// nVin = (328,853.21)Dadc - 10,775,862,000
POLYCAL arCalIDAQ[9]    =   {
                                {CALVER, Uncalibrated, 328853, 0, 10775862000},     // first entry is ideal  
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}, 
                                {CALVER, Uncalibrated, 328853, 0, 10775862000}
                            };

// at risk as there is a KSEG1 buffer that will be init by the CRT in KSEG0
// we depend on the cache being flushed in MainInit() to insure the CRT does not hose us
LOGPARAM logIParam;

/************************************************************************/
/*    ADC logging buffers                                               */
/************************************************************************/
// make sure this is a multiple of 16 bytes. (8*2=16)
uint16_t PHY __attribute__((coherent)) adcHoldingCell[NBRLOGCH] = {0};

/************************************************************************/
/*    DDR ADC logging buffers                                           */
/************************************************************************/
// These are the only buffers in the DDR memory
// we set up the DDR memory to be coherent by address in the linker map
// the coherent attribute does not work on named sections.
// but because we specified KSEG1 addressing in the linker map, this is coherent.
//also not the DDR macro tells the CRT NOT to initialize the buffers
// this for both speed (as the DDR is big) and to prevent the CRT to inavertently put data in the cache
// but that shouldn't happen as KSEG1 address was specified in the map.
uint16_t DDR PHY adcRawBuffer[CSRBUFF];
int16_t DDR PHY adcVoltageBuffer[CSVBUFF];


