/*********************************************************************************************************************************
  Basic_Select.ino
      
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

/*
  MySQL Connector/Arduino Example : basic select

  This example demonstrates how to issue a SELECT query with no parameters
  and use the data returned. For this, we use the Cursor class to execute
  the query and get the results.

  It demonstrates who methods for running queries. The first allows you to
  allocate memory for the cursor and later reclaim it, the second shows how
  to use a single instance of the cursor use throughout a sketch.

  NOTICE: You must download and install the World sample database to run
          this sketch unaltered. See http://dev.mysql.com/doc/index-other.html.

  CAUTION: Don't mix and match the examples. Use one or the other in your
           own sketch -- you'll get compilation errors at the least.

  For more information and documentation, visit the wiki:
  https://github.com/ChuckBell/MySQL_Connector_Arduino/wiki.

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Connect a USB cable to your Arduino
  4) Select the correct board and port
  5) Compile and upload the sketch to your Arduino
  6) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  Note: The MAC address can be anything so long as it is unique on your network.

  Created by: Dr. Charles A. Bell
*/

#include "defines.h"

#include <MySQL_Generic_Ethernet.h>

// Select the static Local IP address according to your local network
IPAddress ip(192, 168, 2, 222);

IPAddress server_addr(192, 168, 2, 112);
uint16_t server_port = 5698;    //3306;

char user[]     = "invited-guest";              // MySQL user login username
char password[] = "the-invited-guest";          // MySQL user login password

char default_database[] = "world";              //"test_arduino";
char default_table[]    = "city";        //"test_arduino";

String default_column   = "population"; 
String default_value    = "New York"; 

// Sample query
//char query[] = "SELECT population FROM world.city WHERE name = 'New York'";

//String query = "SELECT population FROM world.city WHERE name = 'New York'";

String query = String("SELECT ") + default_column + " FROM " + default_database + "." + default_table 
                 + " WHERE name = '" + default_value + "'";

MySQL_Connection conn((Client *)&client);
// Create an instance of the cursor passing in the connection
MySQL_Query sql_query = MySQL_Query(&conn);

void setup()
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect

  Serial.print("\nStarting Basic_Select on " + String(BOARD_NAME));

#if USE_ETHERNET
  Serial.println(" using W5x00/Ethernet Library");
#elif USE_ETHERNET_LARGE
  Serial.println(" using W5x00/EthernetLarge Library");
#elif USE_ETHERNET2
  Serial.println(" using W5x00/Ethernet2 Library");
#elif USE_ETHERNET3
  Serial.println(" using W5x00/Ethernet3 Library");
#elif USE_ETHERNET_LAN8742A
  Serial.println(" using LAN8742A/STM32Ethernet Library");
#elif USE_ETHERNET_ESP8266
  Serial.println(" using W5x00/Ethernet_ESP8266 Library");
#elif USE_CUSTOM_ETHERNET
  Serial.println(" using W5x00/Ethernet Custom Library");
#else
  // Backup if none is selected
  Serial.println(" using W5x00/Ethernet Library");
#endif

  MYSQL_LOGWARN(F("========================="));
  MYSQL_LOGWARN(F("Default SPI pinout:"));
  MYSQL_LOGWARN1(F("MOSI:"), MOSI);
  MYSQL_LOGWARN1(F("MISO:"), MISO);
  MYSQL_LOGWARN1(F("SCK:"),  SCK);
  MYSQL_LOGWARN1(F("SS:"),   SS);
  MYSQL_LOGWARN(F("========================="));

#if defined(ESP8266)
  // For ESP8266, change for other boards if necessary
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   D2    // For ESP8266
  #endif
  
    MYSQL_LOGWARN1(F("ESP8266 setCsPin:"), USE_THIS_SS_PIN);
  
  #if ( USE_ETHERNET || USE_ETHERNET_LARGE || USE_ETHERNET2 )
    // For ESP8266
    // Pin                D0(GPIO16)    D1(GPIO5)    D2(GPIO4)    D3(GPIO0)    D4(GPIO2)    D8
    // Ethernet           0                 X            X            X            X        0
    // Ethernet2          X                 X            X            X            X        0
    // Ethernet3          X                 X            X            X            X        0
    // EthernetLarge      X                 X            X            X            X        0
    // Ethernet_ESP8266   0                 0            0            0            0        0
    // D2 is safe to used for Ethernet, Ethernet2, Ethernet3, EthernetLarge libs
    // Must use library patch for Ethernet, EthernetLarge libraries
    Ethernet.init (USE_THIS_SS_PIN);
  
  #elif USE_ETHERNET3
    // Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
    #ifndef ETHERNET3_MAX_SOCK_NUM
      #define ETHERNET3_MAX_SOCK_NUM      4
    #endif
  
    Ethernet.setCsPin (USE_THIS_SS_PIN);
    Ethernet.init (ETHERNET3_MAX_SOCK_NUM);
  
  #endif  //( USE_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE )

#elif defined(ESP32)

  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   22    // For ESP32
  #endif

  MYSQL_LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);

  // For other boards, to change if necessary
  #if ( USE_ETHERNET || USE_ETHERNET_LARGE || USE_ETHERNET2 )
    // Must use library patch for Ethernet, EthernetLarge libraries
    // ESP32 => GPIO2,4,5,13,15,21,22 OK with Ethernet, Ethernet2, EthernetLarge
    // ESP32 => GPIO2,4,5,15,21,22 OK with Ethernet3
  
    //Ethernet.setCsPin (USE_THIS_SS_PIN);
    Ethernet.init (USE_THIS_SS_PIN);

  #elif USE_ETHERNET3
    // Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
    #ifndef ETHERNET3_MAX_SOCK_NUM
      #define ETHERNET3_MAX_SOCK_NUM      4
    #endif
  
    Ethernet.setCsPin (USE_THIS_SS_PIN);
    Ethernet.init (ETHERNET3_MAX_SOCK_NUM);

  #endif  //( USE_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE )

#else   //defined(ESP8266)
  // unknown board, do nothing, use default SS = 10
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   10    // For other boards
  #endif

  MYSQL_LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);

  // For other boards, to change if necessary
  #if ( USE_ETHERNET || USE_ETHERNET_LARGE || USE_ETHERNET2 )
    // Must use library patch for Ethernet, Ethernet2, EthernetLarge libraries
  
    Ethernet.init (USE_THIS_SS_PIN);

  #elif USE_ETHERNET3
    // Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
    #ifndef ETHERNET3_MAX_SOCK_NUM
      #define ETHERNET3_MAX_SOCK_NUM      4
    #endif
  
    Ethernet.setCsPin (USE_THIS_SS_PIN);
    Ethernet.init (ETHERNET3_MAX_SOCK_NUM);
  
  #endif  //( USE_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE )

#endif    //defined(ESP8266)

  // start the ethernet connection and the server:
  // Use Static IP
  //Ethernet.begin(mac, ip);
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  Ethernet.begin(mac[index]);

  // Just info to know how to connect correctly
  MYSQL_LOGWARN(F("========================="));
  MYSQL_LOGWARN(F("Currently Used SPI pinout:"));
  MYSQL_LOGWARN1(F("MOSI:"), MOSI);
  MYSQL_LOGWARN1(F("MISO:"), MISO);
  MYSQL_LOGWARN1(F("SCK:"),  SCK);
  MYSQL_LOGWARN1(F("SS:"),   SS);
  MYSQL_LOGWARN(F("========================="));

  Serial.print("Using mac index = ");
  Serial.println(index);

  Serial.print("Connected! IP address: ");
  Serial.println(Ethernet.localIP());
  
  Serial.print("Connecting to SQL Server @ ");
  Serial.print(server_addr);
  Serial.println(String(", Port = ") + server_port);
  Serial.println(String("User = ") + user + String(", PW = ") + password + String(", DB = ") + default_database);
  
  if (conn.connect(server_addr, server_port, user, password)) 
  {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}

void loop() 
{
  row_values *row = NULL;
  long head_count = 0;

  Serial.println("1) Demonstrating using a cursor dynamically allocated.");
  // Initiate the query class instance
  MySQL_Query *query_mem = new MySQL_Query(&conn);
  
  // Execute the query
  Serial.println(query);
  query_mem->execute(query.c_str());
  
  // Fetch the columns (required) but we don't use them.
  column_names *columns = query_mem->get_columns();

  // Read the row (we are only expecting the one)
  do 
  {
    row = query_mem->get_next_row();
    
    if (row != NULL) 
    {
      head_count = atol(row->values[0]);
    }
  } while (row != NULL);
  
  // Deleting the cursor also frees up memory used
  delete query_mem;

  // Show the result
  Serial.print("  NYC pop = ");
  Serial.println(head_count);

  delay(500);

  Serial.println("2) Demonstrating using a local, global cursor.");
  
  // Execute the query
  Serial.println(query);
  sql_query.execute(query.c_str());
  
  // Fetch the columns (required) but we don't use them.
  sql_query.get_columns();
  
  // Read the row (we are only expecting the one)
  do 
  {
    row = sql_query.get_next_row();
    if (row != NULL) 
    {
      head_count = atol(row->values[0]);
    }
  } while (row != NULL);
  
  // Now we close the cursor to free any memory
  sql_query.close();

  // Show the result but this time do some math on it
  Serial.print("  NYC pop = ");
  Serial.println(head_count);
  Serial.print("  NYC pop increased by 12 = ");
  Serial.println(head_count + 12);

  delay(60000);
}