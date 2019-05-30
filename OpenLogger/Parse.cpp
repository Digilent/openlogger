/************************************************************************/
/*                                                                      */
/*    Parse.cpp                                                         */
/*                                                                      */
/*    JSON Parsing                                                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    4/2/2018(KeithV): Created                                         */
/************************************************************************/
#include <OpenLogger.h>

#if COMMENT
// Post to URI for cloud data logging
// max rate 1 every 15 seconds
{
  "channel_id": 3,
  "field1": '73',
  "field2": null,
  "field3": null,
  "field4": null,
  "field5": null,
  "field6": null,
  "field7": null,
  "field8": null,
}

STATE FnTemplate(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}
#endif 

#define QPINT0      0x00000001
#define QPINT1      0x00000002
#define QPINT2      0x00000004
#define QPINT3      0x00000008
#define QPINT4      0x00000010
#define QPINT5      0x00000020
#define QPINT6      0x00000040
#define QPINT7      0x00000080

#define QPUINT0     0x00000100
#define QPUINT1     0x00000200
#define QPUINT2     0x00000400
#define QPUINT3     0x00000800
#define QPUINT4     0x00001000
#define QPUINT5     0x00002000
#define QPUINT6     0x00004000
#define QPUINT7     0x00008000

#define QPINTLL0      (QPINT0 | QPINT1)
#define QPINTLL1      (QPINT2 | QPINT3)
#define QPINTLL2      (QPINT4 | QPINT5)
#define QPINTLL3      (QPINT6 | QPINT7)

#define QPUINTLL0     (QPUINT0 | QPUINT1)
#define QPUINTLL1     (QPUINT2 | QPUINT3)
#define QPUINTLL2     (QPUINT4 | QPUINT5)
#define QPUINTLL3     (QPUINT6 | QPUINT7)

#define QPPTR0      0x01010000
#define QPPTR1      0x02020000
#define QPPTR2      0x04040000
#define QPPTR3      0x08080000
#define QPPTR4      0x10100000
#define QPPTR5      0x20200000
#define QPPTR6      0x40400000
#define QPPTR7      0x80800000

typedef struct _QueuedParam
{
    uint32_t    iChannel;
    uint32_t    usage;      //0-7: int, 8-15:uint, 16-23; masks; 24-31 void *
    uint32_t    function;   // 32 bits used to discribe some requested function.
    union
    {
        int32_t     rgi32[8];
        int64_t     rgi64[4];
    };
    union
    {
        uint32_t    rgu32[8];
        uint64_t    rgu64[4];
    };
    uint32_t    rgcbpv32[8]; // the number of bytes pointed to by rgpv[x] 
    void *      rgpv[8];
    uint32_t    iScratch;
    uint8_t     bScratch[1024];
} QueuedParam;

static char const * const rgszStates[] = {
        "idle",
        "done",
        "waiting",
        "waiting for resources",
        "working",
        "finishing",
        "pending",
        "acquiring",
        "running",
        "stopped",
        "overflow",
        "calibrated",
        "calibrating",
        "uncalibrated",
        "failed calibration"
    };
static_assert(((sizeof(rgszStates)/sizeof(char *)) == EndOfCommonStates), "sgszStates does not have the correct number of state strings");

static QueuedParam queuedParams;
static uint32_t defaultAdcSampleFreq = 0;

/************************************************************************/
/********************** to level JSON Commands Strings ******************/
/************************************************************************/
static const char  szDevice[]       = "device";
static const char  szGND[]          = "ground";
static const char  szDC[]           = "dc";
static const char  szGPIO[]         = "gpio";
static const char  szDAQ[]          = "daq";
static const char  szAWG[]          = "awg";
static const char  szTest[]         = "test";
static const char  szUpdate[]       = "update";
static const char  szDbgPrint[]     = "debugPrint";
static const char  szMode[]         = "mode";
static const char  szClusters[]     = "clusters";
static const char  szSectors[]      = "sectors";
static const char  szBytes[]        = "bytes";
static const char  szFree[]         = "free";

/************************************************************************/
/********************** Common Strings **********************************/
/************************************************************************/
static const char  szCommand[]          = "command";
static const char  szStatus[]           = "status";
static const char  szUnsupported[]      = "unsupported";

static const char  szTrue[]             = "true";
static const char  szFalse[]            = "false";
static const char  szZero[]             = "0";
static const char  szOne[]              = "1";
static const char  szTwo[]              = "2";
static const char  szThree[]            = "3";
static const char  szFour[]             = "4";
static const char  szFive[]             = "5";
static const char  szSix[]              = "6";
static const char  szSeven[]            = "7";
static const char  szEight[]            = "8";
static const char  szSetVoltage[]       = "setVoltage";
static const char  szGetVoltage[]       = "getVoltage";
static const char  szVoltage[]          = "voltage";
static const char  szGetCurrentState[]  = "getCurrentState";

static const char  szStatusCode[]   = ",\"statusCode\":";
static const char  szState[]        = "state";

static const char szWait[]          = ",\"wait\":";
static const char szWait0[]         = ",\"wait\":0}";
static const char szWait500[]       = ",\"wait\":500}";
static const char szWaitUntil[]     = ",\"wait\":-1}";

static const char szJSONest[]           = "\":{\"";
static const char szJSONObjMember[]     = "{\"";
static const char szJSONObjEndCB[]      = "\"}";
static const char szJSONObjEndBC[]      = "},";
static const char szJSONObjEndBCQ[]      = "},\"";
static const char szJSONObjEndBB[]      = "}}";
static const char szJSOMemberSepQCB[]   = "\":[";
static const char szJSOMemberSepQCQ[]   = "\":\"";
static const char szJSOMemberSepQC[]    = "\":";
static const char szJSOValueSepQCQ[]    = "\",\"";
static const char szJSOValueSepCQ[]     = ",\"";
static const char szJSOValueSepQC[]     = "\",";
static const char szJSOValueSepBC[]     = "],";

/************************************************************************/
/***************************** GPIO  ************************************/
/************************************************************************/
/************************************************************************/
static const char szValue[]                 = "value";
static const char szDirection[]             = "direction";
static const char szInputStr[]              = "input";
static const char szOutputStr[]             = "output";
static const char szModeState[]             =  ",\"state\":\"Idle\",\"mode\":\"gpio\"";

/************************************************************************/
/***************************** LOG  *************************************/
/************************************************************************/
/************************************************************************/
char const * const rgCSEVNames[CSEVEND]     = {"NONE", "ThingSpeak"};

static const char  szSetParameters[]        = "setParameters";
static const char  szLog[]                  = "log";
static const char  szMaxSampleCount[]       = "maxSampleCount";
static const char  szAdcSampleFreq[]        = "adcSampleFreq";
       const char  szLogOnBoot[]           = "logOnBoot";
static const char  szActualAdcSampleFreq[]  = "actualAdcSampleFreq";
static const char  szSetDefaultAdcFreq[]    = "setDefaultAdcFreq";
static const char  szDefaultAdcSampleFreq[] = "defaultAdcSampleFreq";
static const char  szSampleFreq[]           = "sampleFreq";
static const char  szActualSampleFreq[]     = "actualSampleFreq";
static const char  szStartDelay[]           = "startDelay";
static const char  szActualStartDelay[]     = "actualStartDelay";
static const char  szChannels[]             = "channels";
static const char  szService[]              = "service";
static const char  szAverage[]              = "average";
static const char  szStorageLocation[]      = "storageLocation";
static const char  szURI[]                  = "uri";
static const char  szStartIndex[]           = "startIndex";
static const char  szActualCount[]          = "actualCount";
static const char  szCount[]                = "count";

// stop reasons
static const char  szStopReasonNormal[]     = " \",\"stopReason\":\"NORMAL\"";
static const char  szStopReasonOverflow[]   = " \",\"stopReason\":\"OVERFLOW\"";
static const char  szStopReasonPending[]    = " \",\"stopReason\":\"PENDING\"";
static const char  szStopReasonUnconfig[]   = " \",\"stopReason\":\"UNCONFIGURED\"";
static const char  szStopReasonUnknown[]    = " \",\"stopReason\":\"UNKNOWN\"";
static const char  szStopReasonError[]      = " \",\"stopReason\":\"ERROR\"";
static const char  szStopMessage[]          = " ,\"stopMessage\":\"";
static const char  szStopReasonForce[]      = " \",\"stopReason\":\"FORCE\"";

static const char  szPost[]                 = "POST ";
static const char  szField[]                = "field";
static const char  szTSHdr[]                = " HTTP/1.1\r\nHost: api.thingspeak.com\r\nContent-Type: application/json\r\nConnection: close\r\ncontent-length: ";
       const char  szTSTermHdr[]            = "\r\n\r\n";
#define TScbContentLen 10                    // content length can only be 6 char, need room for the null on the last space
static const char  szTSKey[]                = "{\r\n\"write_api_key\": \"";
static const char  szTSUpdate[]             = "\",\r\n\"updates\": [\r\n";
#if(TSiDelta != 12)
#error fix TSiDelta in OpenLogger.h
#endif
static const char  szTSDelta[]              = "{\"delta_t\":         ";
#if(TScbDelta != 8)
#error fix TScbDelta in OpenLogger.h
#endif
#define TSiField 10
#define TSiValue (sizeof(szTSDelta) + TSiField + 3)     // from the begining of delta to the value
#if(TSdiFValue2Value != 21)
#error fix TSdiFValue2Value in OpenLogger.h
#endif
static const char  szTSField[]              = ",\r\n \"field \":        ";
#if(TScbValue != 7)                         // value length can only be 6 char, need room for the null on the last space; value can be +/-10000
#error fix TScbValue in OpenLogger.h
#endif
       const char  szTSNextDelta[]          = "\r\n},";
       const char  szTSEndUpdate[]          = "\r\n}]}";

static LOGPARAM logParam;

/************************************************************************/
/*************************** FILE IO ************************************/
/************************************************************************/
/************************************************************************/
static const char  szFile[]                 = "file";
static const char  szType[]                 = "type";
static const char  szPath[]                 = "path";
static const char  szFilePosition[]         = "filePosition";
static const char  szActualFilePosition[]   = "actualFilePosition";
static const char  szActualFileSize[]       = "actualFileSize";
static const char  szRequestedLength[]      = "requestedLength";
static const char  szActualLength[]         = "actualLength";
static const char  szBinaryOffset[]         = "binaryOffset";
static const char  szBinaryLength[]         = "binaryLength";
static const char  szWrite[]                = "write";
static const char  szRead[]                 = "read";
static const char  szDelete[]               = "delete";
static const char  szGetFileSize[]          = "getFileSize";
static const char  szGetFreeSpace[]         = "getFreeSpace";
static const char  szListdir[]              = "listdir";
static const char  szMkFS[]                 = "makeFS";
static const char  szFlash[]                = "flash";
static const char  szSD0[]                  = "sd0";

/************************************************************************/
/******************************** AWG Commands **************************/
/************************************************************************/
static const char  szSetRegularWaveform[]   = "setRegularWaveform";
static const char  szRun[]                  = "run";
static const char  szStop[]                 = "stop";
static const char  szSignalType[]           = "signalType";
static const char  szWaveType[]             = "waveType";
static const char  szSine[]                 = "sine";
static const char  szSquare[]               = "square";
static const char  szTriangle[]             = "triangle";
static const char  szSawtooth[]             = "sawtooth";
static const char  szArbitrary[]            = "arbitrary";
static const char  szSignalFreq[]           = "signalFreq";
static const char  szActualSignalFreq[]     = "actualSignalFreq";
static const char  szVpp[]                  = "vpp";
static const char  szActualVpp[]            = "actualVpp";
static const char  szVOffset[]              = "vOffset";
static const char  szActualVOffset[]        = "actualVOffset";

static const char * rgAWGSignals[]     = {"none", szDC, szSine, szSquare, szTriangle, szSawtooth, szArbitrary};

/************************************************************************/
/************************* WiFi / NIC Commands **************************/
/*                 {"device":[{"command":"nicList"}]}                   */
/************************************************************************/
static const char szNicList[]           = "nicList";
static const char szNics[]              = ",\"nics\": [\"wlan0\"]";
static const char szNicGetStatus[]      = "nicGetStatus";
static const char szNicConnect[]        = "nicConnect";
static const char szNicDisconnect[]        = "nicDisconnect";
static const char szAdapter[]           = "adapter";
static const char szWlan0[]             = "wlan0";
static const char szNetworks[]          = "networks";
static const char szConnected[]         = "connected";
static const char szDisconnected[]      = "disconnected";
static const char szParameterSet[]      = "parameterSet";
static const char szActiveParameterSet[] = "activeParameterSet";
static const char szWorkingParameterSet[] = "workingParameterSet";
static const char szForce[]             = "force";
static const char szIpAddress[]         = "ipAddress";
static const char szReason[]            = "reason";

static const char szWiFiSetParameters[] = "wifiSetParameters";
static const char szWiFiSaveParameters[] = "wifiSaveParameters";
static const char szWiFiLoadParameters[] = "wifiLoadParameters";
static const char szWiFiDeleteParameters[] = "wifiDeleteParameters";
static const char szWiFiListParameters[] = "wifiListSavedParameters";
static const char szWiFiScan[]          = "wifiScan";
static const char szWiFiReadScan[]      = "wifiReadScannedNetworks";
static const char szSsid[]              = "ssid";
static const char szBssid[]             = "bssid";
static const char szPassphrase[]        = "passphrase";
static const char szChannel[]           = "channel";
static const char szSignalStrength[]    = "signalStrength";
static const char szSecurityType[]      = "securityType";
static const char szAutoConnect[]       = "autoConnect";
static const char szOpen[]              = "open";
static const char szWep40[]             = "wep40";
static const char szWep104[]            = "wep104";
static const char szWpa[]               = "wpa";
static const char szWpa2[]              = "wpa2";
static const char szEnterprise[]        = "enterprise";
static const char szUnknown[]           = "unknown";
static const char szKeys[]              = "keys";
static const char szKey[]               = "key";
static const char szKeyIndex[]          = "keyIndex";

/************************************************************************/
/************************* mode/Print Commands **************************/
/*                 {"mode":"JSON"}                                      */
/*                 {"mode":"menu"}                                      */
/*                 {"debugPrint":"on"}                                  */
/*                 {"debugPrint":"off"}                                 */
/************************************************************************/
static const char  szNone[]         = "none";
static const char  szOn[]           = "on";
static const char  szOff[]          = "off";
static const char  szJSON[]         = "JSON";
static const char  szMenu[]         = "menu";

/************************************************************************/
/***************************** Update Commands **************************/
/*                 {"device":[{"command":"resetInstruments"}]}          */
/*                 {"device":[{"command":"enterBootloader"}]}           */
/************************************************************************/
static const char szResetInstruments[]      = "resetInstruments";
static const char szEnterBootloader[]       = "enterBootloader";
static const char szStorageGetLocations[]   = "storageGetLocations";
static const char szStorageLocations[]      = "\"storageLocations\":[\"flash\"";

/************************************************************************/
/***************************** Update Commands **************************/
/*                 {"update":[{"command":"updateWFFW"}]}                */
/*                 {"update":[{"command":"updateWFCert"}]}              */
/************************************************************************/
static const char szUpdWFFW[]               = "updateWFFW";
static const char szUpdWFCert[]             = "updateWFCert";

/************************************************************************/
/***************************** Test Commands ****************************/
/*                 {"test":[{"command":"calPSK"}]}                      */
/*                 {"test":[{"command":"fat32"}]}                       */
/*                 {"test":[{"command":"gpio"}]}                        */
/*                 {"test":[{"command":"encoder"}]}                     */
/*                 {"test":[{"command":"ddr"}]}                         */
/*                 {"test":[{"command":"fifo245"}]}                     */
/*                 {"test":[{"command":"run","testNbr":1}]}             */
/************************************************************************/
static const char szCalPSK[]                = "calPSK";
static const char szFat32[]                 = "fat32";
static const char szTestGPIO[]              = "gpio";
static const char szTestGPIOEnd[]           = "gpioEnd";
static const char szTestEncoder[]           = "encoder";
static const char szTestEncoderEnd[]        = "encoderEnd";
static const char szTestDDR[]               = "ddr";
static const char szTestFIFO245[]           = "fifo245";
static const char szTestFIFO245End[]        = "fifo245End";
static const char szTestRun[]               = "run";
static const char szTestNbr[]               = "testNbr";
static const char szReturnNbr[]             = "returnNbr";

/************************************************************************/
/***************************** Loop Statistics **************************/
/*                 {"device":[{"command":"loopStatistics"}]}            */
/************************************************************************/
static const char  szLoopStats[]            = "loopStatistics";
static const char szMinAveMaxNS[]           = " [min,ave,max] ns\":[";
static const char szMinAveMaxCNT[]          = " [min,ave,max,total] count\":[";
static const char szInitTime[]              = ",\"Initialization Tasks [min,ave,max] ns\":[";
static const char szLastJSON[]              = ",\"Last JSON Command ns\":";
static const char szStack[]                 = ",\"Stack [used,free,total]\":[";
static const char szThreadStack[]           = "],\"SD Stack [used,free,total]\":[";
static const char szTargetDerivative[]      = "\",\"Derivative [RATE, BACKLOG]\":[";

/************************************************************************/
/********************************* Calibration **************************/
/*                {"device":[{"command":"calibrationStart"}]}           */
/************************************************************************/
static const char  szCalibrationGetInstructions[]   = "calibrationGetInstructions";
static const char  szCalibrationGetStorageTypes[]   = "calibrationGetStorageTypes";
static const char   szCalibrationStorageTypes[]     = "\"storageTypes\":[\"flash\"";
static const char  szCalibrationStart[]             = "calibrationStart";
static const char  szCalibrationRead[]              = "calibrationRead";
static const char  szCalibrationGetStatus[]         = "calibrationGetStatus";
static const char  szCalibrationSave[]              = "calibrationSave";
static const char  szCalibrationLoad[]              = "calibrationLoad";
static const char  szCalibrationData[]              = ",\"calibrationData\":{\"";

static const char  szPolynomial[]                   = ",\"voltageEquation\":\"uV = A*Dadc + B*Dpwm + C\"";
static const char  szCoefficientA[]                 = ",\"A\":[";
static const char  szCoefficientB[]                 = ",\"B\":[";
static const char  szCoefficientC[]                 = ",\"C\":[";

/************************************************************************/
/********************************* Enumeration **************************/
/*                    {"device":[{"command":"enumerate"}]}              */
/************************************************************************/
static const char  szEnumerate[]    = "enumerate";

static const char szEnumeration1[] = "\
,\
\"statusCode\":0,\
\"wait\":0,\
\"deviceMake\":\"Digilent\",\
\"deviceModel\":\"OpenLogger MZ\",\
\"firmwareVersion\":{";

static const char szEnumeration2[] = "\
},\
\"macAddress\":\"";

#if COMMENT
static const char szEnumeration3[] = "\
\",\
\"calibrationSource\":\"";
#endif

static const char szEnumeration4[] = "\
\",\
\"nics\":{\
\"wlan0\":{\"macAddress\":\"";

static const char szEnumeration5[] = "\"}},\
\"requiredCalibrationVer\":" MKSTR(CALVER) ",\
\"requiredWiFiParameterVer\":" MKSTR(WFVER) ",\
\"awg\":{\
\"numChans\":1,\
\"1\":{\
\"signalTypes\":[\
\"sine\",\
\"square\",\
\"sawtooth\",\
\"triangle\",\
\"dc\"\
],\
\"signalFreqUnits\":0.001,\
\"signalFreqMin\":50,\
\"signalFreqMax\":200000000,\
\"dataType\":\"I16\",\
\"bufferSizeMax\":32760,\
\"sampleFreqUnits\":0.001,\
\"sampleFreqMin\":1526000,\
\"sampleFreqMax\":2000000000,\
\"voltageUnits\":0.001,\
\"dacVpp\":3000,\
\"vOffsetMin\":-1500,\
\"vOffsetMax\":1500,\
\"vOutMin\":-3000,\
\"vOutMax\":3000,\
\"currentUnits\":0.001,\
\"currentMin\":0,\
\"currentMax\":20\
}\
},\
\"dc\":{\
\"numChans\":2,\
\"1\":{\
\"voltageUnits\":0.001,\
\"voltageMin\":-4000,\
\"voltageMax\":4000,\
\"voltageIncrement\":40,\
\"currentUnits\":0.001,\
\"currentMin\":0,\
\"currentMax\":50,\
\"currentIncrement\":0\
},\
\"2\":{\
\"voltageUnits\":0.001,\
\"voltageMin\":-4000,\
\"voltageMax\":4000,\
\"voltageIncrement\":40,\
\"currentUnits\":0.001,\
\"currentMin\":0,\
\"currentMax\":50,\
\"currentIncrement\":0\
}\
},\
\"gpio\":{\
\"numChans\": 8,\
\"currentUnits\":0.001,\
\"sourceCurrentMax\": 7000,\
\"sinkCurrentMax\": 12000\
},\
\"log\": {\
\"daq\": {\
\"fileFormat\":" MKSTR(LOGFMT) ",\
\"fileRevision\":" MKSTR(LOGREV) ",\
\"numChans\": 8,\
\"maxAveraging\": 32768,\
\"maxSamplesInFile\":" MKSTR(LOGMAXSAMPINFILE) ",\
\"numAdc\": 1,\
\"1\": {\
\"adcChans\": 8,\
\"adcAggregateSampleFreq\": [250000000000, 500000000000],\
\"resolution\": 16,\
\"effectiveBits\": 12,\
\"sampleDataType\": \"I16\",\
\"sampleFreqUnits\": 0.000001,\
\"sampleFreqMin\": 1,\
\"delayUnits\": 0.000000000001,\
\"delayMax\": 18446744073709551615,\
\"delayMin\": 0,\
\"voltageUnits\": 0.001,\
\"adcVpp\": 3000,\
\"gains\": [1],\
\"vOffsetMin\": 0,\
\"vOffsetMax\": 0,\
\"inputVoltageMax\": 10000,\
\"inputVoltageMin\": -10000\
},\
\"targets\": [{\
\"ram\": {\
\"bytesMax\": 16000000,\
\"sampleFreqMax\": 500000000000\
},\
\"sd0\": {\
\"bytesMax\": 4294966272,\
\"sampleFreqMax\": 50000000000\
},\
\"cloud\": {\
\"service\": [\"ThingSpeak\"],\
\"sampleFreqMax\": 66666\
}\
}]\
}\
}\
}";

static const char szCalibrationInstructions[] = "\
\"important\":\"AI1+ must be calibrated first\",\
\"step\": [\
\"Connect the Red DC1 lead next to the green lead, to the Orange AI1+ lead at the edge of the connector\",\
\"Connect the Red DC1 lead next to the green lead, to the Blue AI2+ lead next to the Orange AI1+ lead\",\
\"Connect the Red DC1 lead next to the green lead, to the Pink AI3+ lead next to the Blue AI2+ lead\",\
\"Connect the Red DC1 lead next to the green lead, to the Green AI4+ lead next to the Pink AI3+ lead\",\
\"Connect the Red DC1 lead next to the green lead, to the Gray AI5+ lead next to the Green AI4+ lead\",\
\"Connect the Red DC1 lead next to the green lead, to the Purple AI6+ lead next to the Gray AI5+ lead\",\
\"Connect the Red DC1 lead next to the green lead, to the Yellow AI7+ lead next to the Purple AI6+ lead\",\
\"Connect the Red DC1 lead next to the green lead, to the Brown AI8+ lead next to the Yellow AI7+ lead\"\
]";

static const char szCalibrationGndInstructions[] = "\
\"important\":\"Primary calibration must be proformed first\",\
\"step\": [\
\"Connect Orange AI1+ lead to the Orange+White AI1- lead\",\
\"Connect Blue AI2+ lead to the Blue+White AI2- lead\",\
\"Connect Pink AI3+ lead to the Pink+White AI3- lead\",\
\"Connect Green AI4+ lead to the Green+White AI4- lead\",\
\"Connect Gray AI5+ lead to the Gray+White AI5- lead\",\
\"Connect Purple AI6+ lead to the Purple+White AI6- lead\",\
\"Connect Yellow AI7+ lead to the Yellow+White AI7- leadd\",\
\"Connect Brown AI8+ lead to the Brown+White AI8- lead\"\
]";

/************************************************************************/
/********************** Common Functions ********************************/
/************************************************************************/
STATE FnEndOfJSON(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(Idle);
}

STATE FnError(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(JSONLexingError);
}

STATE FnEchoToken(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    switch(token)
    {
        case JSON::tokMemberName:
        case JSON::tokStringValue:
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            strncpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szInput, cbInput);
            oslex.odata[0].cb += cbInput;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            break;

        case JSON::tokObject:
        case JSON::tokEndObject:
        case JSON::tokArray:
        case JSON::tokEndArray:
        case JSON::tokNameSep:
        case JSON::tokValueSep:
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = szInput[0];
            break;

        case JSON::tokNull:       
        case JSON::tokFalse:         
        case JSON::tokTrue:
            strncpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szInput, cbInput);
            oslex.odata[0].cb += cbInput;
            break;

        default:
            break;
    }

    return(Idle);
}

STATE FnChannel(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    char *  szT = (char *) alloca(cbInput+1);

    ASSERT(token == JSON::tokMemberName || token == JSON::tokNumber);

    strncpy(szT, szInput, cbInput);
    szT[cbInput] = '\0';

    queuedParams.iChannel = atoi(szT);

    return(Idle);
}

STATE FnChannelAndEcho(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    FnChannel(token, szInput, cbInput,jEntry);
    FnEchoToken(token, szInput, cbInput, jEntry);

    return(Idle);
}

STATE FnClearQueuedValues(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    uint32_t chT = queuedParams.iChannel;

    // clear everything except the channel
    memset(&queuedParams, 0, sizeof(queuedParams));

    queuedParams.iChannel = chT;

    return(Idle);
}

STATE FnClearQueuedValuesAndEcho(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    FnClearQueuedValues(token, szInput, cbInput, jEntry);

    FnEchoToken(token, szInput, cbInput, jEntry);

    return(Idle);
}

// Record an integer
static STATE FnVAL(JSON::JSONVALUE jval, uint32_t index, char const * szInput, int32_t cbInput)
{
    char *  szT = (char *) alloca(cbInput+1);
    uint32_t mask;

    switch(jval)
    {
    case JSON::jvalINT32:
        mask = 1 << index;
        ASSERT((mask & queuedParams.usage) == 0);
        queuedParams.usage |= mask;
        strncpy(szT, szInput, cbInput);
        szT[cbInput] = '\0';
        queuedParams.rgi32[index] = atoi(szT);
        break;

    case JSON::jvalUINT32:
        mask = 1 << (sizeof(queuedParams.rgi32)/sizeof(int32_t) + index);
        ASSERT((mask & queuedParams.usage) == 0);
        queuedParams.usage |= mask;
        strncpy(szT, szInput, cbInput);
        szT[cbInput] = '\0';
        queuedParams.rgu32[index] = (uint32_t) atoi(szT);
        break;

    case JSON::jvalINT64:
        mask = 3 << index*2;
        ASSERT((mask & queuedParams.usage) == 0);
        queuedParams.usage |= mask;
        strncpy(szT, szInput, cbInput);
        szT[cbInput] = '\0';
        queuedParams.rgi64[index] = strtoll(szT, NULL, 10);
        break;

    case JSON::jvalUINT64:
        mask = 3 << (sizeof(queuedParams.rgi32)/sizeof(int32_t) + index*2);
        ASSERT((mask & queuedParams.usage) == 0);
        queuedParams.usage |= mask;
        strncpy(szT, szInput, cbInput);
        szT[cbInput] = '\0';
        queuedParams.rgu64[index] = strtoull(szT, NULL, 10);
        break;

    case JSON::jvalFUNC:
        queuedParams.function   |= (1 << index);
        break;

    // index into ptr and uint! uses both
    case JSON::jvalPTR:
        mask = 0x0101 << (sizeof(queuedParams.rgi32)/sizeof(int32_t) + sizeof(queuedParams.rgu32)/sizeof(int32_t) + index);
        ASSERT((mask & queuedParams.usage) == 0);
        queuedParams.usage |= mask;
        ASSERT((sizeof(queuedParams.bScratch) - queuedParams.iScratch) > (uint32_t) (cbInput+1));
        memcpy(&queuedParams.bScratch[queuedParams.iScratch], szInput, cbInput);
        queuedParams.rgcbpv32[index]    = cbInput+1;
        queuedParams.rgpv[index]        = (void *) &queuedParams.bScratch[queuedParams.iScratch];
        queuedParams.iScratch          += cbInput;
        queuedParams.bScratch[queuedParams.iScratch++] = 0;
        break;

    default:
        break;
    }
    
    return(Idle);
}

/************************************************************************/
/********************Common Loading Parameter functions *****************/
/************************************************************************/

const JSONENTRY rgESkipValue[] =    {   
    {JSON::tokNameSep,      NULL,   NULL,   JListNone},
    {JSON::tokNumber,       NULL,   NULL,   JListPop},
    {JSON::tokStringValue,  NULL,   NULL,   JListPop},
    {JSON::tokTrue,         NULL,   NULL,   JListPop},
    {JSON::tokFalse,        NULL,   NULL,   JListPop},
    {JSON::tokNull,         NULL,   NULL,   JListPop}
};
const JSONLIST jLSkipValue = {sizeof(rgESkipValue) / sizeof(JSONENTRY), rgESkipValue};

STATE FnPTR0(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 0, szInput, cbInput));
}
const JSONENTRY rgEPtr0[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR0,     JListPop}
};
const JSONLIST jLPTR0 = {sizeof(rgEPtr0) / sizeof(JSONENTRY), rgEPtr0};

STATE FnPTR1(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 1, szInput, cbInput));
}
const JSONENTRY rgEPtr1[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR1,     JListPop}
};
const JSONLIST jLPTR1 = {sizeof(rgEPtr1) / sizeof(JSONENTRY), rgEPtr1};

STATE FnPTR2(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 2, szInput, cbInput));
}
const JSONENTRY rgEPtr2[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR2,     JListPop}
};
const JSONLIST jLPTR2 = {sizeof(rgEPtr2) / sizeof(JSONENTRY), rgEPtr2};

STATE FnPTR3(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 3, szInput, cbInput));
}
const JSONENTRY rgEPtr3[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR3,     JListPop}
};
const JSONLIST jLPTR3 = {sizeof(rgEPtr3) / sizeof(JSONENTRY), rgEPtr3};

STATE FnPTR4(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 4, szInput, cbInput));
}
const JSONENTRY rgEPtr4[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR4,     JListPop}
};
const JSONLIST jLPTR4 = {sizeof(rgEPtr4) / sizeof(JSONENTRY), rgEPtr4};

STATE FnPTR5(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 5, szInput, cbInput));
}
const JSONENTRY rgEPtr5[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR5,     JListPop}
};
const JSONLIST jLPTR5 = {sizeof(rgEPtr5) / sizeof(JSONENTRY), rgEPtr5};

STATE FnPTR6(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 6, szInput, cbInput));
}
const JSONENTRY rgEPtr6[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR6,     JListPop}
};
const JSONLIST jLPTR6 = {sizeof(rgEPtr6) / sizeof(JSONENTRY), rgEPtr6};

STATE FnPTR7(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalPTR, 7, szInput, cbInput));
}
const JSONENTRY rgEPtr7[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  NULL,       FnPTR7,     JListPop}
};
const JSONLIST jLPTR7 = {sizeof(rgEPtr7) / sizeof(JSONENTRY), rgEPtr7};

STATE Fn0INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 0, szInput, cbInput));
}
const JSONENTRY rgEInt0[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn0INT32,   JListPop}
};
const JSONLIST jLInt0 = {sizeof(rgEInt0) / sizeof(JSONENTRY), rgEInt0};

STATE Fn1INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 1, szInput, cbInput));
}
const JSONENTRY rgEInt1[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn1INT32,   JListPop}
};
const JSONLIST jLInt1 = {sizeof(rgEInt1) / sizeof(JSONENTRY), rgEInt1};

STATE Fn2INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 2, szInput, cbInput));
}
const JSONENTRY rgEInt2[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn2INT32,   JListPop}
};
const JSONLIST jLInt2 = {sizeof(rgEInt2) / sizeof(JSONENTRY), rgEInt2};

STATE Fn3INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 3, szInput, cbInput));
}
const JSONENTRY rgEInt3[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn3INT32,   JListPop}
};
const JSONLIST jLInt3 = {sizeof(rgEInt3) / sizeof(JSONENTRY), rgEInt3};

STATE Fn4INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 4, szInput, cbInput));
}
const JSONENTRY rgEInt4[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn4INT32,   JListPop}
};
const JSONLIST jLInt4 = {sizeof(rgEInt4) / sizeof(JSONENTRY), rgEInt4};

STATE Fn5INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 5, szInput, cbInput));
}
const JSONENTRY rgEInt5[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn5INT32,   JListPop}
};
const JSONLIST jLInt5 = {sizeof(rgEInt5) / sizeof(JSONENTRY), rgEInt5};

STATE Fn6INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 6, szInput, cbInput));
}
const JSONENTRY rgEInt6[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn6INT32,   JListPop}
};
const JSONLIST jLInt6 = {sizeof(rgEInt6) / sizeof(JSONENTRY), rgEInt6};

STATE Fn7INT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT32, 7, szInput, cbInput));
}
const JSONENTRY rgEInt7[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn7INT32,   JListPop}
};
const JSONLIST jLInt7 = {sizeof(rgEInt7) / sizeof(JSONENTRY), rgEInt7};

STATE Fn0INT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT64, 0, szInput, cbInput));
}
const JSONENTRY rgEIntLL0[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn0INT64,   JListPop}
};
const JSONLIST jLIntLL0 = {sizeof(rgEIntLL0) / sizeof(JSONENTRY), rgEIntLL0};

STATE Fn1INT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT64, 1, szInput, cbInput));
}
const JSONENTRY rgEIntLL1[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn1INT64,   JListPop}
};
const JSONLIST jLIntLL1 = {sizeof(rgEIntLL1) / sizeof(JSONENTRY), rgEIntLL1};

STATE Fn2INT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT64, 2, szInput, cbInput));
}
const JSONENTRY rgEIntLL2[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn2INT64,   JListPop}
};
const JSONLIST jLIntLL2 = {sizeof(rgEIntLL2) / sizeof(JSONENTRY), rgEIntLL2};

STATE Fn3INT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalINT64, 3, szInput, cbInput));
}
const JSONENTRY rgEIntLL3[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn3INT64,   JListPop}
};
const JSONLIST jLIntLL3 = {sizeof(rgEIntLL3) / sizeof(JSONENTRY), rgEIntLL3};

STATE Fn0UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 0, szInput, cbInput));
}
const JSONENTRY rgEUInt0[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn0UINT32,  JListPop}
};
const JSONLIST jLUInt0 = {sizeof(rgEUInt0) / sizeof(JSONENTRY), rgEUInt0};

STATE Fn1UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 1, szInput, cbInput));
}
const JSONENTRY rgEUInt1[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn1UINT32,   JListPop}
};
const JSONLIST jLUInt1 = {sizeof(rgEUInt1) / sizeof(JSONENTRY), rgEUInt1};

STATE Fn2UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 2, szInput, cbInput));
}
const JSONENTRY rgEUInt2[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn2UINT32,   JListPop}
};
const JSONLIST jLUInt2 = {sizeof(rgEUInt2) / sizeof(JSONENTRY), rgEUInt2};

STATE Fn3UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 3, szInput, cbInput));
}
const JSONENTRY rgEUInt3[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn3UINT32,   JListPop}
};
const JSONLIST jLUInt3 = {sizeof(rgEUInt3) / sizeof(JSONENTRY), rgEUInt3};

STATE Fn4UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 4, szInput, cbInput));
}
const JSONENTRY rgEUInt4[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn4UINT32,   JListPop}
};
const JSONLIST jLUInt4 = {sizeof(rgEUInt4) / sizeof(JSONENTRY), rgEUInt4};

STATE Fn5UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 5, szInput, cbInput));
}
const JSONENTRY rgEUInt5[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn5UINT32,   JListPop}
};
const JSONLIST jLUInt5 = {sizeof(rgEUInt5) / sizeof(JSONENTRY), rgEUInt5};

STATE Fn6UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 6, szInput, cbInput));
}
const JSONENTRY rgEUInt6[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn6UINT32,   JListPop}
};
const JSONLIST jLUInt6 = {sizeof(rgEUInt6) / sizeof(JSONENTRY), rgEUInt6};

STATE Fn7UINT32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT32, 7, szInput, cbInput));
}
const JSONENTRY rgEUInt7[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn7UINT32,   JListPop}
};
const JSONLIST jLUInt7 = {sizeof(rgEUInt7) / sizeof(JSONENTRY), rgEUInt7};

STATE Fn0UINT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT64, 0, szInput, cbInput));
}
const JSONENTRY rgEUIntLL0[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokNumber,       NULL,       Fn0UINT64,  JListPop}
};
const JSONLIST jLUIntLL0 = {sizeof(rgEUIntLL0) / sizeof(JSONENTRY), rgEUIntLL0};

STATE Fn1UINT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT64, 1, szInput, cbInput));
}
const JSONENTRY rgEUIntLL1[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,        JListNone},
    {JSON::tokNumber,       NULL,       Fn1UINT64,   JListPop}
};
const JSONLIST jLUIntLL1 = {sizeof(rgEUIntLL1) / sizeof(JSONENTRY), rgEUIntLL1};

STATE Fn2UINT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT64, 2, szInput, cbInput));
}
const JSONENTRY rgEUIntLL2[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,        JListNone},
    {JSON::tokNumber,       NULL,       Fn2UINT64,   JListPop}
};
const JSONLIST jLUIntLL2 = {sizeof(rgEUIntLL2) / sizeof(JSONENTRY), rgEUIntLL2};

STATE Fn3UINT64(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalUINT64, 3, szInput, cbInput));
}
const JSONENTRY rgEUIntLL3[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,        JListNone},
    {JSON::tokNumber,       NULL,       Fn3UINT64,   JListPop}
};
const JSONLIST jLUIntLL3 = {sizeof(rgEUIntLL3) / sizeof(JSONENTRY), rgEUIntLL3};

STATE FnFunc0(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 0, szInput, cbInput));

}
STATE FnFunc1(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 1, szInput, cbInput));

}
STATE FnFunc2(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 2, szInput, cbInput));

}
STATE FnFunc3(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 3, szInput, cbInput));

}
STATE FnFunc4(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 4, szInput, cbInput));

}
STATE FnFunc5(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 5, szInput, cbInput));

}
STATE FnFunc6(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 6, szInput, cbInput));

}
STATE FnFunc7(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 7, szInput, cbInput));

}
STATE FnFunc8(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 8, szInput, cbInput));

}
STATE FnFunc9(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 9, szInput, cbInput));

}
STATE FnFunc10(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 10, szInput, cbInput));

}
STATE FnFunc11(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 11, szInput, cbInput));

}
STATE FnFunc12(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 12, szInput, cbInput));

}
STATE FnFunc13(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 13, szInput, cbInput));

}
STATE FnFunc14(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 14, szInput, cbInput));

}
STATE FnFunc15(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 15, szInput, cbInput));

}
STATE FnFunc16(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 16, szInput, cbInput));

}
STATE FnFunc17(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 17, szInput, cbInput));

}
STATE FnFunc18(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 18, szInput, cbInput));

}
STATE FnFunc19(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 19, szInput, cbInput));

}
STATE FnFunc20(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 20, szInput, cbInput));

}
STATE FnFunc21(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 21, szInput, cbInput));

}
STATE FnFunc22(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 22, szInput, cbInput));

}
STATE FnFunc23(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 23, szInput, cbInput));

}
STATE FnFunc24(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 24, szInput, cbInput));

}
STATE FnFunc25(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 25, szInput, cbInput));

}
STATE FnFunc26(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 26, szInput, cbInput));

}
STATE FnFunc27(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 27, szInput, cbInput));

}
STATE FnFunc28(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 28, szInput, cbInput));

}
STATE FnFunc29(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 29, szInput, cbInput));

}
STATE FnFunc30(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 30, szInput, cbInput));

}
STATE FnFunc31(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    return(FnVAL(JSON::jvalFUNC, 31, szInput, cbInput));

}

/************************************************************************/
/******************************* GPIO ***********************************/
/************************************************************************/
#if COMMENT
{
   "gpio":{
      "1":[
         {
            "command":"write",
            "value":1
         }
      ]
   }
}
#endif

STATE FnGPIOWrite(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // func bit 1, 0 = input, 1 = output
    uint16 mask = 0x1 << queuedParams.iChannel;

    // GPIO 6 and 7 don't map 1-1
    if(queuedParams.iChannel == 6) mask = mask << 2;
    else if(queuedParams.iChannel == 7) mask = mask << 6;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // if the bit is tristated, and output doesn't make sense
    if((TRISF & mask) != 0)
    {
        utoa(GPIONotOutput, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    // all is good
    else
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

        if((queuedParams.function & 0x1) == 0x1)    LATFSET = mask;
        else                                        LATFCLR = mask;
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}
#if COMMENT

{
   "gpio":{
      "1":[
         {
            "command":"setParameters",
            "direction":"input"
         }
      ],
      "2":[
         {
            "command":"setParameters",
            "direction":"output"
         }
      ]
   }
}
#endif

STATE FnGPIOSetParameters(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // func bit 1, 0 = input, 1 = output
    uint16 mask = 0x1 << queuedParams.iChannel;

    // GPIO 6 and 7 don't map 1-1
    if(queuedParams.iChannel == 6) mask = mask << 2;
    else if(queuedParams.iChannel == 7) mask = mask << 6;

    // always clear the latch
    LATFCLR = mask;

    // if output, clear the tris bit
    if((queuedParams.function & 0x1) == 0x1)    TRISFCLR = mask;

    // if input, set the tris bit
    else                                        TRISFSET = mask;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnGPIORead(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
   // func bit 1, 0 = input, 1 = output
    uint16 mask = 0x1 << queuedParams.iChannel;

    // GPIO 6 and 7 don't map 1-1
    if(queuedParams.iChannel == 6) mask = mask << 2;
    else if(queuedParams.iChannel == 7) mask = mask << 6;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // mode and state
    if((queuedParams.function & 0x2) == 0x2)
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szModeState, sizeof(szModeState)-1); 
        oslex.odata[0].cb += sizeof(szModeState)-1;
    }

    // direction
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDirection, sizeof(szDirection)-1); 
    oslex.odata[0].cb += sizeof(szDirection)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    // if input
    if((TRISF & mask) == mask)
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szInputStr, sizeof(szInputStr)-1); 
        oslex.odata[0].cb += sizeof(szInputStr)-1;
    }  // else output
    else
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szOutputStr, sizeof(szOutputStr)-1); 
        oslex.odata[0].cb += sizeof(szOutputStr)-1;
    }

    // value
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szValue, sizeof(szValue)-1); 
    oslex.odata[0].cb += sizeof(szValue)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;

    // input is a 1
    if((PORTF & mask) == mask)
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szOne, sizeof(szOne)-1); 
        oslex.odata[0].cb += sizeof(szOne)-1;
    }
    else    // else zero
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szZero, sizeof(szZero)-1); 
        oslex.odata[0].cb += sizeof(szZero)-1;
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnGPIOGetCurrentState(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x2;                           // want mode and state
    return(FnGPIORead(token, szInput, cbInput, jEntry));
}


/************************************************************************/
/******************************* WiFi Stop ******************************/
/************************************************************************/
#if COMMENT
{
   "device":[
      {
         "command":"nicList"
      }
   ]
}
#endif

STATE FnNicList(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // list of nics, wlan0 only
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szNics, sizeof(szNics)-1); 
    oslex.odata[0].cb += sizeof(szNics)-1;

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

#if 0
{
   "device":[
      {
         "command":"nicGetStatus",
         "adapter":"wlan0"
      }
   ]
}
#endif
STATE FnNicGetStatus(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // adapter
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szAdapter, sizeof(szAdapter)-1); 
    oslex.odata[0].cb += sizeof(szAdapter)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWlan0, sizeof(szWlan0)-1); 
    oslex.odata[0].cb += sizeof(szWlan0)-1;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    // if we are connected, print out the stuff
    if(deIPcK.isLinked())
    {
        IPv4 ip;

        // connected
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szConnected, sizeof(szConnected)-1); 
        oslex.odata[0].cb += sizeof(szConnected)-1;

        // IP Address
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szIpAddress, sizeof(szIpAddress)-1); 
        oslex.odata[0].cb += sizeof(szIpAddress)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

        // get my IP if any
        if(deIPcK.getMyIP(ip))
        {
            char szIp[32];
            uint32_t cbIP = GetNumb(ip.u8, 4, '.', szIp);

            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szIp, cbIP); 
            oslex.odata[0].cb += cbIP;
        }
        else
        {
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szNone, sizeof(szNone)-1); 
            oslex.odata[0].cb += sizeof(szNone)-1;
        }
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    }

    // otherwise disconnected
    else
    {
        // disconnected
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDisconnected, sizeof(szDisconnected)-1); 
        oslex.odata[0].cb += sizeof(szDisconnected)-1;

        // disconnect reason
	    // M2M_ERR_SCAN_FAIL        = 1,    Indicate that the WINC board has failed to perform the scan operation.
	    // M2M_ERR_JOIN_FAIL        = 2,    Indicate that the WINC board has failed to join the BSS .
	    // M2M_ERR_AUTH_FAIL        = 3,  	Indicate that the WINC board has failed to authenticate with the AP.
	    // M2M_ERR_ASSOC_FAIL       = 4,    Indicate that the WINC board has failed to associate with the AP.
	    // M2M_ERR_CONN_INPROGRESS  = 5,    Indicate that the WINC board has another connection request in progress.

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szReason, sizeof(szReason)-1); 
        oslex.odata[0].cb += sizeof(szReason)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        utoa(wifiActive.lastErrState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    if(wifiActive.ssid[0] != '\0')
    {
        // SSID
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSsid, sizeof(szSsid)-1); 
        oslex.odata[0].cb += sizeof(szSsid)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
        strcpy((char *)&oslex.odata[0].pbOut[oslex.odata[0].cb], wifiActive.ssid); 
        oslex.odata[0].cb += strlen(wifiActive.ssid);

        // Security Type
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSecurityType, sizeof(szSecurityType)-1); 
        oslex.odata[0].cb += sizeof(szSecurityType)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

        switch(wifiActive.wifiType)
        {

            case DEWF_SECURITY_OPEN:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szOpen); 
                break;

//            case DEWF_SECURITY_WEP:
            case DEWF_SECURITY_WEP_40:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWep40); 
                break;

            case DEWF_SECURITY_WEP_104:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWep104); 
                break;

//            case DEWF_SECURITY_WPA:
            case DEWF_SECURITY_WPA_WITH_KEY:
            case DEWF_SECURITY_WPA_WITH_PASS_PHRASE:
            case DEWF_SECURITY_WPA_AUTO_WITH_KEY:
            case DEWF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWpa); 
                break;
 
            case DEWF_SECURITY_WPA2_WITH_KEY:
            case DEWF_SECURITY_WPA2_WITH_PASS_PHRASE:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWpa2); 
                break;

            case DEWF_SECURITY_WPS_PUSH_BUTTON:
            case DEWF_SECURITY_WPS_PIN:
            case DEWF_SECURITY_ENTERPRISE:   
            default:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szUnsupported); 
                break;
        }
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

#if 0
{  
   "device":[  
      {  
         "command":"nicConnect",
         "adapter":"wlan0",
         "parameterSet":"activeParameterSet",
         "force":true
      }
   ]
}
#endif
STATE FnNicConnect(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    char const *    szWait      = szWait0;
    STATE           retState    = Idle;
    bool            fDisconnect = ((queuedParams.function & 0b01) == 0b01);

    WIFINFO& wifiParamSet = (queuedParams.function & 0b10) == 0 ? wifiActive : wifiWorking;

    // if no SSID, we have an error
    if(!fWiFiInit)                                          retState    = WiFiNoNetworksFound;
    else if(wifiParamSet.ssid[0] == '\0')                   retState    = NoSSIDConfigured;
    else if(!fDisconnect && deIPcK.isLinked())              retState    = InstrumentInUse;
    else                                                    szWait      = szWaitUntil;

    // good to go
    if(retState == Idle)
    {
        WIFINFO * pWiFi = &wifiParamSet;

        // if we have a disconnect, put it on the list first
        if(fDisconnect)
        {
            defTask.QueTask(DEFTask::WIFIDisconnect, DEFTask::UNTIL);
        }

        // put the connect on as well
        // &pWiFi pointer to pointer, because the pointer value will be copied
        defTask.QueTask(DEFTask::WIFIConnect, &pWiFi, sizeof(WIFINFO *), DEFTask::UNTIL);
    }

    // JSON response

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // the error code
    utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // wait 0 or -1
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, strlen(szWait)); 
    oslex.odata[0].cb += strlen(szWait);

    return(Idle);
}

#if 0
{
   "device":[
      {
         "command":"nicDisconnect",
         "adapter":"wlan0"
      }
   ]
}
#endif
STATE FnNicDisconnect(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // if connected, disconnect
    if(deIPcK.isLinked())
    {
        defTask.QueTask(DEFTask::WIFIDisconnect, DEFTask::UNTIL);

        // Wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;
    }

    // not connected
    else
    {
        // Wait 0
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
    }

    return(Idle);
}

#if 0
{
   "device":[
      {
         "command":"wifiSetParameters",
         "ssid":"HorseFeathers",         
         "securityType":"wpa2",
         "passphrase":"swordfish",
         "autoConnect":true
      }
   ]
}

// response
{
   "device":[
      {
         "command":"wifiSetParameters",
         "statusCode":0,
         "wait":0
      }
   ]
}
#endif
STATE FnWiFiSetParam(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    STATE    statusCode = Idle;
    uint32_t func       = queuedParams.function & 0x1E;

    // check the parameters
    if( (queuedParams.usage & QPPTR0) == 0                                                          ||              // ssid
        !(func == 0b00000 ||func == 0b00010 ||func == 0b00100 ||func == 0b01000 ||func == 0b10000)  ||              // only one type of security type
        ((func == 0b01000 ||func == 0b10000) && (queuedParams.usage & QPPTR1) == 0)                 ||              // wpa or wpa2, need a passphrase
        queuedParams.rgu32[0] > 5                                                                   )               // key index can't be greater than 5
    {
        statusCode = InvalidParameter;
    }

    // we don't support WEP yet
    else if(func == 0b00010 ||func == 0b00100)
    {
        statusCode = Unimplemented;
    }

    // all is good
    else
    {
        // get the SSID
        memcpy(wifiWorking.ssid, queuedParams.rgpv[0], queuedParams.rgcbpv32[0]);
        wifiWorking.ssid[queuedParams.rgcbpv32[0]] = '\0';

        // auto connect
        wifiWorking.fAutoConnect = (queuedParams.function & 0x1) == 0x1;

        // what is our WiFi protocol?
        switch(func)
        {
            case 0b00000:
                wifiWorking.wifiType = DEWF_SECURITY_OPEN;
                break;

            case 0b00010:
                wifiWorking.wifiType = DEWF_SECURITY_WEP_40;
                wifiWorking.keyIndex = queuedParams.rgu32[0];
                break;

            case 0b00100:
                wifiWorking.wifiType = DEWF_SECURITY_WEP_104;
                wifiWorking.keyIndex = queuedParams.rgu32[0];
                break;

            case 0b01000:
            case 0b10000:
                wifiWorking.wifiType = DEWF_SECURITY_WPA_WITH_PASS_PHRASE;
                memcpy(wifiWorking.passphrase, queuedParams.rgpv[1], queuedParams.rgcbpv32[1]);
                wifiWorking.passphrase[queuedParams.rgcbpv32[1]] = '\0';
                break;
#if 0
            case 0b10000:
                wifiWorking.wifiType = DEWF_SECURITY_WPA2_WITH_PASS_PHRASE;
                memcpy(wifiWorking.passphrase, queuedParams.rgpv[1], queuedParams.rgcbpv32[1]);
                wifiWorking.passphrase[queuedParams.rgcbpv32[1]] = '\0';
                break;
#endif

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // the error code
    utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;
 
    return(Idle);
}

#if 0
{
   "device":[
      {
         "command":"wifiSaveParameters",
         "storageLocation":"flash"
      }
   ]
}

// response
{
   "device":[
      {
         "command":"wifiSaveParameters",
         "statusCode":0,
         "wait":0
      }
   ]
}
#endif
STATE FnWiFiSaveParam(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static STATE    statusCode  = Idle;
    uint32_t        func        = queuedParams.function & 0x3;      // 1 is flash, 2 sd0
    FRESULT         fr          = FR_OK;
    uint32_t        cbWritten   = 0;

    switch(curState)
    {
        case Idle:
            // assume an error
            curState = Done;

            // bad parameters sent
            if(func == 0 || func == 3)
            {
                statusCode = InvalidParameter;
            }
            else if((func & 2) == 2 && !fSd0)
            {
                statusCode = NoSDCard;
            }
            // no SSID give, not set up
            else if(wifiActive.ssid[0] == '\0')
            {
                statusCode = NoSSIDConfigured;
            }

            // did not connect
            else if(IsStateAnError(wifiActive.lastErrState))
            {
                statusCode = wifiActive.lastErrState;
            }

            else
            {
                char szFileName[64];

                ASSERT(wifiActive.passphrase[0] == '\0');

                // make the other file, if exists we will delete
                if( WiFiCreateName(wifiActive.ssid, !wifiActive.fAutoConnect, szFileName, sizeof(szFileName)) > 0                       &&
                   (queuedParams.rgpv[7] = (void *) fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)) != NULL   )
                {
                    curState = FILEexist;
                }
                else
                {
                    statusCode = InvalidFileName;
                }
            }
            break;

        case FILEexist:
            if((fr = DFATFS::fsexists((char *) queuedParams.rgpv[7])) == FR_OK)
            {
                curState = Done;
                if(fileTask.SetUsage(FILETask::DELETING, (func & 1) == 1 ? VOLFLASH : VOLSD, (char *) queuedParams.rgpv[7]))    curState = FILEdelete;
                else                                                                                                            statusCode = FileInUse;     
            }

            // That file didn't work out, see what to do next
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                char szFileName[64];

                // we don't want an error
                fr = FR_OK;

                // create the file we want to write
                if( WiFiCreateName(wifiActive.ssid, wifiActive.fAutoConnect, szFileName, sizeof(szFileName)) > 0            &&
                    fileTask.SetUsage(FILETask::WRITING, (func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)                    )
                {
                    curState = FILEopen;
                }
                else
                {
                    statusCode = FileInUse;
                }
            }
            break;

        case FILEdelete:
            if((fr = DFATFS::fsunlink((char *) queuedParams.rgpv[7])) == FR_OK)
            {
                char szFileName[64];

                // we successfully deleted a file
                queuedParams.rgu32[6] = 1;
                fileTask.ClearUsage(FILETask::DELETING);

                // now make the file we want
                if( WiFiCreateName(wifiActive.ssid, wifiActive.fAutoConnect, szFileName, sizeof(szFileName)) > 0            &&
                    fileTask.SetUsage(FILETask::WRITING, (func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)                    )
                {
                    curState = FILEopen;
                }
                else
                {
                    statusCode = FileInUse;
                }
            }
            break;

        case FILEopen:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsopen(fileTask.GetPath(FILETask::WRITING, true), FA_CREATE_ALWAYS | FA_READ | FA_WRITE)) == FR_OK)
            {
                curState = FILEseek;
            }
            break;

        case FILEseek:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fslseek(0)) == FR_OK)
            {
                wifiActive.wifiVer  = WFVER;
                wifiActive.cbHeader = offsetof(WIFINFO, lastErrState);
                curState    = FILEwrite;
            }
            break;

        // write out the header and then leave the file posistion after the header.
        case FILEwrite:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fswrite(&wifiActive, wifiActive.cbHeader, &cbWritten)) == FR_OK)
            {
                if(cbWritten != wifiActive.cbHeader) fr = FR_NOT_ENOUGH_CORE;
                curState = FILEclose;
            }            
            break;

        // on an error we will close
        case FILEclose:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsclose()) == FR_OK)
            {
                fileTask.ClearUsage(FILETask::WRITING);
                curState = Done;
            }
            break;

        case Done:

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // the error code
            utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
 
            // clean up the states
            curState    = Idle;
            statusCode  = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // file error conditions
    if(fr != FR_OK && fr != FR_WAITING_FOR_THREAD)
    {
        statusCode = MkStateAnError(FILECSTATE | fr);

        switch(curState)
        {
            case Idle:
            case Done:
            case FILEexist:
                curState = Done;
                break;

            case FILEdelete:
                fileTask.ClearUsage(FILETask::DELETING);
                curState = Done;
                break;

            case FILEopen:
            case FILEclose:
                fileTask.ClearUsage(FILETask::WRITING);
                curState = Done;
                break;

            case FILEseek:
            case FILEwrite:
                curState = FILEclose;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    return(curState);
}

#if 0
{
   "device":[
      {
         "command":"wifiLoadParameters",         
         "storageLocation":"flash",
         "ssid":"HorseFeathers"
      }
   ]
}

// response
{
   "device":[
      {
         "command":"wifiLoadParameters",
         "statusCode":0,
         "wait":0
      }
   ]
}
#endif
STATE FnWiFiLoadParam(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static STATE    statusCode  = Idle;
    uint32_t        func        = queuedParams.function & 0x3;      // 1 is flash, 2 sd0
    FRESULT         fr          = FR_OK;
    uint32_t        cbRead      = 0;

    switch(curState)
    {
        case Idle:
            // assume an error
            curState = Done;

            // bad parameters sent
            if(func == 0 || func == 3 || (queuedParams.usage & QPPTR0) != QPPTR0)
            {
                statusCode = InvalidParameter;
            }
            // no sd card
            else if((func & 2) == 2 && !fSd0)
            {
                statusCode = NoSDCard;
            }
            // let give it a try
            else
            {
                char szFileName[64];
                if( WiFiCreateName((char *) queuedParams.rgpv[0], true, szFileName, sizeof(szFileName)) > 0                              &&
                    (queuedParams.rgpv[7] = (void *) fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)) != NULL    )
                {
                    queuedParams.rgu32[7] = 1;
                    curState = FILEexist;
                }
                else
                {
                    statusCode = InvalidFileName;
                }
            }
            break;

        case FILEexist:
            if((fr = DFATFS::fsexists((char *) queuedParams.rgpv[7])) == FR_OK)
            {
                curState = Done;
                if(fileTask.SetUsage(FILETask::WRITING, (func & 1) == 1 ? VOLFLASH : VOLSD, (char *) queuedParams.rgpv[7])) curState = FILEopen;
                else                                                                                                        statusCode = FileInUse;     
            }

            // That file didn't work out, see what to do next
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                char szFileName[64];

                // don't cause and error
                fr = FR_OK;

                if( queuedParams.rgu32[7] == 0                                                                                  ||
                    WiFiCreateName((char *) queuedParams.rgpv[0], false, szFileName, sizeof(szFileName)) == 0                            ||
                    (queuedParams.rgpv[7] = (void *) fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)) == NULL   )
                {
                    statusCode = InvalidFileName;
                    curState = Done;
                }

                // try the next name
                else
                {
                    queuedParams.rgu32[7] = 0;
                }
            }
            break;

        case FILEopen:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsopen(fileTask.GetPath(FILETask::WRITING, true), FA_READ)) == FR_OK)
            {
                curState = FILEseek;
            }
            break;

        case FILEseek:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fslseek(0)) == FR_OK)
            {
                curState    = FILEread;
            }
            break;

        // write out the header and then leave the file posistion after the header.
        case FILEread:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsread(&wifiWorking, offsetof(WIFINFO, lastErrState), &cbRead)) == FR_OK)
            {
                if(cbRead != offsetof(WIFINFO, lastErrState) || wifiWorking.wifiVer < WFVER)    statusCode = InvalidVersion; 
                else                                                                            strcpy(wifiWorking.ssid, (char *) queuedParams.rgpv[0]);
                curState = FILEclose;
            }            
            break;

        // on an error we will close
        case FILEclose:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsclose()) == FR_OK)
            {
                fileTask.ClearUsage(FILETask::WRITING);
                curState = Done;
            }
            break;

        case Done:

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // the error code
            utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
 
            // clean up the states
            curState    = Idle;
            statusCode  = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // file error conditions
    if(fr != FR_OK && fr != FR_WAITING_FOR_THREAD)
    {
        statusCode = MkStateAnError(FILECSTATE | fr);

        switch(curState)
        {
            case Idle:
            case FILEexist:
            case Done:
                curState = Done;
                break;

            case FILEopen:
            case FILEclose:
                fileTask.ClearUsage(FILETask::WRITING);
                curState = Done;
                break;

            case FILEseek:
            case FILEread:
                curState = FILEclose;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    return(curState);
}

#if 0
{
   "device":[
      {
         "command":"wifiDeleteParameters",         
         "storageLocation":"flash",
         "ssid":"HorseFeathers"
      }
   ]
}
// response
{
   "device":[
      {
         "command":"wifiDeleteParameters",
         "statusCode": 0,
         "wait": 0
      }
   ]
}
#endif
STATE FnWiFiDeleteParam(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static STATE    statusCode  = Idle;
    uint32_t        func        = queuedParams.function & 0x3;      // 1 is flash, 2 sd0
    FRESULT         fr          = FR_OK;

    switch(curState)
    {
        case Idle:
            // assume an error
            curState = Done;

            // bad parameters sent
            if(func == 0 || func == 3 || (queuedParams.usage & QPPTR0) != QPPTR0)
            {
                statusCode = InvalidParameter;
            }
            // no sd card
            else if((func & 2) == 2 && !fSd0)
            {
                statusCode = NoSDCard;
            }
            // let give it a try
            else
            {
                char szFileName[64];
                if( WiFiCreateName((char *) queuedParams.rgpv[0], true, szFileName, sizeof(szFileName)) > 0                              &&
                    (queuedParams.rgpv[7] = (void *) fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)) != NULL    )
                {
                    queuedParams.rgu32[7] = 1;
                    curState = FILEexist;
                }
                else
                {
                    statusCode = InvalidFileName;
                }
            }
            break;

        case FILEexist:
            if((fr = DFATFS::fsexists((char *) queuedParams.rgpv[7])) == FR_OK)
            {
                curState = Done;
                if(fileTask.SetUsage(FILETask::DELETING, (func & 1) == 1 ? VOLFLASH : VOLSD, (char *) queuedParams.rgpv[7]))    curState = FILEdelete;
                else                                                                                                            statusCode = FileInUse;     
            }

            // That file didn't work out, see what to do next
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                char szFileName[64];

                // don't cause and error
                fr = FR_OK;

                if( queuedParams.rgu32[7] == 0                                                                                          ||
                    WiFiCreateName((char *) queuedParams.rgpv[0], false, szFileName, sizeof(szFileName)) == 0                           ||
                    (queuedParams.rgpv[7] = (void *) fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)) == NULL  )
                {
                    // tried both names and neither exist
                    if(queuedParams.rgu32[6] == 0) statusCode = InvalidFileName;                   
                    curState = Done;
                }

                // try the next name
                else
                {
                    queuedParams.rgu32[7] = 0;
                }
            }
            break;

        case FILEdelete:
            if((fr = DFATFS::fsunlink((char *) queuedParams.rgpv[7])) == FR_OK)
            {
                char szFileName[64];

                // we successfully deleted a file
                queuedParams.rgu32[6] = 1;
                fileTask.ClearUsage(FILETask::DELETING);

                // see if we have another file to delete
                if( queuedParams.rgu32[7] == 1                                                                                          &&
                    WiFiCreateName((char *) queuedParams.rgpv[0], false, szFileName, sizeof(szFileName)) != 0                           &&
                    (queuedParams.rgpv[7] = (void *) fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName)) != NULL   )
                {
                    curState = FILEexist;
                }
                else
                {
                    curState = Done;
                }              
            }
            break;

        case Done:

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // the error code
            utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
 
            // clean up the states
            curState    = Idle;
            statusCode  = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // file error conditions
    if(fr != FR_OK && fr != FR_WAITING_FOR_THREAD)
    {
        statusCode = MkStateAnError(FILECSTATE | fr);

        switch(curState)
        {
            case Idle:
            case FILEexist:
            case Done:
                curState = Done;
                break;

            case FILEdelete:
                fileTask.ClearUsage(FILETask::DELETING);
                curState = Done;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    return(curState);
}

#if 0
{
   "device":[
      {
         "command":"wifiListSavedParameters",
         "storageLocation":"flash"
      }
   ]
}
// response
{
   "device":[
      {
         "command":"wifiListSavedParameters",
         "statusCode":0,
         "parameterSets":[
            {
               "ssid":"HorseFeathers",
               "securityType":"wpa2",
               "autoConnect":true
            }
         ]
      }
   ]
}
#endif
STATE FnWiFiListParam(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static STATE    statusCode  = Idle;
    uint32_t        func        = queuedParams.function & 0x3;      // 1 is flash, 2 sd0
    FRESULT         fr          = FR_OK;
    WIFINFO         wifi;
    uint32_t        cbRead      = 0;

    switch(curState)
    {
        case Idle:
            // assume an error
            curState = Done;

            // put in the parameter set, this may just be an empty set
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szParameterSet, sizeof(szParameterSet)-1); 
            oslex.odata[0].cb += sizeof(szParameterSet)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCB, sizeof(szJSOMemberSepQCB)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCB)-1;

            // bad parameters sent
            if(func == 0 || func == 3)                                                                              statusCode = InvalidParameter;

            // no sd card
            else if((func & 2) == 2 && !fSd0)                                                                       statusCode = NoSDCard;

            // can we get the directory
            else if(!fileTask.SetUsage(FILETask::DIRECTORY, (func & 1) == 1 ? VOLFLASH : VOLSD, DFATFS::szRoot))    statusCode = FileInUse;

            // good to go
            else                                                                                                    curState = FILEopendir;

            break;

        case FILEopendir:
            // we only check at the root directory
            if((fr = DDIRINFO::fsopendir(fileTask.GetPath(FILETask::DIRECTORY, true))) == FR_OK)
            {
                // set where we can put the long filename.
                DDIRINFO::fssetLongFilename((char *) queuedParams.bScratch);
                DDIRINFO::fssetLongFilenameLength(sizeof(queuedParams.bScratch));
                curState = FILEreaddir;
            }
            break;

        case FILEreaddir:
            if((fr = DDIRINFO::fsreaddir()) == FR_OK)
            {
                char const * szFileName;

                // try and get the long file name
                szFileName = DDIRINFO::fsgetLongFilename();

                // if not, try and get the 8.3 filename.
                if(szFileName[0] == '\0') szFileName = DDIRINFO::fsget8Dot3Filename();

                // now check out the file
                if(szFileName != NULL && szFileName[0] != '\0')
                {
                    // see if it is a right sized file
                    if(DDIRINFO::fsgetFileSize() == offsetof(WIFINFO, lastErrState))
                    {
                        char * szExt = (char *) szFileName;

                        // find the extension
                        while(*szExt != '\0' && *szExt != '.') szExt++;

                        // it is one of our wifi files
                        if(strcmp(szExt, ".WF0") == 0 || strcmp(szExt, ".WF1") == 0)
                        {
                            char const * szQualFile = fileTask.MkQualifiedPath((func & 1) == 1 ? VOLFLASH : VOLSD, szFileName);

                            // terminate the ssid name
                            *szExt = '\0';

                            // move to the auto connect location
                            szExt += 3;

                            // see if we need a value seperator
                            if(oslex.odata[0].pbOut[oslex.odata[0].cb -1] == '}') oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

                            // SSID
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjMember, sizeof(szJSONObjMember)-1); 
                            oslex.odata[0].cb += sizeof(szJSONObjMember)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSsid, sizeof(szSsid)-1); 
                            oslex.odata[0].cb += sizeof(szSsid)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szFileName); 
                            oslex.odata[0].cb += strlen(szFileName);

                            // auto connect
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szAutoConnect, sizeof(szAutoConnect)-1); 
                            oslex.odata[0].cb += sizeof(szAutoConnect)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                            oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
                            if(*szExt == '1')   strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szTrue); 
                            else                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szFalse); 
                            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                            // Security Type; assume the worst, we can't open the file.
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSecurityType, sizeof(szSecurityType)-1); 
                            oslex.odata[0].cb += sizeof(szSecurityType)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szUnknown, sizeof(szUnknown)-1); 
                            oslex.odata[0].cb += sizeof(szUnknown)-1;
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjEndCB, sizeof(szJSONObjEndCB)-1); 
                            oslex.odata[0].cb += sizeof(szJSONObjEndCB)-1;

                            // get the file handle ready if we can read the security type
                            if(szQualFile != NULL && fileTask.SetUsage(FILETask::WRITING, (func & 1) == 1 ? VOLFLASH : VOLSD, szQualFile)) curState = FILEopen;

                        }
                    }
                }

                // we are done
                else
                {
                    curState = FILEclosedir;
                }
            }
            break;

        case FILEopen:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsopen(fileTask.GetPath(FILETask::WRITING, true), FA_READ)) == FR_OK)
            {
                curState = FILEseek;
            }
            break;

        case FILEseek:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fslseek(0)) == FR_OK)
            {
                curState    = FILEread;
            }
            break;

        // write out the header and then leave the file posistion after the header.
        case FILEread:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsread(&wifi, offsetof(WIFINFO, lastErrState), &cbRead)) == FR_OK)
            {
                ASSERT(cbRead == offsetof(WIFINFO, lastErrState));

                // backup to the security type, taking out the unknown and end
                oslex.odata[0].cb -= (sizeof(szJSONObjEndCB) + sizeof(szUnknown) - 2);

                switch(wifi.wifiType)
                {

                    // put in the security type
                    case DEWF_SECURITY_OPEN:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szOpen); 
                        break;

        //            case DEWF_SECURITY_WEP:
                    case DEWF_SECURITY_WEP_40:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWep40); 
                        break;

                    case DEWF_SECURITY_WEP_104:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWep104); 
                        break;

        //            case DEWF_SECURITY_WPA:
                    case DEWF_SECURITY_WPA_WITH_KEY:
                    case DEWF_SECURITY_WPA_WITH_PASS_PHRASE:
                    case DEWF_SECURITY_WPA_AUTO_WITH_KEY:
                    case DEWF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWpa); 
                        break;
 
                    case DEWF_SECURITY_WPA2_WITH_KEY:
                    case DEWF_SECURITY_WPA2_WITH_PASS_PHRASE:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szWpa2); 
                        break;

                    case DEWF_SECURITY_WPS_PUSH_BUTTON:
                    case DEWF_SECURITY_WPS_PIN:
                    case DEWF_SECURITY_ENTERPRISE:   
                    default:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szUnsupported); 
                        break;
                }

                // finish it off
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjEndCB, sizeof(szJSONObjEndCB)-1); 
                oslex.odata[0].cb += sizeof(szJSONObjEndCB)-1;

                curState = FILEclose;
            }            
            break;

        // on an error we will close
        case FILEclose:
            if((fr = FILETask::rgpdFile[FILETask::WRITING]->fsclose()) == FR_OK)
            {
                fileTask.ClearUsage(FILETask::WRITING);
                curState = FILEreaddir;
            }
            break;

        case FILEclosedir:
            // close never cause a new error.
            if(DDIRINFO::fsclosedir() != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::DIRECTORY);
                curState = Done;
            }
            break;

        case Done:
            // close out the array
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // the error code
            utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
 
            // clean up the states
            curState    = Idle;
            statusCode  = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // file error conditions
    if(fr != FR_OK && fr != FR_WAITING_FOR_THREAD)
    {
        statusCode = MkStateAnError(FILECSTATE | fr);

        switch(curState)
        {
            case Idle:
            case Done:
                curState = Done;
                break;

            case FILEopendir:
            case FILEclosedir:
                fileTask.ClearUsage(FILETask::DIRECTORY);
                curState = Done;
                break;

            case FILEopen:
            case FILEclose:
                fileTask.ClearUsage(FILETask::WRITING);
                curState = FILEreaddir;
                break;

            case FILEseek:
            case FILEread:
                curState = FILEclose;
                break;

            case FILEreaddir:
                curState = FILEclosedir;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    return(curState);
}


#if COMMENT
{
   "device":[
      {
         "command": "wifiScan",
         "adapter":"wlan0"
      }
   ]
}

// response
{
   "device":[
      {
         "command": "wifiScan",
         "statusCode": 0,
         "wait": -1         
      }
   ]
}
#endif
STATE FnWiFiScan(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    char const *    szWait      = szWait0;
    STATE           retState    = Idle;
    bool            fDisconnect = ((queuedParams.function & 0b01) == 0b01);

    if(!fWiFiInit)                                          retState    = WiFiNoNetworksFound;
    else if(!fDisconnect && deIPcK.isLinked())              retState    = InstrumentInUse;
    else                                                    szWait      = szWaitUntil;

    // good to go
    if(retState == Idle)
    {
        // if we have a disconnect, put it on the list first
        if(fDisconnect)
        {
            defTask.QueTask(DEFTask::WIFIDisconnect, DEFTask::UNTIL);
        }

        memset(&wifiScanState, 0, sizeof(WFSCANSTATE));
        wifiScanState.scanState = Waiting;

        // put the connect on as well
        // &pWiFi pointer to pointer, because the pointer value will be copied
        defTask.QueTask(DEFTask::WIFIScan, DEFTask::UNTIL);
    }

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // the error code
    utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // wait 
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, strlen(szWait)); 
    oslex.odata[0].cb += strlen(szWait);

    return(Idle);
}

#if COMMENT
{
   "device":[
      {
         "command":"wifiReadScannedNetworks",
         "adapter":"wlan0"
      }
   ]
}
// response
{
   "device":[
      {
         "command":"wifiReadScannedNetworks",
         "statusCode":0,
         "wait":0,
         "adapter":"wlan0",
         "networks":[
            {
               "ssid":"HorseFeathers",
               "bssid":"HorseFeathers",
               "securityType":"wpa2",
               "channel": 1,
               "signalStrength":100
            }
         ]
      }
   ]
}
#endif
STATE FnWiFiReadScan(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static SCANINFO scanInfo;        
    IPSTATUS        status      = ipsSuccess;
    int32_t         cb;

    switch(curState)
    {
        case Idle:
            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // if we don't have data
            if(wifiScanState.scanState != Done)
            {
                utoa(WiFiNoScanData, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // reason       
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szReason, sizeof(szReason)-1); 
                oslex.odata[0].cb += sizeof(szReason)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;

                switch(wifiScanState.scanState)
                {
                    // never did a scan
                    case Idle:
                        utoa(NoScanDataAvailable, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                        break;

                    // currently working on a scan
                    case Waiting:
                    case Working:
                        utoa(wifiScanState.scanState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                        break;

                    // Something prevented the scan
                    case Stopped:
                        utoa(wifiScanState.errState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                        break;

                    default:
                        ASSERT(NEVER_SHOULD_GET_HERE);
                        break;
                }

                // complete and get out
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = Done;
            }

            // scan worked, but there are no networks
            else if(wifiScanState.cNetworks == 0)
            {
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
                curState = Done;
            }

            // we do have data
            else
            {
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
                wifiScanState.iNetwork = 0;
                curState = WIFIScanListAP;
            }

            // adapter
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szAdapter, sizeof(szAdapter)-1); 
            oslex.odata[0].cb += sizeof(szAdapter)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWlan0, sizeof(szWlan0)-1); 
            oslex.odata[0].cb += sizeof(szWlan0)-1;

            // networks
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szNetworks, sizeof(szNetworks)-1); 
            oslex.odata[0].cb += sizeof(szNetworks)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCB, sizeof(szJSOMemberSepQCB)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCB)-1;
            break;

        case WIFIScanListAP:
            if(deIPcK.getScanInfo(wifiScanState.iNetwork, &scanInfo, &status)) 
            {
                // multilple entries in the list
                if(oslex.odata[0].pbOut[oslex.odata[0].cb-1] == '}') oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

                // ssid
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjMember, sizeof(szJSONObjMember)-1); 
                oslex.odata[0].cb += sizeof(szJSONObjMember)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSsid, sizeof(szSsid)-1); 
                oslex.odata[0].cb += sizeof(szSsid)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], scanInfo.ssid, scanInfo.ssidLen); 
                oslex.odata[0].cb += scanInfo.ssidLen;

                // BSSID
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szBssid, sizeof(szBssid)-1); 
                oslex.odata[0].cb += sizeof(szBssid)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                cb = GetNumb(scanInfo.bssid, 6, ':', (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                oslex.odata[0].cb += cb;

                // Security Type
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSecurityType, sizeof(szSecurityType)-1); 
                oslex.odata[0].cb += sizeof(szSecurityType)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                switch(scanInfo.apConfig)
                {
                    case DEWF_SECURITY_OPEN:
                        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szOpen, sizeof(szOpen)); 
                        break;

                    case DEWF_SECURITY_WEP:
                        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWep40, sizeof(szWep40)); 
                        break;

                    case DEWF_SECURITY_WEP_104:
                        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWep104, sizeof(szWep104)); 
                        break;
                        
                    case DEWF_SECURITY_ENTERPRISE:
                        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szEnterprise, sizeof(szEnterprise)); 
                        break;

                    case DEWF_SECURITY_WPA:
                        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWpa, sizeof(szWpa)); 
                        break;

                    default:
                        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szUnknown, sizeof(szUnknown)); 
                        break;
                }
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // channel
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szChannel, sizeof(szChannel)-1); 
                oslex.odata[0].cb += sizeof(szChannel)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;                
                utoa(scanInfo.channel, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // rssi /  signal Strength
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSignalStrength, sizeof(szSignalStrength)-1); 
                oslex.odata[0].cb += sizeof(szSignalStrength)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;                
                itoa(scanInfo.rssi, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // end of the object
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

                // if we are done
                if(++wifiScanState.iNetwork >= wifiScanState.cNetworks) curState = Done;
            }

            // something unexpected happened
            else if(IsIPStatusAnError(status)) 
            {
                curState    = Done;
            }
            break;

        case Done:

            // closing bracket
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

            // wait 
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;

            curState = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(curState);
}

/************************************************************************/
/******************************** Log Stop ******************************/
/************************************************************************/
#if COMMENT
{
    "log":{
        "daq" : {
                "command":"stop"
         	}
    	}
}
#endif

STATE FnLogStop(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    ADCStopSampling();

    // clean up the sampling
    // can't finish if we never ran.
    if(logIParam.state != Idle) 
    {
        logIParam.state = Finishing;
        logIParam.stcd  = STCDForce;
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

/************************************************************************/
/******************************** Log Run *******************************/
/************************************************************************/
#if COMMENT
{
    "log":{
        "daq" : {
                "command":"run",
                "delete":true
         	}
    	}
}
#endif

// states for a the logging.
// Idle = nothing configured
// Pending = starting up
// Running = running
// Finishing = shutting down, finishing writing data
// Stopped = completed
// Overflow = had an overflow error; stopped
STATE FnLogRun(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static  char    scratch[256];
    static  STATE   curState    = Idle;
    static  STATE   statusCode  = Idle;
    static  const uint32_t cbVolRoot = strlen(DFATFS::szFatFsVols[VOLSD]) + strlen(DFATFS::szRoot);
            FRESULT fr          = FR_OK;

    switch(curState)
    {
        case Idle:

            // assume we are going to the done state
            curState = Done;

            // nothing was set up
            if(logParam.nCh == 0)                                                                               statusCode = InstrumentNotConfigured;

            // we are still running, we can't run while running
            else if(logIParam.state == Running || logIParam.state == Finishing || logIParam.state == Pending)   statusCode = InstrumentInUse;

            // if we are logging to the SD card
            else if(logParam.adcTrg.vol == VOLSD)
            {
                // point to the directory we want to list
                strcpy(scratch, DFATFS::szFatFsVols[VOLSD]);
                strcat(scratch, DFATFS::szRoot);

                // there is no SD card plugged in
                if(!fSd0)                                                                                       statusCode = MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM);

                // log on boot will create a unique name, don't need to check exiting filenames
                else if(logParam.fLogOnBoot)                                                                    curState = Done;

                // no matter what, I have to search the directory for files in use, and/or to delete.
                else if(!fileTask.SetUsage(FILETask::DIRECTORY, VOLSD, scratch))                                statusCode = MkStateAnError(FILECSTATE | FR_LOCKED);

                // go for the directory search
                else                                                                                            curState = FILEopendir;
            }

            else if(logParam.adcTrg.vol == VOLCLOUD)
            {

//                logParam.adcTrg.tLastPost = ReadCoreTimer() -  (((uint32_t) CORE_TMR_TICKS_PER_SEC) * minSecBetweenPosts);       // we can post immediately
                logParam.adcTrg.tLastPost = ReadCoreTimer();       // we can post immediately, but timing on the first point is slightly off
            }
            break;

        case FILEopendir:
            // we only check at the root directory
            if((fr = DDIRINFO::fsopendir(scratch)) == FR_OK)
            {
                // set where we can put the long filename.
                DDIRINFO::fssetLongFilename(&scratch[cbVolRoot]);
                DDIRINFO::fssetLongFilenameLength(sizeof(scratch)-cbVolRoot);
                curState = FILEreaddir;
            }

            // shut down
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                statusCode = MkStateAnError(FILECSTATE | fr);

                // got to clean up because we don't want to close the directory
                // as it didn't open
                fr = FR_OK;
                fileTask.ClearUsage(FILETask::DIRECTORY);

                // print the results
                curState = Done;
            }
            break;

        case FILEreaddir:
            if((fr = DDIRINFO::fsreaddir()) == FR_OK)
            {
                char const * szFileName;

                // try and get the long file name
                szFileName = DDIRINFO::fsgetLongFilename();

                // if not, try and get the 8.3 filename.
                if(szFileName[0] == '\0') szFileName = DDIRINFO::fsget8Dot3Filename();

                // now check out the file
                if(szFileName != NULL && szFileName[0] != '\0')
                {
                    char const * pExt = szFileName;

                    while(*pExt != '.' && *pExt != '\0') pExt++;

                    // skip the vol/root and compare only up to the base name; not the extension either.
                    if( strncasecmp(&logParam.adcTrg.szUri[cbVolRoot], szFileName, logParam.adcTrg.iNbr-cbVolRoot)  == 0    &&
                        // and the extension
                        strcasecmp(pExt, logParam.adcTrg.szExt) == 0    )
                    {

                        // one of 2 conditions.
                        // 1. The UI needs to delete this file first, and we are done.
                        // 2. We are going to delete during the deferred run, but we need to know it is not in use

                        // 1. The UI needs to delete this file first, and we are done.
                        if((queuedParams.function & 0x1) == 0)                                              fr = FR_EXIST;

                        // 2. We are going to delete during the deferred run, but we need to know it is not in use
                        else if(fileTask.IsQualPathInUse(fileTask.MkQualifiedPath(VOLSD, szFileName)) != FILETask::NOTHING)  fr = FR_LOCKED;
 
                        // keep going
                    }
                }

                // we are done
                else
                {
                    curState = FILEclosedir;
                }
            }
            break;

        case FILEclosedir:
            // close never cause a new error.
            if(DDIRINFO::fsclosedir() != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::DIRECTORY);
                curState = Done;
            }
            break;

        case Done:

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // put out the status code
            utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDelete, sizeof(szDelete)-1); 
            oslex.odata[0].cb += sizeof(szDelete)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;

            // are we deleting the files on run, or giving and error if the files exist.
            if((queuedParams.function & 0x1) != 0)
            {
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szTrue, sizeof(szTrue)-1); 
                oslex.odata[0].cb += sizeof(szTrue)-1;
            }
            else
            {
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szFalse, sizeof(szFalse)-1); 
                oslex.odata[0].cb += sizeof(szFalse)-1;
            }

            // got and error, print out the error
            if(IsStateAnError(statusCode))
            {
                // wait 0 
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
                oslex.odata[0].cb += sizeof(szWait0)-1;
            }

            // we are good to schedule the RuN
            else
            {
                // copy over the data
                memcpy(&logIParam, &logParam, sizeof(logIParam));
                logIParam.state = Pending;
                logParam.state  = Idle;

                // put the run on the deferred task.
                defTask.QueTask(DEFTask::FileLog, -1);

                // wait Until 
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
                oslex.odata[0].cb += sizeof(szWaitUntil)-1;
            }

            statusCode = Idle;
            curState = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // got a file error, set error code and close
    if(!(fr == FR_OK || fr == FR_WAITING_FOR_THREAD))
    {     
        statusCode = MkStateAnError(FILECSTATE | fr); 
        curState = FILEclosedir;
    }

    return(curState);
 }

/************************************************************************/
/************************* Log Common Param *****************************/
/************************************************************************/
void FnLogCommonState(LOGPARAM& lp, bool fPrintRunningCounts)
{
    bool        fNeedComma = false;
    uint32_t i;

    // sample count
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szMaxSampleCount, sizeof(szMaxSampleCount)-1); 
    oslex.odata[0].cb += sizeof(szMaxSampleCount)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    illtoa(lp.csMax/lp.cActiveCh, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // default adc freq
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDefaultAdcSampleFreq, sizeof(szDefaultAdcSampleFreq)-1); 
    oslex.odata[0].cb += sizeof(szDefaultAdcSampleFreq)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    ulltoa((defaultAdcSampleFreq * 1000000ull), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // adc freq
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualAdcSampleFreq, sizeof(szActualAdcSampleFreq)-1); 
    oslex.odata[0].cb += sizeof(szActualAdcSampleFreq)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    if(lp.fAdcFreqMax)  ulltoa((LOGMAXEXADCFREQ * 1000000ull), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    else                ulltoa(((LOGMAXEXADCFREQ * 1000000ull)/2), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // sample freq
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualSampleFreq, sizeof(szActualSampleFreq)-1); 
    oslex.odata[0].cb += sizeof(szActualSampleFreq)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    ulltoa(lp.uHz, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // start delay
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualStartDelay, sizeof(szActualStartDelay)-1); 
    oslex.odata[0].cb += sizeof(szActualStartDelay)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    ulltoa(lp.startpsDelay, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // Log on boot
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szLogOnBoot, sizeof(szLogOnBoot)-1); 
    oslex.odata[0].cb += sizeof(szLogOnBoot)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    if(lp.fLogOnBoot)   
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szTrue, sizeof(szTrue)-1);
        oslex.odata[0].cb += sizeof(szTrue)-1;
    }
    else
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szFalse, sizeof(szFalse)-1);
        oslex.odata[0].cb += sizeof(szFalse)-1;
    }

    // target
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStorageLocation, sizeof(szStorageLocation)-1); 
    oslex.odata[0].cb += sizeof(szStorageLocation)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[lp.adcTrg.vol]);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // if we have a URI
    if(lp.adcTrg.vol != VOLRAM)
    {

        // put the URI out
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szURI, sizeof(szURI)-1); 
        oslex.odata[0].cb += sizeof(szURI)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

        // if we have and SD file, just put out the relative path to root
        // if logging, this file will be fully qualified.
        if(lp.adcTrg.vol == VOLSD)
        {
            uint32_t cchVolRoot = strlen(DFATFS::szFatFsVols[VOLSD]) + strlen(DFATFS::szRoot);
            char const * szQualFile = fileTask.MkQualifiedPath(VOLSD, lp.adcTrg.szUri);

            // we only want the relative path from the root
            // this will be the active file we are logging too, 
            // if complete it will go back to _0 as that is where the header was written.
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], &szQualFile[cchVolRoot]);
        }

        // Otherwise it is a cloud URL; put out unchanged.
        else
        {
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], lp.adcTrg.szUri);
        }
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // if we have a cloud service; put out the service
        if(lp.adcTrg.vol == VOLCLOUD)
        {
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szService, sizeof(szService)-1); 
            oslex.odata[0].cb += sizeof(szService)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgCSEVNames[lp.adcTrg.cloudService]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        }
    }

    // channels
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szChannels, sizeof(szChannels)-1); 
    oslex.odata[0].cb += sizeof(szChannels)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCB, sizeof(szJSOMemberSepQCB)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCB)-1;

    for(i=0; i<NBRLOGCH; i++)
    {
        if(lp.rgLogCh[i].cAve != 0)
        {
            // if a comman is needed for multiple channel entries
            if(fNeedComma) oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
            fNeedComma = true;

            // channel number
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjMember, sizeof(szJSONObjMember)-1); 
            oslex.odata[0].cb += sizeof(szJSONObjMember)-1;
            utoa(i+1, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);

            // Opening of the channel object
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
            oslex.odata[0].cb += sizeof(szJSONest)-1;

            // the average
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szAverage, sizeof(szAverage)-1); 
            oslex.odata[0].cb += sizeof(szAverage)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
            utoa(lp.rgLogCh[i].cAve, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // end of the channel information
 //           memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjEndCB, sizeof(szJSONObjEndCB)-1); 
 //           oslex.odata[0].cb += sizeof(szJSONObjEndCB)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
        }
    }
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';    

    // print the current running counts
    if(fPrintRunningCounts)
    {
        uint64_t startIndex = ((lp.csTotal > CSVBUFF) ? (lp.csTotal - CSVBUFF) : 0) / lp.cActiveCh;

        // start index in the voltage buffer
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStartIndex, sizeof(szStartIndex)-1); 
        oslex.odata[0].cb += sizeof(szStartIndex)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        ulltoa(startIndex, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // total sample count taken into the voltage buffer
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualCount, sizeof(szActualCount)-1); 
        oslex.odata[0].cb += sizeof(szActualCount)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        ulltoa(lp.csTotal/lp.cActiveCh, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // if we have a target, what are its counts.
        if(lp.adcTrg.vol != VOLRAM)
        {   
            // target 
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[lp.adcTrg.vol]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
            oslex.odata[0].cb += sizeof(szJSONest)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szState, sizeof(szState)-1); 
            oslex.odata[0].cb += sizeof(szState)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

            // the running state of the target; and warnings.
            if(lp.state != Idle)
            {
                // not running
                if(lp.adcTrg.state == Idle)
                {
                    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Stopped]);
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                    switch(lp.adcTrg.stcd)
                    {

                        case STCDNormal:
                        case STCDForce:
                            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonNormal, sizeof(szStopReasonNormal)-1); 
                            oslex.odata[0].cb += sizeof(szStopReasonNormal)-1;
                            break;

                        case STCDError:
                                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonError, sizeof(szStopReasonError)-1); 
                                oslex.odata[0].cb += sizeof(szStopReasonError)-1;

                                if(lp.adcTrg.vol == VOLCLOUD)
                                {
                                    char * pch = lp.adcTrg.szResponse;
                                    for( ; *pch != '\0'; pch++) 
                                    {
                                        if(*pch == '\"')                        *pch = '$';
                                        else if(*pch == '\r' || *pch == '\n')   *pch = ' ';
                                    }

                                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopMessage, sizeof(szStopMessage)-1); 
                                    oslex.odata[0].cb += sizeof(szStopMessage)-1;
                                    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], lp.adcTrg.szResponse);
                                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                                    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
                                }
                                break;
                
                        case STCDOverflow:
                                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonOverflow, sizeof(szStopReasonOverflow)-1); 
                                oslex.odata[0].cb += sizeof(szStopReasonOverflow)-1;
                                break;

                        case  STCDUnknown:
                                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonUnknown, sizeof(szStopReasonUnknown)-1); 
                                oslex.odata[0].cb += sizeof(szStopReasonUnknown)-1;
                                break;

                        default:
                            ASSERT(NEVER_SHOULD_GET_HERE);
                            break;
                    }
                }

                // running
                else
                {
                    int64_t min, ave, max, total;

                    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Running]);
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                    // derivative           
                    loopStats.GetStats(LOOPSTATS::DBDLADCTRG, min, ave, max, total);
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szTargetDerivative, sizeof(szTargetDerivative)-1); 
                    oslex.odata[0].cb += sizeof(szTargetDerivative)-1;
                    illtoa(ave, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
                    illtoa(total, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';    
                }

                // the actual count of the target
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualCount, sizeof(szActualCount)-1); 
                oslex.odata[0].cb += sizeof(szActualCount)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
                ulltoa((lp.adcTrg.csProcessed/logIParam.cActiveCh), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';    
            }
        }
    }
}

/************************************************************************/
/************************* Log getCurrentState **************************/
/************************************************************************/
#if COMMENT

{
    "log":{
        "daq" : {
                "command":"getCurrentState"
         	}
    	}
}


{
	"log": {
		"daq": {
			"command": "getCurrentState",
			"statusCode": 0,
			"state": "stopped",
			"stopReason": "NORMAL",
			"startIndex": 0,
			"actualCount": 0,
			"maxSampleCount": -1,
			"actualSampleFreq": 50000000000,
			"actualStartDelay": 0,
			"channels": [{
				"1": {
					"average": 32,
					"storageLocation": "ram",
					"overflow": "circular",
					"actualGain": 1,
					"actualVOffset": 0
				}
			}, {
				"2": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log",
					"overflow": "circular",
					"actualGain": 1,
					"actualVOffset": 0
				}
			}],
			"wait": 0
		}
	}
}

#endif

// states for a the logging.
// Idle = nothing configured
// Pending = starting up
// Running = running
// Finishing = shutting down, finishing writing data
// Stopped = completed
// Overflow = had an overflow error; stopped
STATE FnLogGetCurrentState(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // state
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szState, sizeof(szState)-1); 
    oslex.odata[0].cb += sizeof(szState)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    // actively configured but not running
    if(logParam.state == Pending && (logIParam.state == Idle || logIParam.state == Stopped || logIParam.state == Overflow))
    {
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Stopped]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonPending, sizeof(szStopReasonPending)-1); 
        oslex.odata[0].cb += sizeof(szStopReasonPending)-1;

        // put out queued state
        FnLogCommonState(logParam, false);
    }

    // running or ran, but nothing new pending
    else if(logIParam.state != Idle)
    {

         switch(logIParam.state)
        {
            case Pending:
            case Running:
            case Finishing:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[logIParam.state]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
                break;
                
            case Stopped:
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Stopped]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                switch(logIParam.stcd)
                {
                    case STCDNormal:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonNormal); 
                        break;

                    case STCDForce:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonForce); 
                        break;
                        
                    case STCDError:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonError); 
                        break;
                        
                    case STCDOverflow:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonOverflow); 
                        break;

                    default:
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonUnknown);     
                        break;
                }
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }

        // put out the current states
        FnLogCommonState(logIParam, true);
    }

    // if we have nothing running or that ran, and nothing pending
    else if(logIParam.state == Idle)
    {
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Stopped]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonUnconfig, sizeof(szStopReasonUnconfig)-1); 
        oslex.odata[0].cb += sizeof(szStopReasonUnconfig)-1;
    }

    // some other state
    // we could be setting up the logParam, so we would be in some sort of working state
    else
    {
        ASSERT(NEVER_SHOULD_GET_HERE);
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Stopped]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStopReasonUnknown, sizeof(szStopReasonUnknown)-1); 
        oslex.odata[0].cb += sizeof(szStopReasonUnknown)-1;
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}
 
/************************************************************************/
/****************************** Log Read ********************************/
/************************************************************************/
#if COMMENT
{
    "log":{
        "daq" : {
                "command":" read ",
                "channels":[1,2,3],         
                "startIndex":0,
                "count":-1
                }
    }
}


12F\r\n
{
    "log": {
        "daq": {
                "command":"read",
                "statusCode":0,
                "channels":[1,2,3],         
                "startIndex":0,
                "actualCount":1,
                "actualSampleFreq":500000000000,
//                "actualGain":1,
//                "actualVOffset":0,
//                "actualStartDelay":0,
//                "maxSampleCount":-1,
//                "overflow":"circular",
                "binaryOffset":0,
                "binaryLength":2,
                "wait":0
        }
    }
}\r\n
2\r\n
<Binary Data>\r\n
0\r\n
\r\n
#endif

STATE FnLogRead(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    uint32_t i;
    uint32_t chMask = 0x0100; 
    int64_t  iStart; 
    int64_t  iEnd; 
    int64_t  iEndTotal;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // no channels specified
    if(logIParam.cActiveCh == 0)
    {
            // put in the status code
            utoa(InvalidChannel, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
            return(Idle);
    }

    // channels don't match
    for(i=0; i<NBRLOGCH; i++, chMask <<= 1)
    {
        // we hit an error
        if(((logIParam.rgLogCh[i].cAve == 0) ^ (((queuedParams.function & chMask) == 0))) == 1)
        {
            // put in the status code
            utoa(InvalidChannel, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
            return(Idle);
        }
    }

    // calculate what we can return
    // find start index, leave 1/8 result buffer for timing slop
#if((CSVBUFF % 8) != 0 || (CSVBUFF/8) < CSMINBUF || ((CSVBUFF/8) % CSMINBUF) != 0)
#error Our range must be a mult of 840
#endif

    // if not running we can get all of the samples 
    if(logIParam.state != Running)  iStart = (logIParam.csTotal - CSVBUFF);

    // if running we have to leave a buffer to allow fill while reading
    else                            iStart = (logIParam.csTotal - 7*CSVBUFF/8); 

    iStart /= logIParam.cActiveCh;

    // if below what we want, move up
    if(iStart < (int64_t) queuedParams.rgu64[0])  iStart = queuedParams.rgu64[0];

    // find end index
    iEndTotal = logIParam.csTotal/logIParam.cActiveCh;

    // if -1, go to the end of of all samples taken
    if(queuedParams.rgi64[0] < 0)       iEnd = iEndTotal;

    // otherwise get was was asked for
    else                                iEnd = queuedParams.rgu64[0] + queuedParams.rgi64[0];

    // if too high, bring it back.
    if(iEnd > iEndTotal)  iEnd = iEndTotal;

    // see if we have anything to return
    if(iEnd <= iStart)
    {
        // put in the status code
        utoa(ValueOutOfRange, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // wait 0
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
        return(Idle);
    }

    // we should be good to go
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szChannels, sizeof(szChannels)-1); 
    oslex.odata[0].cb += sizeof(szChannels)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCB, sizeof(szJSOMemberSepQCB)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCB)-1;

    for(i=0; i<NBRLOGCH; i++)
    {
        if(logIParam.rgLogCh[i].cAve != 0)
        {
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '1' + i;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
        }

    }
    oslex.odata[0].pbOut[oslex.odata[0].cb-1] = ']';

    // start index
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStartIndex, sizeof(szStartIndex)-1); 
    oslex.odata[0].cb += sizeof(szStartIndex)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    ulltoa(iStart, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // total sample count
    i = iEnd - iStart;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualCount, sizeof(szActualCount)-1); 
    oslex.odata[0].cb += sizeof(szActualCount)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    utoa(i, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // sample freq
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualSampleFreq, sizeof(szActualSampleFreq)-1); 
    oslex.odata[0].cb += sizeof(szActualSampleFreq)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    ulltoa(logIParam.uHz, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // binary offset
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szBinaryOffset, sizeof(szBinaryOffset)-1); 
    oslex.odata[0].cb += sizeof(szBinaryOffset)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // binary length
    oslex.odata[oslex.cOData].cb = i*logIParam.cActiveCh*sizeof(int16_t);
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szBinaryLength, sizeof(szBinaryLength)-1); 
    oslex.odata[0].cb += sizeof(szBinaryLength)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    utoa(oslex.odata[oslex.cOData].cb, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    // Set up for the binary read
    oslex.odata[oslex.cOData].pbOut = (uint8_t *) KVA_2_KSEG1(adcVoltageBuffer + ((iStart*logIParam.cActiveCh) % CSVBUFF));
    oslex.odata[oslex.cOData].ReadData = &OSPAR::ReadVoltageLog;
    oslex.cOData++;

    return(Idle);
}

/************************************************************************/
/************************* Log Set Parameters ***************************/
/************************************************************************/
#if COMMENT
{
    "log":{
        "daq" : {
                "command":""setDefaultAdcFreq": ",
		"adcSampleFreq": 500000000000,
         	}
    	}
}
{
    "log":{
        "daq" : {
                "command":""setDefaultAdcFreq": ",
		"adcSampleFreq": 250000000000,
         	}
    	}
}
{
    "log":{
        "daq" : {
                "command":""setDefaultAdcFreq": ",
		"adcSampleFreq": 0,
         	}
    	}
}
#endif
STATE FnLogSetDefaultAdcFreq(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    STATE statusCode = Idle;

    // if we are in auto mode, to select the frequency
    if((queuedParams.usage & QPUINTLL2) != QPUINTLL2 || queuedParams.rgu64[2] == 0)                                                 defaultAdcSampleFreq = 0;
    else if(queuedParams.rgu64[2] == ((LOGMAXEXADCFREQ * 1000000ull)/2) || queuedParams.rgu64[2] == (LOGMAXEXADCFREQ * 1000000ull)) defaultAdcSampleFreq = (uint32_t) (queuedParams.rgu64[2]/1000000ull);
    else                                                                                                                            statusCode = InvalidParameter;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // put in the status code
    utoa(statusCode, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // adc freq
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDefaultAdcSampleFreq, sizeof(szDefaultAdcSampleFreq)-1); 
    oslex.odata[0].cb += sizeof(szDefaultAdcSampleFreq)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    ulltoa((defaultAdcSampleFreq * 1000000ull), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

#if COMMENT
{
    "log":{
        "daq" : {
                "command":"setParameters",
               	"maxSampleCount":1000,
                "adcSampleFreq" : 500000000000,
                "sampleFreq":50000000,
                "startDelay":0,
		        "channels" : [
            		{"1":	{
				            "average":32,
                		    "storageLocation":"ram",
                		    "uri":""
            			    }
			        },
            		{"2":	{
				            "average":1,
                		    "storageLocation":"sd0",
                		    "uri":"channel2Data.log"
            			    }
			        },
            		{"3":	{
				            "average":8,
               			    "storageLocation":"cloud",
                		    "uri":"http://www.abc.com:123/samples?method=post"
            			    }
			        }
			    ]
        	}
    	}
}

returns
{
	"log": {
		"daq": {
			"command": "setParameters",
			"statusCode": 0,
			"maxSampleCount": 1000,
            "actualAdcSampleFreq" : 500000000000,
			"actualSampleFreq": 62500000000,
			"actualStartDelay": 0,
			"channels": [{
				"1": {
					"average": 32,
					"storageLocation": "ram"
				}
			}, {
				"2": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log"
				}
			}, {
				"3": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log"
				}
			}, {
				"4": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log"
				}
			}, {
				"5": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log"
				}
			}, {
				"6": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log"
				}
			}, {
				"7": {
					"average": 1,
					"storageLocation": "sd0",
					"uri": "channel2Data.log"
				}
			}, {
				"8": {
					"average": 8,
					"storageLocation": "ram"
				}
			}],
			"wait": 0
		}
	}
}
#endif

STATE FnLogSetParm(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static uint32   cbWritten   = 0;
    static FRESULT  fr          = FR_OK;

    if(IsStateAnError(logParam.state))
    {
        // status
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
        oslex.odata[0].cb += sizeof(szStatusCode)-1;

        // put in the status code
        utoa(logParam.state, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // wait 0
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;

        logParam.state = Idle;
    }

    else
    {
        uint32_t     i;

        switch(logParam.state)
        {
            case Idle:
                {
                    int32_t     sCh, nCh, eCh, iCh;

                    // assume an error
                    logParam.state = Done;

                    // init some statics
                    cbWritten       = 0;
                    fr              = FR_OK;

                    // storage location
                    if(queuedParams.rgpv[7] == NULL || strcmp((const char *) queuedParams.rgpv[7], rgVOLNames[VOLRAM]) == 0)    logParam.adcTrg.vol    = VOLRAM;                                            
                    else if(strcmp((const char *) queuedParams.rgpv[7], rgVOLNames[VOLSD]) == 0)                                logParam.adcTrg.vol    = VOLSD;
                    else if(strcmp((const char *) queuedParams.rgpv[7], rgVOLNames[VOLCLOUD]) == 0)                             logParam.adcTrg.vol    = VOLCLOUD;
                    else                                                                                                        logParam.adcTrg.vol    = VOLNONE;

                    // URI
                    if(queuedParams.rgpv[6] != NULL) strcpy(logParam.adcTrg.szUri, (const char *) queuedParams.rgpv[6]);

                    // colect cloud servic info
                    if(logParam.adcTrg.vol == VOLCLOUD)
                    {
                        // cloud service; right now, this boils down to CSEVNONE or CSEVTHINGSPEAK
                        // start out with no cloud service
                        logParam.adcTrg.cloudService   = CSEVNONE;

                        // user specfied cloud service
                        if(queuedParams.rgpv[5] != NULL)
                        {
                            if(stricmp((const char *) queuedParams.rgpv[5], rgCSEVNames[CSEVTHINGSPEAK]) == 0) logParam.adcTrg.cloudService  = CSEVTHINGSPEAK;
                        }

                        // default cloud service, none given but target is the cloud
                        else if(logParam.adcTrg.vol == VOLCLOUD)   logParam.adcTrg.cloudService  = CSEVTHINGSPEAK;
                    }

                    // see how many channels I will be using.
                    for(i=0, sCh=0, nCh=0; i<NBRLOGCH; i++) if(logParam.rgLogCh[i].cAve != 0) 
                    {
                        if(sCh == 0) sCh = i+1;
                        nCh = i+1;
                    }

                    // if only one channel is selected
                    if(sCh == nCh) nCh = 1;
                    else sCh = 1;

                    ASSERT(nCh > 0);  // if nCh == 0, then sCh == 0, and the condition above sCh == nCh would make nCh = 1

                    if((queuedParams.usage & QPINTLL0) != QPINTLL0) queuedParams.rgi64[0] = -1;                     // max samples count
                    if((queuedParams.usage & QPUINTLL1) != QPUINTLL1) queuedParams.rgu64[1] = 0;                    // start delay

                    // if an adc free running speed was not explicitly set, figure one out.
                    if((queuedParams.usage & QPUINTLL2) != QPUINTLL2 || queuedParams.rgu64[2] == 0) 
                    {
                        // FYI: if we fix the opamp output resistor (~82 ohms) we can run at full speed all of the time.
                        // if our default speed is specified, set it no matter what.
                        if(defaultAdcSampleFreq != 0)                                               queuedParams.rgu64[2] = defaultAdcSampleFreq * 1000000ull;

                        // otherwise if the default is auto select, set the slowest speed that will work.
                        // We have a problem with coupling on the ADC if it runs at a full 500kS/s, as the MUX switches through the channels
                        // the S&H is not fully at voltage. 
                        // as a result some of the charge leaks into the next sample and appears as a charge coupled crosstalk on the next channel
                        // so run the ADC at half speed if we can and there is no charge coupling.
                        else if((nCh * queuedParams.rgu64[0]) > ((LOGMAXEXADCFREQ * 1000000ull)/2)) queuedParams.rgu64[2] = LOGMAXEXADCFREQ * 1000000ull;       // we have to run at full speed

                        // we can run at half speed, so set that to ensure we settle in time
                        else                                                                        queuedParams.rgu64[2] = ((LOGMAXEXADCFREQ * 1000000ull)/2); // we can run at half speed and get less shadowing
                    }

                    // status
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
                    oslex.odata[0].cb += sizeof(szStatusCode)-1;

                    // check for invalid parameters on the setparm
                    if( sCh == 0                                                                                                                            ||  // must specify at least 1 channel, can't start at channel 0
                        ((queuedParams.usage & QPUINTLL0) != QPUINTLL0)                                                                                     ||  // must be given a sample rate
                        (queuedParams.rgu64[2] != (LOGMAXEXADCFREQ * 1000000ull) && queuedParams.rgu64[2] != ((LOGMAXEXADCFREQ * 1000000ull)/2))            ||  // adc free running sample rate must be a support value
                        logParam.adcTrg.vol == VOLNONE                                                                                                      ||  // must log somewhere, even if to ram
                        (logParam.adcTrg.vol != VOLRAM && queuedParams.rgpv[6] == NULL)                                                                     ||  // if we are logging to a target, we must know where
                        (logParam.adcTrg.vol == VOLCLOUD && (   logParam.adcTrg.cloudService == CSEVNONE                                                    ||      // must specify a cloud service
                                                                queuedParams.rgpv[4] == NULL                                                                ||      // must specify a key
                                                                (logParam.adcTrg.pbWrite = (uint8_t *) strstr(logParam.adcTrg.szUri, szChannels)) == NULL   ||      // must specify a channel
                                                                 logParam.adcTrg.pbWrite == (uint8_t *) logParam.adcTrg.szUri                               ||      // must not be the start of the URI
                                                                *(--logParam.adcTrg.pbWrite) != '/'                                                         ))  )   // channel must start with a /
                    {
                        // put in the status code
                        utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                    }

                    // is the sampling freq in a supported range
                    else if(!(0 < queuedParams.rgu64[0] && queuedParams.rgu64[0] <= (queuedParams.rgu64[2] / nCh)))
                    {     
                        utoa(ValueOutOfRange, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                    }

                    // we are good to go.
                    else
                    {
                        // no error
                        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
                        logParam.state = Working;

                        logParam.fLogOnBoot = (queuedParams.function & 0x1) == 0x1;

                        logParam.nCh            = nCh;
                        logParam.sCh            = sCh;
                        logParam.fAdcFreqMax    = queuedParams.rgu64[2] == (LOGMAXEXADCFREQ * 1000000ull);

                        // calculate the number of active channels
                        eCh            = sCh + nCh - 1;
                        for(iCh=sCh-1; iCh<eCh; iCh++) if(logParam.rgLogCh[iCh].cAve != 0) logParam.cActiveCh++; 

                        // if this is a cloud we need to have timing based on seconds
                        if(logParam.adcTrg.vol == VOLCLOUD)
                        {
                            char * szCur = NULL;
 
                            // fastest we can do is once a second
                            if(queuedParams.rgu64[0] > 1000000)
                            {
                                logParam.adcTrg.secPerSampl = 1;
                                queuedParams.rgu64[0] = 1000000;
                            }
                            else
                            {
                                // invert and round to get seconds
                                logParam.adcTrg.secPerSampl = (uint32_t) (1000000 + (queuedParams.rgu64[0]/2)) / queuedParams.rgu64[0];

                                // invert and round to get actual freq in hertz for sec
                                queuedParams.rgu64[0] = (1000000ull + (logParam.adcTrg.secPerSampl/2)) / logParam.adcTrg.secPerSampl;
                            }

                            // make the ThingSpeak POST requist packet.
                            strcpy(logParam.adcTrg.szPOSTBuf, szPost);
                            strcat(logParam.adcTrg.szPOSTBuf, (char *) logParam.adcTrg.pbWrite);
                            strcat(logParam.adcTrg.szPOSTBuf, szTSHdr);
                            logParam.adcTrg.iContentLen = strlen(logParam.adcTrg.szPOSTBuf);

                            // leave room for a content length, and the header terminator
                            logParam.adcTrg.iUpdate = logParam.adcTrg.iContentLen + TScbContentLen + sizeof(szTSTermHdr);

                            // put the write key and update packets in
                            strcpy(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iUpdate], szTSKey);
                            strcat(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iUpdate], (char *) queuedParams.rgpv[4]);
                            strcat(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iUpdate], szTSUpdate);
                            logParam.adcTrg.iDelta = logParam.adcTrg.iUpdate + strlen(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iUpdate]);
                            logParam.adcTrg.iFirstValue = logParam.adcTrg.iDelta + TSiValue;

                            // put a delta in
                            memcpy(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta], szTSDelta, sizeof(szTSDelta)-1);
                            szCur = &logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta] + sizeof(szTSDelta)-1;

                            // put in the channels
                            for(iCh=sCh-1; iCh<eCh; iCh++) if(logParam.rgLogCh[iCh].cAve != 0) 
                            {
                                memcpy(szCur, szTSField, sizeof(szTSField)-1);
                                szCur[TSiField] = '1' + iCh;
                                szCur += sizeof(szTSField)-1;
                            }

                            logParam.adcTrg.cbDelta = szCur - &logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta];

                            // get the null terminator
                            memcpy(szCur, szTSEndUpdate, sizeof(szTSEndUpdate));

                            // get what one packet looks like
                            logParam.adcTrg.cbMinConLen = strlen(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iUpdate]);

                            // get what one delta entry looks like
                            logParam.adcTrg.cbSampleConLen = strlen(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta]) - sizeof(szTSEndUpdate) + sizeof(szTSNextDelta);

                            // we want to NULL before the terminator, after the last field value, this is just before the next/End update
                            *szCur = '\0';
                            

#if 0
{
#define CSAMPLES 5
 
    int32_t i, j, cbT, cbS;
    char * pch;

    // put out the POST and HEADER
    utoa(logParam.adcTrg.cbMinConLen + (CSAMPLES-1) * logParam.adcTrg.cbSampleConLen, &logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iContentLen], 10);
    strcat(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iContentLen], szTSTermHdr);
    Serial.print(logParam.adcTrg.szPOSTBuf);

    // Put out the update
    logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta] = '\0';
    Serial.print(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iUpdate]);
    logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta] = '{';

    szCur = NULL;
    for(i=0; i<CSAMPLES; i++)
    {
        char * szCurValue = &logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta] + TSiValue;

        if(szCur != NULL) Serial.print(szCur);
        else szCur = (char *) szTSNextDelta;

        pch = &logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta + TSiDelta];
        utoa(i, pch, 10);
        cbT = strlen(pch);
        cbS = TScbDelta - cbT;
        pch = &pch[cbT];
        for(j=0; j<cbS; j++, pch++) *pch = ' ';

        for(iCh=0; iCh<((int32_t) logParam.cActiveCh); iCh++) 
        {
            int32_t j;

            utoa(100000/(1 << iCh), szCurValue, 10);
            cbT = strlen(szCurValue);
            cbS = TScbValue - cbT;
            pch = &szCurValue[cbT];
            for(j=0; j<cbS; j++, pch++) *pch = ' ';

            szCurValue += TSdiFValue2Value;
        }
        Serial.print(&logParam.adcTrg.szPOSTBuf[logParam.adcTrg.iDelta]);

    }
    Serial.println(szTSEndUpdate);
}
#endif

                        }
                    }
                }
                break;

            case Working:
                {
                    uint32_t eCh            = logParam.sCh + logParam.nCh - 1;

                    // since we are talking seconds, our ISR count will be greater than 5; with the ISR involved our timing can be as far off as 15ppm
                    // with a 10ppm clock, we now can be as far off as 25ppm or 2.16 seconds off per day.
                    logParam.uHz            = SPSPreScalarAndPeriod(queuedParams.rgu64[0], 1000000, TMRPBCLK, &logParam.smpTmrPrescalar, &logParam.smpTmrPeriod, &logParam.smpTmrISRCnt);

                    // we are going to do this in nano seconds.
                    // our clock can only get to within 10ns anyway, so rounding doesn't do much either.
                    logParam.startpsDelay   = DelayPreScalarAndPeriod(queuedParams.rgu64[1], 1000000000000ull, TMRPBCLK, &logParam.dlyTmrPrescalar, &logParam.dlyTmrPeriod, &logParam.dlyTmrISRCnt);

                    // if only 1 channel, the start channel may not be 1, and the end channel will have to be 1 higer.
                    for(i=logParam.sCh-1; i<eCh; i++)
                    {
                        LOGCHANNEL& ch  = logParam.rgLogCh[i];

                        ch.iCh          = i+1;
                        ch.iADCValue    = logParam.nCh > 1 ? (logParam.nCh - i - 1) : 0;

                        // the polynomial is in nVolts, we need to change to uVolts to fit in a 32 bit value
                        // we also want to do shifts for divides, so we want to scale by 1024/1000 for conversions to mvolts
                        // also, we want to account for rounding, so add 512, so when we shift (divide) by 1024 we round
                        // and we MUST do positive math if both rounding (add 512) and shifting is to work

                        // we have....
                        // nV = A*Dadc - C
                        // converting to uV so the constants will fit in a 32 bit int we have
                        // uV = (A*Dadc - C) / 1000
                        // but we want to shift for divide to get mvolts out of uvolts, so make it a power of 2 (1024), scale close to uV
                        // u2V = 1024/1000 * (A*Dadc - C) / 1000
                        // expanding...
                        // u2V = 1024*A*Dadc/1000000 - 1024*C/1000000
                        // later we will divide by 1024 to get mvolts, we need to add 512 to do the rounding; do divide with a >>10
                        // mV = (1024*A*Dadc/1000000 - 1024*C/1000000 + 512) >> 10
                        // we also want to make sure our math does not go negative for rounding or shifting so add a constant that we can subtract later
                        // the constant must be much greater than 1024*C/1000000 and a power of 2 so we can divide evenly with a shift
                        // 2^^25 does the job....
                        // adding zero (2^^25 - 2^^25)                        
                        // mV = (1024*A*Dadc/1000000 - 1024*C/1000000 + 512 + (2^^25 - 2^^25)) >> 10
                        // extract -2^^25 out of the >> 10 we have
                        // mV = ((1024*A*Dadc/1000000 - 1024*C/1000000 + 512 + 2^^25) >> 10 - 2^^15
                        // Make u2A = 1024*A/1000000
                        // Make u2C = 2^^25 - 1024*C/1000000 + 512...... This will always be a positive number.
                        // mV = ((u2A*Dadc + u2C) >> 10) - 2^^15

                        // Make u2A = 1024*A/1000000 (plus a rounding constant of 500000)
                        // ch.u2A           = (int32_t) (((arCalIDAQ[ch.iCh].A * 1024) + 500000) / 1000000);

                        // Make u2C = 2^^25 - 1024*C/1000000 + 512...... This will always be a positive number.
                        //ch.u2C           = (int32_t) (TWOto25 - (((arCalIDAQ[ch.iCh].C * 1024) + 500000) / 1000000) + 512);


                        // Let's try this again. The above uses uVolts, which is 1000x lower than mV, but the ADC is 16 bits, or 65536, which is 65,536 >> 1000, 
                        // so rounding error can be as much as +/-16mV if we are off by 0.5 in our rounded to uV A constant.
                        // we definitely want to only do 32 bit math, so we have to scale some.
                        // After tons of validation and the expected constants of A = 328853 and C = 10775862000, we can scale by 2^^17
                        // In the end, we want to convert our nV to mV or divide by 1,000,000
                        // 2^^11 / (5*6 * 2^^17) == 0.000001 = 1/1,000,000; so go ahead and scale our constants by 2^^11/5^^6 and in the end we will shift by 17.
                        // 2^^11 = 2048, 5^^6 = 15,625, 2^^17 = 131,072
                        // our equation is 
                        // nV = A*Dadc - C
                        // applying our conversion we have.
                        // mV = ((2048/15,625)*A - (2048/15,625)*C)/131,072
                        // since we are going to divide (shift) at the end by 131,072 for rounding we need to add half of that or 65536
                        // mV = ((2048/15,625)*A - (2048/15,625)*C + 65536)/131,072
                        // but because I don't want to go negative on the calculation, I will add enought to ensure -(2048/15,625)*C is positive, so add zero by adding and subtracting 2^^31
                        // mV = ((2048/15,625)*A + (2^^31 - 2^^31) - (2048/15,625)*C + 65536)/131,072
                        // put the -2^^31 outside of everything by dividing by 2^^17 or subtract 2^^14 = 16,384
                        // mV = (((2048/15,625)*A + 2^^31 - (2048/15,625)*C + 65536)/131,072) - 16,384   Now we know (2^^31 - (2048/15,625)*C) will be positive

                        // Our converter A' = (2048/15,625)*A
                        ch.u2A           = (uint32_t) (((arCalIDAQ[ch.iCh].A * 2048) + 7813) / 15625);

                        // Our converted C' = 2^^31 - (2048/15,625)*C + 65536
                        ch.u2C           = (uint32_t) (0x80000000ull - (((arCalIDAQ[ch.iCh].C * 2048) + 7813) / 15625) + 65536);

                        // we need to check that a maximum ADC value will not overflow a 2^^32
                        ASSERT((((uint64_t) ch.u2A) * 65536ull + ((uint64_t) ch.u2C)) <= 0xFFFFFFFFull)

                    }
                    logParam.csMax = queuedParams.rgi64[0] * logParam.cActiveCh;    

                    // if the SD card was selected
                    switch(logParam.adcTrg.vol)
                    {
                        case VOLSD:
                            {
                                char const *    pCurFrom    = fileTask.MkQualifiedPath(VOLSD, logParam.adcTrg.szUri);
                                char const *    pCurEnd     = pCurFrom + strlen(pCurFrom);
                                char *          pCurTo      = logParam.adcTrg.szUri;

                                // copy up to the .
                                logParam.adcTrg.csEOF = 0;
                                for(; pCurFrom < pCurEnd && *pCurFrom != '.'; pCurFrom++, pCurTo++) *pCurTo = *pCurFrom;

                                // if this is a log on boot, append the log on boot number in
                                if(logParam.fLogOnBoot)
                                {
                                    *pCurTo++ = '_';
                                    *pCurTo++ = 'L';
                                    *pCurTo++ = 'O';
                                    *pCurTo++ = 'B';
                                    logParam.adcTrg.iLOB = pCurTo - logParam.adcTrg.szUri;
                                    *pCurTo++ = '0';        // the first number
                                    *pCurTo++ = '0';        // the second number
                                    *pCurTo++ = '0';        // the third number
                                }

                                // append the _0 index.
                                *pCurTo++ = '_';
                                logParam.adcTrg.iNbr = pCurTo - logParam.adcTrg.szUri;
                                *pCurTo++ = '0';        // the first number

                                // now the extenstion
                                strcpy(logParam.adcTrg.szExt, pCurFrom);
                                strcpy(pCurTo, logParam.adcTrg.szExt);
                            }
                            break;

                        case VOLCLOUD:
                            break;

                        default:
                            break;
                    }

                    // if log on boot, save to log on boot in flash
                    if(logParam.fLogOnBoot) logParam.state = LOBWaitWrite;

                    // otherwise delete the log on boot file
                    else                    logParam.state = LOBWaitDelete;
                }
                break;

            case LOBWaitWrite:
                if(fileTask.SetUsage(FILETask::WRITING, VOLFLASH, szLogOnBoot)) logParam.state = FILEopening;
                break;

            case FILEopening:
                fr = FILETask::rgpdFile[FILETask::WRITING]->fsopen(fileTask.GetPath(FILETask::WRITING, true), FA_WRITE | FA_CREATE_ALWAYS);
                if( fr == FR_OK)
                {
                    logParam.state = FILEwrite;
                }
                else if(fr != FR_WAITING_FOR_THREAD)
                {
                    logParam.state = Stopped;
                }
                break;

            case FILEwrite:
                fr = FILETask::rgpdFile[FILETask::WRITING]->fswrite(&logParam, sizeof(logParam), &cbWritten, 100);
                if(fr == FR_OK) 
                {
                    ASSERT(sizeof(logParam) == cbWritten);
                    logParam.state = FILEclose;
                }
                else if(fr != FR_WAITING_FOR_THREAD)
                {
                    logParam.state = FILEclose;
                }
                break;

            case FILEclose:
                if(FILETask::rgpdFile[FILETask::WRITING]->fsclose() != FR_WAITING_FOR_THREAD)
                {
                    fileTask.ClearUsage(FILETask::WRITING);
                    if(fr == FR_OK)     logParam.state = Finishing;
                    else                logParam.state = Stopped;
                }
                break;

            case LOBWaitDelete:
                if(fileTask.SetUsage(FILETask::DELETING, VOLFLASH, szLogOnBoot)) logParam.state = FILEdelete;
                break;

            case FILEdelete:
                fr = DFATFS::fsunlink(fileTask.GetPath(FILETask::DELETING, true));
                if(fr != FR_WAITING_FOR_THREAD)
                {
                    fileTask.ClearUsage(FILETask::DELETING);
                   
                    if(fr == FR_OK || fr == FR_NO_FILE) logParam.state = Finishing;
                    else                                logParam.state = Stopped;
                }
                break;

            case Stopped:

                // take the zero status code out
                oslex.odata[0].cb--;

                // put in the status code
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // just print out the error code and get out
                logParam.state = Done;
                break;

            case Finishing:
                FnLogCommonState(logParam, false);

                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
                oslex.odata[0].cb += sizeof(szWait0)-1;

                // pending says we have data ready to go
                // as long as logParam is not cleard by anothe rall to SetParam, we 
                // stay in the Pending state
                logParam.state = Pending;
                //fall thru

            case Pending:

                // pending says we are ready!
                return(Idle);
                break;

            case Done:

                // wait 0
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
                oslex.odata[0].cb += sizeof(szWait0)-1;
                logParam.state = Idle;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    return(logParam.state);
}
 
STATE FnLogChannel(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    if(queuedParams.iChannel == 0 || queuedParams.iChannel > NBRLOGCH)
    {
        logParam.state = InvalidChannel;
    }
    else if(logParam.rgLogCh[queuedParams.iChannel-1].cAve != 0)
    {
        logParam.state = InvalidSyntax;
    }
    else
    {
        uint32_t    iCh = queuedParams.iChannel - 1;
        LOGCHANNEL& ch  = logParam.rgLogCh[iCh];

        // average count
        ch.cAve                     = queuedParams.rgu32[7];
        if(ch.cAve == 0) ch.cAve    = 1;

        // what is the pwr of 2 average      
        for(ch.pwr2Ave=0; (ch.cAve >> ch.pwr2Ave) > 1; ch.pwr2Ave++);
        if((1ul << ch.pwr2Ave) != ch.cAve) logParam.state  = InvalidSyntax;
    }

    // clear our channel info
    queuedParams.usage     &= ~(QPUINT7);

    queuedParams.rgu32[7]   = 0;        // average
//    queuedParams.rgpv[7]    = NULL;     // storage location
//    queuedParams.rgpv[6]    = NULL;     // URI
//    queuedParams.rgpv[5]    = NULL;     // Cloud Service
 
    // clear the masks
//    queuedParams.usage     &= ~(QPUINT7 | QPPTR7 | QPPTR6| QPPTR5);
//    queuedParams.iScratch   = 0;        // reset the scratch location

    return(Idle);
}

STATE FnLogValueChannel(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    FnChannel(token, szInput, cbInput, jEntry);
    FnLogChannel(token, szInput, cbInput, jEntry);
    return(Idle);
}

STATE FnClearLogParmAndEcho(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    memset(&logParam, 0, sizeof(logParam));
    return(FnEchoToken(token, szInput, cbInput, jEntry));
}

/************************************************************************/
/************************* Print/JSON/menu ******************************/
/*************** mode JSON == debugPrint off ****************************/
/*************** mode menu == debugPrint on *****************************/
/************************************************************************/
#if COMMENT
{
   "mode":"JSON"
}

{
   "mode":"menu"
}

{
   "debugPrint":"on"
}

{
   "debugPrint":"off"
}
#endif

STATE FnPrintOn(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    Serial.EnablePrint(true);
    return(FnEchoToken(token, szInput, cbInput, jEntry));
}

STATE FnPrintOff(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    Serial.EnablePrint(false);
    return(FnEchoToken(token, szInput, cbInput, jEntry));
}

/************************************************************************/
/************************** Update Commands *****************************/
/************************************************************************/
// You must use RealTerm to "Send File"
// Also, this is a BLOCKING CALL!
#if COMMENT
{  
    "update":[
    {
        "command":"updateWFFW"
    },
    {
        "command":"updateWFCert"
    }
    ]
}

{  
    "update":[
    {
        "command":"updateWFFW"
    }
    ]
}

{  
    "update":[
    {
        "command":"updateWFCert"
    }
    ]
}
#endif

STATE FnUpdWFFW(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
   // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    defTask.QueTask(DEFTask::UpdWFFW, -1);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
         
    // wait Until
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
    oslex.odata[0].cb += sizeof(szWaitUntil)-1;

    return(Idle);
}

STATE FnUpdWFCert(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
   // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    defTask.QueTask(DEFTask::UpdWFCert, -1);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
         
    // wait Until
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
    oslex.odata[0].cb += sizeof(szWaitUntil)-1;

    return(Idle);
}

/************************************************************************/
/************************** Test Commands *******************************/
/************************************************************************/
/**************** If the test modules are not linked ********************/
/**************** these commands will not be present ********************/
/***** and will return a WeakReferenceNotPresent (2684354598) error *****/
/************************************************************************/
/************************************************************************/
#if COMMENT
{  
    "test":[
    {
        "command":"calPSK"
    }
    ]
}

{  
    "test":[
    {
        "command":"fat32"
    }
    ]
}

{  
    "test":[
    {
        "command":"gpio"
    }
    ]
}

{  
    "test":[
    {
        "command":"gpioEnd"
    }
    ]
}

{  
    "test":[
    {
        "command":"encoder"
    }
    ]
}

{  
    "test":[
    {
        "command":"encoderEnd"
    }
    ]
}

{  
    "test":[
    {
        "command":"ddr"
    }
    ]
}

{  
    "test":[
    {
        "command":"fifo245"
    }
    ]
}

{  
    "test":[
    {
        "command":"fifo245End"
    }
    ]
}

// Manufacturing test
{
    "test":[
        {
            "command":"run",
            "testNbr": 2
        }
    ]
}


// reponse format:
{
    "test":[
        {
            "command":"run",
            "statusCode": 0,
            "wait": 0,
            "returnNbr": <number>
        }
    ]
}

#endif

STATE FnManufacturningTest(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    STATE       curState    = Idle;
    STATE       retStatus   = Idle;
    uint32_t    msWait      = 0;

    // check that we got a test number
    if((queuedParams.usage & QPINT0) != QPINT0) 
    {
        curState = WeakReferenceNotPresent;
    }

    // if the manufacturing test is not part of the X project
    // MfgTest is a weak reference
    else if(MfgTest == NULL)
    {
        retStatus = Unimplemented;
    }

    // run the test.
    else
    {
        curState = MfgTest(queuedParams.rgi32[0], retStatus, msWait);
    }

    if(curState == Idle)
    {
        // status
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
        oslex.odata[0].cb += sizeof(szStatusCode)-1;

        // put in the status code
        utoa(retStatus, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;

        // return Number
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szReturnNbr, sizeof(szReturnNbr)-1); 
        oslex.odata[0].cb += sizeof(szReturnNbr)-1;

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        
        // test number
        itoa(queuedParams.rgi32[0], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    else if(IsStateAnError(curState))
    {
        utoa(curState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        curState = JSONWait0;
    }

    // still working
    else
    {
        return(curState);
    }

    // wait 
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, sizeof(szWait)-1); 
    oslex.odata[0].cb += sizeof(szWait)-1;

    // how long to wait
    utoa(msWait, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

    return(Idle);
}

STATE FnTestFIFO245End(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(runTest == TESTFIFO245)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTFIFO245End;
    }
    else
    {
        utoa(InstrumentNotConfigured, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnTestFIFO245(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(FIFOEcho == NULL)
    {
        utoa(WeakReferenceNotPresent, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);       
    }
    else if(runTest == TESTNone)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        jsonFT245.EnableIO(false);

        runTest = TESTFIFO245;

        // wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;

        return(Idle);
    }
    else
    {
        utoa(AnotherTestInUse, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnTestDDR(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(TestDDR == NULL)
    {
        utoa(WeakReferenceNotPresent, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);        
    }
    else if(runTest == TESTNone)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTDDR;

        // wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;

        return(Idle);
    }
    else
    {
        utoa(AnotherTestInUse, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnTestEncoderEnd(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(runTest == TESTEncoder)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTEncoderEnd;
    }
    else
    {
        utoa(InstrumentNotConfigured, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnTestEncoder(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(TestTasks == NULL)
    {
        utoa(WeakReferenceNotPresent, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);      
    }
    else if(runTest == TESTNone)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTEncoder;

        // wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;

        return(Idle);
    }
    else
    {
        utoa(AnotherTestInUse, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnTestGPIOEnd(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(runTest == TESTGPIO)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTGPIOEnd;
    }
    else
    {
        utoa(GPIONotRunning, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait Until
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnTestGPIO(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(TestTasks == NULL)
    {
        utoa(WeakReferenceNotPresent, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);       
    }
    else if(runTest == TESTNone)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTGPIO;

        // wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;

        return(Idle);
    }
    else
    {
        utoa(AnotherTestInUse, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnCalPSK(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(TestTasks == NULL)
    {
        utoa(WeakReferenceNotPresent, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);      
    }
    else if(runTest == TESTNone)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTCalPSK;

        // wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;

        return(Idle);
    }
    else
    {
        utoa(AnotherTestInUse, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

STATE FnFat32(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(FATTest == NULL)
    {
        utoa(WeakReferenceNotPresent, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);       
    }
    else if(runTest == TESTNone)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        runTest = TESTFat32;

        // wait Until
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
        oslex.odata[0].cb += sizeof(szWaitUntil)-1;

        return(Idle);
    }
    else
    {
        utoa(AnotherTestInUse, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }
         
    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

/************************************************************************/
/********************** Enumeration *************************************/
/************************************************************************/
#if COMMENT
{  
    "device":[  
    {  
        "command":"enumerate"
    }
    ]
}

{  
    "device":  
    {  
        "command":"enumerate"
    },
    "device":  
    {  
        "command":"loopStatistics"
    }
}

{  
    "device":  
    {  
        "command":"enumerate"
    },
    "device":[  
    {  
        "command":"loopStatistics"
    }
    ]
}

{  
    "device":[  
    {  
        "command":"enumerate"
    }
    ],
    "device":[  
    {  
        "command":"loopStatistics"
    }
    ]
}
#endif

STATE FnEnumeration(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // oslex.odata[0].pbOut
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szEnumeration1, sizeof(szEnumeration1)-1); 
    oslex.odata[0].cb += sizeof(szEnumeration1)-1;

    // put version number in
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szEnumVersion);
    oslex.odata[0].cb += strlen(szEnumVersion);

    // put MAC address out
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szEnumeration2, sizeof(szEnumeration2)-1); 
    oslex.odata[0].cb += sizeof(szEnumeration2)-1;

    // Print out our MAC address
    GetNumb(macMyMacAddr.u8, sizeof(macMyMacAddr), ':', (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

//    memcpy(&pchJSONRespBuff[odata[0].cb], szEnumeration3, sizeof(szEnumeration3)-1); 
//    odata[0].cb += sizeof(szEnumeration3)-1;

    // put in the calibration source  
//    strcpy(&pchJSONRespBuff[odata[0].cb], rgCFGNames[((IDHDR *) rgInstr[OSC2_ID])->cfg]);
//    odata[0].cb += strlen(&pchJSONRespBuff[odata[0].cb]);

    // put MAC address out
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szEnumeration4, sizeof(szEnumeration4)-1); 
    oslex.odata[0].cb += sizeof(szEnumeration4)-1;

    // Print out our MAC address
    GetNumb(macMyMacAddr.u8, sizeof(macMyMacAddr), ':', (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szEnumeration5, sizeof(szEnumeration5)-1); 
    oslex.odata[0].cb += sizeof(szEnumeration5)-1;

    return(Idle);
}

/************************************************************************/
/********************** Calibration *************************************/
/************************************************************************/
#if COMMENT
{
   "device":[
    {
       "command":"calibrationStart"
    }
   ]
}


{
   "device":[
    {
       "command":"calibrationStart",
       "dc": 1
    }
   ]
}

{
   "device":[
    {
       "command":"calibrationStart",
       "awg": 1
    }
   ]
}

{
	"device": [{
		"command": "calibrationStart",
		"dc": [1, 2],
		"awg": 1,
		"daq": [1, 2, 3, 4, 5, 6, 7, 8]
	}]
}

{
   "device":[
    {
       "command":"calibrationStart",
       "daq": 8
    }
   ]
}

{
   "device":[
    {
       "command":"calibrationStart",
       "dc": [1, 2],
       "awg": 1
    }
   ]
}

{
   "device":[
    {
       "command":"calibrationStart",
       "dc": [1, 2]
    }, 
    {  
        "command":"loopStatistics"
    }          
   ]
}
#endif
STATE FnCalibrateStart(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    bool fGnd = (queuedParams.function & 0x10000) == 0x10000;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // check for errors
    if(fGnd && (queuedParams.function & 0xFF) != 0)
    {
        utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;      
        return(Idle);
    }

    // we are good to go
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // if no calibration scheduled, schedule them all
    if((queuedParams.function & 0xFFFF) == 0) queuedParams.function |= 0xFFFF;
        
    // schedule the tasks
    if((queuedParams.function & 0x00000001) && !fGnd)
    {
        defTask.QueTask(DEFTask::CalDC1,     1100);     // DC 1
        arCalIDC[1].state = Calibrating;
    }

    if((queuedParams.function & 0x00000002) && !fGnd)
    {
        arCalIDC[2].state = Calibrating;
        defTask.QueTask(DEFTask::CalDC2,     1100);     // DC 2
    }

    if((queuedParams.function & 0x00000010) && !fGnd)
    {
        calIAWG.polyCal.state = Calibrating;
        defTask.QueTask(DEFTask::CalAWG,     4500);     // AWG 1
    }

    if(queuedParams.function & 0x00000100)
    {
        arCalIDAQ[1].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ1,     500);     // DAQ 1 GND
        else        defTask.QueTask(DEFTask::CalDAQ1,       1500);     // DAQ 1
    }

    if(queuedParams.function & 0x00000200)
    {
        arCalIDAQ[2].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ2,     500);     // DAQ 2 GND
        else        defTask.QueTask(DEFTask::CalDAQ2,       1500);     // DAQ 2
    }

    if(queuedParams.function & 0x00000400)
    {
        arCalIDAQ[3].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ3,     500);     // DAQ 3 GND
        else        defTask.QueTask(DEFTask::CalDAQ3,       1500);     // DAQ 3
    }

    if(queuedParams.function & 0x00000800)
    {
        arCalIDAQ[4].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ4,     500);     // DAQ 4 GND
        else        defTask.QueTask(DEFTask::CalDAQ4,       1500);     // DAQ 4
    }

    if(queuedParams.function & 0x00001000)
    {
        arCalIDAQ[5].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ5,     500);     // DAQ 5 GND
        else        defTask.QueTask(DEFTask::CalDAQ5,       1500);     // DAQ 5
    }

    if(queuedParams.function & 0x00002000)
    {
        arCalIDAQ[6].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ6,     500);     // DAQ 6 GND
        else        defTask.QueTask(DEFTask::CalDAQ6,       1500);     // DAQ 6
    }

    if(queuedParams.function & 0x00004000)
    {
        arCalIDAQ[7].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ7,     500);     // DAQ 7 GND
        else        defTask.QueTask(DEFTask::CalDAQ7,       1500);     // DAQ 7
    }

    if(queuedParams.function & 0x00008000)
    {
        arCalIDAQ[8].state = Calibrating;
        if(fGnd)    defTask.QueTask(DEFTask::CalGndDAQ8,     500);     // DAQ 8 GND
        else        defTask.QueTask(DEFTask::CalDAQ8,       1500);     // DAQ 8
    }
 
    // wait ?
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, sizeof(szWait)-1); 
    oslex.odata[0].cb += sizeof(szWait)-1;
    itoa(defTask.GetWaitTime(), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

    return(Idle);
}

// DC calibration channels
STATE FnCalDC1(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0001;
    return(Idle);
}

STATE FnCalDC2(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0002;
    return(Idle);
}

// AWG calibration channels
STATE FnCalAWG1(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0010;
    return(Idle);
}

// generic channels or DAQ calibration channels, if NOT used for calibration channels as ALL instruments are part of that and can't share channel locaitons in the mask
STATE FnSetCh1(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0100;
    return(Idle);
}

STATE FnSetCh2(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0200;
    return(Idle);
}

STATE FnSetCh3(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0400;
    return(Idle);
}

STATE FnSetCh4(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x0800;
    return(Idle);
}

STATE FnSetCh5(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x1000;
    return(Idle);
}

STATE FnSetCh6(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x2000;
    return(Idle);
}

STATE FnSetCh7(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x4000;
    return(Idle);
}

STATE FnSetCh8(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    queuedParams.function |= 0x8000;
    return(Idle);
}

/************************************************************************/
/********************** Calibration Read ********************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{  
    "device":[  
    {  
        "command":"calibrationRead"
    }
    ]
}

{
	"device": [{
		"command": "calibrationStart",
		"dc": [1, 2],
		"awg": 1,
		"daq": [1, 2, 3, 4, 5, 6, 7, 8]
	}]
}
#endif


STATE FnCalibrateRead(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-2); 
    oslex.odata[0].cb += sizeof(szWait0)-2;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPolynomial, sizeof(szPolynomial)-1); 
    oslex.odata[0].cb += sizeof(szPolynomial)-1;
 
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCalibrationData, sizeof(szCalibrationData)-1); 
    oslex.odata[0].cb += sizeof(szCalibrationData)-1;


    //*************************** DC  ********************************
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDC, sizeof(szDC)-1); 
    oslex.odata[0].cb += sizeof(szDC)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    //*************************** DC 1  ********************************
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szOne, sizeof(szOne)-1); 
    oslex.odata[0].cb += sizeof(szOne)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDC[1].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDC[1].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDC[1].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientB, sizeof(szCoefficientB)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientB)-1;

    illtoa(arCalIDC[1].B, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDC[0].B, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDC[1].B, arCalIDC[0].B, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDC[1].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDC[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDC[1].C, arCalIDC[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';
    
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DC 2  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szTwo, sizeof(szTwo)-1); 
    oslex.odata[0].cb += sizeof(szTwo)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDC[1].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDC[1].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDC[1].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientB, sizeof(szCoefficientB)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientB)-1;

    illtoa(arCalIDC[2].B, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDC[0].B, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDC[2].B, arCalIDC[0].B, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDC[2].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDC[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDC[2].C, arCalIDC[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** AWG   ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szAWG, sizeof(szAWG)-1); 
    oslex.odata[0].cb += sizeof(szAWG)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    //*************************** AWG 1  ********************************
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szOne, sizeof(szOne)-1); 
    oslex.odata[0].cb += sizeof(szOne)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(calIAWG.polyCal.state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[calIAWG.polyCal.state]); 
    oslex.odata[0].cb += strlen(rgszStates[calIAWG.polyCal.state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
   
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientB, sizeof(szCoefficientB)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientB)-1;

    illtoa(calIAWG.polyCal.B, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(11995, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(calIAWG.polyCal.B, 11995, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(calIAWG.polyCal.C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(2099125, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(calIAWG.polyCal.C, 2099125, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ   ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szDAQ, sizeof(szDAQ)-1); 
    oslex.odata[0].cb += sizeof(szDAQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    //*************************** DAQ 1  ********************************
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szOne, sizeof(szOne)-1); 
    oslex.odata[0].cb += sizeof(szOne)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[1].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[1].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[1].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[1].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[1].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[1].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[1].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 2  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szTwo, sizeof(szTwo)-1); 
    oslex.odata[0].cb += sizeof(szTwo)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[2].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[2].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[2].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[2].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[2].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[2].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[2].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 3  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szThree, sizeof(szThree)-1); 
    oslex.odata[0].cb += sizeof(szThree)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[3].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[3].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[3].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[3].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[3].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[3].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[3].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 4  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szFour, sizeof(szFour)-1); 
    oslex.odata[0].cb += sizeof(szFour)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[4].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[4].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[4].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[4].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[4].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[4].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[4].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 5  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szFive, sizeof(szFive)-1); 
    oslex.odata[0].cb += sizeof(szFive)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[5].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[5].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[5].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[5].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[5].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[5].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[5].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 6  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSix, sizeof(szSix)-1); 
    oslex.odata[0].cb += sizeof(szSix)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[6].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[6].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[6].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[6].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[6].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[6].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[6].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 7  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSeven, sizeof(szSeven)-1); 
    oslex.odata[0].cb += sizeof(szSeven)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[7].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[7].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[7].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[7].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[7].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[7].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[7].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    //*************************** DAQ 8  ********************************
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szEight, sizeof(szEight)-1); 
    oslex.odata[0].cb += sizeof(szEight)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
    oslex.odata[0].cb += sizeof(szJSONest)-1;

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    ASSERT(arCalIDAQ[8].state < EndOfCommonStates);
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[arCalIDAQ[8].state]); 
    oslex.odata[0].cb += strlen(rgszStates[arCalIDAQ[8].state]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientA, sizeof(szCoefficientA)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientA)-1;

    illtoa(arCalIDAQ[8].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].A, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[8].A, arCalIDAQ[0].A, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCoefficientC, sizeof(szCoefficientC)-1); 
    oslex.odata[0].cb += sizeof(szCoefficientC)-1;

    illtoa(arCalIDAQ[8].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    illtoa(arCalIDAQ[0].C, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
    
    GetPercent(arCalIDAQ[8].C, arCalIDAQ[0].C, 2, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 20);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

    return(Idle);
}


STATE FnCalibrateGetStatus(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{  
    int32_t i = 0;
    STATE   rgCalibrationStates[11] = { calIAWG.polyCal.state, 
                                        arCalIDC[1].state, arCalIDC[2].state,
                                        arCalIDAQ[1].state, arCalIDAQ[2].state, arCalIDAQ[3].state, arCalIDAQ[4].state,
                                        arCalIDAQ[5].state, arCalIDAQ[6].state, arCalIDAQ[7].state, arCalIDAQ[8].state};
    uint32_t    cCalibrated         = 0;
    uint32_t    cCalibrating        = 0;
    uint32_t    cUncalibrated       = 0;
    uint32_t    cFailedCalibration  = 0;
    int32_t     cWait               = 0;

    for(i=0; i<11; i++)
    {
        switch(rgCalibrationStates[i])
        {
            case Calibrated:
                cCalibrated++;
                break;

            case Calibrating:
                cCalibrating++;
                break;

            case Uncalibrated:
                cUncalibrated++;
                break;

            case FailedCalibration:
                cFailedCalibration++;
                break;

            default:
                ASSERT(NEVER_SHOULD_GET_HERE);
                break;
        }
    }

    // status code
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatus, sizeof(szStatus)-1); 
    oslex.odata[0].cb += sizeof(szStatus)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;

    // everything is calibrated
    if(cCalibrated == 11)
    {
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Calibrated]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    // something failed to calibrate
    else if(cFailedCalibration > 0)
    {
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[FailedCalibration]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    else if(cCalibrating > 0)
    {
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Calibrating]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        cWait   = defTask.GetWaitTime(DEFTask::CalAWG);

        i = defTask.GetWaitTime(DEFTask::CalDC1);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDC2);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ1);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ2);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ3);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ4);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ5);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ6);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ7);
        if(i > cWait) cWait = i;
        i = defTask.GetWaitTime(DEFTask::CalDAQ8);
        if(i > cWait) cWait = i;
    }

    // uncalibrated
    else 
    {
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Uncalibrated]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    // wait  amount
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, sizeof(szWait)-1); 
    oslex.odata[0].cb += sizeof(szWait)-1;      
    itoa(cWait, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

    return(Idle);
}

/************************************************************************/
/********************** Calibration Save ********************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
   "device":[
      {
         "command":"calibrationSave",
         "type": "flash"
      }
   ]
}

{
   "device":[
      {
         "command":"calibrationSave",
         "type": "sd0"
      }
   ]
}

{
   "device":[
    {
       "command":"calibrationStart"
    }
   ]
}
#endif
STATE FnCalibrateSave(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    bool fCalibrated = calIAWG.polyCal.state == Calibrated;
    int32_t i;

    // Check that all are calibrated
    for(i=1; i<3; i++) fCalibrated &= arCalIDC[i].state == Calibrated;
    for(i=1; i<9; i++) fCalibrated &= arCalIDAQ[i].state == Calibrated;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(!(queuedParams.function == 0x1 || queuedParams.function == 0x2))
    {
        utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    else if(fCalibrated)
    {
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

        arCalIDC[0].vol = queuedParams.function == 0x1 ? VOLFLASH : VOLSD;
        defTask.QueTask(DEFTask::SaveCalibration, 1000);     
 
        // wait ?
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, sizeof(szWait)-1); 
        oslex.odata[0].cb += sizeof(szWait)-1;
        itoa(defTask.GetWaitTime(), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

        return(Idle);
    }

    else
    {
        utoa(MkStateAnError(Uncalibrated), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;      

    return(Idle);
}

/************************************************************************/
/****************** Calibration Get Instructions ************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
   "device":[
      {
         "command":"calibrationGetInstructions"
      }
   ]
}
#endif
STATE FnCalibrateGetInstructions(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    bool fGnd = (queuedParams.function & 0x10000) == 0x10000;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    if(fGnd)
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCalibrationGndInstructions, sizeof(szCalibrationGndInstructions)-1); 
        oslex.odata[0].cb += sizeof(szCalibrationGndInstructions)-1;
    }
    else
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCalibrationInstructions, sizeof(szCalibrationInstructions)-1); 
        oslex.odata[0].cb += sizeof(szCalibrationInstructions)-1;
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;      

    return(Idle);
}

/************************************************************************/
/****************** Calibration Get Storage Types ***********************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
   "device":[
      {
         "command":"calibrationGetStorageTypes"
      }
   ]
}
#endif
STATE FnCalibrateGetStorageTypes(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szCalibrationStorageTypes, sizeof(szCalibrationStorageTypes)-1); 
    oslex.odata[0].cb += sizeof(szCalibrationStorageTypes)-1;

    if(fSd0)
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSD0, sizeof(szSD0)-1); 
        oslex.odata[0].cb += sizeof(szSD0)-1;

        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    }
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;      

    return(Idle);
}

/************************************************************************/
/********************** Calibration Load ********************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
   "device":[
      {
         "command":"calibrationLoad",
         "type": "flash"
      }
   ]
}

{
   "device":[
      {
         "command":"calibrationLoad",
         "type": "sd0"
      }
   ]
}
#endif
STATE FnCalibrateLoad(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    static STATE curState = Idle;
    STATE retState = Idle;

    switch(curState)
    {
        case Idle:
            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            if(!(queuedParams.function == 0x1 || queuedParams.function == 0x2))
            {
                utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = Done;
            }

            // use the parameter as a temp
            queuedParams.rgu32[0] = queuedParams.function == 0x1 ? VOLFLASH : VOLSD;
            curState = FILEread;
            break;

        case FILEread:
            if( (retState = ReadCalibrationData((VOLTYPE) queuedParams.rgu32[0])) == Idle)
            {
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
                curState = Done;
            }
            else if(IsStateAnError(retState))
            {
                utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = Done;
            }
            break;

        case Done:

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1; 
            curState = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(curState);
}

/************************************************************************/
/*************************** FILE IO ************************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
    "file": [{
        "command":"read",
        "type":"sd0",
        "path":"cordova.js",
        "filePosition":0,
        "requestedLength":-1
    }]
}

{
    "file": [{
        "command":"read",
        "type":"sd0",
        "path":"./build/main.js",
        "filePosition":0,
        "requestedLength":-1
    }]
}

{
    "file": [{
        "command":"read",
        "type":"sd0",
        "path":"abc.js",
        "filePosition":0,
        "requestedLength":-1
    }]
}

{  
    "device":[  
    {  
        "command":"loopStatistics"
    }
    ]
}

#endif

STATE FnFileWrite(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    FRESULT         fr          = FR_OK;

    switch(curState)
    {
        case Idle:
 
            // type:            Func0 => flash, Func1 => sd0 ; put in QPUINT7
            // path:            QPPTR0
            // fileposition:    QPUINT0
            // binaryOffset     QPUINT1
            // binaryLength     QPUINT2
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // check to see if we got all of the parameters we need
            if( (((QPUINT0 | QPUINT1  | QPUINT2 | QPPTR0) & queuedParams.usage) != (QPUINT0 | QPUINT1 | QPUINT2 | QPPTR0))  || 
                (0x3 & queuedParams.function) == 0                                                                          ||
                (0x3 & queuedParams.function) == 0x3                                                                        )
            {
                utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
                break;
            }
            queuedParams.rgu32[7] = (queuedParams.function & 0x1) ? VOLFLASH : VOLSD;

            // SD card not loaded
            if(queuedParams.rgu32[7] == VOLSD && !fSd0)
            {
                 utoa(MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
                break;
            }

            if(!fileTask.SetUsage(FILETask::WRITING, (VOLTYPE) queuedParams.rgu32[7], (char *) queuedParams.rgpv[0]))
            {
                utoa(MkStateAnError(FILECSTATE | FR_LOCKED), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
                break;
            }
            curState = WaitingForResources;

        case WaitingForResources:
            fr = FILETask::rgpdFile[FILETask::WRITING]->fsopen(fileTask.GetPath(FILETask::WRITING, true), FA_WRITE | FA_OPEN_ALWAYS);
            if(fr == FR_OK)
            {
                 curState = JSONSeekFile;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                ASSERT(fileTask.GetPath(FILETask::WRITING, true) != NULL);
                fileTask.ClearUsage(FILETask::WRITING);
                curState = JSONWait0;
            }
            break;

        case JSONSeekFile:
            fr = FILETask::rgpdFile[FILETask::WRITING]->fslseek(queuedParams.rgi32[0]);
            if(fr == FR_OK)
            {
                curState = JSONWriteFile;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONClose;
            }
            break;

        case JSONWriteFile:

            // successful status code
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

            // File Data
            oslex.idata[oslex.cIData].iBinary = queuedParams.rgu32[1];
            oslex.idata[oslex.cIData].cb = queuedParams.rgu32[2];
            oslex.idata[oslex.cIData].WriteData = &OSPAR::WriteFile;

            // type
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
            oslex.odata[0].cb += sizeof(szType)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[queuedParams.rgu32[7]]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // path
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPath, sizeof(szPath)-1); 
            oslex.odata[0].cb += sizeof(szPath)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], (char *) queuedParams.rgpv[0]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
 
            // actual file position
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualFilePosition, sizeof(szActualFilePosition)-1); 
            oslex.odata[0].cb += sizeof(szActualFilePosition)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';
            utoa(queuedParams.rgu32[0], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // actual length
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualLength, sizeof(szActualLength)-1); 
            oslex.odata[0].cb += sizeof(szActualLength)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';
            itoa(oslex.idata[oslex.cIData].cb, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            oslex.cIData++;

            curState = JSONWait0;
            break;

        case JSONClose:
            if(FILETask::rgpdFile[FILETask::WRITING]->fsclose() != FR_WAITING_FOR_THREAD)
            {
                ASSERT(fileTask.GetPath(FILETask::WRITING, true) != NULL);
                fileTask.ClearUsage(FILETask::WRITING);
                curState = JSONWait0;
            }
            break;

        case JSONWait0:
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
            curState = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(curState);
}

#if COMMENT
C0
{
    "file": {
        "command":"write",
        "type":"sd0",
        "path":"HelloWorld.txt",
        "filePosition":0,
        "binaryOffset":0,
        "binaryLength":13
    }
}
D
Hello World 4
0

#endif

STATE FnFileRead(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    static STATE    curState    = Idle;
    FRESULT         fr          = FR_OK;
    int32_t         i, j;

    switch(curState)
    {
        case Idle:
 
            // type:            Func0 => flash, Func1 => sd0 ; put in QPUINT7
            // path:            QPPTR0
            // fileposition:    QPUINT0
            // requestedLength: QPINT1
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // check to see if we got all of the parameters we need
            if( (((QPUINT0 | QPINT1 | QPPTR0) & queuedParams.usage) != (QPUINT0 | QPINT1 | QPPTR0))   || 
                (0x3 & queuedParams.function) == 0                                                  ||
                (0x3 & queuedParams.function) == 0x3                                                )
            {
                utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
                break;
            }

            queuedParams.rgu32[7] = (queuedParams.function & 0x1) ? VOLFLASH : VOLSD;

            // SD card not loaded
            if(queuedParams.rgu32[7] == VOLSD && !fSd0)
            {
                 utoa(MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
                break;
            }

            if(!fileTask.SetUsage(FILETask::READING, (VOLTYPE) queuedParams.rgu32[7], (char *) queuedParams.rgpv[0]))
            {
                utoa(MkStateAnError(FILECSTATE | FR_LOCKED), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
                break;
            }
            curState = WaitingForResources;

        case WaitingForResources:
            fr = FILETask::rgpdFile[FILETask::READING]->fsopen(fileTask.GetPath(FILETask::READING, true), FA_READ);
            if(fr == FR_OK)
            {
                // can we read the requested data?
                if(queuedParams.rgi32[1] == -1) queuedParams.rgi32[1] = FILETask::rgpdFile[FILETask::READING]->fssize() - queuedParams.rgu32[0];
                if(queuedParams.rgu32[0] > FILETask::rgpdFile[FILETask::READING]->fssize() || (queuedParams.rgu32[0] + (uint32_t) queuedParams.rgi32[1]) > FILETask::rgpdFile[FILETask::READING]->fssize())
                {
                    utoa(ValueOutOfRange, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                    curState = JSONClose;
                }
                else
                {
                    curState = JSONSeekFile;
                }
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                ASSERT(fileTask.GetPath(FILETask::READING, true) != NULL);
                fileTask.ClearUsage(FILETask::READING);
                curState = JSONWait0;
            }
            break;

        case JSONSeekFile:
            fr = FILETask::rgpdFile[FILETask::READING]->fslseek(queuedParams.rgu32[0]);
            if(fr == FR_OK)
            {
                curState = JSONReadFile;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONClose;
            }
            break;

        case JSONReadFile:
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

            // File Data
            oslex.odata[oslex.cOData].pbOut = NULL;
            oslex.odata[oslex.cOData].cb = queuedParams.rgi32[1];
            oslex.odata[oslex.cOData].ReadData = &OSPAR::ReadFile;

            // type
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
            oslex.odata[0].cb += sizeof(szType)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[queuedParams.rgu32[7]]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // path
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPath, sizeof(szPath)-1); 
            oslex.odata[0].cb += sizeof(szPath)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], (char *) queuedParams.rgpv[0]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
 
            // actual file position
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualFilePosition, sizeof(szActualFilePosition)-1); 
            oslex.odata[0].cb += sizeof(szActualFilePosition)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';
            utoa(queuedParams.rgu32[0], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // actual length
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualLength, sizeof(szActualLength)-1); 
            oslex.odata[0].cb += sizeof(szActualLength)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';
            itoa(queuedParams.rgi32[1], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // offset in the OSBJ
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szBinaryOffset, sizeof(szBinaryOffset)-1); 
            oslex.odata[0].cb += sizeof(szBinaryOffset)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';

            for(i=1, j=0; i<oslex.cOData; i++) j += oslex.odata[i].cb;
            utoa(j, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // length of binary 
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szBinaryLength, sizeof(szBinaryLength)-1); 
            oslex.odata[0].cb += sizeof(szBinaryLength)-1;
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';
            itoa(oslex.odata[oslex.cOData].cb, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            oslex.cOData++;

            curState = JSONWait0;
            break;

        case JSONClose:
            if(FILETask::rgpdFile[FILETask::READING]->fsclose() != FR_WAITING_FOR_THREAD)
            {
                ASSERT(fileTask.GetPath(FILETask::READING, true) != NULL);
                fileTask.ClearUsage(FILETask::READING);
                curState = JSONWait0;
            }
            break;

        case JSONWait0:
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;
            curState = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    return(curState);
}

#if COMMENT
{
    "file": [{
        "command": "delete",
        "type": "sd0",
        "path": "HelloWorld.txt"
    }]
}

{
    "file": [{
        "command": "delete",
        "type": "flash",
        "path": "HelloWorld.txt"
    }]
}
#endif

STATE FnFileDelete(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static STATE    retState    = Idle;
    static bool     fWait0      = false;
    FRESULT         fr          = FR_OK;

    switch(curState)
    {
        case Idle:

            // check for input errors
            // type:            Func0 => flash, Func1 => sd0 ; put in QPUINT7
            // path:            QPPTR0
            // VOLID:           QPUINT7
            // check to see if we got all of the parameters we need
            if( ((QPPTR0 & queuedParams.usage) != QPPTR0)   || 
                (0x3 & queuedParams.function) == 0          ||
                (0x3 & queuedParams.function) == 0x3        )
            {
                retState    =  InvalidParameter;
                fWait0      = true;
                curState    = Done;
                break;
            }

            queuedParams.rgu32[7] = (queuedParams.function & 0x1) ? VOLFLASH : VOLSD;

            // SD card not loaded
            if(queuedParams.rgu32[7] == VOLSD && !fSd0)
            {
                retState    = MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM);
                fWait0      = true;
                curState    = Done;
                break;
            }

            if(!fileTask.SetUsage(FILETask::DELETING, (VOLTYPE) queuedParams.rgu32[7], (char *) queuedParams.rgpv[0]))
            {
                retState    =  FileInUse;
                fWait0      = true;
                curState    = Done;
                break;
            }
            curState    = FILEexist;
            
            // fall thru
            
        case FILEexist:
            if((fr = DFATFS::fsexists(fileTask.GetPath(FILETask::DELETING, true))) == FR_OK)
            {
                // put on the deferred queue
                defTask.QueTask(DEFTask::FileDelete, -1);
                curState = Done;
            }

            // clear it if the file does not exist
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::DELETING);
                // error code will set the state
            }
            break;

         case Done:
            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // put out the status code, retState == FR_OK, which is status 0
            utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // if no errors, then we can print out the file info
            if(retState != InvalidParameter)
            {
                // type
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
                oslex.odata[0].cb += sizeof(szType)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[queuedParams.rgu32[7]]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // path
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPath, sizeof(szPath)-1); 
                oslex.odata[0].cb += sizeof(szPath)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], (char *) queuedParams.rgpv[0]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
            }

            // print wait time.
            if(fWait0)
            {
                // put out the wait
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
                oslex.odata[0].cb += sizeof(szWait0)-1;
            }
            else
            {
                // put out the wait
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaitUntil, sizeof(szWaitUntil)-1); 
                oslex.odata[0].cb += sizeof(szWaitUntil)-1;
            }
          
            // we are done
            retState    = Idle;
            curState    = Idle;
            fWait0      = false;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    // on an error
    if(!(fr == FR_OK || fr == FR_WAITING_FOR_THREAD))
    {
        retState = MkStateAnError(FILECSTATE | fr);
        fWait0 = true;
        curState = Done;
    }
  
    return(curState);
}

#if COMMENT
{
    "file": [{
        "command": "getFileSize",
        "type": "sd0",
        "path": "HelloWorld.txt"
    }]
}

{
    "file": [{
        "command": "getFileSize",
        "type": "flash",
        "path": "HelloWorld.txt"
    }]
}
#endif

STATE FnFileGetSize(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static FRESULT  fr          = FR_OK;

    switch(curState)
    {
        case Idle:

             // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // check for input errors
            // type:            Func0 => flash, Func1 => sd0 ; put in QPUINT7
            // path:            QPPTR0
            // VOLID:           QPUINT7
            // check to see if we got all of the parameters we need
            if( ((QPPTR0 & queuedParams.usage) != QPPTR0)   || 
                (0x3 & queuedParams.function) == 0          ||
                (0x3 & queuedParams.function) == 0x3        )
            {
                utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                fr = FR_INVALID_PARAMETER;
                curState = JSONWait0;
                break;
            }

            queuedParams.rgu32[7] = (queuedParams.function & 0x1) ? VOLFLASH : VOLSD;

            // SD card not loaded
            if(queuedParams.rgu32[7] == VOLSD && !fSd0)
            {
                utoa(MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                fr = FR_NO_FILESYSTEM;
                curState = JSONWait0;
                break;
            }

            if(!fileTask.SetUsage(FILETask::READING, (VOLTYPE) queuedParams.rgu32[7], (char *) queuedParams.rgpv[0]))
            {
                utoa(MkStateAnError(FILECSTATE | FR_LOCKED), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                fr = FR_LOCKED;
                curState = JSONWait0;
                break;
            }

            curState = FILEopen;
            // fall thru

        case FILEopen:

            fr = FILETask::rgpdFile[FILETask::READING]->fsopen(fileTask.GetPath(FILETask::READING, true), FA_READ);
            if(fr == FR_OK)
            {

                // good status
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

                // can we read the requested data?
                queuedParams.rgu32[6] = FILETask::rgpdFile[FILETask::READING]->fssize();        // can go to 4GB
                curState = FILEclose;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                ASSERT(fileTask.GetPath(FILETask::READING, true) != NULL);
                fileTask.ClearUsage(FILETask::READING);
                curState = JSONWait0;
            }
            break;

        case FILEclose:
            if(FILETask::rgpdFile[FILETask::READING]->fsclose() != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::READING);
                curState = JSONWait0;
            }
            break;

         case JSONWait0:

            // if no parameter errors, then we can print out the parameters
            if(fr != FR_INVALID_PARAMETER)
            {
                // type
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
                oslex.odata[0].cb += sizeof(szType)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[queuedParams.rgu32[7]]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // path
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPath, sizeof(szPath)-1); 
                oslex.odata[0].cb += sizeof(szPath)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], (char *) queuedParams.rgpv[0]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // if we got the size, print it out
                if(fr == FR_OK)
                {
                    // file size
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualFileSize, sizeof(szActualFileSize)-1); 
                    oslex.odata[0].cb += sizeof(szActualFileSize)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
                    utoa(queuedParams.rgu32[6], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                 }
                 else
                 {
                    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
                 }
            }

            // put out the wait
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;

            // we are done
            fr          = FR_OK;
            curState    = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;

    }
  
    return(curState);
}

#if COMMENT
{
    "file": [{
        "command": "getFreeSpace",
        "type": "sd0",
    }]
}

{
    "file": [{
        "command": "getFileSize",
        "type": "flash",
        "path": "HelloWorld.txt"
    }]
}
#endif

STATE FnFileGetFreeSpace(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState    = Idle;
    static FRESULT  fr          = FR_OK;
    static uint32_t cClusters;
    static uint32_t cSecClust;

    switch(curState)
    {
        case Idle:

            fr = FR_OK;

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // check for input errors
            // type:            Func0 => flash, Func1 => sd0 ; put in QPUINT7
            // VOLID:           QPUINT7
            // check to see if we got all of the parameters we need
            if( (0x3 & queuedParams.function) == 0          ||
                (0x3 & queuedParams.function) == 0x3        )
            {
                utoa(InvalidParameter, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                fr = FR_INVALID_PARAMETER;
                curState = JSONWait0;
                break;
            }

            queuedParams.rgu32[7] = (queuedParams.function & 0x1) ? VOLFLASH : VOLSD;

            // SD card not loaded
            if(queuedParams.rgu32[7] == VOLSD && !fSd0)
            {
                utoa(MkStateAnError(FILECSTATE | FR_NO_FILESYSTEM), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                fr = FR_NO_FILESYSTEM;
                curState = JSONWait0;
                break;
            }

            curState = FILEgetsize;
            // fall thru

        case FILEgetsize:
            if((fr = DFATFS::fsgetfree(DFATFS::szFatFsVols[queuedParams.rgu32[7]], &cClusters, &cSecClust)) == FR_OK)
            {

                // okay status code
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
                curState = JSONWait0;
            }
            else if(fr != FR_WAITING_FOR_THREAD)
            {
                utoa(MkStateAnError(FILECSTATE | fr), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                curState = JSONWait0;
            }
            break;

         case JSONWait0:

            // if no parameter errors, then we can print out the parameters
            if(fr != FR_INVALID_PARAMETER)
            {
                // type
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
                oslex.odata[0].cb += sizeof(szType)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
                strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[queuedParams.rgu32[7]]);
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                // if we got the size, print it out
                if(fr == FR_OK)
                {
                    uint32_t cFSectors = cClusters * cSecClust;
                    uint64_t cFBytes = ((uint64_t) cFSectors) * 512;

                    // clusters                
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
                    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szClusters, sizeof(szClusters)-1); 
                    oslex.odata[0].cb += sizeof(szClusters)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
                    utoa(cClusters, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                    // sectors
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSectors, sizeof(szSectors)-1); 
                    oslex.odata[0].cb += sizeof(szSectors)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
                    utoa(cFSectors, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

                    // bytes
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szBytes, sizeof(szBytes)-1); 
                    oslex.odata[0].cb += sizeof(szBytes)-1;
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
                    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
                    ulltoa(cFBytes, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                 }
                 else
                 {
                    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
                 }
            }

            // put out the wait
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;

            // we are done
            fr          = FR_OK;
            curState    = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;

    }
  
    return(curState);
}


#if COMMENT
{
    "file":[{
        "command":"getCurrentState",
    }]
}

// Returns:
{
	"file": {
		"command": "getCurrentState",
		"statusCode": 0,
		"deleting": {
			"type": "sd0",
			"path": "HelloWorld.txt"
		},
		"wait": 0
	}
}
#endif

STATE FnFileGetCurrentState(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    uint32_t i;

    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // loop through and see what is active
    for(i=1; i<FILETask::ENDDOING; i++)
    {
        if(fileTask.GetPath((FILETask::DOING) i, true) != NULL)
        {
            // what are we doing?
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], FILETask::rgszDoing[i]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // what is the type            
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONest, sizeof(szJSONest)-1); 
            oslex.odata[0].cb += sizeof(szJSONest)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
            oslex.odata[0].cb += sizeof(szType)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[fileTask.GetVol((FILETask::DOING) i)]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // what is the file
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPath, sizeof(szPath)-1); 
            oslex.odata[0].cb += sizeof(szPath)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], fileTask.GetPath((FILETask::DOING) i, false));
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSONObjEndCB, sizeof(szJSONObjEndCB)-1); 
            oslex.odata[0].cb += sizeof(szJSONObjEndCB)-1;
        }
    }

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

#if COMMENT
{
    "file": [{
        "command": "makeFS",
        "type": "sd0"
    }]
}

{
    "file": [{
        "command": "makeFS",
        "type": "flash"
    }]
}

// Returns:
{
    "file": [{
        "command": "makeFS",
        "type": "sd0",
        "statusCode": 0,
        "wait": 0
    }]
}
#endif

STATE FnFileMkFS(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static  STATE   curState    = Idle;
    static  STATE   retState    = Idle;
            FRESULT fr          = FR_OK;
    char const * const szVol    = ((queuedParams.function & 0x3) == 1) ? DFATFS::szFatFsVols[VOLFLASH] : DFATFS::szFatFsVols[VOLSD];

    // 0 == flash
    // 1 == sd0
    // queuedParams.function   = (1 << index);

    switch(curState)
    {
        case Idle:
            
            // assume an error
            curState = Done;

            // check parameters.
            if(((queuedParams.function & 3) == 0  || (queuedParams.function & 3) == 3)) retState = InvalidParameter;

            // no sd card plugged in
            else if((queuedParams.function & 3) == 2 && GetGPIO(SD_PIN_DET))            retState = NoSDCard;               

            // sd or flash, no error
            else                                                                        curState = FILEunmount;

            break;

        case FILEunmount:
            if(DFATFS::fsunmount(szVol) != FR_WAITING_FOR_THREAD)
            {
                curState = FILEmkfs;
            }
            break;

        case FILEmkfs:
            if((queuedParams.function & 0x3) == 2)  fr = DFATFS::fsmkfs(dSDVol);
            else                                    fr = DFATFS::fsmkfs(flashVol);

            if(fr != FR_WAITING_FOR_THREAD)
            {
                if(fr == FR_OK)
                {
                    curState = FILEmount;
                }
                else
                {
                    retState = MkStateAnError(FILECSTATE | fr);
                    curState = Done;
                }
            }
            break;


        case FILEmount:
            if((queuedParams.function & 0x3) == 2)  fr = DFATFS::fsmount(dSDVol, DFATFS::szFatFsVols[VOLSD], 1);
            else                                    fr = DFATFS::fsmount(flashVol, DFATFS::szFatFsVols[VOLFLASH], 1);

            if(fr != FR_WAITING_FOR_THREAD)
            {
                curState = Done;
                if(fr == FR_OK) fSd0        = true;
                else            retState    = MkStateAnError(FILECSTATE | fr);
            }
            break;

        case Done:

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // put out the status code, retState == FR_OK, which is status 0
            utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // type
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
            oslex.odata[0].cb += sizeof(szType)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[(((queuedParams.function & 0x3) == 2) ? VOLSD : VOLFLASH)]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // wait 0
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;

            retState = Idle;
            curState = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;

    }

    return(curState);
}

#if COMMENT
{
    "file": [{
        "command": "listdir",
        "type": "sd0",
        "path": "/"
    }]
}

{
    "file": [{
        "command": "listdir",
        "type": "flash",
        "path": "/"
    }]
}

// Returns:
{
    "file": [{
        "command": "listdir",
        "type": "sd0",
        "path": "/",
        "files": ["fileone.txt", "filetwo.bin"],
        "statusCode": 0,
        "wait": 0
    }]
}
#endif

STATE FnFileListdir(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static  STATE       curState    = Idle;
    static  STATE       retState    = Idle;
            FRESULT     fr          = FR_OK;

    switch(curState)
    {
        case Idle:

            retState    = Idle;

            // check for input errors
            // type:            Func0 => flash, Func1 => sd0 ; put in QPUINT7
            // path:            QPPTR0
            // VOLID:           QPUINT7
            // check to see if we got all of the parameters we need
            if( ((QPPTR0 & queuedParams.usage) != QPPTR0)   || 
                (0x3 & queuedParams.function) == 0          ||
                (0x3 & queuedParams.function) == 0x3        )
            {
                fr =  FR_INVALID_PARAMETER;
                curState = JSONWait0;
                break;
            }

            queuedParams.rgu32[7] = (queuedParams.function & 0x1) ? VOLFLASH : VOLSD;

            // SD card not loaded
            if(queuedParams.rgu32[7] == VOLSD && !fSd0)
            {
                fr = FR_NO_FILESYSTEM;
                curState = JSONWait0;
                break;
            }

            // say we are calibrating
            if(!fileTask.SetUsage(FILETask::DIRECTORY, (VOLTYPE) queuedParams.rgu32[7], (char *) queuedParams.rgpv[0]))  
            {
                fr = FR_LOCKED;
                curState = JSONWait0;
                break;
            }

            // type
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szType, sizeof(szType)-1); 
            oslex.odata[0].cb += sizeof(szType)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgVOLNames[queuedParams.rgu32[7]]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // path
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szPath, sizeof(szPath)-1); 
            oslex.odata[0].cb += sizeof(szPath)-1;
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
            oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
            strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], (char *) queuedParams.rgpv[0]);
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

            curState = FILEopendir;
            // fall thru

        case FILEopendir:
            if((fr = DDIRINFO::fsopendir(fileTask.GetPath(FILETask::DIRECTORY, true))) == FR_OK)
            { 
                // files
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
                oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szFile, sizeof(szFile)-1); 
                oslex.odata[0].cb += sizeof(szFile)-1;
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = 's';
                memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCB, sizeof(szJSOMemberSepQCB)-1); 
                oslex.odata[0].cb += sizeof(szJSOMemberSepQCB)-1;
                
                DDIRINFO::fssetLongFilename((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb+1]);
                DDIRINFO::fssetLongFilenameLength(sizeof(pchJSONRespBuff) - oslex.odata[0].cb - 1);

                curState = FILEreaddir;
            }
            break;

        case FILEreaddir:
            if((fr = DDIRINFO::fsreaddir()) != FR_WAITING_FOR_THREAD)
            {
                const char * szFileName = NULL;

                // only get the file if we got it, otherwise pretend we are done
                if(fr == FR_OK)
                {
                    // make room for a quote
                    oslex.odata[0].cb++;  

                    // try and get the long file name
                    szFileName = DDIRINFO::fsgetLongFilename();

                    if(szFileName[0] == '\0' && (szFileName = DDIRINFO::fsget8Dot3Filename()) != NULL && szFileName[0] != '\0')
                    {
                        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], szFileName);
                    }
                }

                if(szFileName != NULL && szFileName[0] != '\0')
                {
                    oslex.odata[0].pbOut[oslex.odata[0].cb-1] = '\"';
                    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
                    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQC, sizeof(szJSOValueSepQC)-1); 
                    oslex.odata[0].cb += sizeof(szJSOValueSepQC)-1;

                    DDIRINFO::fssetLongFilename((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb+1]);
                    DDIRINFO::fssetLongFilenameLength(sizeof(pchJSONRespBuff) - oslex.odata[0].cb - 1);
                }

                // we are done
                else
                {
                    // if this is first file in the list, there will be a "," after the last file name
                    // but we went forward to allow for the ", so look back 2 for the ",", if it is there move back 2
                    if(oslex.odata[0].pbOut[oslex.odata[0].cb-2] == ',') oslex.odata[0].cb -= 2;

                    // but, if there is nothing in the list, we allowed for the ", we have to back up 1
                    else oslex.odata[0].cb--;

                    // put the bracket in. The status code string leads with a '," so we don't have to add that
                    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';
                    curState = FILEclosedir;
                }
            }
            break;

        case FILEclosedir:
            // close never cause a new error.
            if(DDIRINFO::fsclosedir() != FR_WAITING_FOR_THREAD)
            {
                fileTask.ClearUsage(FILETask::DIRECTORY);
                curState = JSONWait0;
            }
            break;

        case JSONWait0:

            // status
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // put out the status code, retState == FR_OK, which is status 0
            utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            // put out the wait
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;

            // we are done
            retState    = Idle;
            curState    = Idle;
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;

    }

    // see if we have the file
    if(fr == FR_WAITING_FOR_THREAD)
    {
        return(WaitingForResources);
    }
    else if(fr  != FR_OK)
    {
        retState = MkStateAnError(FILECSTATE | fr); 
        if(curState != JSONWait0) curState = FILEclosedir;
    }
  
    return(curState);
}

/************************************************************************/
/*************************** Enter the bootloader ***********************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{  
    "device":[  
    {  
        "command":"enterBootloader"
    }
    ]
}
#endif

STATE FnEnterBootloader(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    defTask.QueTask(DEFTask::SoftReset, 1000);
 
    // wait ?
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, sizeof(szWait)-1); 
    oslex.odata[0].cb += sizeof(szWait)-1;
    itoa(defTask.GetWaitTime(), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

    return(Idle);
}


/************************************************************************/
/*************************** Reset Instruments **************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{  
    "device":[  
    {  
        "command":"resetInstruments"
    }
    ]
}
#endif

STATE FnResetInstruments(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    defTask.QueTask(DEFTask::ResetInstruments, 500);
 
    // wait ?
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait, sizeof(szWait)-1); 
    oslex.odata[0].cb += sizeof(szWait)-1;
    itoa(defTask.GetWaitTime(), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '}';

    return(Idle);
}

/************************************************************************/
/***************************** storageGetLocation ***********************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
   "device":[
      {
         "command":"storageGetLocations"
      }
   ]
}
#endif
STATE FnStorageGetLocations(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStorageLocations, sizeof(szStorageLocations)-1); 
    oslex.odata[0].cb += sizeof(szStorageLocations)-1;

    if(fSd0)
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSD0, sizeof(szSD0)-1); 
        oslex.odata[0].cb += sizeof(szSD0)-1;

        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
    }
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;      

    return(Idle);
}

/********************** Loop Statistics *********************************/
/************** Times are reported in nano-sec **************************/
/*************** average loop time under 1ms ****************************/
/*************** max 0 - 20ms is an okay loop time **********************/
/*************** max 20 - 40ms is a yellow flag loop time ***************/
/*************** max 40 - 60ms is a red flag loop time ******************/
/*************** max 60 - 64+ ms, we are dead in the water **************/
/************************************************************************/
#if COMMENT
{  
    "device":[  
    {  
        "command":"loopStatistics"
    }
    ]
}

{  
    "test":[
    {
        "command":"fat32"
    }
    ]
}

#endif

STATE FnLoopStats(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{   
    uint32_t i;

   // status
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    for(i=0; i<LOOPSTATS::STEND; i++)
    {
        int64_t min, ave, max, total;
        bool fCnt = loopStats.GetStats((LOOPSTATS::STATID) i, min, ave, max, total);


        // put the initialization time right before the flash init time
        // keep the initialization times together.
        if(i == LOOPSTATS::INITFLSHVOL)
        {
            // Initialization times
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szInitTime, sizeof(szInitTime)-1); 
            oslex.odata[0].cb += sizeof(szInitTime)-1;

            illtoa(tMinInit*10, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

            illtoa(tAveInit*10, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

            illtoa(tMaxInit*10, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';
        }

        if(min != 0 || ave != 0 || max != 0)
        {
            uint32_t cbsz = strlen(LOOPSTATS::agszStats[i]);
            uint32_t mult = fCnt ? 1 : 10;

            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';

            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], LOOPSTATS::agszStats[i], cbsz); 
            oslex.odata[0].cb += cbsz;

            if(fCnt)    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szMinAveMaxCNT, sizeof(szMinAveMaxCNT)); 
            else        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szMinAveMaxNS, sizeof(szMinAveMaxNS)); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            
            illtoa(min*mult, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

            illtoa(ave*mult, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

            illtoa(max*mult, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
            oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

            if(fCnt)
            {
                oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';
                illtoa(total, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
                oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
            }

            oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';
        }
    }

    // how long the last JSON command was
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szLastJSON, sizeof(szLastJSON)-1); 
    oslex.odata[0].cb += sizeof(szLastJSON)-1;
    utoa(oslex.tLastCmd*10, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
 
    // Stack Low water mark
    pStackLowWater = FindLowWaterMark(pBottomOfStack, pStackLowWater);

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStack, sizeof(szStack)-1); 
    oslex.odata[0].cb += sizeof(szStack)-1;

    itoa((pTopOfStack - pStackLowWater), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    itoa((pStackLowWater - pBottomOfStack), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    itoa((pTopOfStack - pBottomOfStack), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // Thread Stack Low Water Mark
    pThreadStackLowWater = FindLowWaterMark(pBottomOfThreadStack, pThreadStackLowWater);

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szThreadStack, sizeof(szThreadStack)-1); 
    oslex.odata[0].cb += sizeof(szThreadStack)-1;

    itoa((pTopOfThreadStack - pThreadStackLowWater), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    itoa((pThreadStackLowWater - pBottomOfThreadStack), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

    itoa((pTopOfThreadStack - pBottomOfThreadStack), (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    oslex.odata[0].pbOut[oslex.odata[0].cb++] = ']';

    // wait 0
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(Idle);
}

/************************************************************************/
/********************** DC Channel Commands *****************************/
/************************************************************************/
#if COMMENT
{
	"dc": {
		"1": [{
				"command": "setVoltage",
				"voltage": 3300
			},
			{
				"command": "getVoltage"
			}
		],
		"2": {
			"command": "setVoltage",
			"voltage": -3300
		}
	}
}

{
    "dc":{
        "1":[
        {
            "command":"setVoltage",
            "voltage":2500
        }
        ]
    }
}

{
    "dc":{
        "1":[
        {
            "command":"getVoltage"
        }
        ],
        "2":[
        {
            "command":"getVoltage"
        }
        ]
    }
}

{
	"dc": {
		"1": {
			"command": "getVoltage"
		},
		"2": {
			"command": "getVoltage"
		}

	}
}

{
    "dc":{
        "2":[
        {
            "command":"setVoltage",
            "voltage":0
        }
        ]
    }
}

{
    "dc":{
        "2":
        {
            "command":"getVoltage"
        }
        
    }
}

{
   "device":[
    {
       "command":"calibrationStart"
    }
   ]
}

#endif

STATE FnDCGetVoltage(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    STATE retState;
    int32_t uVolts;

    if(queuedParams.iChannel == 1) retState = FBAWGorDCuV(CH_DC1_FB, &uVolts);
    else if(queuedParams.iChannel == 2) retState = FBAWGorDCuV(CH_DC2_FB, &uVolts);
    else return(InvalidChannel);

    if(retState == Idle)
    {

        // status
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
        oslex.odata[0].cb += sizeof(szStatusCode)-1;
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = ',';

        // voltage
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szVoltage, sizeof(szVoltage)-1); 
        oslex.odata[0].cb += sizeof(szVoltage)-1;
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '\"';
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = ':';       
        itoa((uVolts + 500)/1000, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // wait 0
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;

    }
    else if(IsStateAnError(retState))
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
        oslex.odata[0].cb += sizeof(szStatusCode)-1;
        utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
    }

    return(retState);
}

STATE FnDCGetCurrentState(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    STATE retState;
    int32_t uVolts;

    if(queuedParams.iChannel == 1) retState = FBAWGorDCuV(CH_DC1_FB, &uVolts);
    else if(queuedParams.iChannel == 2) retState = FBAWGorDCuV(CH_DC2_FB, &uVolts);
    else return(InvalidChannel);

    if(retState == Idle)
    {

        // status
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
        oslex.odata[0].cb += sizeof(szStatusCode)-1;
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

        // DC is always in a running state
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szState, sizeof(szState)-1); 
        oslex.odata[0].cb += sizeof(szState)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[Running]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // voltage
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szVoltage, sizeof(szVoltage)-1); 
        oslex.odata[0].cb += sizeof(szVoltage)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        itoa((uVolts + 500)/1000, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // wait 0
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
    }
    else if(IsStateAnError(retState))
    {
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
        oslex.odata[0].cb += sizeof(szStatusCode)-1;
        utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
    }

    return(retState);
}

STATE FnDCSetVoltage(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    // initial string.
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // check to see if we got all of the parameters we need
    if((QPINT0 & queuedParams.usage) != QPINT0)
    {
        utoa(NoDataAvailable, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
    }

    // see if we are in range
    else if(queuedParams.rgi32[0] < -4000 || 4000 < queuedParams.rgi32[0]) 
    {
        utoa(ValueOutOfRange, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
    }
    else
    {
        // change the DC out
        DCSetvoltage(queuedParams.iChannel, queuedParams.rgi32[0]);

        // write out the status and wait
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait500, sizeof(szWait500)-1); 
        oslex.odata[0].cb += sizeof(szWait500)-1;
    }

    return(Idle);
}

/************************************************************************/
/********************** AWG SetRegularWaveform  *************************/
/************************************************************************/
#if COMMENT
{
   "awg":{
      "1":[
         {
            "command":"setRegularWaveform",
            "signalType":"sine",
            "signalFreq":9000000,
            "vpp":3000,
            "vOffset":0 
         }
      ]
   }
}

{
   "awg":{
      "1":[
         {
            "command":"setRegularWaveform",
            "signalType":"triangle",
            "signalFreq":1000,
            "vpp":3000,
            "vOffset":300 
         }
      ]
   }
}

#endif
STATE FnAWGSetRegularWaveform(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    static STATE    curState = Idle;
    WAVEFORM        waveform = waveNone;
    int32_t         mVMax = 0;

    switch(curState)
    {
        case Idle:

            // initial string.
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
            oslex.odata[0].cb += sizeof(szStatusCode)-1;

            // the waveform
            switch(queuedParams.function)
            {
                case 0x00000001:
                    waveform = waveSine;
                    break;

                case 0x00000002:
                    waveform = waveSquare;
                    break;

                case 0x00000004:
                    waveform = waveTriangle;
                    break;

                case 0x00000008:
                    waveform = waveDC;
                    break;

                case 0x00000010:
                    waveform = waveSawtooth;
                    break;

                default:
                    break;
            }

            // see how high we are going
            mVMax = queuedParams.rgi32[2];                   // offset
            if(mVMax < 0) mVMax *= -1;                      // make positive for abs value calculation
            mVMax += (queuedParams.rgi32[1] + 1) / 2;        // add half P2P

            if( (waveform == waveNone)                                                                              ||
                ((queuedParams.usage & (QPINT1 | QPINT2)) != (QPINT1 | QPINT2))                                     ||
                ((queuedParams.rgi32[0] < 0) || ((AWGMAXFREQ * 1000) < queuedParams.rgi32[0]))                      ||
                (waveform != waveDC && (((queuedParams.usage & QPINT0) != QPINT0) || queuedParams.rgi32[0] == 0))   ||
                (waveform == waveDC && (queuedParams.rgi32[0] != 0))                                                ||
                (AWGMAXP2P < queuedParams.rgi32[1])                                                                 ||
                (AWGMAXP2P < mVMax)                                                                                 )
            {
                curState = InvalidParameter;
            }
            else 
            {
                queuedParams.rgi32[3] = (int32_t) waveform;       // use this static to hold the waveform
                curState = AWGSetWaveform;
            }
            break;

        case AWGSetWaveform:
            {
                STATE retState = AWGSetRegularWaveform(paramAWG, (WAVEFORM) queuedParams.rgi32[3] , (uint32_t) queuedParams.rgi32[0], queuedParams.rgi32[1], queuedParams.rgi32[2]);              
                if(retState == Idle || IsStateAnError(retState)) curState = retState;                
            }
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;

    }

    // we are done
    if(curState == Idle)
    {

        // write out the status
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
      
        // signal type
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szSignalType, sizeof(szSignalType)-1); 
        oslex.odata[0].cb += sizeof(szSignalType)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
        strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgAWGSignals[paramAWG.waveform]);
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // actualSignalFreq
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualSignalFreq, sizeof(szActualSignalFreq)-1); 
        oslex.odata[0].cb += sizeof(szActualSignalFreq)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        utoa(paramAWG.actualmHz, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // actualVpp
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualVpp, sizeof(szActualVpp)-1); 
        oslex.odata[0].cb += sizeof(szActualVpp)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        itoa(queuedParams.rgi32[1], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
 
        // actualVOffset
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
        oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualVOffset, sizeof(szActualVOffset)-1); 
        oslex.odata[0].cb += sizeof(szActualVOffset)-1;
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
        oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
        itoa(queuedParams.rgi32[2], (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        // if wavegen is running, we need to wait for the change
        if(paramIAWG.state == Running)
        {
            AWGRun(paramAWG);

            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait500, sizeof(szWait500)-1); 
            oslex.odata[0].cb += sizeof(szWait500)-1;       
        }
        else
        {
            memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
            oslex.odata[0].cb += sizeof(szWait0)-1;       
        }
    }

    // got an error
    else if(IsStateAnError(curState))
    {
        utoa(curState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;
        curState = Idle;
    }

    return(curState);
}

/************************************************************************/
/********************************* AWG Run  *****************************/
/************************************************************************/
#if COMMENT
{
   "awg":{
      "1":[
         {
            "command":"run"
         }
      ]
   }
}

{
   "awg":{
      "1":
         {
            "command":"run"
         }      
   }
}

{
   "awg":{
      "1":[
         {
            "command":"stop"
         }
      ]
   }
}


{
   "awg":{
      "1":[
         {
            "command":"setRegularWaveform",
            "signalType":"sawtooth",
            "signalFreq":100000,
            "vpp":3000,
            "vOffset":0 
         }
      ]
   }
}

{
   "awg":{
      "1":[
         {
            "command":"setRegularWaveform",
            "signalType":"sine",
            "signalFreq":100000,
            "vpp":3000,
            "vOffset":0 
         },
         {
            "command":"run"
         }
      ]
   }
}

{
   "device":[
    {
       "command":"calibrationStart"
    }
   ]
}

{  
    "device":[  
    {  
        "command":"loopStatistics"
    }
    ]
}



#endif
STATE FnAWGetCurrentState(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;
    oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

    // if wavegen is running, we need to wait for the change
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szState, sizeof(szState)-1); 
    oslex.odata[0].cb += sizeof(szState)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgszStates[paramIAWG.state]);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // signal type
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWaveType, sizeof(szWaveType)-1); 
    oslex.odata[0].cb += sizeof(szWaveType)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQCQ, sizeof(szJSOMemberSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQCQ)-1;
    strcpy((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], rgAWGSignals[paramIAWG.waveform]);
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // actualSignalFreq
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepQCQ, sizeof(szJSOValueSepQCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepQCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualSignalFreq, sizeof(szActualSignalFreq)-1); 
    oslex.odata[0].cb += sizeof(szActualSignalFreq)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    utoa(paramIAWG.actualmHz, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    // actualVpp
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualVpp, sizeof(szActualVpp)-1); 
    oslex.odata[0].cb += sizeof(szActualVpp)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    itoa(paramIAWG.actualVpp, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
 
    // actualVOffset
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOValueSepCQ, sizeof(szJSOValueSepCQ)-1); 
    oslex.odata[0].cb += sizeof(szJSOValueSepCQ)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szActualVOffset, sizeof(szActualVOffset)-1); 
    oslex.odata[0].cb += sizeof(szActualVOffset)-1;
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szJSOMemberSepQC, sizeof(szJSOMemberSepQC)-1); 
    oslex.odata[0].cb += sizeof(szJSOMemberSepQC)-1;
    itoa(paramIAWG.actualVOffset, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
    oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;       

    return(Idle);
}

STATE FnAWGRun(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{

    ASSERT(paramAWG.state == Idle);

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    if(paramAWG.waveform == waveNone)
    {
        // write out the status
        utoa(AWGWaveformNotSet, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
        oslex.odata[0].cb += sizeof(szWait0)-1;       
   }
    else
    {
        // write out the status
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';

        // BUG BUG: must fix this for multiple calls
        AWGRun(paramAWG);

        memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait500, sizeof(szWait500)-1); 
        oslex.odata[0].cb += sizeof(szWait500)-1;       
    }

    return(Idle);
}

/************************************************************************/
/********************************* AWG Stop *****************************/
/************************************************************************/
#if COMMENT
{
   "awg":{
      "1":[
         {
            "command":"stop"
         }
      ]
   }
}
#endif
STATE FnAWGStop(JSON::JSONTOKEN token, char const * szInput, int32_t cbInput, const JSONENTRY& jEntry)
{
    STATE retState = AWGStop();

    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szStatusCode, sizeof(szStatusCode)-1); 
    oslex.odata[0].cb += sizeof(szStatusCode)-1;

    // done
    if(retState == Idle)
    {
        // write out good status
        oslex.odata[0].pbOut[oslex.odata[0].cb++] = '0';
    }

    // got an error
    else if(IsStateAnError(retState))
    {
        // put out the error status
        utoa(retState, (char *) &oslex.odata[0].pbOut[oslex.odata[0].cb], 10); 
        oslex.odata[0].cb += strlen((char *) &oslex.odata[0].pbOut[oslex.odata[0].cb]);
        retState = Idle;
    }

    // something else, still working
    else 
    {
        return(retState);
    }

    // put out the wait
    memcpy(&oslex.odata[0].pbOut[oslex.odata[0].cb], szWait0, sizeof(szWait0)-1); 
    oslex.odata[0].cb += sizeof(szWait0)-1;

    return(retState);
}
/************************************************************************/
/********************** JSON PARSING TABLES *****************************/
/************************************************************************/

// channel selection [1,2,3,4,5,6,7,8]
const JSONENTRY rgESetCh[] =    {   
    {JSON::tokNumber,       szOne,      FnFunc8,    JListPop},
    {JSON::tokNumber,       szTwo,      FnFunc9,    JListPop},
    {JSON::tokNumber,       szThree,    FnFunc10,   JListPop},
    {JSON::tokNumber,       szFour,     FnFunc11,   JListPop},
    {JSON::tokNumber,       szFive,     FnFunc12,   JListPop},
    {JSON::tokNumber,       szSix,      FnFunc13,   JListPop},
    {JSON::tokNumber,       szSeven,    FnFunc14,   JListPop},
    {JSON::tokNumber,       szEight,    FnFunc15,   JListPop},
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokValueSep,     NULL,       NULL,       JListPush},
    {JSON::tokArray,        NULL,       NULL,       JListPush},
    {JSON::tokEndArray,     NULL,       NULL,       JListPop}
};
const JSONLIST JLSetCh = {sizeof(rgESetCh) / sizeof(JSONENTRY), rgESetCh};

/************************************************************************/
/************************** Update Commands *****************************/
/************************************************************************/
// Test Fat32; Level 5
const JSONENTRY rgEUpdWFFW[] =    {   
    {JSON::tokEndObject,    NULL,       FnUpdWFFW,      JListPop3}
};
const JSONLIST jLUpdWFFW = {sizeof(rgEUpdWFFW) / sizeof(JSONENTRY), rgEUpdWFFW};

// Test Fat32; Level 5
const JSONENTRY rgEUpdWFCert[] =    {   
    {JSON::tokEndObject,    NULL,       FnUpdWFCert,    JListPop3}
};
const JSONLIST jLUpdWFCert = {sizeof(rgEUpdWFCert) / sizeof(JSONENTRY), rgEUpdWFCert};

// Test; Level 4
const JSONENTRY rgEUpdateCmd[] = {   
    {JSON::tokNameSep,      NULL,           FnEchoToken,    JListNone},
    {JSON::tokStringValue,  szUpdWFFW,      FnEchoToken,    jLUpdWFFW},
    {JSON::tokStringValue,  szUpdWFCert,    FnEchoToken,    jLUpdWFCert}
};
const JSONLIST jLUpdateCmd = {sizeof(rgEUpdateCmd) / sizeof(JSONENTRY), rgEUpdateCmd};

// Test; Level 2,3
const JSONENTRY rgEUpdate[] = {   
    {JSON::tokNameSep,      NULL,       FnEchoToken,    JListNone},
    {JSON::tokArray,        NULL,       FnEchoToken,    JListPush},
    {JSON::tokObject,       NULL,       FnEchoToken,    JListNone},
    {JSON::tokMemberName,   szCommand,  FnEchoToken,    jLUpdateCmd},
    {JSON::tokValueSep,     NULL,       FnEchoToken,    JListPush},
    {JSON::tokEndArray,     NULL,       FnEchoToken,    JListPop}
};
const JSONLIST jLUpdate = {sizeof(rgEUpdate) / sizeof(JSONENTRY), rgEUpdate};

/************************************************************************/
/************************ Test Commands *********************************/
/*************************** Level 5 ************************************/
/************************************************************************/
// Test FIFO245 Test; Level 5
const JSONENTRY rgEFIFO245[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestFIFO245,     JListPop3}
};
const JSONLIST jLFIFO245 = {sizeof(rgEFIFO245) / sizeof(JSONENTRY), rgEFIFO245};

// Test FIFO245End Test; Level 5
const JSONENTRY rgEFIFO245End[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestFIFO245End,  JListPop3}
};
const JSONLIST jLFIFO245End = {sizeof(rgEFIFO245End) / sizeof(JSONENTRY), rgEFIFO245End};

// Test DDR Test; Level 5
const JSONENTRY rgEDDR[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestDDR,     JListPop3}
};
const JSONLIST jLDDR = {sizeof(rgEDDR) / sizeof(JSONENTRY), rgEDDR};

// Test Encoder Test; Level 5
const JSONENTRY rgEEncoder[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestEncoder,     JListPop3}
};
const JSONLIST jLEncoder = {sizeof(rgEEncoder) / sizeof(JSONENTRY), rgEEncoder};

// Test EncoderEnd Test; Level 5
const JSONENTRY rgEEncoderEnd[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestEncoderEnd,  JListPop3}
};
const JSONLIST jLEncoderEnd = {sizeof(rgEEncoderEnd) / sizeof(JSONENTRY), rgEEncoderEnd};

// Test GIPO Test; Level 5
const JSONENTRY rgEGpio[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestGPIO,     JListPop3}
};
const JSONLIST jLGpio = {sizeof(rgEGpio) / sizeof(JSONENTRY), rgEGpio};

// Test GIPOEnd Test; Level 5
const JSONENTRY rgEGpioEnd[] =    {   
    {JSON::tokEndObject,    NULL,       FnTestGPIOEnd,  JListPop3}
};
const JSONLIST jLGpioEnd = {sizeof(rgEGpioEnd) / sizeof(JSONENTRY), rgEGpioEnd};

// Test CalPSK; Level 5
const JSONENTRY rgECalPSK[] =    {   
    {JSON::tokEndObject,    NULL,       FnCalPSK,       JListPop3}
};
const JSONLIST jLCalPSK = {sizeof(rgECalPSK) / sizeof(JSONENTRY), rgECalPSK};

// Test Fat32; Level 5
const JSONENTRY rgEFat32[] =    {   
    {JSON::tokEndObject,    NULL,       FnFat32,       JListPop3}
};
const JSONLIST jLFat32 = {sizeof(rgEFat32) / sizeof(JSONENTRY), rgEFat32};

// Manufacturing Test; Level 5
const JSONENTRY rgEManufacturing[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,                   JListNone},
    {JSON::tokMemberName,   szTestNbr,  NULL,                   jLInt0},
    {JSON::tokEndObject,    NULL,       FnManufacturningTest,   JListPop3}
};
const JSONLIST jLEManufacturing = {sizeof(rgEManufacturing) / sizeof(JSONENTRY), rgEManufacturing};

// Test; Level 4
const JSONENTRY rgETestCmd[] = {   
    {JSON::tokNameSep,      NULL,               FnEchoToken,                JListNone},
    {JSON::tokStringValue,  szCalPSK,           FnEchoToken,                jLCalPSK},
    {JSON::tokStringValue,  szFat32,            FnEchoToken,                jLFat32},
    {JSON::tokStringValue,  szTestRun,          FnClearQueuedValuesAndEcho, jLEManufacturing},
    {JSON::tokStringValue,  szTestGPIO,         FnEchoToken,                jLGpio},
    {JSON::tokStringValue,  szTestGPIOEnd,      FnEchoToken,                jLGpioEnd},
    {JSON::tokStringValue,  szTestEncoder,      FnEchoToken,                jLEncoder},
    {JSON::tokStringValue,  szTestEncoderEnd,   FnEchoToken,                jLEncoderEnd},
    {JSON::tokStringValue,  szTestDDR,          FnEchoToken,                jLDDR},
    {JSON::tokStringValue,  szTestFIFO245,      FnEchoToken,                jLFIFO245},
    {JSON::tokStringValue,  szTestFIFO245End,   FnEchoToken,                jLFIFO245End}
};
const JSONLIST jLTestCmd = {sizeof(rgETestCmd) / sizeof(JSONENTRY), rgETestCmd};

// Test; Level 2,3
const JSONENTRY rgETest[] = {   
    {JSON::tokNameSep,      NULL,       FnEchoToken,    JListNone},
    {JSON::tokArray,        NULL,       FnEchoToken,    JListPush},
    {JSON::tokObject,       NULL,       FnEchoToken,    JListNone},
    {JSON::tokMemberName,   szCommand,  FnEchoToken,    jLTestCmd},
    {JSON::tokValueSep,     NULL,       FnEchoToken,    JListPush},
    {JSON::tokEndArray,     NULL,       FnEchoToken,    JListPop}
};
const JSONLIST jLTest = {sizeof(rgETest) / sizeof(JSONENTRY), rgETest};

/************************************************************************/
/*************************** FILE Commands ******************************/
/************************************************************************/
/************************************************************************/
#if COMMENT
{
    "file": [{
        "command":"read",
        "type":"flash",
        "path":"hardprofiledata.json",
        "filePosition":0,
        "requestedLength":-1
    }]
}
#endif

// File Type; Level 6
const JSONENTRY rgEType[] =    {   
    {JSON::tokNameSep,      NULL,               NULL,           JListNone},
    {JSON::tokStringValue,  szFlash,            FnFunc0,        JListPop},
    {JSON::tokStringValue,  szSD0,              FnFunc1,        JListPop},
};
const JSONLIST jLType = {sizeof(rgEType) / sizeof(JSONENTRY), rgEType};

// File GetFileSize; Level 5
const JSONENTRY rgEFileGetSize[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szType,             NULL,           jLType},
    {JSON::tokMemberName,   szPath,             NULL,           jLPTR0},
    {JSON::tokEndObject,    NULL,               FnFileGetSize,  JListPop2}
};
const JSONLIST jLFileGetSize = {sizeof(rgEFileGetSize) / sizeof(JSONENTRY), rgEFileGetSize};

// File GetFileSize; Level 5
const JSONENTRY rgEFileGetFreeSpace[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,               JListNone},
    {JSON::tokMemberName,   szType,             NULL,               jLType},
    {JSON::tokMemberName,   szPath,             NULL,               jLPTR0},
    {JSON::tokEndObject,    NULL,               FnFileGetFreeSpace, JListPop2}
};
const JSONLIST jLFileGetFreeSpace = {sizeof(rgEFileGetFreeSpace) / sizeof(JSONENTRY), rgEFileGetFreeSpace};

// File Delete; Level 5
const JSONENTRY rgEFileDelete[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szType,             NULL,           jLType},
    {JSON::tokMemberName,   szPath,             NULL,           jLPTR0},
    {JSON::tokEndObject,    NULL,               FnFileDelete,   JListPop2}
};
const JSONLIST jLFileDelete = {sizeof(rgEFileDelete) / sizeof(JSONENTRY), rgEFileDelete};

// File Listdir; Level 5
const JSONENTRY rgEFileListdir[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szType,             NULL,           jLType},
    {JSON::tokMemberName,   szPath,             NULL,           jLPTR0},
    {JSON::tokEndObject,    NULL,               FnFileListdir,  JListPop2}
};
const JSONLIST jLFileListdir = {sizeof(rgEFileListdir) / sizeof(JSONENTRY), rgEFileListdir};

// File MkFS; Level 5
const JSONENTRY rgEFileMkFS[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szType,             NULL,           jLType},
    {JSON::tokEndObject,    NULL,               FnFileMkFS,     JListPop2}
};
const JSONLIST jLFileMkFS = {sizeof(rgEFileMkFS) / sizeof(JSONENTRY), rgEFileMkFS};

// File getCurrentState; Level 5
const JSONENTRY rgEFileGetCurrentState[] =    {   
    {JSON::tokEndObject,    NULL,               FnFileGetCurrentState,  JListPop2}
};
const JSONLIST jLFileGetCurrentState = {sizeof(rgEFileGetCurrentState) / sizeof(JSONENTRY), rgEFileGetCurrentState};

// File Write; Level 5
const JSONENTRY rgEFileWrite[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szType,             NULL,           jLType},
    {JSON::tokMemberName,   szPath,             NULL,           jLPTR0},
    {JSON::tokMemberName,   szFilePosition,     NULL,           jLUInt0},
    {JSON::tokMemberName,   szBinaryOffset,     NULL,           jLUInt1},
    {JSON::tokMemberName,   szBinaryLength,     NULL,           jLUInt2},
    {JSON::tokEndObject,    NULL,               FnFileWrite,    JListPop2}
};
const JSONLIST jLFileWrite = {sizeof(rgEFileWrite) / sizeof(JSONENTRY), rgEFileWrite};

// File Read; Level 5
const JSONENTRY rgEFileRead[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szType,             NULL,           jLType},
    {JSON::tokMemberName,   szPath,             NULL,           jLPTR0},
    {JSON::tokMemberName,   szFilePosition,     NULL,           jLUInt0},
    {JSON::tokMemberName,   szRequestedLength,  NULL,           jLInt1},
    {JSON::tokEndObject,    NULL,               FnFileRead,     JListPop2}
};
const JSONLIST jLFileRead = {sizeof(rgEFileRead) / sizeof(JSONENTRY), rgEFileRead};

// File Command, Level 3,4
const JSONENTRY rgEFileEntry[] = {
    {JSON::tokNameSep,      NULL,               FnEchoToken,                JListNone},
    {JSON::tokArray,        NULL,               FnEchoToken,                JListPush},
    {JSON::tokObject,       NULL,               FnClearQueuedValuesAndEcho, JListNone},
    {JSON::tokMemberName,   szCommand,          FnEchoToken,                JListNone},
    {JSON::tokStringValue,  szRead,             FnEchoToken,                jLFileRead},
    {JSON::tokStringValue,  szWrite,            FnEchoToken,                jLFileWrite},
    {JSON::tokStringValue,  szDelete,           FnEchoToken,                jLFileDelete},
    {JSON::tokStringValue,  szGetFileSize,      FnEchoToken,                jLFileGetSize},
    {JSON::tokStringValue,  szGetFreeSpace,     FnEchoToken,                jLFileGetFreeSpace},
    {JSON::tokStringValue,  szListdir,          FnEchoToken,                jLFileListdir},
    {JSON::tokStringValue,  szMkFS,             FnEchoToken,                jLFileMkFS},
    {JSON::tokStringValue,  szGetCurrentState,  FnEchoToken,                jLFileGetCurrentState},
    {JSON::tokValueSep,     NULL,               FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,               FnEchoToken,                JListPop}
};
const JSONLIST jLFile = {sizeof(rgEFileEntry) / sizeof(JSONENTRY), rgEFileEntry};


/************************************************************************/
/**************************** AWG Commands  *****************************/
/************************************************************************/
#if COMMENT
{
   "device":[
    {
       "command":"calibrationStart",
       "awg": 1
    }
   ]
}


{
   "awg":{
      "1":[
         {
            "command":"setRegularWaveform",
            "signalType":"sine",
            "signalFreq":1000000,
            "vpp":3000,
            "vOffset":0 
         }
      ]
   }
}

#endif

// AWG Stop; Level 5
const JSONENTRY rgEAWGetCurrentState[] =    {   
    {JSON::tokEndObject,    NULL,           FnAWGetCurrentState,    JListPop2}
};
const JSONLIST jLAWGetCurrentState = {sizeof(rgEAWGetCurrentState) / sizeof(JSONENTRY), rgEAWGetCurrentState};

// AWG Stop; Level 5
const JSONENTRY rgEAWGStop[] =    {   
    {JSON::tokEndObject,    NULL,           FnAWGStop,    JListPop2}
};
const JSONLIST jLAWGStop = {sizeof(rgEAWGStop) / sizeof(JSONENTRY), rgEAWGStop};

// AWG Run; Level 5
const JSONENTRY rgEAWGRun[] =    {   
    {JSON::tokEndObject,    NULL,           FnAWGRun,    JListPop2}
};
const JSONLIST jLAWGRun = {sizeof(rgEAWGRun) / sizeof(JSONENTRY), rgEAWGRun};

// AWG Signal Type; Level 6
const JSONENTRY rgEAWGSignalType[] =    {   
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokStringValue,  szSine,     FnFunc0,    JListPop},
    {JSON::tokStringValue,  szSquare,   FnFunc1,    JListPop},
    {JSON::tokStringValue,  szTriangle, FnFunc2,    JListPop},
    {JSON::tokStringValue,  szDC,       FnFunc3,    JListPop},
    {JSON::tokStringValue,  szSawtooth, FnFunc4,    JListPop}
};
const JSONLIST jLAWGSignalType = {sizeof(rgEAWGSignalType) / sizeof(JSONENTRY), rgEAWGSignalType};

// AWG SetRegularWaveform; Level 5
const JSONENTRY rgEAWGSetRegularWaveform[] =    {   
    {JSON::tokValueSep,     NULL,           NULL,                       JListNone},
    {JSON::tokMemberName,   szSignalType,   NULL,                       jLAWGSignalType},
    {JSON::tokMemberName,   szSignalFreq,   NULL,                       jLInt0},
    {JSON::tokMemberName,   szVpp,          NULL,                       jLInt1},
    {JSON::tokMemberName,   szVOffset,      NULL,                       jLInt2},
    {JSON::tokEndObject,    NULL,           FnAWGSetRegularWaveform,    JListPop2}
};
const JSONLIST jLAWGSetRegularWaveform = {sizeof(rgEAWGSetRegularWaveform) / sizeof(JSONENTRY), rgEAWGSetRegularWaveform};

// Select a AWG command; level 4
const JSONENTRY rgEAWGCommand[] = {
    {JSON::tokNameSep,      NULL,                   FnEchoToken,    JListNone},
    {JSON::tokStringValue,  szSetRegularWaveform,   FnEchoToken,    jLAWGSetRegularWaveform},
    {JSON::tokStringValue,  szGetCurrentState,      FnEchoToken,    jLAWGetCurrentState},
    {JSON::tokStringValue,  szRun,                  FnEchoToken,    jLAWGRun},
    {JSON::tokStringValue,  szStop,                 FnEchoToken,    jLAWGStop}
};
const JSONLIST jLAWGCommand = {sizeof(rgEAWGCommand) / sizeof(JSONENTRY), rgEAWGCommand};

// AWG channels; Level 2,3
const JSONENTRY rgEAWGChannel[] = {   
    {JSON::tokMemberName,   szOne,      FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szCommand,  FnEchoToken,                jLAWGCommand},
    {JSON::tokNameSep,      NULL,       FnEchoToken,                JListNone},
    {JSON::tokValueSep,     NULL,       FnEchoToken,                JListNone},
    {JSON::tokObject,       NULL,       FnClearQueuedValuesAndEcho, JListNone},
    {JSON::tokEndObject,    NULL,       FnEchoToken,                JListPop},
    {JSON::tokArray,        NULL,       FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,       FnEchoToken,                JListPop}
};
const JSONLIST jLAWGChannel = {sizeof(rgEAWGChannel) / sizeof(JSONENTRY), rgEAWGChannel};


/************************************************************************/
/************************** DC Commands *********************************/
/************************************************************************/

// DC GetCurrentState; Level 5
const JSONENTRY rgEDCGetCurrentState[] =    {   
    {JSON::tokEndObject,    NULL,   FnDCGetCurrentState,    JListPop2}
};
const JSONLIST jLDCGetCurrentState = {sizeof(rgEDCGetCurrentState) / sizeof(JSONENTRY), rgEDCGetCurrentState};

// DC GetVoltage; Level 5
const JSONENTRY rgEDCGetVoltage[] =    {   
    {JSON::tokEndObject,    NULL,   FnDCGetVoltage, JListPop2}
};
const JSONLIST jLDCGetVoltage = {sizeof(rgEDCGetVoltage) / sizeof(JSONENTRY), rgEDCGetVoltage};

// DC SetVoltage; Level 5
const JSONENTRY rgEDCSetVoltage[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,           JListNone},
    {JSON::tokMemberName,   szVoltage,  NULL,           jLInt0},
    {JSON::tokEndObject,    NULL,       FnDCSetVoltage, JListPop2}
};
const JSONLIST jLDCSetVoltage = {sizeof(rgEDCSetVoltage) / sizeof(JSONENTRY), rgEDCSetVoltage};

// Select a DC command; level 4
const JSONENTRY rgEDCCommand[] = {
    {JSON::tokNameSep,      NULL,               FnEchoToken,    JListNone},
    {JSON::tokStringValue,  szSetVoltage,       FnEchoToken,    jLDCSetVoltage},
    {JSON::tokStringValue,  szGetVoltage,       FnEchoToken,    jLDCGetVoltage},
    {JSON::tokStringValue,  szGetCurrentState,  FnEchoToken,    jLDCGetCurrentState}
};
const JSONLIST jLDCCommand = {sizeof(rgEDCCommand) / sizeof(JSONENTRY), rgEDCCommand};

// DC channels; Level 2,3
const JSONENTRY rgEDCChannel[] = {   
    {JSON::tokMemberName,   szOne,      FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szTwo,      FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szCommand,  FnEchoToken,                jLDCCommand},
    {JSON::tokNameSep,      NULL,       FnEchoToken,                JListNone},
    {JSON::tokValueSep,     NULL,       FnEchoToken,                JListNone},
    {JSON::tokObject,       NULL,       FnClearQueuedValuesAndEcho, JListNone},
    {JSON::tokEndObject,    NULL,       FnEchoToken,                JListPop},
    {JSON::tokArray,        NULL,       FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,       FnEchoToken,                JListPop}
};
const JSONLIST jLDCChannel = {sizeof(rgEDCChannel) / sizeof(JSONENTRY), rgEDCChannel};

/************************************************************************/
/************************** GPIO Commands *******************************/
/************************************************************************/
// GPIO SetParameters; Level 5
const JSONENTRY rgEGPIOWrite[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,           JListNone},
    {JSON::tokMemberName,   szValue,    NULL,           JListNone},
    {JSON::tokNameSep,      NULL,       NULL,           JListNone},
    {JSON::tokNumber,       szZero,     NULL,           JListNone},
    {JSON::tokNumber,       szOne,      FnFunc0,        JListNone},
    {JSON::tokEndObject,    NULL,       FnGPIOWrite,    JListPop1}
};
const JSONLIST jLGPIOWrite = {sizeof(rgEGPIOWrite) / sizeof(JSONENTRY), rgEGPIOWrite};

// GPIO SetParameters; Level 5
const JSONENTRY rgEGPIOSetParameters[] =    {   
    {JSON::tokValueSep,     NULL,           NULL,                   JListNone},
    {JSON::tokMemberName,   szDirection,    NULL,                   JListNone},
    {JSON::tokNameSep,      NULL,           NULL,                   JListNone},
    {JSON::tokStringValue,  szInputStr,     NULL,                   JListNone},
    {JSON::tokStringValue,  szOutputStr,    FnFunc0,                JListNone},
    {JSON::tokEndObject,    NULL,           FnGPIOSetParameters,    JListPop1}
};
const JSONLIST jLGPIOSetParameters = {sizeof(rgEGPIOSetParameters) / sizeof(JSONENTRY), rgEGPIOSetParameters};

// GPIO GetCurrentState; Level 5
const JSONENTRY rgEGPIOGetCurrentState[] =    {   
    {JSON::tokEndObject,    NULL,   FnGPIOGetCurrentState, JListPop1}
};
const JSONLIST jLGPIOGetCurrentState = {sizeof(rgEGPIOGetCurrentState) / sizeof(JSONENTRY), rgEGPIOGetCurrentState};

// GPIO Read; Level 5
const JSONENTRY rgEGPIORead[] =    {   
    {JSON::tokEndObject,    NULL,   FnGPIORead, JListPop1}
};
const JSONLIST jLGPIORead = {sizeof(rgEGPIORead) / sizeof(JSONENTRY), rgEGPIORead};

// GPIO channels; Level 2,3
const JSONENTRY rgEGPIOChannel[] = {   
    {JSON::tokMemberName,   szZero,             FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szOne,              FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szTwo,              FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szThree,            FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szFour,             FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szFive,             FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szSix,              FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szSeven,            FnChannelAndEcho,           JListNone}, 
    {JSON::tokMemberName,   szCommand,          FnEchoToken,                JListNone},
    {JSON::tokStringValue,  szGetCurrentState,  FnEchoToken,                jLGPIOGetCurrentState},
    {JSON::tokStringValue,  szRead,             FnEchoToken,                jLGPIORead},
    {JSON::tokStringValue,  szSetParameters,    FnEchoToken,                jLGPIOSetParameters},
    {JSON::tokStringValue,  szWrite,            FnEchoToken,                jLGPIOWrite},
    {JSON::tokNameSep,      NULL,               FnEchoToken,                JListNone},
    {JSON::tokValueSep,     NULL,               FnEchoToken,                JListNone},
    {JSON::tokObject,       NULL,               FnClearQueuedValuesAndEcho, JListNone},
    {JSON::tokEndObject,    NULL,               FnEchoToken,                JListPop},
    {JSON::tokArray,        NULL,               FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,               FnEchoToken,                JListPop}
};
const JSONLIST jLGPIOChannel = {sizeof(rgEGPIOChannel) / sizeof(JSONENTRY), rgEGPIOChannel};

/************************************************************************/
/************************ Log Commands **********************************/
/************************************************************************/
const JSONENTRY rgELogChannelEntry[] =    {   
    {JSON::tokNameSep,      NULL,               NULL,           JListNone},
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokObject,       NULL,               NULL,           JListNone},
    {JSON::tokMemberName,   szAverage,          NULL,           jLUInt7},
    {JSON::tokMemberName,   szStorageLocation,  NULL,           jLSkipValue},
    {JSON::tokMemberName,   szURI,              NULL,           jLSkipValue},
    {JSON::tokMemberName,   szService,          NULL,           jLSkipValue},
    {JSON::tokEndObject,    NULL,               FnLogChannel,   JListPop}
};
const JSONLIST jLLogChannelEntry = {sizeof(rgELogChannelEntry) / sizeof(JSONENTRY), rgELogChannelEntry};

const JSONENTRY rgELogChannels[] =    {   
    {JSON::tokNameSep,      NULL,               NULL,               JListNone},
    {JSON::tokObject,       NULL,               NULL,               JListNone},
    {JSON::tokMemberName,   NULL,               FnChannel,          jLLogChannelEntry},
    {JSON::tokNumber,       NULL,               FnLogValueChannel,  JListPop},
    {JSON::tokArray,        NULL,               NULL,               JListPush},
    {JSON::tokValueSep,     NULL,               NULL,               JListPush},
    {JSON::tokEndArray,     NULL,               NULL,               JListPop},
    {JSON::tokEndObject,    NULL,               NULL,               JListPop}
};
const JSONLIST jLLogChannels = {sizeof(rgELogChannels) / sizeof(JSONENTRY), rgELogChannels};

const JSONENTRY rgELogSetDefaultAdcFreq[] =    {   
    {JSON::tokMemberName,   szAdcSampleFreq,    NULL,                   jLUIntLL2},
    {JSON::tokValueSep,     NULL,               NULL,                   JListNone},
    {JSON::tokEndObject,    NULL,               FnLogSetDefaultAdcFreq, JListPop2}
};
const JSONLIST jLLogSetDefaultAdcFreq = {sizeof(rgELogSetDefaultAdcFreq) / sizeof(JSONENTRY), rgELogSetDefaultAdcFreq};

const JSONENTRY rgELogSetParameters[] =    {   
    {JSON::tokMemberName,   szMaxSampleCount,   NULL,           jLIntLL0},
    {JSON::tokMemberName,   szSampleFreq,       NULL,           jLUIntLL0},
    {JSON::tokMemberName,   szStartDelay,       NULL,           jLUIntLL1},
    {JSON::tokMemberName,   szAdcSampleFreq,    NULL,           jLUIntLL2},
    {JSON::tokMemberName,   szStorageLocation,  NULL,           jLPTR7},
    {JSON::tokMemberName,   szURI,              NULL,           jLPTR6},
    {JSON::tokMemberName,   szService,          NULL,           jLPTR5},
    {JSON::tokMemberName,   szKey,              NULL,           jLPTR4},
    {JSON::tokMemberName,   szChannels,         NULL,           jLLogChannels},
    {JSON::tokMemberName,   szLogOnBoot,        NULL,           JListNone},
    {JSON::tokNameSep,      NULL,               NULL,           JListNone},
    {JSON::tokTrue,         NULL,               FnFunc0,        JListNone},
    {JSON::tokFalse,        NULL,               NULL,           JListNone},
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokEndObject,    NULL,               FnLogSetParm,   JListPop2}
};
const JSONLIST jLLogSetParameters = {sizeof(rgELogSetParameters) / sizeof(JSONENTRY), rgELogSetParameters};

const JSONENTRY rgELogRun[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,       JListNone},
    {JSON::tokMemberName,   szDelete,   NULL,       JListNone},
    {JSON::tokNameSep,      NULL,       NULL,       JListNone},
    {JSON::tokTrue,         NULL,       FnFunc0,    JListNone},
    {JSON::tokFalse,        NULL,       NULL,       JListNone},
    {JSON::tokEndObject,    NULL,       FnLogRun,   JListPop2}
};
const JSONLIST jLLogRun = {sizeof(rgELogRun) / sizeof(JSONENTRY), rgELogRun};

const JSONENTRY rgELogStop[] =    {   
    {JSON::tokEndObject,    NULL,               FnLogStop,   JListPop2}
};
const JSONLIST jLLogStop = {sizeof(rgELogStop) / sizeof(JSONENTRY), rgELogStop};

const JSONENTRY rgELogGetCurrentState[] =    {   
    {JSON::tokEndObject,    NULL,               FnLogGetCurrentState,   JListPop2}
};
const JSONLIST jLLogGetCurrentState = {sizeof(rgELogGetCurrentState) / sizeof(JSONENTRY), rgELogGetCurrentState};

const JSONENTRY rgELogRead[] =    {   
    {JSON::tokMemberName,   szCount,            NULL,           jLIntLL0},
    {JSON::tokMemberName,   szStartIndex,       NULL,           jLUIntLL0},
    {JSON::tokMemberName,   szChannels,         NULL,           JLSetCh},
    {JSON::tokValueSep,     NULL,               NULL,           JListNone},
    {JSON::tokEndObject,    NULL,               FnLogRead,      JListPop2}
};
const JSONLIST jLLogRead = {sizeof(rgELogRead) / sizeof(JSONENTRY), rgELogRead};

const JSONENTRY rgEDAQ[] =    {   
    {JSON::tokNameSep,      NULL,                   FnEchoToken,                JListNone},
    {JSON::tokObject,       NULL,                   FnClearQueuedValuesAndEcho, JListNone},
    {JSON::tokMemberName,   szCommand,              FnEchoToken,                JListNone},
    {JSON::tokStringValue,  szSetParameters,        FnClearLogParmAndEcho,      jLLogSetParameters},  
    {JSON::tokStringValue,  szRun,                  FnEchoToken,                jLLogRun},  
    {JSON::tokStringValue,  szStop,                 FnEchoToken,                jLLogStop},  
    {JSON::tokStringValue,  szGetCurrentState,      FnEchoToken,                jLLogGetCurrentState},  
    {JSON::tokStringValue,  szRead,                 FnEchoToken,                jLLogRead},  
    {JSON::tokStringValue,  szSetDefaultAdcFreq,    FnEchoToken,                jLLogSetDefaultAdcFreq},  
    {JSON::tokArray,        NULL,                   FnEchoToken,                JListPush},
    {JSON::tokValueSep,     NULL,                   FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,                   FnEchoToken,                JListPop}
};
const JSONLIST jLDAQ = {sizeof(rgEDAQ) / sizeof(JSONENTRY), rgEDAQ};

const JSONENTRY rgELog[] =    {   
    {JSON::tokNameSep,      NULL,               FnEchoToken,                JListNone},
    {JSON::tokObject,       NULL,               FnEchoToken,                JListPush},
    {JSON::tokMemberName,   szDAQ,              FnEchoToken,                jLDAQ},
    {JSON::tokArray,        NULL,               FnEchoToken,                JListPush},
    {JSON::tokValueSep,     NULL,               FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,               FnEchoToken,                JListPop},
    {JSON::tokEndObject,    NULL,               FnEchoToken,                JListPop2}
};
const JSONLIST jLLog = {sizeof(rgELog) / sizeof(JSONENTRY), rgELog};

/************************************************************************/
/**************************** WiFi Commands *****************************/
/************************************************************************/
const JSONENTRY rgEDeviceNicConnect[] =    {   
    {JSON::tokValueSep,     NULL,                   NULL,           JListNone},
    {JSON::tokMemberName,   szAdapter,              NULL,           JListNone},
    {JSON::tokStringValue,  szWlan0,                NULL,           JListNone},
    {JSON::tokMemberName,   szParameterSet,         NULL,           JListNone},
    {JSON::tokStringValue,  szActiveParameterSet,   NULL,           JListNone},
    {JSON::tokStringValue,  szWorkingParameterSet,  FnFunc1,        JListNone},
    {JSON::tokMemberName,   szForce,                NULL,           JListNone},
    {JSON::tokTrue,         NULL,                   FnFunc0,        JListNone},
    {JSON::tokFalse,        NULL,                   NULL,           JListNone},
    {JSON::tokNameSep,      NULL,                   NULL,           JListNone},
    {JSON::tokEndObject,    NULL,                   FnNicConnect,   JListPop2}
};
const JSONLIST jLDeviceNicConnect = {sizeof(rgEDeviceNicConnect) / sizeof(JSONENTRY), rgEDeviceNicConnect};

const JSONENTRY rgEDeviceNicDisconnect[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,               JListNone},
    {JSON::tokMemberName,   szAdapter,  NULL,               JListNone},
    {JSON::tokStringValue,  szWlan0,    NULL,               JListNone},
    {JSON::tokNameSep,      NULL,       NULL,               JListNone},
    {JSON::tokEndObject,    NULL,       FnNicDisconnect,    JListPop2}
};
const JSONLIST jLDeviceNicDisconnect = {sizeof(rgEDeviceNicDisconnect) / sizeof(JSONENTRY), rgEDeviceNicDisconnect};

// Device nicList; Level 4
const JSONENTRY rgEDeviceNicList[] =    {   
    {JSON::tokEndObject,    NULL,       FnNicList,    JListPop2}
};
const JSONLIST jLDeviceNicList = {sizeof(rgEDeviceNicList) / sizeof(JSONENTRY), rgEDeviceNicList};

// Device nicGetStatus; Level 4
const JSONENTRY rgEDeviceNicGetStatus[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,           JListNone},
    {JSON::tokMemberName,   szAdapter,  NULL,           JListNone},
    {JSON::tokStringValue,  szWlan0,    NULL,           JListNone},
    {JSON::tokNameSep,      NULL,       NULL,           JListNone},
    {JSON::tokEndObject,    NULL,       FnNicGetStatus, JListPop2}
};
const JSONLIST jLDeviceNicGetStatus = {sizeof(rgEDeviceNicGetStatus) / sizeof(JSONENTRY), rgEDeviceNicGetStatus};

// Device wifiSetParameters; Level 4
const JSONENTRY rgEDeviceWiFiSetParam[] =    {   
    {JSON::tokValueSep,     NULL,           NULL,               JListNone},
    {JSON::tokMemberName,   szSsid,         NULL,               jLPTR0},
    {JSON::tokMemberName,   szPassphrase,   NULL,               jLPTR1},
    {JSON::tokMemberName,   szSecurityType, NULL,               JListNone},
    {JSON::tokMemberName,   szAutoConnect,  NULL,               JListNone},
    {JSON::tokMemberName,   szKeys,         NULL,               jLPTR2},
    {JSON::tokMemberName,   szKeyIndex,     NULL,               jLUInt0},
    {JSON::tokMemberName,   szAutoConnect,  NULL,               JListNone},
    {JSON::tokTrue,         NULL,           FnFunc0,            JListNone},
    {JSON::tokFalse,        NULL,           NULL,               JListNone},
    {JSON::tokStringValue,  szOpen,         NULL,               JListNone},
    {JSON::tokStringValue,  szWep40,        FnFunc1,            JListNone},
    {JSON::tokStringValue,  szWep104,       FnFunc2,            JListNone},
    {JSON::tokStringValue,  szWpa,          FnFunc3,            JListNone},
    {JSON::tokStringValue,  szWpa2,         FnFunc4,            JListNone},
    {JSON::tokNameSep,      NULL,           NULL,               JListNone},
    {JSON::tokEndObject,    NULL,           FnWiFiSetParam,     JListPop2}
};
const JSONLIST jLDeviceWiFiSetParam = {sizeof(rgEDeviceWiFiSetParam) / sizeof(JSONENTRY), rgEDeviceWiFiSetParam};

const JSONENTRY rgEDeviceWiFiSaveParam[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,               JListNone},
    {JSON::tokMemberName,   szStorageLocation,  NULL,               JListNone},
    {JSON::tokNameSep,      NULL,               NULL,               JListNone},
    {JSON::tokStringValue,  szFlash,            FnFunc0,            JListNone},
    {JSON::tokStringValue,  szSD0,              FnFunc1,            JListNone},
    {JSON::tokEndObject,    NULL,               FnWiFiSaveParam,    JListPop2}
};
const JSONLIST jLDeviceWiFiSaveParam = {sizeof(rgEDeviceWiFiSaveParam) / sizeof(JSONENTRY), rgEDeviceWiFiSaveParam};

const JSONENTRY rgEDeviceWiFiListParam[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,               JListNone},
    {JSON::tokMemberName,   szStorageLocation,  NULL,               JListNone},
    {JSON::tokNameSep,      NULL,               NULL,               JListNone},
    {JSON::tokStringValue,  szFlash,            FnFunc0,            JListNone},
    {JSON::tokStringValue,  szSD0,              FnFunc1,            JListNone},
    {JSON::tokEndObject,    NULL,               FnWiFiListParam,    JListPop2}
};
const JSONLIST jLDeviceWiFiListParam = {sizeof(rgEDeviceWiFiListParam) / sizeof(JSONENTRY), rgEDeviceWiFiListParam};

const JSONENTRY rgEDeviceWiFiLoadParam[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,               JListNone},
    {JSON::tokNameSep,      NULL,               NULL,               JListNone},
    {JSON::tokMemberName,   szStorageLocation,  NULL,               JListNone},
    {JSON::tokMemberName,   szSsid,             NULL,               jLPTR0},
    {JSON::tokStringValue,  szFlash,            FnFunc0,            JListNone},
    {JSON::tokStringValue,  szSD0,              FnFunc1,            JListNone},
    {JSON::tokEndObject,    NULL,               FnWiFiLoadParam,    JListPop2}
};
const JSONLIST jLDeviceWiFiLoadParam = {sizeof(rgEDeviceWiFiLoadParam) / sizeof(JSONENTRY), rgEDeviceWiFiLoadParam};

const JSONENTRY rgEDeviceWiFiDeleteParam[] =    {   
    {JSON::tokValueSep,     NULL,               NULL,               JListNone},
    {JSON::tokNameSep,      NULL,               NULL,               JListNone},
    {JSON::tokMemberName,   szStorageLocation,  NULL,               JListNone},
    {JSON::tokMemberName,   szSsid,             NULL,               jLPTR0},
    {JSON::tokStringValue,  szFlash,            FnFunc0,            JListNone},
    {JSON::tokStringValue,  szSD0,              FnFunc1,            JListNone},
    {JSON::tokEndObject,    NULL,               FnWiFiDeleteParam,  JListPop2}
};
const JSONLIST jLDeviceWiFiDeleteParam = {sizeof(rgEDeviceWiFiDeleteParam) / sizeof(JSONENTRY), rgEDeviceWiFiDeleteParam};

const JSONENTRY rgEDeviceWiFiScan[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,               JListNone},
    {JSON::tokMemberName,   szAdapter,  NULL,               JListNone},
    {JSON::tokStringValue,  szWlan0,    NULL,               JListNone},
    {JSON::tokMemberName,   szForce,    NULL,               JListNone},
    {JSON::tokTrue,         NULL,       FnFunc0,            JListNone},
    {JSON::tokFalse,        NULL,       NULL,               JListNone},
    {JSON::tokNameSep,      NULL,       NULL,               JListNone},
    {JSON::tokEndObject,    NULL,       FnWiFiScan,         JListPop2}
};
const JSONLIST jLDeviceWiFiScan = {sizeof(rgEDeviceWiFiScan) / sizeof(JSONENTRY), rgEDeviceWiFiScan};

const JSONENTRY rgEDeviceWiFiReadScan[] =    {   
    {JSON::tokValueSep,     NULL,       NULL,               JListNone},
    {JSON::tokMemberName,   szAdapter,  NULL,               JListNone},
    {JSON::tokStringValue,  szWlan0,    NULL,               JListNone},
    {JSON::tokNameSep,      NULL,       NULL,               JListNone},
    {JSON::tokEndObject,    NULL,       FnWiFiReadScan,     JListPop2}
};
const JSONLIST jLDeviceWiFiReadScan = {sizeof(rgEDeviceWiFiReadScan) / sizeof(JSONENTRY), rgEDeviceWiFiReadScan};

/************************************************************************/
/********************** Debug Print Commands ****************************/
/************************************************************************/
const JSONENTRY rgEDbgPrint[] = {   
    {JSON::tokNameSep,      NULL,       FnEchoToken,    JListNone},
    {JSON::tokStringValue,  szOn,       FnPrintOn,      JListNone}, 
    {JSON::tokStringValue,  szOff,      FnPrintOff,     JListNone}, 
    {JSON::tokStringValue,  szJSON,     FnPrintOff,     JListNone}, 
    {JSON::tokStringValue,  szMenu,     FnPrintOn,      JListNone}, 
    {JSON::tokEndObject,    NULL,       FnEchoToken,    JListPop2}
};
const JSONLIST jLDbgPrint = {sizeof(rgEDbgPrint) / sizeof(JSONENTRY), rgEDbgPrint};

// Device StorageGetLocations, Level 4
const JSONENTRY rgEDeviceStorageGetLocations[] =    {   
    {JSON::tokEndObject,    NULL,   FnStorageGetLocations,   JListPop2}
};
const JSONLIST jLDeviceStorageGetLocations = {sizeof(rgEDeviceStorageGetLocations) / sizeof(JSONENTRY), rgEDeviceStorageGetLocations};

// Device CalibrateGetStorageTypes, Level 4
const JSONENTRY rgEDeviceCalibrateGetStorageTypes[] =    {   
    {JSON::tokEndObject,    NULL,   FnCalibrateGetStorageTypes,   JListPop2}
};
const JSONLIST jLDeviceCalibrateGetStorageTypes = {sizeof(rgEDeviceCalibrateGetStorageTypes) / sizeof(JSONENTRY), rgEDeviceCalibrateGetStorageTypes};

// Device CalibrationGetInstructions, Level 4
const JSONENTRY rgEDeviceCalibrateGetInstructions[] =    {   
    {JSON::tokValueSep,     NULL,   NULL,                       JListNone},
    {JSON::tokMemberName,   szGND,  NULL,                       JListNone},
    {JSON::tokNameSep,      NULL,   NULL,                       JListNone},
    {JSON::tokTrue,         NULL,   FnFunc16,                   JListNone},
    {JSON::tokFalse,        NULL,   NULL,                       JListNone},
    {JSON::tokEndObject,    NULL,   FnCalibrateGetInstructions, JListPop2}
};
const JSONLIST jLDeviceCalibrateGetInstructions = {sizeof(rgEDeviceCalibrateGetInstructions) / sizeof(JSONENTRY), rgEDeviceCalibrateGetInstructions};

// Device CalibrateRead, Level 4
const JSONENTRY rgEDeviceCalibrateRead[] =    {   
    {JSON::tokEndObject,    NULL,   FnCalibrateRead,   JListPop2}
};
const JSONLIST jLDeviceCalibrateRead = {sizeof(rgEDeviceCalibrateRead) / sizeof(JSONENTRY), rgEDeviceCalibrateRead};

// Device CalibrateRead, Level 4
const JSONENTRY rgEDeviceCalibrateGetStatus[] =    {   
    {JSON::tokEndObject,    NULL,   FnCalibrateGetStatus,   JListPop2}
};
const JSONLIST jLDeviceCalibrateGetStatus = {sizeof(rgEDeviceCalibrateGetStatus) / sizeof(JSONENTRY), rgEDeviceCalibrateGetStatus};

// AWG channel calibrate; level 5 
const JSONENTRY rgECalAWG[] =    {   
    {JSON::tokNameSep,      NULL,   NULL,               JListNone},
    {JSON::tokNumber,       szOne,  FnFunc4,            JListPop}
};
const JSONLIST JCalAWG = {sizeof(rgECalAWG) / sizeof(JSONENTRY), rgECalAWG};

// DC channel calibrate; level 5 
const JSONENTRY rgECalDC[] =    {   
    {JSON::tokNumber,       szOne,  FnFunc0,            JListPop},
    {JSON::tokNumber,       szTwo,  FnFunc1,            JListPop},
    {JSON::tokNameSep,      NULL,   NULL,               JListNone},
    {JSON::tokValueSep,     NULL,   NULL,               JListPush},
    {JSON::tokArray,        NULL,   NULL,               JListPush},
    {JSON::tokEndArray,     NULL,   NULL,               JListPop}
};
const JSONLIST JCalDC = {sizeof(rgECalDC) / sizeof(JSONENTRY), rgECalDC};

// Device Calibration, Level 4
const JSONENTRY rgEDeviceCalibrateStart[] =    {   
    {JSON::tokValueSep,     NULL,   NULL,               JListNone},
    {JSON::tokMemberName,   szDAQ,  NULL,               JLSetCh},
    {JSON::tokMemberName,   szDC,   NULL,               JCalDC},
    {JSON::tokMemberName,   szAWG,  NULL,               JCalAWG},
    {JSON::tokMemberName,   szGND,  NULL,               JListNone},
    {JSON::tokNameSep,      NULL,   NULL,               JListNone},
    {JSON::tokTrue,         NULL,   FnFunc16,           JListNone},
    {JSON::tokFalse,        NULL,   NULL,               JListNone},
    {JSON::tokEndObject,    NULL,   FnCalibrateStart,   JListPop2}
};
const JSONLIST jLDeviceCalibrateStart = {sizeof(rgEDeviceCalibrateStart) / sizeof(JSONENTRY), rgEDeviceCalibrateStart};

// Device Calibration Load, Level 4
const JSONENTRY rgEDeviceCalibrateLoad[] =    {   
    {JSON::tokValueSep,     NULL,   NULL,               JListNone},
    {JSON::tokMemberName,   szType, NULL,               jLType},
    {JSON::tokEndObject,    NULL,   FnCalibrateLoad,    JListPop2}
};
const JSONLIST jLDeviceCalibrateLoad = {sizeof(rgEDeviceCalibrateLoad) / sizeof(JSONENTRY), rgEDeviceCalibrateLoad};

// Device Calibration Save, Level 4
const JSONENTRY rgEDeviceCalibrateSave[] =    {   
    {JSON::tokValueSep,     NULL,   NULL,               JListNone},
    {JSON::tokMemberName,   szType, NULL,               jLType},
    {JSON::tokEndObject,    NULL,   FnCalibrateSave,    JListPop2}
};
const JSONLIST jLDeviceCalibrateSave = {sizeof(rgEDeviceCalibrateSave) / sizeof(JSONENTRY), rgEDeviceCalibrateSave};

// Device Enter bootloader; Level 4
const JSONENTRY rgEDeviceEnterBootloader[] =    {   
    {JSON::tokEndObject,    NULL,       FnEnterBootloader,    JListPop2}
};
const JSONLIST jLDeviceEnterBootloader = {sizeof(rgEDeviceEnterBootloader) / sizeof(JSONENTRY), rgEDeviceEnterBootloader};

// Device Reset Instruments; Level 4
const JSONENTRY rgEDeviceResetInstruments[] =    {   
    {JSON::tokEndObject,    NULL,       FnResetInstruments,    JListPop2}
};
const JSONLIST jLDeviceResetInstruments = {sizeof(rgEDeviceResetInstruments) / sizeof(JSONENTRY), rgEDeviceResetInstruments};

// Device LoopStatistics; Level 4
const JSONENTRY rgEDeviceLoopStats[] =    {   
    {JSON::tokEndObject,    NULL,       FnLoopStats,    JListPop2}
};
const JSONLIST jLDeviceLoopStats = {sizeof(rgEDeviceLoopStats) / sizeof(JSONENTRY), rgEDeviceLoopStats};

// Device Enumerate; Level 4
const JSONENTRY rgEDeviceEnum[] =    {   
    {JSON::tokEndObject,    NULL,       FnEnumeration,  JListPop2}
};
const JSONLIST jLDeviceEnum = {sizeof(rgEDeviceEnum) / sizeof(JSONENTRY), rgEDeviceEnum};

// Device; Level 2,3
const JSONENTRY rgEDevice[] = {   
    {JSON::tokStringValue,  szEnumerate,                    FnEchoToken,                jLDeviceEnum},
    {JSON::tokStringValue,  szCalibrationGetInstructions,   FnEchoToken,                jLDeviceCalibrateGetInstructions},   
    {JSON::tokStringValue,  szCalibrationGetStorageTypes,   FnEchoToken,                jLDeviceCalibrateGetStorageTypes},  
    {JSON::tokStringValue,  szStorageGetLocations,          FnEchoToken,                jLDeviceStorageGetLocations},      
    {JSON::tokStringValue,  szCalibrationRead,              FnEchoToken,                jLDeviceCalibrateRead},   
    {JSON::tokStringValue,  szCalibrationStart,             FnClearQueuedValuesAndEcho, jLDeviceCalibrateStart},   
    {JSON::tokStringValue,  szCalibrationSave,              FnClearQueuedValuesAndEcho, jLDeviceCalibrateSave},   
    {JSON::tokStringValue,  szCalibrationLoad,              FnClearQueuedValuesAndEcho, jLDeviceCalibrateLoad},  
    {JSON::tokStringValue,  szCalibrationGetStatus,         FnEchoToken,                jLDeviceCalibrateGetStatus},  
    {JSON::tokStringValue,  szLoopStats,                    FnEchoToken,                jLDeviceLoopStats},
    {JSON::tokStringValue,  szEnterBootloader,              FnEchoToken,                jLDeviceEnterBootloader},  
    {JSON::tokStringValue,  szResetInstruments,             FnEchoToken,                jLDeviceResetInstruments},  
    {JSON::tokStringValue,  szNicList,                      FnEchoToken,                jLDeviceNicList}, 
    {JSON::tokStringValue,  szNicGetStatus,                 FnEchoToken,                jLDeviceNicGetStatus}, 
    {JSON::tokStringValue,  szNicConnect,                   FnClearQueuedValuesAndEcho, jLDeviceNicConnect}, 
    {JSON::tokStringValue,  szNicDisconnect,                FnEchoToken,                jLDeviceNicDisconnect}, 
    {JSON::tokStringValue,  szWiFiSetParameters,            FnClearQueuedValuesAndEcho, jLDeviceWiFiSetParam},  
    {JSON::tokStringValue,  szWiFiSaveParameters,           FnClearQueuedValuesAndEcho, jLDeviceWiFiSaveParam}, 
    {JSON::tokStringValue,  szWiFiLoadParameters,           FnClearQueuedValuesAndEcho, jLDeviceWiFiLoadParam}, 
    {JSON::tokStringValue,  szWiFiDeleteParameters,         FnClearQueuedValuesAndEcho, jLDeviceWiFiDeleteParam}, 
    {JSON::tokStringValue,  szWiFiListParameters,           FnClearQueuedValuesAndEcho, jLDeviceWiFiListParam}, 
    {JSON::tokStringValue,  szWiFiScan,                     FnClearQueuedValuesAndEcho, jLDeviceWiFiScan}, 
    {JSON::tokStringValue,  szWiFiReadScan,                 FnEchoToken,                jLDeviceWiFiReadScan},    
    {JSON::tokMemberName,   szCommand,                      FnEchoToken,                JListNone},
    {JSON::tokNameSep,      NULL,                           FnEchoToken,                JListNone},
    {JSON::tokValueSep,     NULL,                           FnEchoToken,                JListPush},
    {JSON::tokObject,       NULL,                           FnEchoToken,                JListNone},
    {JSON::tokArray,        NULL,                           FnEchoToken,                JListPush},
    {JSON::tokEndArray,     NULL,                           FnEchoToken,                JListPop}
};
const JSONLIST jLDevice = {sizeof(rgEDevice) / sizeof(JSONENTRY), rgEDevice};

/************************************************************************/
/********************** Top Level Instruments/ commands *****************/
/************************************************************************/
// All of the instruments we support; Level 1
const JSONENTRY rgEModule[] =   {
    {JSON::tokValueSep,     NULL,       FnEchoToken,    JListNone},
    {JSON::tokMemberName,   szDevice,   FnEchoToken,    jLDevice}, 
    {JSON::tokMemberName,   szDC,       FnEchoToken,    jLDCChannel}, 
    {JSON::tokMemberName,   szGPIO,     FnEchoToken,    jLGPIOChannel}, 
    {JSON::tokMemberName,   szAWG,      FnEchoToken,    jLAWGChannel}, 
    {JSON::tokMemberName,   szFile,     FnEchoToken,    jLFile}, 
    {JSON::tokMemberName,   szLog,      FnEchoToken,    jLLog}, 
    {JSON::tokMemberName,   szTest,     FnEchoToken,    jLTest}, 
    {JSON::tokMemberName,   szUpdate,   FnEchoToken,    jLUpdate}, 
    {JSON::tokMemberName,   szDbgPrint, FnEchoToken,    jLDbgPrint}, 
    {JSON::tokMemberName,   szMode,     FnEchoToken,    jLDbgPrint}, 
    {JSON::tokEndObject,    NULL,       FnEchoToken,    JListPop}  // return end of token?
};
const JSONLIST jLModule = {sizeof(rgEModule) / sizeof(JSONENTRY), rgEModule};

/************************************************************************/
/********************** JSON master object entrypoint *******************/
/************************************************************************/
// Level Zero
const JSONENTRY rgETop[] = {
    {JSON::tokObject,       NULL,       FnEchoToken,    jLModule},
    {JSON::tokEndOfJSON,    NULL,       FnEndOfJSON,    JListNone}
};
const JSONLIST jLTop = {sizeof(rgETop) / sizeof(JSONENTRY), rgETop};

/************************************************************************/
/********************** JSON Class Declaration **************************/
/************************************************************************/
// create the JSON class object and point to the top of the list
// JSON jCommands = JSON(jLTop);
OSPAR oslex = OSPAR(jLTop);



