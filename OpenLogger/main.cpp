/************************************************************************/
/*                                                                      */
/*    main.cpp                                                          */
/*                                                                      */
/*    Main function for the OpenLogger Application                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/1/2018 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>
#include <math.h>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
    STATE           mainState   = MAINPrtHdr;
    STATE           retState    = Idle;
    int64_t         tTotal;

    // this will take around 500ms because of an intentional delay to allow
    // the COM port to turn around after programming via the bootloader.
    // also some initial configuration is done here.
    MainInit();

    while (1) {

        switch(mainState)
        {
            case MAINPrtHdr:

                Serial.begin(SERIALBAUDRATE);
                Serial.println();
                Serial.print("OpenLogger v");
                Serial.println(szProgVersion);
                Serial.println("Written by: Keith Vogel, Digilent");
                Serial.println("Copyright 2018 Digilent");
                Serial.println();

                // set up the loop stat counts
                loopStats.SetAsCount(LOOPSTATS::THREADCNT);
                loopStats.SetAsCount(LOOPSTATS::SDBUSYCNT); 
                loopStats.SetAsCount(LOOPSTATS::ADCBACKLOG);                 
                loopStats.SetAsCount(LOOPSTATS::ADCTRGBACKLOG);    
                loopStats.SetAsCount(LOOPSTATS::DBDLADCTRG);   
                loopStats.SetAsCount(LOOPSTATS::HTTPRECONNECT);   

                mainState = MAIN3V0;      
                break;

            // the references MUST come first 3V0, then 1V5
            case MAIN3V0:
                if(FBREF3V0uV(&uVRef3V0) == Idle) mainState = MAIN1V5;
                break;

            case MAIN1V5:
                if(FBREF1V5uV(&uVRef1V5) == Idle) mainState = MAIN1V8;
                break;

            case MAIN1V8:
                if(FBREF1V8uV(&uVRef1V8) == Idle) mainState = MAIN5V0;
                break;

            case MAIN5V0:
                if(FBUSB5V0uV(&uVUSB) == Idle) mainState = MAIN3V3;
                break;

            case MAIN3V3:
                if(FBVCC3V3uV(&uV3V3) == Idle) mainState = MAIN5V0N;
                break;

            case MAIN5V0N:
                if(FBNUSB5V0uV(&uVNUSB) == Idle) mainState = MAINPrtVolts;
                break;

            case MAINPrtVolts:
                Serial.print("USB+:     ");
                Serial.print(uVUSB);
                Serial.println("uV");

                Serial.print("VCC  3.3: ");
                Serial.print(uV3V3);
                Serial.println("uV");

                Serial.print("VRef 3.0: ");
                Serial.print(uVRef3V0);
                Serial.println("uV");

                Serial.print("VCC  1.8: ");
                Serial.print(uVRef1V8);
                Serial.println("uV");

                Serial.print("VRef 1.5: ");
                Serial.print(uVRef1V5);
                Serial.println("uV");

                Serial.print("VSS -4.5:-");
                Serial.print(uVNUSB);
                Serial.println("uV");
                Serial.println();

                mainState = MAINIntFlashFileVol;
                break;

    //**************************************************************************
    //**************************  Init Instruments *****************************
    //**************************************************************************
            case MAINIntFlashFileVol:

                if((retState = FileOpenFlash()) == Idle)
                {
                    Serial.println("Flash file system initialized");
                    Serial.println();
                    mainState = MAINInitADC;
                }
                else if(IsStateAnError(retState))
                {
                    Serial.print("Unable to initialize the flash file system, Error: 0x");
                    Serial.println(retState, 16);
                    Serial.println();
                    mainState = MAINInitADC;
                }     
                loopStats.RecordTime(LOOPSTATS::INITFLSHVOL);
                break;

            case MAINInitADC:
                InitADC();
                mainState = MAINInitAWG;
                break;

            case MAINInitAWG:

// look in nm_bsp.h to enable/disable
// if we are not using the AWG for printing, then set up the AWG
#if (M2M_LOG_LEVEL == M2M_LOG_NONE)  

                AWGInit();

// otherwise we are doing debug prints for the WiFi
#else

                // open the WINC seral
                //                WINCSerial.begin(115200);
                WINCSerial.begin(460800);

                // clear the buffer.
                while(WINCSerial.available()) WINCSerial.read();
#endif                
                
                mainState = MAINInitWiFi;
//                mainState = MAINRunUI;
                break;

    //**************************************************************************
    //**********************************  WIFI  ********************************
    //**************************************************************************
            case MAINInitWiFi:               
                if((retState = WiFiInit()) == Idle)
                {
//                    mainState = WIFIScan;
                    mainState = MAINInitCalibrationData;
                }
                else if(IsStateAnError(retState))
                {
                    Serial.print("Unable to Init WiFi Module, Error: 0x");
                    Serial.println(retState, 16);
                    Serial.print("Unable to read calibration data because I have no MAC address");
                    mainState = LOBAuto;
                }
                loopStats.RecordTime(LOOPSTATS::WIFIINIT);
                break;
               
            case MAINInitCalibrationData:
                if((retState = ReadCalibrationData(VOLFLASH)) == Idle)
                {
                    Serial.println("Read Calibration data from Flash");
                    Serial.println();
//                    mainState = WIFIConnect;
                    mainState = SDMounting;
                }
                else if(IsStateAnError(retState))
                {
                    Serial.println("No Calibration data was found in Flash");
                    Serial.println();
//                    mainState = WIFIConnect;
                    mainState = SDMounting;
                }     
                loopStats.RecordTime(LOOPSTATS::INITCALDATA);
                break;

                // wait for the SD card to be mounted before trying to connect
            case SDMounting:
                if(!fSd0Trans) mainState = WIFIConnect;
                break;

            case WIFIConnect:  
                if((retState = WiFiAutoConnect()) == Idle || IsStateAnError(retState))
                {
                    mainState = LOBAuto;
                }
                loopStats.RecordTime(LOOPSTATS::WIFICONN);
                break;

            case LOBAuto:

                // we will have an SD card already if there because if SD Mount; so if SD logging we can go right for it
                // we will be linked, but maybe not have an IP, so if we are doing auto log on WiFi, we need to wait for the IP address
                // internal logging we can go for it immediately
                // no logging we will exit.
                if(AutoLogOnBootTask() == Idle) mainState = MAINRunUI;
                break;

    //**************************************************************************
    //**********************  Prepare for JSON  ********************************
    //**************************************************************************
            case MAINRunUI:

                // init for JSON input
                oslex.Init(OSPAR::ICDNone);

                // Turn on Serial JSON IO processing
                jsonSerial.Purge();
                jsonSerial.EnableIO(true);
                jsonPost.Purge();
                jsonPost.EnableIO(true);
                jsonFT245.Purge();
                jsonFT245.EnableIO(true);

                LEDTask(LEDReady);

                // Say we are ready for JSON commands
                Serial.println("Ready for JSON Commands");

                // do tests or just nothing after this
                loopStats.GetStats(LOOPSTATS::SUPERLOOP, tMinInit, tAveInit, tMaxInit, tTotal);

                loopStats.ClearEntry(LOOPSTATS::SUPERLOOP);
                loopStats.ClearEntry(LOOPSTATS::SDSWAP);
                loopStats.ClearEntry(LOOPSTATS::SWITCH);
                // loopStats = LOOPSTATS();

                mainState = MAINUILoop;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);

            case MAINUILoop:
                break;
        }

    //**************************************************************************
    //****************************  LOOP TASKS  ********************************
    //**************************************************************************
        loopStats.RecordTime(LOOPSTATS::SWITCH);

        ADCTask();
        loopStats.RecordTime(LOOPSTATS::ADC);

        ADCTargetsTask();
        loopStats.RecordTime(LOOPSTATS::ADCTRG);

//        CloudTask();
//        loopStats.RecordTime(LOOPSTATS::CLOUDTRG);

        // only if we linked in the test tasks
        if(TestTasks != NULL)
        {
            TestTasks();
            loopStats.RecordTime(LOOPSTATS::TEST);
        }

        defTask.PeriodicTasks();
        loopStats.RecordTime(LOOPSTATS::DEFERRED);

        // Web Server
        HTTPTask();                                 // Keep the HTTP Server alive
        loopStats.RecordTime(LOOPSTATS::HTTP);

        // Network stack
        DEIPcK::periodicTasks();                    // Keep IP Stack alive
        loopStats.RecordTime(LOOPSTATS::DEIP);

        // service JSON serial input
        oslex.ProcessJSON(jsonSerial);
        oslex.ProcessJSON(jsonPost);
        oslex.ProcessJSON(jsonFT245);
        loopStats.RecordTime(LOOPSTATS::JSON);

        // service serial tasks
        Serial.PeriodicTask(&SERIAL_OUTPUT_DMA);    // service serial tasks    
        loopStats.RecordTime(LOOPSTATS::SERIAL);

        // FIFO245 input
        // TODO, this will be a JSON FIFO245
        jsonFT245.PeriodicTask();                              // service FIFO245 tasks
        loopStats.RecordTime(LOOPSTATS::FT245);

        // LED control
        LEDTask(Idle);
        loopStats.RecordTime(LOOPSTATS::LED);

        // SD card insertion
        SdHotSwapTask();
        loopStats.RecordTime(LOOPSTATS::SDSWAP);

        // should be last to calculate the loop statistics
        loopStats.UpdateStats();
    }

    return 0;
}



// m2m_wifi_set_system_time
// root_certificate_downloader.exe -n N File1.cer File2.cer .... FileN.