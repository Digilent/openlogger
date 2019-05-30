/************************************************************************/
/*                                                                      */
/*    WiFiStubs.c                                                       */
/*                                                                      */
/*    Header file containing the OpenScope Build Version number         */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2/1/2017 (KeithV): Created                                        */
/************************************************************************/
#include <OpenLogger.h>
#include <malloc.h>

#include "./utility/bus_wrapper/include/nm_bus_wrapper.h"
#include "WINC1500Adaptor.h"

tstrNmBusCapabilities egstrNmBusCapabilities = {256};
tpfNmBspIsr fnIsr = NULL;

// this is unused, we poll because polling is as fast as we can go
#if 0
void __attribute__((nomips16, at_vector(_EXTERNAL_3_VECTOR),interrupt(IPL1SRS))) ISRWiFi(void)
{
    IFS0bits.INT3IF = 0;
    fnIsr();   
}
#endif

extern uint8_t gmgmt[4096];
extern void wifi_event_cb(uint8 u8MsgType, void * pvMsg);
extern void wifi_ethernet_cb(uint8 u8MsgType, void * pvMsg,void * pvCtrlBuf);
extern void WINCSerilTask(void);
extern tpfNmBspIsr fnIsr;

void WINC_Task(void)
{
    // polling is as good as we can get.
    // but we want to make sure we clear all events
    // form the WINC before quitting.
    do {           
        if(!GetGPIO(WIFI_PIN_INT) && fnIsr) fnIsr();
        m2m_wifi_handle_events(NULL);                   
    } while(!GetGPIO(WIFI_PIN_INT) && fnIsr);
    
        
#if (M2M_LOG_LEVEL != M2M_LOG_NONE)
    WINCSerilTask();
#endif  
}

bool WINCInitWiFi(void) 
{
    tstrWifiInitParam param;
    param.strEthInitParam.pfAppWifiCb = wifi_event_cb;
    param.strEthInitParam.pfAppEthCb  = wifi_ethernet_cb;
    param.pfAppWifiCb = wifi_event_cb;
    param.pfAppMonCb  = NULL;
  
    param.strEthInitParam.au8ethRcvBuf = KVA_2_KSEG1(gmgmt);
    param.strEthInitParam.u16ethRcvBufSize = sizeof(gmgmt);

#ifdef ETH_MODE  
    param.strEthInitParam.u8EthernetEnable = M2M_WIFI_MODE_ETHERNET;    // bypass mode
#else    
    param.strEthInitParam.u8EthernetEnable = M2M_WIFI_MODE_NORMAL;    // start the IP stack
#endif  
    
    nm_bsp_init();

    return(m2m_wifi_init(&param) == M2M_SUCCESS);   
 }

sint8 nm_bsp_init(void)
{
    /* Initialize chip IOs. */
    // set outputs
    GPIO(WIFI_PIN_CE, CLR);        // Chip Enable
    INITRIS(WIFI_PIN_CE, CLR);

    GPIO(WIFI_PIN_RST, CLR);       // Reset
    INITRIS(WIFI_PIN_RST, CLR);

    GPIO(WIFI_PIN_WAKE, SET);      // Wake
    INITRIS(WIFI_PIN_WAKE, CLR);

    GPIO(WIFI_PIN_CS, SET);        // CS/SS
    INITRIS(WIFI_PIN_CS, CLR);

    GPIO(WIFI_PIN_SCK, CLR);       // SCK4
    INITRIS(WIFI_PIN_SCK, CLR);

    GPIO(WIFI_PIN_MOSI, CLR);      // MOSI
    INITRIS(WIFI_PIN_MOSI, CLR);

    GPIO(WIFI_PIN_MTSR, CLR);      // WiFi RXD (MZ TXD) .... NOTE, LEAVE TRI-STATED
    INITRIS(WIFI_PIN_MTSR, SET);    //  .... NOTE, LEAVE TRI-STATED

    // Inputs
    INITRIS(WIFI_PIN_INT, SET);     //INT
    INITRIS(WIFI_PIN_MISO, SET);    // MISO
    INITRIS(WIFI_PIN_MRST, SET);    // WiFi TXD (MZ RXD)

                                    // set up PPS
    // PPS MAPPINGs
    // PPS Mapping
//    WIFI_PPS_INT;
    WIFI_PPS_SS;
    WIFI_PPS_MISO;
    WIFI_PPS_MOSI;

    /* Perform chip reset. */
    nm_bsp_sleep(100);
    GPIO(WIFI_PIN_CE, SET);        // Chip Enable
    nm_bsp_sleep(100);
    GPIO(WIFI_PIN_RST, SET);
    nm_bsp_sleep(100);

    return(M2M_SUCCESS);
}

void nm_bsp_reset(void)
{
    GPIO(WIFI_PIN_CE, CLR);
    GPIO(WIFI_PIN_RST, CLR);
    nm_bsp_sleep(100);
    GPIO(WIFI_PIN_CE, SET);
    nm_bsp_sleep(100);
    GPIO(WIFI_PIN_RST, SET);
    nm_bsp_sleep(100);
}

void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
    fnIsr = pfIsr;
    
    // we have the handler, we can enable the ISR
//    if(fnIsr != NULL) IEC0bits.INT3IE = 1;
}

void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
//        IEC0bits.INT3IE     = u8Enable;
}

void nm_bsp_sleep(uint32 u32TimeMsec)
{

    uint32 tStart = ReadCoreTimer();
    while((ReadCoreTimer() - tStart) < (u32TimeMsec * CORE_TMR_TICKS_PER_MSEC));
}

sint8 nm_bus_ioctl(uint8 u8Cmd, void* pvParameter)
{
    tstrNmSpiRw *   pspi = (tstrNmSpiRw *) pvParameter;
//    uint8 *         pZero = (uint8 *) KVA_2_KSEG1(alloca(pspi->u16Sz));
    uint8 *         pZero = (uint8 *) alloca(pspi->u16Sz);
    uint16_t        i = 0;
    uint16_t        j = 0;

    // very confusing... but the tstrNmSpiRw is in reference to the slave, not master. 
    // so input buffer means input to the slave (MOSI), output means output from the slave (MISO).
    uint8 *         pMOSI = (pspi->pu8InBuf == NULL)    ? pZero : pspi->pu8InBuf;
    uint8 *         pMISO = (pspi->pu8OutBuf == NULL)   ? pZero : pspi->pu8OutBuf;

    // NM_BUS_IOCTL_RW is the only supported SPI command
    if(u8Cmd != NM_BUS_IOCTL_RW) return(M2M_SPI_FAIL);

    // nothing to do
    if(pspi->u16Sz == 0) return(M2M_SUCCESS);
    
    // set our allocated buffer to zero if in use
    if(pMOSI == pZero) memset(pZero, 0, pspi->u16Sz);

#if 0
    // make sure we are in kseg1, non-cached memory
    if((uint8 *) KVA_2_KSEG1(pMOSI) != pMOSI) CACHE_2_PHY(pMOSI, pspi->u16Sz);

    // sending
    WIFISNDDMA.DCHxSSA      = KVA_2_PA(pMOSI);  // source buffer
    WIFISNDDMA.DCHxSSIZ     = pspi->u16Sz;      // source size

    // recieving
    WIFIRCVDMA.DCHxDSA      = KVA_2_PA(pMISO);  // target buffer result buffer
    WIFIRCVDMA.DCHxDSIZ     = pspi->u16Sz;      // result buffer size

                                                // start the DMA channels
//    WIFISNDDMA.DCHxCON.CHEN = 1;                // turn on sending DMA
    WIFIRCVDMA.DCHxCON.CHEN = 1;                // trun on recieving DMA

    // now loop sending data to the SPI as fast as I can
    for(i = 0; i < pspi->u16Sz; i++)
    {
        // wait for an available byte in the transmit buffer
        // a one says the transmit FIFO is full.
        while(WIFISPI.SPIxSTAT.SPITBF);  // spin while full

        // transmit the next byte.
        WIFISPI.SPIxBUF = pMOSI[i];
    }

    // wait til complete
    // don't want to hammer the DMA by checking CHEN in a tight loop, we could, but lets ease up
    // we know we are running at 25MHz, and a min command is 5 bytes long; about 1.6us, so only check once a usec
    while(WIFIRCVDMA.DCHxCON.CHEN)
    {
        uint32 tStart = ReadCoreTimer();
        while((ReadCoreTimer() - tStart) < CORE_TMR_TICKS_PER_USEC);
    }

    // the sending channel should be done as well
//    ASSERT(WIFISNDDMA.DCHxCON.CHEN == 0);

    // make sure we are in kseg0 if needed, non-cached memory
    if(pspi->pu8OutBuf != NULL && (uint8 *) KVA_2_KSEG1(pMISO) != pMISO) PHY_2_CACHE(pMISO, pspi->u16Sz);
#endif

    while(i < pspi->u16Sz)
    {
        // if we have room to transmit, put one out.
        if(!WIFISPI.SPIxSTAT.SPITBF) WIFISPI.SPIxBUF = pMOSI[i++];

        // if something is ready to come in, get them all
        while(!WIFISPI.SPIxSTAT.SPIRBE) pMISO[j++] = WIFISPI.SPIxBUF;
    }

    while(j < pspi->u16Sz)
    {
        if(!WIFISPI.SPIxSTAT.SPIRBE) pMISO[j++] = WIFISPI.SPIxBUF;
    }

    return(M2M_SUCCESS);
}

sint8 nm_bus_deinit(void)
{
    return(0);
}


// set up the SPI interface to the WILC1000
// RF4  is IRQ
// RF12 is RST
// RC4  is RXD (WILC)
// RA14 is CE
// RD10 is SCK4
// RF8  is SS
// RF5  is MISO
// RA15 is MOSI


sint8 nm_bus_init(void * pFoo)
{

    // WIFE IRQ on F4, INT3
    // we really don't use this as we just look at the pin directly
//    WIFIINTCON      = 0;                // falling edge 
//    WIFIIPCP        = WIFI_INT_PRI;     // int priority
//    WIFIIPCS        = 0;                // sub priority
//    WIFIIF          = 0;
//    WIFIIE          = 0;    // We don't use the ISR for interrupts

    // turn off interrupts for SPI
//    WIFISPIEIE      = 0;
//    WIFISPIRXIE     = 0;
//    WIFISPITXIE     = 0;
//    WIFISPIEIF      = 0;
//    WIFISPIRXIF     = 0;
//    WIFISPITXIF     = 0;

    // set up the SPI control registers
    // the WILC1000 is on SPI4
    WIFISPI.SPIxCON.w        = 0;
    WIFISPI.SPIxCON2.w       = 0;

    WIFISPI.SPIxCON.MSTEN    = 1;
    WIFISPI.SPIxCON.MSSEN    = 1;        // we want to control the CS pin automatically
    WIFISPI.SPIxCON.FRMPOL   = 0;        // we want the CS to be low active. 

    WIFISPI.SPIxCON.MCLKSEL  = 0;        // we want to use the PBCLK as our clock source
    WIFISPI.SPIxCON.ENHBUF   = 1;        // we want to use the FIFOs
    WIFISPI.SPIxCON.MODE16   = 0;        // we want to be in 8 bits per value
    WIFISPI.SPIxCON.MODE32   = 0;        // we want to be in 8 bits per value

        //    SPI Mode    Clock Polarity  Clock Phase Clock Edge
        //                (CPOL/CKP)      (CPHA)      (CKE/NCPHA)   
        //        0         0               0           1
        //        1	        0	            1	        0
        //        2	        1	            0	        1
        //        3	        1	            1	        0
        // WILC1000 CPOL == 0; CPHA == 0
        // MZ   CKP == 0; CKE == 1
    WIFISPI.SPIxCON.CKP      = 0;        // CPOL == 1; Idle high, active low
    WIFISPI.SPIxCON.CKE      = 1;        // CPHA == 0; SDO changes on trailing edge of preceeding clock
    WIFISPI.SPIxCON.SMP      = 0;        // Read data at the middle of the output time. The output is put applied at the end of the last clock (rising edge), this will read the beginning of the current clock (falling edge).

//    WIFIDMASPI.SPIxCON.STXISEL  = 0b01;     // generate interrupt when transmit buffer has any room
//    WIFIDMASPI.SPIxCON.SRXISEL  = 0b01;     // generate a DMA interrupt when the FIFO is not empty

    WIFISPI.SPIxSTAT.SPIROV  = 0;        // clear the overrun flag

    // we can run up to 48MHz
    WIFISPI.SPIxBRG          = 1;        // set to 1 == 25MHz, 2 == 16.667MHz   

#if 0
    // DMA channel 5 & 6 for SPI WiFi I/o
    // transmit
    WIFISNDDMA.DCHxCON.CHPRI    = WIFI_SPI_RECV_DMAPRI; // Lowist priority DMA
    WIFISNDDMA.DCHxCON.CHAEN    = 0;                    // just once through, no cycling 
    WIFISNDDMA.DCHxECON.CHSIRQ  = WIFISPITXVECTOR;      // When there is room in the transmit, send a char
    WIFISNDDMA.DCHxECON.SIRQEN  = 1;                    // enable start IRQ
    WIFISNDDMA.DCHxSSA          = (uint32_t) NULL;      // source buffer
    WIFISNDDMA.DCHxSSIZ         = 0;                    // source size
    WIFISNDDMA.DCHxCSIZ         = 1;                    // cell transfer size 1 byte
    WIFISNDDMA.DCHxDSA          = KVA_2_PA(&WIFISPI.SPIxBUF);   // SPI buffer
    WIFISNDDMA.DCHxDSIZ         = 1;                    // result buffer size

    // recieve    
    WIFIRCVDMA.DCHxCON.CHPRI    = WIFI_SPI_RECV_DMAPRI; // Low priority on the DMA
    WIFIRCVDMA.DCHxCON.CHAEN    = 0;                    // just once through, no cycling 
    WIFIRCVDMA.DCHxECON.CHSIRQ  = WIFISPIRXVECTOR;      // There is a word in the recieve buffer
    WIFIRCVDMA.DCHxECON.SIRQEN  = 1;                    // enable start IRQ
    WIFIRCVDMA.DCHxSSA          = KVA_2_PA(&WIFISPI.SPIxBUF);   // source buffer
    WIFIRCVDMA.DCHxSSIZ         = 1;                    // source size
    WIFIRCVDMA.DCHxCSIZ         = 1;                    // cell transfer size 1 byte
    WIFIRCVDMA.DCHxDSA          = (uint32_t) NULL;      // target buffer result buffer
    WIFIRCVDMA.DCHxDSIZ         = 0;                    // result buffer size
#endif

    ASSERT(WIFISPI.SPIxSTAT.SPIRBE);

    // turn on the SPI controller
    WIFISPI.SPIxCON.ON = 1;

    return(M2M_SUCCESS);
}

void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
{
    wfWINC1500.priv.lastEvent = u8WiFiEvent;
    switch(u8WiFiEvent)
    {
        // when we connect or disconnect
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
        {
            tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged*)pvMsg;
            
            // Error codes
            // 1: M2M_ERR_SCAN_FAIL ; Indicate that the WINC board has failed to perform the scan operation.
            // 2: M2M_ERR_JOIN_FAIL ; Indicate that the WINC board has failed to join the BSS 							
            // 3: M2M_ERR_AUTH_FAIL ; Indicate that the WINC board has failed to authenticate with the AP								
            // 4: M2M_ERR_ASSOC_FAIL ; Indicate that the WINC board has failed to associate with the AP
            // 5: M2M_ERR_CONN_INPROGRESS

            wfWINC1500.priv.connectionStatus    = pstrWifiState->u8CurrState;
            wfWINC1500.priv.connectionError     = pstrWifiState->u8ErrCode;  
            wfWINC1500.priv.fMRFBusy            = false;
            
            if(wfWINC1500.priv.connectionError == M2M_SUCCESS && wfWINC1500.priv.connectionStatus == M2M_WIFI_DISCONNECTED) 
            {
                    wfWINC1500.priv.connectionError = M2M_WIFI_DISCONNECTED;
//                    wfWINC1500.priv.connectionError = M2M_ERR_ASSOC_FAIL;
            }
        }
        break;

        case M2M_WIFI_RESP_SCAN_DONE:
        {
            tstrM2mScanDone * ptstrM2mScanDone = (tstrM2mScanDone *) pvMsg;
            wfWINC1500.priv.cScanResults = ptstrM2mScanDone->u8NumofCh;
            wfWINC1500.priv.fMRFBusy = false;
        }
        break;

        case M2M_WIFI_RESP_SCAN_RESULT:
        {            
            memcpy(&wfWINC1500.priv.scanResult, pvMsg, sizeof(tstrM2mWifiscanResult));
            wfWINC1500.priv.fMRFBusy = false;            
        }
        break;
        
        case M2M_WIFI_RESP_CONN_INFO:
//        {
//            tstrM2MConnInfo		*pstrConnInfo = (tstrM2MConnInfo*)pvMsg;

//            printf("CONNECTED AP INFO\n");
//            printf("SSID     			: %s\n",pstrConnInfo->acSSID);
//            printf("SEC TYPE 			: %d\n",pstrConnInfo->u8SecType);
//            printf("Signal Strength		: %d\n", pstrConnInfo->s8RSSI); 
//            printf("Local IP Address	: %d.%d.%d.%d\n", 
//                pstrConnInfo->au8IPAddr[0] , pstrConnInfo->au8IPAddr[1], pstrConnInfo->au8IPAddr[2], pstrConnInfo->au8IPAddr[3]);
//        }
//        break;

        case M2M_WIFI_RESP_CURRENT_RSSI:
//            sint8	*rssi = (sint8*)pvMsg;


        case M2M_WIFI_REQ_DHCP_CONF:
//            memcpy(&wifiCB.ipConfig, pvMsg, sizeof(tstrM2MIPConfig));
//            break;

        case M2M_WIFI_REQ_LSN_INT:
        /*!< Set WiFi listen interval.
        */

        case M2M_WIFI_REQ_CONNECT:
        /*!< Connect with AP command.
        */
        case M2M_WIFI_REQ_DEFAULT_CONNECT:
        /*!< Connect with default AP command.
        */
        case M2M_WIFI_RESP_DEFAULT_CONNECT:
        /*!< Request connection information response.
        */
        case M2M_WIFI_REQ_DISCONNECT:
        /*!< Request to disconnect from AP command.
        */
        case M2M_WIFI_REQ_SLEEP:
        /*!< Set PS mode command.
        */
        case M2M_WIFI_REQ_WPS_SCAN:
        /*!< Request WPS scan command.
        */
        case M2M_WIFI_REQ_WPS:
        /*!< Request WPS start command.
        */
        case M2M_WIFI_REQ_START_WPS:
        /*!< This command is for internal use by the WINC and 
            should not be used by the host driver.
        */
        case M2M_WIFI_REQ_DISABLE_WPS:
        /*!< Request to disable WPS command.
        */
        case M2M_WIFI_RESP_IP_CONFIGURED:
        /*!< This command is for internal use by the WINC and 
            should not be used by the host driver.
        */
        case M2M_WIFI_RESP_IP_CONFLICT:
        /*!< Response indicating a conflict in obtained IP address.
            The user should re attempt the DHCP request.
        */
        case M2M_WIFI_REQ_ENABLE_MONITORING:
        /*!< Request to enable monitor mode  command.
        */
        case M2M_WIFI_REQ_DISABLE_MONITORING:
        /*!< Request to disable monitor mode  command.
        */
        case M2M_WIFI_RESP_WIFI_RX_PACKET:
        /*!< Indicate that a packet was received in monitor mode.
        */
        case M2M_WIFI_REQ_SEND_WIFI_PACKET:
        /*!< Send packet in monitor mode.
        */
        case M2M_WIFI_REQ_DOZE:
        /*!< Used to force the WINC to sleep in manual PS mode.
        */
        case M2M_WIFI_MAX_STA_ALL:
              
        case M2M_WIFI_REQ_GET_CONN_INFO:
        case M2M_WIFI_REQ_SCAN:
        case M2M_WIFI_REQ_SCAN_RESULT:
        case M2M_WIFI_REQ_SEND_ETHERNET_PACKET:
        case M2M_WIFI_RESP_ETHERNET_RX_PACKET:
        case M2M_WIFI_REQ_SET_SCAN_OPTION:
        case M2M_WIFI_REQ_SET_SCAN_REGION:
        case M2M_WIFI_REQ_SET_MAC_MCAST:
        default:
            break;
    }
}

void wifi_ethernet_cb(uint8 u8MsgType, void * pvMsg,void * pvCtrlBuf)
{
    tstrM2mIpCtrlBuf *  pstrM2mIpCtrlBuf    = (tstrM2mIpCtrlBuf *) pvCtrlBuf;       

    ASSERT(pstrM2mIpCtrlBuf->u16RemainigDataSize == 0);
    if(pstrM2mIpCtrlBuf->u16DataSize > 0)
    {
        IPSTACK * pIpStack = RRHPAlloc(wfWINC1500.adpWINC1500.hAdpHeap, pstrM2mIpCtrlBuf->u16DataSize + sizeof(IPSTACK));
        if(pIpStack != NULL)
        {

            // fill in info about the frame data
            pIpStack->fFrameIsParsed    = FALSE;
            pIpStack->fFreeIpStackToAdp = TRUE;
            pIpStack->headerOrder       = NETWORK_ORDER;
            pIpStack->pPayload          = ((uint8_t *) pIpStack) + sizeof(IPSTACK);
            pIpStack->cbPayload         = pstrM2mIpCtrlBuf->u16DataSize;

            memcpy(pIpStack->pPayload, pvMsg, pIpStack->cbPayload);

            pIpStack->fOwnedByAdp = true;
            FFInPacket(&wfWINC1500.priv.ffptRead, pIpStack);
        }
    }
}

