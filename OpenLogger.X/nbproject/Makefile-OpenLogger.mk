#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-OpenLogger.mk)" "nbproject/Makefile-local-OpenLogger.mk"
include nbproject/Makefile-local-OpenLogger.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=OpenLogger
MACRO1=C:/
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../OpenLogger/Version.c ../OpenLogger/FeedBack.c ../OpenLogger/DDR.c ../OpenLogger/Initialize.c ../OpenLogger/OnReset.c ../OpenLogger/Threads.c ../OpenLogger/GlobalData.cpp ../OpenLogger/Helper.cpp ../OpenLogger/LEDs.cpp ../OpenLogger/LoopStats.cpp ../OpenLogger/OSSerial.cpp ../OpenLogger/main.cpp ../OpenLogger/OSMath.cpp ../OpenLogger/TimeOutTmr9.cpp ../OpenLogger/AWG.cpp ../OpenLogger/ADC.cpp ../OpenLogger/WiFi.cpp ../OpenLogger/File.cpp ../OpenLogger/LexJSON.cpp ../OpenLogger/Parse.cpp ../OpenLogger/FIFO245.cpp ../OpenLogger/JsonIO.cpp ../OpenLogger/DeferredTasks.cpp ../OpenLogger/DCOut.cpp ../OpenLogger/ADCTargets.cpp ../OpenLogger/CloudTask.cpp ../OpenLogger/LogOnBoot.cpp ../OpenLogger/libraries/DEIPcK/utility/DHCP.c ../OpenLogger/libraries/DEIPcK/utility/DNS.c ../OpenLogger/libraries/DEIPcK/utility/HeapMgr.c ../OpenLogger/libraries/DEIPcK/utility/ICMP.c ../OpenLogger/libraries/DEIPcK/utility/IPStack.c ../OpenLogger/libraries/DEIPcK/utility/InternetLayer.c ../OpenLogger/libraries/DEIPcK/utility/LinkLayer.c ../OpenLogger/libraries/DEIPcK/utility/SNTPv4.c ../OpenLogger/libraries/DEIPcK/utility/System.c ../OpenLogger/libraries/DEIPcK/utility/TCP.c ../OpenLogger/libraries/DEIPcK/utility/TCPRFC793.c ../OpenLogger/libraries/DEIPcK/utility/TCPStateMachine.c ../OpenLogger/libraries/DEIPcK/utility/UDP.c ../OpenLogger/libraries/DEIPcK/DEIPcK.cpp ../OpenLogger/libraries/DEIPcK/TCPServer.cpp ../OpenLogger/libraries/DEIPcK/TCPSocket.cpp ../OpenLogger/libraries/DEIPcK/UDPServer.cpp ../OpenLogger/libraries/DEIPcK/UDPSocket.cpp ../OpenLogger/libraries/DEWFcK/DEWFcK.cpp ../OpenLogger/libraries/DEWFcK/CalPSK.c ../OpenLogger/libraries/DFATFS/utility/ccsbcs.c ../OpenLogger/libraries/DFATFS/utility/fs_ff.c ../OpenLogger/libraries/DFATFS/DFATFS.cpp ../OpenLogger/libraries/DFATFS/fs_diskio.cpp ../OpenLogger/libraries/DMASerial/DMASerial.cpp ../OpenLogger/libraries/DSDVOL/DSDVOL.cpp ../OpenLogger/libraries/DSPI/DSPI.cpp ../OpenLogger/libraries/FLASHVOL/FLASHVOL.cpp ../OpenLogger/libraries/FLASHVOL/flash.c ../OpenLogger/libraries/HTTPServer/HTMLDefaultPage.cpp ../OpenLogger/libraries/HTTPServer/HTMLOptions.cpp ../OpenLogger/libraries/HTTPServer/HTMLPostCmd.cpp ../OpenLogger/libraries/HTTPServer/HTMLReboot.cpp ../OpenLogger/libraries/HTTPServer/HTMLSDPage.cpp ../OpenLogger/libraries/HTTPServer/HTTPHelpers.cpp ../OpenLogger/libraries/HTTPServer/ProcessClient.cpp ../OpenLogger/libraries/HTTPServer/ProcessServer.cpp ../OpenLogger/libraries/HTTPServer/deOpenScopeWebServer.cpp ../OpenLogger/libraries/WINC1500/utility/common/source/nm_common.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ate_mode.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_crypto.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ota.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_periph.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ssl.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_wifi.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmi2c.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmspi.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmuart.c ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/asn1.c ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/hash.c ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/x509_cert.c ../OpenLogger/libraries/WINC1500/utility/root_cert/root_setup.c ../OpenLogger/libraries/WINC1500/utility/socket/source/socket.c ../OpenLogger/libraries/WINC1500/utility/spi_flash/source/spi_flash.c ../OpenLogger/libraries/WINC1500/WINC1500.cpp ../OpenLogger/libraries/WINC1500/WINC1500Adaptor.c ../OpenLogger/libraries/WINC1500/WiFiStubs.c ../OpenLogger/libraries/WINC1500/WiFiUpdateFW.cpp

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/315570249/Version.o ${OBJECTDIR}/_ext/315570249/FeedBack.o ${OBJECTDIR}/_ext/315570249/DDR.o ${OBJECTDIR}/_ext/315570249/Initialize.o ${OBJECTDIR}/_ext/315570249/OnReset.o ${OBJECTDIR}/_ext/315570249/Threads.o ${OBJECTDIR}/_ext/315570249/GlobalData.o ${OBJECTDIR}/_ext/315570249/Helper.o ${OBJECTDIR}/_ext/315570249/LEDs.o ${OBJECTDIR}/_ext/315570249/LoopStats.o ${OBJECTDIR}/_ext/315570249/OSSerial.o ${OBJECTDIR}/_ext/315570249/main.o ${OBJECTDIR}/_ext/315570249/OSMath.o ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o ${OBJECTDIR}/_ext/315570249/AWG.o ${OBJECTDIR}/_ext/315570249/ADC.o ${OBJECTDIR}/_ext/315570249/WiFi.o ${OBJECTDIR}/_ext/315570249/File.o ${OBJECTDIR}/_ext/315570249/LexJSON.o ${OBJECTDIR}/_ext/315570249/Parse.o ${OBJECTDIR}/_ext/315570249/FIFO245.o ${OBJECTDIR}/_ext/315570249/JsonIO.o ${OBJECTDIR}/_ext/315570249/DeferredTasks.o ${OBJECTDIR}/_ext/315570249/DCOut.o ${OBJECTDIR}/_ext/315570249/ADCTargets.o ${OBJECTDIR}/_ext/315570249/CloudTask.o ${OBJECTDIR}/_ext/315570249/LogOnBoot.o ${OBJECTDIR}/_ext/713764681/DHCP.o ${OBJECTDIR}/_ext/713764681/DNS.o ${OBJECTDIR}/_ext/713764681/HeapMgr.o ${OBJECTDIR}/_ext/713764681/ICMP.o ${OBJECTDIR}/_ext/713764681/IPStack.o ${OBJECTDIR}/_ext/713764681/InternetLayer.o ${OBJECTDIR}/_ext/713764681/LinkLayer.o ${OBJECTDIR}/_ext/713764681/SNTPv4.o ${OBJECTDIR}/_ext/713764681/System.o ${OBJECTDIR}/_ext/713764681/TCP.o ${OBJECTDIR}/_ext/713764681/TCPRFC793.o ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o ${OBJECTDIR}/_ext/713764681/UDP.o ${OBJECTDIR}/_ext/1053035756/DEIPcK.o ${OBJECTDIR}/_ext/1053035756/TCPServer.o ${OBJECTDIR}/_ext/1053035756/TCPSocket.o ${OBJECTDIR}/_ext/1053035756/UDPServer.o ${OBJECTDIR}/_ext/1053035756/UDPSocket.o ${OBJECTDIR}/_ext/1053443220/DEWFcK.o ${OBJECTDIR}/_ext/1053443220/CalPSK.o ${OBJECTDIR}/_ext/619019611/ccsbcs.o ${OBJECTDIR}/_ext/619019611/fs_ff.o ${OBJECTDIR}/_ext/1053723902/DFATFS.o ${OBJECTDIR}/_ext/1053723902/fs_diskio.o ${OBJECTDIR}/_ext/1111403984/DMASerial.o ${OBJECTDIR}/_ext/1065821242/DSDVOL.o ${OBJECTDIR}/_ext/72617732/DSPI.o ${OBJECTDIR}/_ext/763762401/FLASHVOL.o ${OBJECTDIR}/_ext/763762401/flash.o ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o ${OBJECTDIR}/_ext/788046553/HTMLOptions.o ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o ${OBJECTDIR}/_ext/788046553/HTMLReboot.o ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o ${OBJECTDIR}/_ext/788046553/ProcessClient.o ${OBJECTDIR}/_ext/788046553/ProcessServer.o ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o ${OBJECTDIR}/_ext/1745786060/nm_common.o ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o ${OBJECTDIR}/_ext/1251742121/m2m_hif.o ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o ${OBJECTDIR}/_ext/1251742121/m2m_ota.o ${OBJECTDIR}/_ext/1251742121/m2m_periph.o ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o ${OBJECTDIR}/_ext/1251742121/nmasic.o ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o ${OBJECTDIR}/_ext/1251742121/nmbus.o ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o ${OBJECTDIR}/_ext/1251742121/nmdrv.o ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o ${OBJECTDIR}/_ext/1251742121/nmi2c.o ${OBJECTDIR}/_ext/1251742121/nmspi.o ${OBJECTDIR}/_ext/1251742121/nmuart.o ${OBJECTDIR}/_ext/643812449/asn1.o ${OBJECTDIR}/_ext/643812449/hash.o ${OBJECTDIR}/_ext/643812449/x509_cert.o ${OBJECTDIR}/_ext/1595687850/root_setup.o ${OBJECTDIR}/_ext/1799007404/socket.o ${OBJECTDIR}/_ext/1558412232/spi_flash.o ${OBJECTDIR}/_ext/788383207/WINC1500.o ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o ${OBJECTDIR}/_ext/788383207/WiFiStubs.o ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/315570249/Version.o.d ${OBJECTDIR}/_ext/315570249/FeedBack.o.d ${OBJECTDIR}/_ext/315570249/DDR.o.d ${OBJECTDIR}/_ext/315570249/Initialize.o.d ${OBJECTDIR}/_ext/315570249/OnReset.o.d ${OBJECTDIR}/_ext/315570249/Threads.o.d ${OBJECTDIR}/_ext/315570249/GlobalData.o.d ${OBJECTDIR}/_ext/315570249/Helper.o.d ${OBJECTDIR}/_ext/315570249/LEDs.o.d ${OBJECTDIR}/_ext/315570249/LoopStats.o.d ${OBJECTDIR}/_ext/315570249/OSSerial.o.d ${OBJECTDIR}/_ext/315570249/main.o.d ${OBJECTDIR}/_ext/315570249/OSMath.o.d ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d ${OBJECTDIR}/_ext/315570249/AWG.o.d ${OBJECTDIR}/_ext/315570249/ADC.o.d ${OBJECTDIR}/_ext/315570249/WiFi.o.d ${OBJECTDIR}/_ext/315570249/File.o.d ${OBJECTDIR}/_ext/315570249/LexJSON.o.d ${OBJECTDIR}/_ext/315570249/Parse.o.d ${OBJECTDIR}/_ext/315570249/FIFO245.o.d ${OBJECTDIR}/_ext/315570249/JsonIO.o.d ${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d ${OBJECTDIR}/_ext/315570249/DCOut.o.d ${OBJECTDIR}/_ext/315570249/ADCTargets.o.d ${OBJECTDIR}/_ext/315570249/CloudTask.o.d ${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d ${OBJECTDIR}/_ext/713764681/DHCP.o.d ${OBJECTDIR}/_ext/713764681/DNS.o.d ${OBJECTDIR}/_ext/713764681/HeapMgr.o.d ${OBJECTDIR}/_ext/713764681/ICMP.o.d ${OBJECTDIR}/_ext/713764681/IPStack.o.d ${OBJECTDIR}/_ext/713764681/InternetLayer.o.d ${OBJECTDIR}/_ext/713764681/LinkLayer.o.d ${OBJECTDIR}/_ext/713764681/SNTPv4.o.d ${OBJECTDIR}/_ext/713764681/System.o.d ${OBJECTDIR}/_ext/713764681/TCP.o.d ${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d ${OBJECTDIR}/_ext/713764681/UDP.o.d ${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d ${OBJECTDIR}/_ext/1053035756/TCPServer.o.d ${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d ${OBJECTDIR}/_ext/1053035756/UDPServer.o.d ${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d ${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d ${OBJECTDIR}/_ext/1053443220/CalPSK.o.d ${OBJECTDIR}/_ext/619019611/ccsbcs.o.d ${OBJECTDIR}/_ext/619019611/fs_ff.o.d ${OBJECTDIR}/_ext/1053723902/DFATFS.o.d ${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d ${OBJECTDIR}/_ext/1111403984/DMASerial.o.d ${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d ${OBJECTDIR}/_ext/72617732/DSPI.o.d ${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d ${OBJECTDIR}/_ext/763762401/flash.o.d ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d ${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d ${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d ${OBJECTDIR}/_ext/788046553/ProcessClient.o.d ${OBJECTDIR}/_ext/788046553/ProcessServer.o.d ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d ${OBJECTDIR}/_ext/1745786060/nm_common.o.d ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d ${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d ${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d ${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d ${OBJECTDIR}/_ext/1251742121/nmasic.o.d ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d ${OBJECTDIR}/_ext/1251742121/nmbus.o.d ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d ${OBJECTDIR}/_ext/1251742121/nmdrv.o.d ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d ${OBJECTDIR}/_ext/1251742121/nmi2c.o.d ${OBJECTDIR}/_ext/1251742121/nmspi.o.d ${OBJECTDIR}/_ext/1251742121/nmuart.o.d ${OBJECTDIR}/_ext/643812449/asn1.o.d ${OBJECTDIR}/_ext/643812449/hash.o.d ${OBJECTDIR}/_ext/643812449/x509_cert.o.d ${OBJECTDIR}/_ext/1595687850/root_setup.o.d ${OBJECTDIR}/_ext/1799007404/socket.o.d ${OBJECTDIR}/_ext/1558412232/spi_flash.o.d ${OBJECTDIR}/_ext/788383207/WINC1500.o.d ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d ${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/315570249/Version.o ${OBJECTDIR}/_ext/315570249/FeedBack.o ${OBJECTDIR}/_ext/315570249/DDR.o ${OBJECTDIR}/_ext/315570249/Initialize.o ${OBJECTDIR}/_ext/315570249/OnReset.o ${OBJECTDIR}/_ext/315570249/Threads.o ${OBJECTDIR}/_ext/315570249/GlobalData.o ${OBJECTDIR}/_ext/315570249/Helper.o ${OBJECTDIR}/_ext/315570249/LEDs.o ${OBJECTDIR}/_ext/315570249/LoopStats.o ${OBJECTDIR}/_ext/315570249/OSSerial.o ${OBJECTDIR}/_ext/315570249/main.o ${OBJECTDIR}/_ext/315570249/OSMath.o ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o ${OBJECTDIR}/_ext/315570249/AWG.o ${OBJECTDIR}/_ext/315570249/ADC.o ${OBJECTDIR}/_ext/315570249/WiFi.o ${OBJECTDIR}/_ext/315570249/File.o ${OBJECTDIR}/_ext/315570249/LexJSON.o ${OBJECTDIR}/_ext/315570249/Parse.o ${OBJECTDIR}/_ext/315570249/FIFO245.o ${OBJECTDIR}/_ext/315570249/JsonIO.o ${OBJECTDIR}/_ext/315570249/DeferredTasks.o ${OBJECTDIR}/_ext/315570249/DCOut.o ${OBJECTDIR}/_ext/315570249/ADCTargets.o ${OBJECTDIR}/_ext/315570249/CloudTask.o ${OBJECTDIR}/_ext/315570249/LogOnBoot.o ${OBJECTDIR}/_ext/713764681/DHCP.o ${OBJECTDIR}/_ext/713764681/DNS.o ${OBJECTDIR}/_ext/713764681/HeapMgr.o ${OBJECTDIR}/_ext/713764681/ICMP.o ${OBJECTDIR}/_ext/713764681/IPStack.o ${OBJECTDIR}/_ext/713764681/InternetLayer.o ${OBJECTDIR}/_ext/713764681/LinkLayer.o ${OBJECTDIR}/_ext/713764681/SNTPv4.o ${OBJECTDIR}/_ext/713764681/System.o ${OBJECTDIR}/_ext/713764681/TCP.o ${OBJECTDIR}/_ext/713764681/TCPRFC793.o ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o ${OBJECTDIR}/_ext/713764681/UDP.o ${OBJECTDIR}/_ext/1053035756/DEIPcK.o ${OBJECTDIR}/_ext/1053035756/TCPServer.o ${OBJECTDIR}/_ext/1053035756/TCPSocket.o ${OBJECTDIR}/_ext/1053035756/UDPServer.o ${OBJECTDIR}/_ext/1053035756/UDPSocket.o ${OBJECTDIR}/_ext/1053443220/DEWFcK.o ${OBJECTDIR}/_ext/1053443220/CalPSK.o ${OBJECTDIR}/_ext/619019611/ccsbcs.o ${OBJECTDIR}/_ext/619019611/fs_ff.o ${OBJECTDIR}/_ext/1053723902/DFATFS.o ${OBJECTDIR}/_ext/1053723902/fs_diskio.o ${OBJECTDIR}/_ext/1111403984/DMASerial.o ${OBJECTDIR}/_ext/1065821242/DSDVOL.o ${OBJECTDIR}/_ext/72617732/DSPI.o ${OBJECTDIR}/_ext/763762401/FLASHVOL.o ${OBJECTDIR}/_ext/763762401/flash.o ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o ${OBJECTDIR}/_ext/788046553/HTMLOptions.o ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o ${OBJECTDIR}/_ext/788046553/HTMLReboot.o ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o ${OBJECTDIR}/_ext/788046553/ProcessClient.o ${OBJECTDIR}/_ext/788046553/ProcessServer.o ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o ${OBJECTDIR}/_ext/1745786060/nm_common.o ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o ${OBJECTDIR}/_ext/1251742121/m2m_hif.o ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o ${OBJECTDIR}/_ext/1251742121/m2m_ota.o ${OBJECTDIR}/_ext/1251742121/m2m_periph.o ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o ${OBJECTDIR}/_ext/1251742121/nmasic.o ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o ${OBJECTDIR}/_ext/1251742121/nmbus.o ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o ${OBJECTDIR}/_ext/1251742121/nmdrv.o ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o ${OBJECTDIR}/_ext/1251742121/nmi2c.o ${OBJECTDIR}/_ext/1251742121/nmspi.o ${OBJECTDIR}/_ext/1251742121/nmuart.o ${OBJECTDIR}/_ext/643812449/asn1.o ${OBJECTDIR}/_ext/643812449/hash.o ${OBJECTDIR}/_ext/643812449/x509_cert.o ${OBJECTDIR}/_ext/1595687850/root_setup.o ${OBJECTDIR}/_ext/1799007404/socket.o ${OBJECTDIR}/_ext/1558412232/spi_flash.o ${OBJECTDIR}/_ext/788383207/WINC1500.o ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o ${OBJECTDIR}/_ext/788383207/WiFiStubs.o ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o

# Source Files
SOURCEFILES=../OpenLogger/Version.c ../OpenLogger/FeedBack.c ../OpenLogger/DDR.c ../OpenLogger/Initialize.c ../OpenLogger/OnReset.c ../OpenLogger/Threads.c ../OpenLogger/GlobalData.cpp ../OpenLogger/Helper.cpp ../OpenLogger/LEDs.cpp ../OpenLogger/LoopStats.cpp ../OpenLogger/OSSerial.cpp ../OpenLogger/main.cpp ../OpenLogger/OSMath.cpp ../OpenLogger/TimeOutTmr9.cpp ../OpenLogger/AWG.cpp ../OpenLogger/ADC.cpp ../OpenLogger/WiFi.cpp ../OpenLogger/File.cpp ../OpenLogger/LexJSON.cpp ../OpenLogger/Parse.cpp ../OpenLogger/FIFO245.cpp ../OpenLogger/JsonIO.cpp ../OpenLogger/DeferredTasks.cpp ../OpenLogger/DCOut.cpp ../OpenLogger/ADCTargets.cpp ../OpenLogger/CloudTask.cpp ../OpenLogger/LogOnBoot.cpp ../OpenLogger/libraries/DEIPcK/utility/DHCP.c ../OpenLogger/libraries/DEIPcK/utility/DNS.c ../OpenLogger/libraries/DEIPcK/utility/HeapMgr.c ../OpenLogger/libraries/DEIPcK/utility/ICMP.c ../OpenLogger/libraries/DEIPcK/utility/IPStack.c ../OpenLogger/libraries/DEIPcK/utility/InternetLayer.c ../OpenLogger/libraries/DEIPcK/utility/LinkLayer.c ../OpenLogger/libraries/DEIPcK/utility/SNTPv4.c ../OpenLogger/libraries/DEIPcK/utility/System.c ../OpenLogger/libraries/DEIPcK/utility/TCP.c ../OpenLogger/libraries/DEIPcK/utility/TCPRFC793.c ../OpenLogger/libraries/DEIPcK/utility/TCPStateMachine.c ../OpenLogger/libraries/DEIPcK/utility/UDP.c ../OpenLogger/libraries/DEIPcK/DEIPcK.cpp ../OpenLogger/libraries/DEIPcK/TCPServer.cpp ../OpenLogger/libraries/DEIPcK/TCPSocket.cpp ../OpenLogger/libraries/DEIPcK/UDPServer.cpp ../OpenLogger/libraries/DEIPcK/UDPSocket.cpp ../OpenLogger/libraries/DEWFcK/DEWFcK.cpp ../OpenLogger/libraries/DEWFcK/CalPSK.c ../OpenLogger/libraries/DFATFS/utility/ccsbcs.c ../OpenLogger/libraries/DFATFS/utility/fs_ff.c ../OpenLogger/libraries/DFATFS/DFATFS.cpp ../OpenLogger/libraries/DFATFS/fs_diskio.cpp ../OpenLogger/libraries/DMASerial/DMASerial.cpp ../OpenLogger/libraries/DSDVOL/DSDVOL.cpp ../OpenLogger/libraries/DSPI/DSPI.cpp ../OpenLogger/libraries/FLASHVOL/FLASHVOL.cpp ../OpenLogger/libraries/FLASHVOL/flash.c ../OpenLogger/libraries/HTTPServer/HTMLDefaultPage.cpp ../OpenLogger/libraries/HTTPServer/HTMLOptions.cpp ../OpenLogger/libraries/HTTPServer/HTMLPostCmd.cpp ../OpenLogger/libraries/HTTPServer/HTMLReboot.cpp ../OpenLogger/libraries/HTTPServer/HTMLSDPage.cpp ../OpenLogger/libraries/HTTPServer/HTTPHelpers.cpp ../OpenLogger/libraries/HTTPServer/ProcessClient.cpp ../OpenLogger/libraries/HTTPServer/ProcessServer.cpp ../OpenLogger/libraries/HTTPServer/deOpenScopeWebServer.cpp ../OpenLogger/libraries/WINC1500/utility/common/source/nm_common.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ate_mode.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_crypto.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ota.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_periph.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ssl.c ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_wifi.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv_crt.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmi2c.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmspi.c ../OpenLogger/libraries/WINC1500/utility/driver/source/nmuart.c ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/asn1.c ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/hash.c ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/x509_cert.c ../OpenLogger/libraries/WINC1500/utility/root_cert/root_setup.c ../OpenLogger/libraries/WINC1500/utility/socket/source/socket.c ../OpenLogger/libraries/WINC1500/utility/spi_flash/source/spi_flash.c ../OpenLogger/libraries/WINC1500/WINC1500.cpp ../OpenLogger/libraries/WINC1500/WINC1500Adaptor.c ../OpenLogger/libraries/WINC1500/WiFiStubs.c ../OpenLogger/libraries/WINC1500/WiFiUpdateFW.cpp


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=PIC32MZ2064DAG169
ProjectDir="C:\Digilent\GithubOpenLogger\OpenLogger\OpenLogger.X"
ConfName=OpenLogger
ImagePath="dist\OpenLogger\${IMAGE_TYPE}\OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\OpenLogger\${IMAGE_TYPE}"
ImageName="OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-OpenLogger.mk dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [if ${IsDebug} == "false" MergeHex.bat  ${ImagePath} ${OUTPUT_SUFFIX} ./OPEN_LOGGER_MZ.hex]"
	@if ${IsDebug} == "false" MergeHex.bat  ${ImagePath} ${OUTPUT_SUFFIX} ./OPEN_LOGGER_MZ.hex
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=32MZ2064DAG169
MP_LINKER_FILE_OPTION=,--script="OpenLoggerRevB.ld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/315570249/Version.o: ../OpenLogger/Version.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Version.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Version.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Version.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Version.o.d" -o ${OBJECTDIR}/_ext/315570249/Version.o ../OpenLogger/Version.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/FeedBack.o: ../OpenLogger/FeedBack.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/FeedBack.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/FeedBack.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/FeedBack.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/FeedBack.o.d" -o ${OBJECTDIR}/_ext/315570249/FeedBack.o ../OpenLogger/FeedBack.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/DDR.o: ../OpenLogger/DDR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/DDR.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/DDR.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/DDR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/DDR.o.d" -o ${OBJECTDIR}/_ext/315570249/DDR.o ../OpenLogger/DDR.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/Initialize.o: ../OpenLogger/Initialize.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Initialize.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Initialize.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Initialize.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Initialize.o.d" -o ${OBJECTDIR}/_ext/315570249/Initialize.o ../OpenLogger/Initialize.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/OnReset.o: ../OpenLogger/OnReset.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/OnReset.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/OnReset.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/OnReset.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/OnReset.o.d" -o ${OBJECTDIR}/_ext/315570249/OnReset.o ../OpenLogger/OnReset.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/Threads.o: ../OpenLogger/Threads.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Threads.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Threads.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Threads.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Threads.o.d" -o ${OBJECTDIR}/_ext/315570249/Threads.o ../OpenLogger/Threads.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/DHCP.o: ../OpenLogger/libraries/DEIPcK/utility/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/DHCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/DHCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/DHCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/DHCP.o.d" -o ${OBJECTDIR}/_ext/713764681/DHCP.o ../OpenLogger/libraries/DEIPcK/utility/DHCP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/DNS.o: ../OpenLogger/libraries/DEIPcK/utility/DNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/DNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/DNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/DNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/DNS.o.d" -o ${OBJECTDIR}/_ext/713764681/DNS.o ../OpenLogger/libraries/DEIPcK/utility/DNS.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/HeapMgr.o: ../OpenLogger/libraries/DEIPcK/utility/HeapMgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/HeapMgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/HeapMgr.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/HeapMgr.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/HeapMgr.o.d" -o ${OBJECTDIR}/_ext/713764681/HeapMgr.o ../OpenLogger/libraries/DEIPcK/utility/HeapMgr.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/ICMP.o: ../OpenLogger/libraries/DEIPcK/utility/ICMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/ICMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/ICMP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/ICMP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/ICMP.o.d" -o ${OBJECTDIR}/_ext/713764681/ICMP.o ../OpenLogger/libraries/DEIPcK/utility/ICMP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/IPStack.o: ../OpenLogger/libraries/DEIPcK/utility/IPStack.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/IPStack.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/IPStack.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/IPStack.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/IPStack.o.d" -o ${OBJECTDIR}/_ext/713764681/IPStack.o ../OpenLogger/libraries/DEIPcK/utility/IPStack.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/InternetLayer.o: ../OpenLogger/libraries/DEIPcK/utility/InternetLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/InternetLayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/InternetLayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/InternetLayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/InternetLayer.o.d" -o ${OBJECTDIR}/_ext/713764681/InternetLayer.o ../OpenLogger/libraries/DEIPcK/utility/InternetLayer.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/LinkLayer.o: ../OpenLogger/libraries/DEIPcK/utility/LinkLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/LinkLayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/LinkLayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/LinkLayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/LinkLayer.o.d" -o ${OBJECTDIR}/_ext/713764681/LinkLayer.o ../OpenLogger/libraries/DEIPcK/utility/LinkLayer.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/SNTPv4.o: ../OpenLogger/libraries/DEIPcK/utility/SNTPv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/SNTPv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/SNTPv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/SNTPv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/SNTPv4.o.d" -o ${OBJECTDIR}/_ext/713764681/SNTPv4.o ../OpenLogger/libraries/DEIPcK/utility/SNTPv4.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/System.o: ../OpenLogger/libraries/DEIPcK/utility/System.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/System.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/System.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/System.o.d" -o ${OBJECTDIR}/_ext/713764681/System.o ../OpenLogger/libraries/DEIPcK/utility/System.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/TCP.o: ../OpenLogger/libraries/DEIPcK/utility/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/TCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/TCP.o.d" -o ${OBJECTDIR}/_ext/713764681/TCP.o ../OpenLogger/libraries/DEIPcK/utility/TCP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/TCPRFC793.o: ../OpenLogger/libraries/DEIPcK/utility/TCPRFC793.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPRFC793.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d" -o ${OBJECTDIR}/_ext/713764681/TCPRFC793.o ../OpenLogger/libraries/DEIPcK/utility/TCPRFC793.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/TCPStateMachine.o: ../OpenLogger/libraries/DEIPcK/utility/TCPStateMachine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d" -o ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o ../OpenLogger/libraries/DEIPcK/utility/TCPStateMachine.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/UDP.o: ../OpenLogger/libraries/DEIPcK/utility/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/UDP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/UDP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/UDP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/UDP.o.d" -o ${OBJECTDIR}/_ext/713764681/UDP.o ../OpenLogger/libraries/DEIPcK/utility/UDP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1053443220/CalPSK.o: ../OpenLogger/libraries/DEWFcK/CalPSK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053443220" 
	@${RM} ${OBJECTDIR}/_ext/1053443220/CalPSK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053443220/CalPSK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053443220/CalPSK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053443220/CalPSK.o.d" -o ${OBJECTDIR}/_ext/1053443220/CalPSK.o ../OpenLogger/libraries/DEWFcK/CalPSK.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/619019611/ccsbcs.o: ../OpenLogger/libraries/DFATFS/utility/ccsbcs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/619019611" 
	@${RM} ${OBJECTDIR}/_ext/619019611/ccsbcs.o.d 
	@${RM} ${OBJECTDIR}/_ext/619019611/ccsbcs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/619019611/ccsbcs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/619019611/ccsbcs.o.d" -o ${OBJECTDIR}/_ext/619019611/ccsbcs.o ../OpenLogger/libraries/DFATFS/utility/ccsbcs.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/619019611/fs_ff.o: ../OpenLogger/libraries/DFATFS/utility/fs_ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/619019611" 
	@${RM} ${OBJECTDIR}/_ext/619019611/fs_ff.o.d 
	@${RM} ${OBJECTDIR}/_ext/619019611/fs_ff.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/619019611/fs_ff.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/619019611/fs_ff.o.d" -o ${OBJECTDIR}/_ext/619019611/fs_ff.o ../OpenLogger/libraries/DFATFS/utility/fs_ff.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/763762401/flash.o: ../OpenLogger/libraries/FLASHVOL/flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/763762401" 
	@${RM} ${OBJECTDIR}/_ext/763762401/flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/763762401/flash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/763762401/flash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/763762401/flash.o.d" -o ${OBJECTDIR}/_ext/763762401/flash.o ../OpenLogger/libraries/FLASHVOL/flash.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1745786060/nm_common.o: ../OpenLogger/libraries/WINC1500/utility/common/source/nm_common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1745786060" 
	@${RM} ${OBJECTDIR}/_ext/1745786060/nm_common.o.d 
	@${RM} ${OBJECTDIR}/_ext/1745786060/nm_common.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1745786060/nm_common.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1745786060/nm_common.o.d" -o ${OBJECTDIR}/_ext/1745786060/nm_common.o ../OpenLogger/libraries/WINC1500/utility/common/source/nm_common.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ate_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ate_mode.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_crypto.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_crypto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_crypto.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_hif.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_hif.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_ota.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ota.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ota.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_ota.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ota.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_periph.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_periph.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_periph.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_periph.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_periph.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_ssl.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ssl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ssl.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_wifi.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_wifi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_wifi.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmasic.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmasic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmasic.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmasic.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmasic_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmbus.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmbus.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmbus.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmbus.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmbus_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmdrv.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmdrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmdrv.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmdrv.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmi2c.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmi2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmi2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmi2c.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmi2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmi2c.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmi2c.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmi2c.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmspi.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmspi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmspi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmspi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmspi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmspi.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmspi.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmspi.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmuart.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmuart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmuart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmuart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmuart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmuart.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmuart.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmuart.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/643812449/asn1.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/asn1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/643812449" 
	@${RM} ${OBJECTDIR}/_ext/643812449/asn1.o.d 
	@${RM} ${OBJECTDIR}/_ext/643812449/asn1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/643812449/asn1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/643812449/asn1.o.d" -o ${OBJECTDIR}/_ext/643812449/asn1.o ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/asn1.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/643812449/hash.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/hash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/643812449" 
	@${RM} ${OBJECTDIR}/_ext/643812449/hash.o.d 
	@${RM} ${OBJECTDIR}/_ext/643812449/hash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/643812449/hash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/643812449/hash.o.d" -o ${OBJECTDIR}/_ext/643812449/hash.o ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/hash.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/643812449/x509_cert.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/x509_cert.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/643812449" 
	@${RM} ${OBJECTDIR}/_ext/643812449/x509_cert.o.d 
	@${RM} ${OBJECTDIR}/_ext/643812449/x509_cert.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/643812449/x509_cert.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/643812449/x509_cert.o.d" -o ${OBJECTDIR}/_ext/643812449/x509_cert.o ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/x509_cert.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1595687850/root_setup.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/root_setup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1595687850" 
	@${RM} ${OBJECTDIR}/_ext/1595687850/root_setup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1595687850/root_setup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1595687850/root_setup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1595687850/root_setup.o.d" -o ${OBJECTDIR}/_ext/1595687850/root_setup.o ../OpenLogger/libraries/WINC1500/utility/root_cert/root_setup.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1799007404/socket.o: ../OpenLogger/libraries/WINC1500/utility/socket/source/socket.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1799007404" 
	@${RM} ${OBJECTDIR}/_ext/1799007404/socket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1799007404/socket.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1799007404/socket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1799007404/socket.o.d" -o ${OBJECTDIR}/_ext/1799007404/socket.o ../OpenLogger/libraries/WINC1500/utility/socket/source/socket.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1558412232/spi_flash.o: ../OpenLogger/libraries/WINC1500/utility/spi_flash/source/spi_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1558412232" 
	@${RM} ${OBJECTDIR}/_ext/1558412232/spi_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/1558412232/spi_flash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1558412232/spi_flash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1558412232/spi_flash.o.d" -o ${OBJECTDIR}/_ext/1558412232/spi_flash.o ../OpenLogger/libraries/WINC1500/utility/spi_flash/source/spi_flash.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o: ../OpenLogger/libraries/WINC1500/WINC1500Adaptor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d" -o ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o ../OpenLogger/libraries/WINC1500/WINC1500Adaptor.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/788383207/WiFiStubs.o: ../OpenLogger/libraries/WINC1500/WiFiStubs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiStubs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d" -o ${OBJECTDIR}/_ext/788383207/WiFiStubs.o ../OpenLogger/libraries/WINC1500/WiFiStubs.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
else
${OBJECTDIR}/_ext/315570249/Version.o: ../OpenLogger/Version.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Version.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Version.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Version.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Version.o.d" -o ${OBJECTDIR}/_ext/315570249/Version.o ../OpenLogger/Version.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/FeedBack.o: ../OpenLogger/FeedBack.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/FeedBack.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/FeedBack.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/FeedBack.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/FeedBack.o.d" -o ${OBJECTDIR}/_ext/315570249/FeedBack.o ../OpenLogger/FeedBack.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/DDR.o: ../OpenLogger/DDR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/DDR.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/DDR.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/DDR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/DDR.o.d" -o ${OBJECTDIR}/_ext/315570249/DDR.o ../OpenLogger/DDR.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/Initialize.o: ../OpenLogger/Initialize.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Initialize.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Initialize.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Initialize.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Initialize.o.d" -o ${OBJECTDIR}/_ext/315570249/Initialize.o ../OpenLogger/Initialize.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/OnReset.o: ../OpenLogger/OnReset.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/OnReset.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/OnReset.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/OnReset.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/OnReset.o.d" -o ${OBJECTDIR}/_ext/315570249/OnReset.o ../OpenLogger/OnReset.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/315570249/Threads.o: ../OpenLogger/Threads.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Threads.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Threads.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Threads.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Threads.o.d" -o ${OBJECTDIR}/_ext/315570249/Threads.o ../OpenLogger/Threads.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/DHCP.o: ../OpenLogger/libraries/DEIPcK/utility/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/DHCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/DHCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/DHCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/DHCP.o.d" -o ${OBJECTDIR}/_ext/713764681/DHCP.o ../OpenLogger/libraries/DEIPcK/utility/DHCP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/DNS.o: ../OpenLogger/libraries/DEIPcK/utility/DNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/DNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/DNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/DNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/DNS.o.d" -o ${OBJECTDIR}/_ext/713764681/DNS.o ../OpenLogger/libraries/DEIPcK/utility/DNS.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/HeapMgr.o: ../OpenLogger/libraries/DEIPcK/utility/HeapMgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/HeapMgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/HeapMgr.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/HeapMgr.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/HeapMgr.o.d" -o ${OBJECTDIR}/_ext/713764681/HeapMgr.o ../OpenLogger/libraries/DEIPcK/utility/HeapMgr.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/ICMP.o: ../OpenLogger/libraries/DEIPcK/utility/ICMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/ICMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/ICMP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/ICMP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/ICMP.o.d" -o ${OBJECTDIR}/_ext/713764681/ICMP.o ../OpenLogger/libraries/DEIPcK/utility/ICMP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/IPStack.o: ../OpenLogger/libraries/DEIPcK/utility/IPStack.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/IPStack.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/IPStack.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/IPStack.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/IPStack.o.d" -o ${OBJECTDIR}/_ext/713764681/IPStack.o ../OpenLogger/libraries/DEIPcK/utility/IPStack.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/InternetLayer.o: ../OpenLogger/libraries/DEIPcK/utility/InternetLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/InternetLayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/InternetLayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/InternetLayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/InternetLayer.o.d" -o ${OBJECTDIR}/_ext/713764681/InternetLayer.o ../OpenLogger/libraries/DEIPcK/utility/InternetLayer.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/LinkLayer.o: ../OpenLogger/libraries/DEIPcK/utility/LinkLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/LinkLayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/LinkLayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/LinkLayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/LinkLayer.o.d" -o ${OBJECTDIR}/_ext/713764681/LinkLayer.o ../OpenLogger/libraries/DEIPcK/utility/LinkLayer.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/SNTPv4.o: ../OpenLogger/libraries/DEIPcK/utility/SNTPv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/SNTPv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/SNTPv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/SNTPv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/SNTPv4.o.d" -o ${OBJECTDIR}/_ext/713764681/SNTPv4.o ../OpenLogger/libraries/DEIPcK/utility/SNTPv4.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/System.o: ../OpenLogger/libraries/DEIPcK/utility/System.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/System.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/System.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/System.o.d" -o ${OBJECTDIR}/_ext/713764681/System.o ../OpenLogger/libraries/DEIPcK/utility/System.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/TCP.o: ../OpenLogger/libraries/DEIPcK/utility/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/TCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/TCP.o.d" -o ${OBJECTDIR}/_ext/713764681/TCP.o ../OpenLogger/libraries/DEIPcK/utility/TCP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/TCPRFC793.o: ../OpenLogger/libraries/DEIPcK/utility/TCPRFC793.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPRFC793.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/TCPRFC793.o.d" -o ${OBJECTDIR}/_ext/713764681/TCPRFC793.o ../OpenLogger/libraries/DEIPcK/utility/TCPRFC793.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/TCPStateMachine.o: ../OpenLogger/libraries/DEIPcK/utility/TCPStateMachine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/TCPStateMachine.o.d" -o ${OBJECTDIR}/_ext/713764681/TCPStateMachine.o ../OpenLogger/libraries/DEIPcK/utility/TCPStateMachine.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/713764681/UDP.o: ../OpenLogger/libraries/DEIPcK/utility/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713764681" 
	@${RM} ${OBJECTDIR}/_ext/713764681/UDP.o.d 
	@${RM} ${OBJECTDIR}/_ext/713764681/UDP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713764681/UDP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/713764681/UDP.o.d" -o ${OBJECTDIR}/_ext/713764681/UDP.o ../OpenLogger/libraries/DEIPcK/utility/UDP.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1053443220/CalPSK.o: ../OpenLogger/libraries/DEWFcK/CalPSK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053443220" 
	@${RM} ${OBJECTDIR}/_ext/1053443220/CalPSK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053443220/CalPSK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053443220/CalPSK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053443220/CalPSK.o.d" -o ${OBJECTDIR}/_ext/1053443220/CalPSK.o ../OpenLogger/libraries/DEWFcK/CalPSK.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/619019611/ccsbcs.o: ../OpenLogger/libraries/DFATFS/utility/ccsbcs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/619019611" 
	@${RM} ${OBJECTDIR}/_ext/619019611/ccsbcs.o.d 
	@${RM} ${OBJECTDIR}/_ext/619019611/ccsbcs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/619019611/ccsbcs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/619019611/ccsbcs.o.d" -o ${OBJECTDIR}/_ext/619019611/ccsbcs.o ../OpenLogger/libraries/DFATFS/utility/ccsbcs.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/619019611/fs_ff.o: ../OpenLogger/libraries/DFATFS/utility/fs_ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/619019611" 
	@${RM} ${OBJECTDIR}/_ext/619019611/fs_ff.o.d 
	@${RM} ${OBJECTDIR}/_ext/619019611/fs_ff.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/619019611/fs_ff.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/619019611/fs_ff.o.d" -o ${OBJECTDIR}/_ext/619019611/fs_ff.o ../OpenLogger/libraries/DFATFS/utility/fs_ff.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/763762401/flash.o: ../OpenLogger/libraries/FLASHVOL/flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/763762401" 
	@${RM} ${OBJECTDIR}/_ext/763762401/flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/763762401/flash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/763762401/flash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/763762401/flash.o.d" -o ${OBJECTDIR}/_ext/763762401/flash.o ../OpenLogger/libraries/FLASHVOL/flash.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1745786060/nm_common.o: ../OpenLogger/libraries/WINC1500/utility/common/source/nm_common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1745786060" 
	@${RM} ${OBJECTDIR}/_ext/1745786060/nm_common.o.d 
	@${RM} ${OBJECTDIR}/_ext/1745786060/nm_common.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1745786060/nm_common.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1745786060/nm_common.o.d" -o ${OBJECTDIR}/_ext/1745786060/nm_common.o ../OpenLogger/libraries/WINC1500/utility/common/source/nm_common.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ate_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_ate_mode.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ate_mode.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_crypto.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_crypto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_crypto.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_crypto.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_crypto.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_hif.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_hif.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_hif.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_hif_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_hif_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_ota.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ota.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ota.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_ota.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_ota.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ota.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_periph.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_periph.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_periph.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_periph.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_periph.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_periph.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_ssl.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ssl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_ssl.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_ssl.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_ssl.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/m2m_wifi.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_wifi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/m2m_wifi.o.d" -o ${OBJECTDIR}/_ext/1251742121/m2m_wifi.o ../OpenLogger/libraries/WINC1500/utility/driver/source/m2m_wifi.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmasic.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmasic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmasic.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmasic.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmasic_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmasic_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmasic_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmasic_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmbus.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmbus.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmbus.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmbus.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmbus_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmbus_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmbus_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmbus_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmdrv.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmdrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmdrv.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmdrv.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv_crt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmdrv_crt.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmdrv_crt.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmi2c.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmi2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmi2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmi2c.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmi2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmi2c.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmi2c.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmi2c.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmspi.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmspi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmspi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmspi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmspi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmspi.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmspi.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmspi.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1251742121/nmuart.o: ../OpenLogger/libraries/WINC1500/utility/driver/source/nmuart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1251742121" 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmuart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1251742121/nmuart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1251742121/nmuart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1251742121/nmuart.o.d" -o ${OBJECTDIR}/_ext/1251742121/nmuart.o ../OpenLogger/libraries/WINC1500/utility/driver/source/nmuart.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/643812449/asn1.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/asn1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/643812449" 
	@${RM} ${OBJECTDIR}/_ext/643812449/asn1.o.d 
	@${RM} ${OBJECTDIR}/_ext/643812449/asn1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/643812449/asn1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/643812449/asn1.o.d" -o ${OBJECTDIR}/_ext/643812449/asn1.o ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/asn1.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/643812449/hash.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/hash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/643812449" 
	@${RM} ${OBJECTDIR}/_ext/643812449/hash.o.d 
	@${RM} ${OBJECTDIR}/_ext/643812449/hash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/643812449/hash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/643812449/hash.o.d" -o ${OBJECTDIR}/_ext/643812449/hash.o ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/hash.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/643812449/x509_cert.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/x509_cert.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/643812449" 
	@${RM} ${OBJECTDIR}/_ext/643812449/x509_cert.o.d 
	@${RM} ${OBJECTDIR}/_ext/643812449/x509_cert.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/643812449/x509_cert.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/643812449/x509_cert.o.d" -o ${OBJECTDIR}/_ext/643812449/x509_cert.o ../OpenLogger/libraries/WINC1500/utility/root_cert/x509/x509_cert.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1595687850/root_setup.o: ../OpenLogger/libraries/WINC1500/utility/root_cert/root_setup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1595687850" 
	@${RM} ${OBJECTDIR}/_ext/1595687850/root_setup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1595687850/root_setup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1595687850/root_setup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1595687850/root_setup.o.d" -o ${OBJECTDIR}/_ext/1595687850/root_setup.o ../OpenLogger/libraries/WINC1500/utility/root_cert/root_setup.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1799007404/socket.o: ../OpenLogger/libraries/WINC1500/utility/socket/source/socket.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1799007404" 
	@${RM} ${OBJECTDIR}/_ext/1799007404/socket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1799007404/socket.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1799007404/socket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1799007404/socket.o.d" -o ${OBJECTDIR}/_ext/1799007404/socket.o ../OpenLogger/libraries/WINC1500/utility/socket/source/socket.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/1558412232/spi_flash.o: ../OpenLogger/libraries/WINC1500/utility/spi_flash/source/spi_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1558412232" 
	@${RM} ${OBJECTDIR}/_ext/1558412232/spi_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/1558412232/spi_flash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1558412232/spi_flash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1558412232/spi_flash.o.d" -o ${OBJECTDIR}/_ext/1558412232/spi_flash.o ../OpenLogger/libraries/WINC1500/utility/spi_flash/source/spi_flash.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o: ../OpenLogger/libraries/WINC1500/WINC1500Adaptor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o.d" -o ${OBJECTDIR}/_ext/788383207/WINC1500Adaptor.o ../OpenLogger/libraries/WINC1500/WINC1500Adaptor.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
${OBJECTDIR}/_ext/788383207/WiFiStubs.o: ../OpenLogger/libraries/WINC1500/WiFiStubs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiStubs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WiFiStubs.o.d" -o ${OBJECTDIR}/_ext/788383207/WiFiStubs.o ../OpenLogger/libraries/WINC1500/WiFiStubs.c   -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io  -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/315570249/GlobalData.o: ../OpenLogger/GlobalData.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/GlobalData.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/GlobalData.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/GlobalData.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/GlobalData.o.d" -o ${OBJECTDIR}/_ext/315570249/GlobalData.o ../OpenLogger/GlobalData.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/Helper.o: ../OpenLogger/Helper.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Helper.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Helper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Helper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Helper.o.d" -o ${OBJECTDIR}/_ext/315570249/Helper.o ../OpenLogger/Helper.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LEDs.o: ../OpenLogger/LEDs.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LEDs.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LEDs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LEDs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LEDs.o.d" -o ${OBJECTDIR}/_ext/315570249/LEDs.o ../OpenLogger/LEDs.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LoopStats.o: ../OpenLogger/LoopStats.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LoopStats.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LoopStats.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LoopStats.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LoopStats.o.d" -o ${OBJECTDIR}/_ext/315570249/LoopStats.o ../OpenLogger/LoopStats.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/OSSerial.o: ../OpenLogger/OSSerial.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSSerial.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSSerial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/OSSerial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/OSSerial.o.d" -o ${OBJECTDIR}/_ext/315570249/OSSerial.o ../OpenLogger/OSSerial.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/main.o: ../OpenLogger/main.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/main.o.d" -o ${OBJECTDIR}/_ext/315570249/main.o ../OpenLogger/main.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/OSMath.o: ../OpenLogger/OSMath.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSMath.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSMath.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/OSMath.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/OSMath.o.d" -o ${OBJECTDIR}/_ext/315570249/OSMath.o ../OpenLogger/OSMath.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o: ../OpenLogger/TimeOutTmr9.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d" -o ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o ../OpenLogger/TimeOutTmr9.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/AWG.o: ../OpenLogger/AWG.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/AWG.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/AWG.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/AWG.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/AWG.o.d" -o ${OBJECTDIR}/_ext/315570249/AWG.o ../OpenLogger/AWG.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/ADC.o: ../OpenLogger/ADC.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/ADC.o.d" -o ${OBJECTDIR}/_ext/315570249/ADC.o ../OpenLogger/ADC.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/WiFi.o: ../OpenLogger/WiFi.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/WiFi.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/WiFi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/WiFi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/WiFi.o.d" -o ${OBJECTDIR}/_ext/315570249/WiFi.o ../OpenLogger/WiFi.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/File.o: ../OpenLogger/File.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/File.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/File.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/File.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/File.o.d" -o ${OBJECTDIR}/_ext/315570249/File.o ../OpenLogger/File.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LexJSON.o: ../OpenLogger/LexJSON.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LexJSON.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LexJSON.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LexJSON.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LexJSON.o.d" -o ${OBJECTDIR}/_ext/315570249/LexJSON.o ../OpenLogger/LexJSON.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/Parse.o: ../OpenLogger/Parse.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Parse.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Parse.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Parse.o.d" -o ${OBJECTDIR}/_ext/315570249/Parse.o ../OpenLogger/Parse.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/FIFO245.o: ../OpenLogger/FIFO245.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/FIFO245.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/FIFO245.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/FIFO245.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/FIFO245.o.d" -o ${OBJECTDIR}/_ext/315570249/FIFO245.o ../OpenLogger/FIFO245.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/JsonIO.o: ../OpenLogger/JsonIO.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/JsonIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/JsonIO.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/JsonIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/JsonIO.o.d" -o ${OBJECTDIR}/_ext/315570249/JsonIO.o ../OpenLogger/JsonIO.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/DeferredTasks.o: ../OpenLogger/DeferredTasks.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/DeferredTasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d" -o ${OBJECTDIR}/_ext/315570249/DeferredTasks.o ../OpenLogger/DeferredTasks.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/DCOut.o: ../OpenLogger/DCOut.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/DCOut.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/DCOut.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/DCOut.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/DCOut.o.d" -o ${OBJECTDIR}/_ext/315570249/DCOut.o ../OpenLogger/DCOut.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/ADCTargets.o: ../OpenLogger/ADCTargets.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADCTargets.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADCTargets.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/ADCTargets.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/ADCTargets.o.d" -o ${OBJECTDIR}/_ext/315570249/ADCTargets.o ../OpenLogger/ADCTargets.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/CloudTask.o: ../OpenLogger/CloudTask.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/CloudTask.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/CloudTask.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/CloudTask.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/CloudTask.o.d" -o ${OBJECTDIR}/_ext/315570249/CloudTask.o ../OpenLogger/CloudTask.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LogOnBoot.o: ../OpenLogger/LogOnBoot.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LogOnBoot.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d" -o ${OBJECTDIR}/_ext/315570249/LogOnBoot.o ../OpenLogger/LogOnBoot.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/DEIPcK.o: ../OpenLogger/libraries/DEIPcK/DEIPcK.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/DEIPcK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d" -o ${OBJECTDIR}/_ext/1053035756/DEIPcK.o ../OpenLogger/libraries/DEIPcK/DEIPcK.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/TCPServer.o: ../OpenLogger/libraries/DEIPcK/TCPServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/TCPServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/TCPServer.o.d" -o ${OBJECTDIR}/_ext/1053035756/TCPServer.o ../OpenLogger/libraries/DEIPcK/TCPServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/TCPSocket.o: ../OpenLogger/libraries/DEIPcK/TCPSocket.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPSocket.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d" -o ${OBJECTDIR}/_ext/1053035756/TCPSocket.o ../OpenLogger/libraries/DEIPcK/TCPSocket.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/UDPServer.o: ../OpenLogger/libraries/DEIPcK/UDPServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/UDPServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/UDPServer.o.d" -o ${OBJECTDIR}/_ext/1053035756/UDPServer.o ../OpenLogger/libraries/DEIPcK/UDPServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/UDPSocket.o: ../OpenLogger/libraries/DEIPcK/UDPSocket.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPSocket.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d" -o ${OBJECTDIR}/_ext/1053035756/UDPSocket.o ../OpenLogger/libraries/DEIPcK/UDPSocket.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053443220/DEWFcK.o: ../OpenLogger/libraries/DEWFcK/DEWFcK.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053443220" 
	@${RM} ${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053443220/DEWFcK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d" -o ${OBJECTDIR}/_ext/1053443220/DEWFcK.o ../OpenLogger/libraries/DEWFcK/DEWFcK.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053723902/DFATFS.o: ../OpenLogger/libraries/DFATFS/DFATFS.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053723902" 
	@${RM} ${OBJECTDIR}/_ext/1053723902/DFATFS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053723902/DFATFS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053723902/DFATFS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053723902/DFATFS.o.d" -o ${OBJECTDIR}/_ext/1053723902/DFATFS.o ../OpenLogger/libraries/DFATFS/DFATFS.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053723902/fs_diskio.o: ../OpenLogger/libraries/DFATFS/fs_diskio.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053723902" 
	@${RM} ${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053723902/fs_diskio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d" -o ${OBJECTDIR}/_ext/1053723902/fs_diskio.o ../OpenLogger/libraries/DFATFS/fs_diskio.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1111403984/DMASerial.o: ../OpenLogger/libraries/DMASerial/DMASerial.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1111403984" 
	@${RM} ${OBJECTDIR}/_ext/1111403984/DMASerial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1111403984/DMASerial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1111403984/DMASerial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1111403984/DMASerial.o.d" -o ${OBJECTDIR}/_ext/1111403984/DMASerial.o ../OpenLogger/libraries/DMASerial/DMASerial.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1065821242/DSDVOL.o: ../OpenLogger/libraries/DSDVOL/DSDVOL.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1065821242" 
	@${RM} ${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d 
	@${RM} ${OBJECTDIR}/_ext/1065821242/DSDVOL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d" -o ${OBJECTDIR}/_ext/1065821242/DSDVOL.o ../OpenLogger/libraries/DSDVOL/DSDVOL.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/72617732/DSPI.o: ../OpenLogger/libraries/DSPI/DSPI.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/72617732" 
	@${RM} ${OBJECTDIR}/_ext/72617732/DSPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/72617732/DSPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/72617732/DSPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/72617732/DSPI.o.d" -o ${OBJECTDIR}/_ext/72617732/DSPI.o ../OpenLogger/libraries/DSPI/DSPI.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/763762401/FLASHVOL.o: ../OpenLogger/libraries/FLASHVOL/FLASHVOL.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/763762401" 
	@${RM} ${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d 
	@${RM} ${OBJECTDIR}/_ext/763762401/FLASHVOL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d" -o ${OBJECTDIR}/_ext/763762401/FLASHVOL.o ../OpenLogger/libraries/FLASHVOL/FLASHVOL.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o: ../OpenLogger/libraries/HTTPServer/HTMLDefaultPage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o ../OpenLogger/libraries/HTTPServer/HTMLDefaultPage.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLOptions.o: ../OpenLogger/libraries/HTTPServer/HTMLOptions.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLOptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLOptions.o ../OpenLogger/libraries/HTTPServer/HTMLOptions.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o: ../OpenLogger/libraries/HTTPServer/HTMLPostCmd.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o ../OpenLogger/libraries/HTTPServer/HTMLPostCmd.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLReboot.o: ../OpenLogger/libraries/HTTPServer/HTMLReboot.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLReboot.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLReboot.o ../OpenLogger/libraries/HTTPServer/HTMLReboot.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLSDPage.o: ../OpenLogger/libraries/HTTPServer/HTMLSDPage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o ../OpenLogger/libraries/HTTPServer/HTMLSDPage.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTTPHelpers.o: ../OpenLogger/libraries/HTTPServer/HTTPHelpers.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d" -o ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o ../OpenLogger/libraries/HTTPServer/HTTPHelpers.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/ProcessClient.o: ../OpenLogger/libraries/HTTPServer/ProcessClient.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessClient.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessClient.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/ProcessClient.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/ProcessClient.o.d" -o ${OBJECTDIR}/_ext/788046553/ProcessClient.o ../OpenLogger/libraries/HTTPServer/ProcessClient.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/ProcessServer.o: ../OpenLogger/libraries/HTTPServer/ProcessServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/ProcessServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/ProcessServer.o.d" -o ${OBJECTDIR}/_ext/788046553/ProcessServer.o ../OpenLogger/libraries/HTTPServer/ProcessServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o: ../OpenLogger/libraries/HTTPServer/deOpenScopeWebServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d" -o ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o ../OpenLogger/libraries/HTTPServer/deOpenScopeWebServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788383207/WINC1500.o: ../OpenLogger/libraries/WINC1500/WINC1500.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WINC1500.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WINC1500.o.d" -o ${OBJECTDIR}/_ext/788383207/WINC1500.o ../OpenLogger/libraries/WINC1500/WINC1500.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o: ../OpenLogger/libraries/WINC1500/WiFiUpdateFW.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d" -o ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o ../OpenLogger/libraries/WINC1500/WiFiUpdateFW.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
else
${OBJECTDIR}/_ext/315570249/GlobalData.o: ../OpenLogger/GlobalData.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/GlobalData.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/GlobalData.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/GlobalData.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/GlobalData.o.d" -o ${OBJECTDIR}/_ext/315570249/GlobalData.o ../OpenLogger/GlobalData.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/Helper.o: ../OpenLogger/Helper.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Helper.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Helper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Helper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Helper.o.d" -o ${OBJECTDIR}/_ext/315570249/Helper.o ../OpenLogger/Helper.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LEDs.o: ../OpenLogger/LEDs.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LEDs.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LEDs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LEDs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LEDs.o.d" -o ${OBJECTDIR}/_ext/315570249/LEDs.o ../OpenLogger/LEDs.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LoopStats.o: ../OpenLogger/LoopStats.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LoopStats.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LoopStats.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LoopStats.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LoopStats.o.d" -o ${OBJECTDIR}/_ext/315570249/LoopStats.o ../OpenLogger/LoopStats.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/OSSerial.o: ../OpenLogger/OSSerial.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSSerial.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSSerial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/OSSerial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/OSSerial.o.d" -o ${OBJECTDIR}/_ext/315570249/OSSerial.o ../OpenLogger/OSSerial.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/main.o: ../OpenLogger/main.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/main.o.d" -o ${OBJECTDIR}/_ext/315570249/main.o ../OpenLogger/main.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/OSMath.o: ../OpenLogger/OSMath.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSMath.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/OSMath.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/OSMath.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/OSMath.o.d" -o ${OBJECTDIR}/_ext/315570249/OSMath.o ../OpenLogger/OSMath.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o: ../OpenLogger/TimeOutTmr9.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o.d" -o ${OBJECTDIR}/_ext/315570249/TimeOutTmr9.o ../OpenLogger/TimeOutTmr9.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/AWG.o: ../OpenLogger/AWG.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/AWG.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/AWG.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/AWG.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/AWG.o.d" -o ${OBJECTDIR}/_ext/315570249/AWG.o ../OpenLogger/AWG.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/ADC.o: ../OpenLogger/ADC.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/ADC.o.d" -o ${OBJECTDIR}/_ext/315570249/ADC.o ../OpenLogger/ADC.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/WiFi.o: ../OpenLogger/WiFi.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/WiFi.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/WiFi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/WiFi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/WiFi.o.d" -o ${OBJECTDIR}/_ext/315570249/WiFi.o ../OpenLogger/WiFi.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/File.o: ../OpenLogger/File.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/File.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/File.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/File.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/File.o.d" -o ${OBJECTDIR}/_ext/315570249/File.o ../OpenLogger/File.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LexJSON.o: ../OpenLogger/LexJSON.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LexJSON.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LexJSON.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LexJSON.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LexJSON.o.d" -o ${OBJECTDIR}/_ext/315570249/LexJSON.o ../OpenLogger/LexJSON.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/Parse.o: ../OpenLogger/Parse.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/Parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/Parse.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/Parse.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/Parse.o.d" -o ${OBJECTDIR}/_ext/315570249/Parse.o ../OpenLogger/Parse.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/FIFO245.o: ../OpenLogger/FIFO245.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/FIFO245.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/FIFO245.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/FIFO245.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/FIFO245.o.d" -o ${OBJECTDIR}/_ext/315570249/FIFO245.o ../OpenLogger/FIFO245.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/JsonIO.o: ../OpenLogger/JsonIO.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/JsonIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/JsonIO.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/JsonIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/JsonIO.o.d" -o ${OBJECTDIR}/_ext/315570249/JsonIO.o ../OpenLogger/JsonIO.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/DeferredTasks.o: ../OpenLogger/DeferredTasks.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/DeferredTasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/DeferredTasks.o.d" -o ${OBJECTDIR}/_ext/315570249/DeferredTasks.o ../OpenLogger/DeferredTasks.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/DCOut.o: ../OpenLogger/DCOut.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/DCOut.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/DCOut.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/DCOut.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/DCOut.o.d" -o ${OBJECTDIR}/_ext/315570249/DCOut.o ../OpenLogger/DCOut.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/ADCTargets.o: ../OpenLogger/ADCTargets.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADCTargets.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/ADCTargets.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/ADCTargets.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/ADCTargets.o.d" -o ${OBJECTDIR}/_ext/315570249/ADCTargets.o ../OpenLogger/ADCTargets.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/CloudTask.o: ../OpenLogger/CloudTask.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/CloudTask.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/CloudTask.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/CloudTask.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/CloudTask.o.d" -o ${OBJECTDIR}/_ext/315570249/CloudTask.o ../OpenLogger/CloudTask.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/315570249/LogOnBoot.o: ../OpenLogger/LogOnBoot.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/315570249" 
	@${RM} ${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d 
	@${RM} ${OBJECTDIR}/_ext/315570249/LogOnBoot.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/315570249/LogOnBoot.o.d" -o ${OBJECTDIR}/_ext/315570249/LogOnBoot.o ../OpenLogger/LogOnBoot.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/DEIPcK.o: ../OpenLogger/libraries/DEIPcK/DEIPcK.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/DEIPcK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/DEIPcK.o.d" -o ${OBJECTDIR}/_ext/1053035756/DEIPcK.o ../OpenLogger/libraries/DEIPcK/DEIPcK.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/TCPServer.o: ../OpenLogger/libraries/DEIPcK/TCPServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/TCPServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/TCPServer.o.d" -o ${OBJECTDIR}/_ext/1053035756/TCPServer.o ../OpenLogger/libraries/DEIPcK/TCPServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/TCPSocket.o: ../OpenLogger/libraries/DEIPcK/TCPSocket.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/TCPSocket.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/TCPSocket.o.d" -o ${OBJECTDIR}/_ext/1053035756/TCPSocket.o ../OpenLogger/libraries/DEIPcK/TCPSocket.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/UDPServer.o: ../OpenLogger/libraries/DEIPcK/UDPServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/UDPServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/UDPServer.o.d" -o ${OBJECTDIR}/_ext/1053035756/UDPServer.o ../OpenLogger/libraries/DEIPcK/UDPServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053035756/UDPSocket.o: ../OpenLogger/libraries/DEIPcK/UDPSocket.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053035756" 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053035756/UDPSocket.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053035756/UDPSocket.o.d" -o ${OBJECTDIR}/_ext/1053035756/UDPSocket.o ../OpenLogger/libraries/DEIPcK/UDPSocket.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053443220/DEWFcK.o: ../OpenLogger/libraries/DEWFcK/DEWFcK.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053443220" 
	@${RM} ${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053443220/DEWFcK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053443220/DEWFcK.o.d" -o ${OBJECTDIR}/_ext/1053443220/DEWFcK.o ../OpenLogger/libraries/DEWFcK/DEWFcK.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053723902/DFATFS.o: ../OpenLogger/libraries/DFATFS/DFATFS.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053723902" 
	@${RM} ${OBJECTDIR}/_ext/1053723902/DFATFS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053723902/DFATFS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053723902/DFATFS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053723902/DFATFS.o.d" -o ${OBJECTDIR}/_ext/1053723902/DFATFS.o ../OpenLogger/libraries/DFATFS/DFATFS.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1053723902/fs_diskio.o: ../OpenLogger/libraries/DFATFS/fs_diskio.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1053723902" 
	@${RM} ${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1053723902/fs_diskio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1053723902/fs_diskio.o.d" -o ${OBJECTDIR}/_ext/1053723902/fs_diskio.o ../OpenLogger/libraries/DFATFS/fs_diskio.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1111403984/DMASerial.o: ../OpenLogger/libraries/DMASerial/DMASerial.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1111403984" 
	@${RM} ${OBJECTDIR}/_ext/1111403984/DMASerial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1111403984/DMASerial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1111403984/DMASerial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1111403984/DMASerial.o.d" -o ${OBJECTDIR}/_ext/1111403984/DMASerial.o ../OpenLogger/libraries/DMASerial/DMASerial.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/1065821242/DSDVOL.o: ../OpenLogger/libraries/DSDVOL/DSDVOL.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1065821242" 
	@${RM} ${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d 
	@${RM} ${OBJECTDIR}/_ext/1065821242/DSDVOL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/1065821242/DSDVOL.o.d" -o ${OBJECTDIR}/_ext/1065821242/DSDVOL.o ../OpenLogger/libraries/DSDVOL/DSDVOL.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/72617732/DSPI.o: ../OpenLogger/libraries/DSPI/DSPI.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/72617732" 
	@${RM} ${OBJECTDIR}/_ext/72617732/DSPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/72617732/DSPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/72617732/DSPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/72617732/DSPI.o.d" -o ${OBJECTDIR}/_ext/72617732/DSPI.o ../OpenLogger/libraries/DSPI/DSPI.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/763762401/FLASHVOL.o: ../OpenLogger/libraries/FLASHVOL/FLASHVOL.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/763762401" 
	@${RM} ${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d 
	@${RM} ${OBJECTDIR}/_ext/763762401/FLASHVOL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/763762401/FLASHVOL.o.d" -o ${OBJECTDIR}/_ext/763762401/FLASHVOL.o ../OpenLogger/libraries/FLASHVOL/FLASHVOL.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o: ../OpenLogger/libraries/HTTPServer/HTMLDefaultPage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLDefaultPage.o ../OpenLogger/libraries/HTTPServer/HTMLDefaultPage.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLOptions.o: ../OpenLogger/libraries/HTTPServer/HTMLOptions.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLOptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLOptions.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLOptions.o ../OpenLogger/libraries/HTTPServer/HTMLOptions.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o: ../OpenLogger/libraries/HTTPServer/HTMLPostCmd.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLPostCmd.o ../OpenLogger/libraries/HTTPServer/HTMLPostCmd.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLReboot.o: ../OpenLogger/libraries/HTTPServer/HTMLReboot.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLReboot.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLReboot.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLReboot.o ../OpenLogger/libraries/HTTPServer/HTMLReboot.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTMLSDPage.o: ../OpenLogger/libraries/HTTPServer/HTMLSDPage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTMLSDPage.o.d" -o ${OBJECTDIR}/_ext/788046553/HTMLSDPage.o ../OpenLogger/libraries/HTTPServer/HTMLSDPage.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/HTTPHelpers.o: ../OpenLogger/libraries/HTTPServer/HTTPHelpers.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/HTTPHelpers.o.d" -o ${OBJECTDIR}/_ext/788046553/HTTPHelpers.o ../OpenLogger/libraries/HTTPServer/HTTPHelpers.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/ProcessClient.o: ../OpenLogger/libraries/HTTPServer/ProcessClient.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessClient.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessClient.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/ProcessClient.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/ProcessClient.o.d" -o ${OBJECTDIR}/_ext/788046553/ProcessClient.o ../OpenLogger/libraries/HTTPServer/ProcessClient.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/ProcessServer.o: ../OpenLogger/libraries/HTTPServer/ProcessServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/ProcessServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/ProcessServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/ProcessServer.o.d" -o ${OBJECTDIR}/_ext/788046553/ProcessServer.o ../OpenLogger/libraries/HTTPServer/ProcessServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o: ../OpenLogger/libraries/HTTPServer/deOpenScopeWebServer.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788046553" 
	@${RM} ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d 
	@${RM} ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o.d" -o ${OBJECTDIR}/_ext/788046553/deOpenScopeWebServer.o ../OpenLogger/libraries/HTTPServer/deOpenScopeWebServer.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788383207/WINC1500.o: ../OpenLogger/libraries/WINC1500/WINC1500.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WINC1500.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WINC1500.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WINC1500.o.d" -o ${OBJECTDIR}/_ext/788383207/WINC1500.o ../OpenLogger/libraries/WINC1500/WINC1500.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o: ../OpenLogger/libraries/WINC1500/WiFiUpdateFW.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/788383207" 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d 
	@${RM} ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fno-exceptions -fno-check-new -fno-enforce-eh-specs -DF_CPU=200000000UL -DMPIDE=150 -I"../OpenLogger" -Wall -MMD -MF "${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o.d" -o ${OBJECTDIR}/_ext/788383207/WiFiUpdateFW.o ../OpenLogger/libraries/WINC1500/WiFiUpdateFW.cpp  -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -c  -mno-smart-io -ffunction-sections  -fdata-sections  -g3  -mdebugger  -Wcast-align  -fno-short-double  -fframe-base-loclist -mnewlib-libc -std=gnu++11
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    OpenLoggerRevB.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -g -mdebugger -D__MPLAB_DEBUGGER_ICD3=1 -mprocessor=$(MP_PROCESSOR_OPTION) -mnewlib-libc  -o dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)   -mreserve=data@0x0:0x27F   -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=__MPLAB_DEBUGGER_ICD3=1,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml,--gc-sections
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   OpenLoggerRevB.ld OPEN_LOGGER_MZ.hex
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -mnewlib-libc  -o dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -save-temps=obj -DXPRJ_OpenLogger=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml,--gc-sections
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
	@echo "Creating unified hex file"
	@"C:/Program Files (x86)/Microchip/MPLABX/v5.20/mplab_platform/platform/../mplab_ide/modules/../../bin/hexmate" --edf="C:/Program Files (x86)/Microchip/MPLABX/v5.20/mplab_platform/platform/../mplab_ide/modules/../../dat/en_msgs.txt" dist/${CND_CONF}/${IMAGE_TYPE}/OpenLogger.X.${IMAGE_TYPE}.hex OPEN_LOGGER_MZ.hex -odist/${CND_CONF}/production/OpenLogger.X.production.unified.hex

endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/OpenLogger
	${RM} -r dist/OpenLogger
