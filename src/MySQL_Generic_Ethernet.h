/*********************************************************************************************************************************
  MySQL_Generic_Ethernet.h
      
  Library for communicating with a MySQL or MariaDB Server
  
  Based on and modified from Dr. Charles A. Bell's MySQL_Connector_Arduino Library https://github.com/ChuckBell/MySQL_Connector_Arduino
  to support nRF52, SAMD21/SAMD51, SAM DUE, STM32F/L/H/G/WB/MP1, ESP8266, ESP32, etc. boards using W5x00, ENC28J60, LAM8742A Ethernet,
  WiFiNINA, ESP-AT, built-in ESP8266/ESP32 WiFi.

  The library provides simple and easy Client interface to MySQL or MariaDB Server.
  
  Built by Khoi Hoang https://github.com/khoih-prog/MySQL_MariaDB_Generic
  Licensed under MIT license
  Version: 1.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      13/08/2020 Initial coding/porting to support nRF52, SAM DUE and SAMD21/SAMD51 boards using W5x00 Ethernet
                                  (using Ethernet, EthernetLarge, Ethernet2, Ethernet3 library) and WiFiNINA
 **********************************************************************************************************************************/

#ifndef MYSQL_GENERIC_ETHERNET_H
#define MYSQL_GENERIC_ETHERNET_H

#include <SPI.h>

#if USE_ETHERNET
  #include "Ethernet.h"
  EthernetClient client;
  #warning Use Ethernet lib
#elif USE_ETHERNET_LARGE
  #include "EthernetLarge.h"
  EthernetClient client;
  #warning Use EthernetLarge lib
#elif USE_ETHERNET2
  #include "Ethernet2.h"
  EthernetClient client;
  #warning Use Ethernet2 lib
#elif USE_ETHERNET3
  #include "Ethernet3.h"
  EthernetClient client;
  #warning Use Ethernet3 lib  
#elif USE_ETHERNET_ESP8266
  #include "Ethernet_ESP8266.h"
  EthernetClient client;
  #warning Use Ethernet_ESP8266 lib
#elif USE_ETHERNET_LAN8742A
  #include <LwIP.h>
  #include <STM32Ethernet.h>
  EthernetClient client;
  #warning Use Ethernet_LAN8742A lib
#elif USE_CUSTOM_ETHERNET
  #include "Ethernet_XYZ.h"
  EthernetClient client;
  #warning Use Custom Ethernet library from EthernetWrapper. You must include a library here or error.
#else
  // Backup if none is selected
  #define USE_ETHERNET          true
  #include "Ethernet.h"
  EthernetClient client;
  #warning Use Ethernet lib
#endif

#include <MySQL_Generic_Connection.h>
#include <MySQL_Generic_Query.h>
 
#endif    //MYSQL_GENERIC_ETHERNET_H