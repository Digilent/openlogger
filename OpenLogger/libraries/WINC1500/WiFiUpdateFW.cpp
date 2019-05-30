/************************************************************************/
/*                                                                      */
/*    WiFiUpdateFW.cpp                                                  */
/*                                                                      */
/*    Updates the FW on the WINC1500                                    */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    3/20/2018 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>

// this is very specific to this file; don't try to bring in programmer.h anywhere else.
extern "C"
{
#undef FLASH_START_ADDR         // conflict with MZ flash start address
#include "utility/root_cert/root_setup.h"
}
#if(FLASH_SECTOR_SZ != M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE)
#error Assume WiFi Flash Sector size is the same as RootCert cache size
#endif

static uint8_t  flashSectorBuffer[2][FLASH_SECTOR_SZ];
static STATE    state               = Idle;
static STATE    nextState           = Idle;
static uint32_t cMbFlash            = 0;
static uint32_t iFlash              = 0;
static uint32_t cbTotalFlash        = 0;
static uint32_t cbAvailableMax      = 0;
static uint32_t tReadFlashTimout    = 0;

static STATE HTTPCurState = Idle;
static bool  fSerialEnable  = false;

STATE WiFiUpdateFW(void)
{
    static uint32_t cbProgFlash         = 0;
    static uint32_t addrFlash           = 0;
    static uint32_t iSectorData         = 0;
    static bool     fClearingInput      = false;
    
    switch(state)
    {
        case Idle:
            // we want to be able to print to the output
            // but we will be locked in the parsing.
            ASSERT(jsonSerial.state == JSONListening || jsonSerial.state == Idle);
            fSerialEnable = jsonSerial.EnableIO(false);
            HTTPCurState = HTTPState;
            HTTPEnable(false);
            Serial.purge();
                
            nm_bsp_init();
            state = WIFIFlashEnterDownloadMode;

        case WIFIFlashEnterDownloadMode:
                if(m2m_wifi_download_mode() == M2M_SUCCESS)
                {
                    cMbFlash = spi_flash_get_size();
                    
                    Serial.print("The Flash is ");
                    Serial.print(cMbFlash);
                    Serial.println("Mb.");
                            
                    Serial.print("Or the Flash is ");
                    Serial.print((cMbFlash * 131072));
                    Serial.println(" Bytes");
                    Serial.println();        
                       
                    state = WIFIFlashGetFirstFlashSector; 
                }
                break;
                
        case WIFIFlashGetFirstFlashSector:
            if(spi_flash_read(flashSectorBuffer[0], 0, 4096) == M2M_SUCCESS)
            {
                Serial.println("Got the first Flash Sector");
                Serial.print("Boot flash code: ");
                Serial.print((char) flashSectorBuffer[0][0]);
                Serial.print((char) flashSectorBuffer[0][1]);
                Serial.print((char) flashSectorBuffer[0][2]);
                Serial.println((char) flashSectorBuffer[0][3]);
                memset(flashSectorBuffer[0], 0, sizeof(flashSectorBuffer[0]));
            }
            else
            {
                Serial.println("Failed to get the first flash sector");
            }
            state = WIFIFlashFlushingUART;
            break;

        case WIFIFlashFlushingUART:
            {
                Serial.println("Flushing UART buffer");
                while(Serial.available() > 0)
                {
                    Serial.read();
                }
                Serial.println("Ready to read file");

                iFlash          = 0;
                cbAvailableMax  = 0;
                cbTotalFlash    = 0;
                fClearingInput  = false;
                nextState       = WIFIFlashReadUARTData;
                state           = WIFIFlashWaitForDownload;
            }
            break;

        case WIFIFlashWaitForDownload:
            if(Serial.available() > 0)
            {
                state       = WIFIFlashReadUARTData;
            }
            break;

        // erase page
        case WIFIFlashEraseFlash:
            {
                cbProgFlash = iFlash;
                addrFlash   = cbTotalFlash - iFlash;

                ASSERT((addrFlash % 4096) == 0);
                Serial.print("Erase at flash addr: 0x");
                Serial.println(addrFlash, 16);

                // toggle sector location
                iSectorData = (iSectorData+1) % 2;               
                fClearingInput = true;
                iFlash = 0;
                state = WIFIFlashReadUARTData;  

                if(spi_flash_erase(addrFlash, 4096) != M2M_SUCCESS)
                {
                    Serial.print("Unable to erase at flash addr: 0x");
                    Serial.println(addrFlash, 16);

                    state = WIFIFlashStats;  
                }                   
            }
            break;

            // read bytes to clear buffer
        case WIFIFlashProgramFlash:
            {
                uint32_t iSectorProg = (iSectorData+1) % 2; 

                Serial.print("Program at flash addr: 0x");
                Serial.println(addrFlash, 16);
                Serial.print("Programming 0x");
                Serial.print(cbProgFlash, 16);
                Serial.println(" bytes");

                state = nextState;

                if(spi_flash_write(flashSectorBuffer[iSectorProg], addrFlash, 4096) != M2M_SUCCESS)
                {
                    Serial.print("Unable to program at flash addr: 0x");
                    Serial.println(addrFlash, 16);

                    state = WIFIFlashStats;  
                }                   
            }
            break;

        // read a page, or time out
        case WIFIFlashReadUARTData:
            {
                uint32_t cbAvailable;

                if(cbAvailableMax < (cbAvailable = Serial.available())) cbAvailableMax = cbAvailable;

                if(cbAvailable > 0)
                {
                    do
                    {
                        flashSectorBuffer[iSectorData][iFlash++] = Serial.read();
                        cbTotalFlash++;

                    } while(Serial.available() > 0 && iFlash < 4096);

                    tReadFlashTimout = ReadCoreTimer();
                }

                if(fClearingInput)
                {
                    fClearingInput = false;
                    state = WIFIFlashProgramFlash;
                }
                else if(iFlash == 4096)
                {
                    state = WIFIFlashEraseFlash;
                }                   
                else if((ReadCoreTimer() - tReadFlashTimout) > 5*CORE_TMR_TICKS_PER_SEC)
                {
                    nextState = WIFIFlashStats;
                    if(iFlash > 0)
                    {
                        state = WIFIFlashEraseFlash;
                    }
                    else
                    {
                        state = WIFIFlashStats;    
                    }                       
                }
            }
            break;

        // program while reading
        case WIFIFlashStats:
                Serial.print("Finished programming 0x");
                Serial.print(cbTotalFlash, 16);
                Serial.println(" bytes");

                Serial.print("Largest backlog of 0x");
                Serial.print(cbAvailableMax, 16);
                Serial.println(" bytes");
                
                jsonSerial.EnableIO(fSerialEnable);
                HTTPEnable(HTTPCurState != Idle);

                state = Idle;               
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
                
    }
                
    return(state);
}

STATE WiFiUpdateCert(void)
{
    switch(state)
    {
        case Idle:
            // we want to be able to print to the output
            // but we will be locked in the parsing.
            fSerialEnable = jsonSerial.EnableIO(false);
            HTTPCurState = HTTPState;
            HTTPEnable(false);
                
            nm_bsp_init();
            state = WIFIFlashEnterDownloadMode;

        case WIFIFlashEnterDownloadMode:
                if(m2m_wifi_download_mode() == M2M_SUCCESS)
                {
                    cMbFlash = spi_flash_get_size();
                    
                    Serial.print("The Flash is ");
                    Serial.print(cMbFlash);
                    Serial.println("Mb.");
                            
                    Serial.print("Or the Flash is ");
                    Serial.print((cMbFlash * 131072));
                    Serial.println(" Bytes");
                    Serial.println();        
                       
                    state = WIFIFlashGetFirstFlashSector; 
                }
                break;
                
        case WIFIFlashGetFirstFlashSector:
            if(programmer_read_cert_image(flashSectorBuffer[0]) == M2M_SUCCESS)
            {
                state = WIFIFlashFlushingUART;
            }
            else
            {
                state = Idle; 
            }
            break;

        case WIFIFlashFlushingUART:
            {
                Serial.println("Flushing UART buffer");
                while(Serial.available() > 0)
                {
                    Serial.read();
                }
                Serial.println("Ready to read DER Encoded Root Certificate file");

                iFlash          = 0;
                cbAvailableMax  = 0;
                state           = WIFIFlashWaitForDownload;
            }
            break;

        case WIFIFlashWaitForDownload:
            if(Serial.available() > 0)
            {
                state       = WIFIFlashReadUARTData;
            }
            break;

        // read a page, or time out
        case WIFIFlashReadUARTData:
            {
                uint32_t i;
                uint32_t cbAvailable = Serial.available();
                
                if(iFlash + cbAvailable >= M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE) cbAvailable = M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE - iFlash;
                
                if(cbAvailable > 0)
                {
                    if(cbAvailableMax < cbAvailable) cbAvailableMax = cbAvailable;
                    for(i=0; i<cbAvailable; i++) flashSectorBuffer[1][iFlash++] = Serial.read();
                    tReadFlashTimout = ReadCoreTimer();
                }
                
                if(iFlash >= M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE || (ReadCoreTimer() - tReadFlashTimout) > 5*CORE_TMR_TICKS_PER_SEC)
                {
                    state = WIFIFlashProgramFlash;    
                }
            }
            break;

        case WIFIFlashProgramFlash:
            {
                Serial.println("Programming Certificate");
                if(WriteRootCertificate((char *) "DUMMY", (char *) flashSectorBuffer[1], iFlash) == M2M_SUCCESS)
                {
                    Serial.println("Certificate Programmed Successfully");
                }
                else
                {
                    Serial.println("Unable to Program Certificate");                    
                }
                state = WIFIFlashStats;                                     
            }
            break;

        // program while reading
        case WIFIFlashStats:
                Serial.print("Finished programming 0x");
                Serial.print(iFlash, 16);
                Serial.println(" bytes");

                Serial.print("Largest backlog of 0x");
                Serial.print(cbAvailableMax, 16);
                Serial.println(" bytes");
                
                jsonSerial.EnableIO(fSerialEnable);
                HTTPEnable(HTTPCurState != Idle);
 
                state = Idle;               
            break;
            
        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
                
    }
                
    return(state);
}

