/*********************************************************************************************************************************
  Complex_Select_WiFiNINA.ino

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
  MySQL Connector/Arduino Example : complex select

  This example demonstrates how to issue a SELECT query with parameters that
  we provide from code. Thus, it demonstrates how to build query parameters
  dynamically.

  Notice also the sketch demonstrates how to read columns and rows from
  the result set. Study this example until you are familiar with how to
  do this before writing your own sketch to read and consume query results.

  For more information and documentation, visit the wiki:
  https://github.com/ChuckBell/MySQL_Connector_Arduino/wiki.

  NOTICE: You must download and install the World sample database to run
          this sketch unaltered. See http://dev.mysql.com/doc/index-other.html.

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
#include "Credentials.h"

#include <MySQL_Generic_WiFiNINA.h>
IPAddress server_addr(192, 168, 2, 112);
uint16_t server_port = 5698;    //3306;

char default_database[] = "world";              //"test_arduino";
char default_table[]    = "city";               //"test_arduino";

// Sample query
//
// Notice the "%lu" - that's a placeholder for the parameter we will
// supply. See sprintf() documentation for more formatting specifier
// options
const char QUERY_POP[] = "SELECT name, population FROM world.city WHERE population > %lu ORDER BY population DESC;";

char query[128];

MySQL_Connection conn((Client *)&client);

int status = WL_IDLE_STATUS;

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect

  Serial.println("\nStarting Complex_Select_WiFiNINA on " + String(BOARD_NAME));

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    //delay(10000);
  }

  printWifiStatus();

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
  Serial.println("====================================================");
  Serial.println("> Running SELECT with dynamically supplied parameter");

  // Initiate the query class instance
  MySQL_Query *query_mem = new MySQL_Query(&conn);
  
  // Supply the parameter for the query
  // Here we use the QUERY_POP as the format string and query as the
  // destination. This uses twice the memory so another option would be
  // to allocate one buffer for all formatted queries or allocate the
  // memory as needed (just make sure you allocate enough memory and
  // free it when you're done!).
  sprintf(query, QUERY_POP, 9000000);
  
  // Execute the query
  query_mem->execute(query);
  
  // Fetch the columns and print them
  column_names *cols = query_mem->get_columns();

  for (int f = 0; f < cols->num_fields; f++) 
  {
    Serial.print(cols->fields[f]->name);
    
    if (f < cols->num_fields - 1) 
    {
      Serial.print(',');
    }
  }
  
  Serial.println();
  
  // Read the rows and print them
  row_values *row = NULL;
  
  do 
  {
    row = query_mem->get_next_row();
    
    if (row != NULL) 
    {
      for (int f = 0; f < cols->num_fields; f++) 
      {
        Serial.print(row->values[f]);
        
        if (f < cols->num_fields - 1) 
        {
          Serial.print(',');
        }
      }
      
      Serial.println();
    }
  } while (row != NULL);
  
  // Deleting the cursor also frees up memory used
  delete query_mem;

  delay(60000);
}