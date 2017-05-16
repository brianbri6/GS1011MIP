/*
   Cosm / Pachube Client
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to create a Client for cosm.com on the Shield. Arduino 1.0.1 compatible
   A Client is created for the IP of api.cosm.com on port 80, using the Shield which, can be connected to a feed on api.cosm.com and prints the data of the analog pin 1 on the feed.
   Steps:
    1. Upload the CosmClient.ino and TCP library onto the Arduino with the WiFi Shield.
    2. Every time you run the code the value of the analog pin 1 of Arduino is added to the Cosm Feed mentioned in the code.
   Request to api.cosm.com:
     PUT /v2/feeds/<FEED ID>.csv HTTP/1.1\r\nHost: api.cosm.com\r\nX-ApiKey: <API KEY>\r\nUser-Agent: <USER AGENT/ PROJECT NAME>\r\nContent-Length: <Length of data>\r\nContent-Type: text/csv\r\nConnection: close\r\n\r\n
     Sensor1,<value of Analog pin>
   Where "Sensor1,<value of Analog pin>" is the main data whose value is to be put on the Feed.
   The following values are used in the program for-
     Project ID/FEED ID:    82525
     API KEY:    4c9QGeVwNDTmjTZLRoYhGsWOA8mSAKxSU2UrYnRMTyszST0g
     Project Name/USER AGENT:    Cosm Arduino Example (82525)
   To see the previous or current values for this feed go to: cosm.com/feeds/82525
   To create your own Feed-
    1. Create an account on cosm.com
    2. Click on the button "Device/Feed"
    3. Select Arduino
    4. Give a title to your feed and Click "Next".
    5. Add tags for your feed and Click "Next".
    6. Click "Create".
    7. From the program automatically generated, use the lines defined immediately after the header files to get the values of API KEY, FEED ID, and USER AGENT.
       Example:
        #define APIKEY         "4c9QGeVwNDTmjTZLRoYhGsWOA8mSAKxSU2UrYnRMTyszST0g" // your cosm api key
        #define FEEDID         82525 // your feed ID
        #define USERAGENT      "Cosm Arduino Example (82525)" // user agent is the project name
    8. Copy these values and insert them at the relevant locations in this example.
    9. You can now observe the pin value on your feed every time you successfully run this program.
   \r\n is used to separate the GET method and Host request header; \r\n\r\n is used to indicate end of request header.
   Before executing this code, go to the directory wherever you have installed the Arduino 1.0.1 files and do this-
   a) In folder "arduino-1.0.1\hardware\arduino\cores\arduino"            b) In folder "arduino-1.0.1\libraries\SoftwareSerial"             
      go to file "HardwareSerial.cpp"                                        go to file "SoftwareSerial.h"
      find the line where it is written-                                     find the line where it is written-
          #if (RAMEND < 1000)                                                     #define _SS_MAX_RX_BUFF 64 // RX buffer size     
              #define SERIAL_BUFFER_SIZE 16                                  and change it to-
          #else                                                                   #define _SS_MAX_RX_BUFF 128 // RX buffer size
              #define SERIAL_BUFFER_SIZE 64
          #endif
      and change it to-
          #if (RAMEND < 1000)
              #define SERIAL_BUFFER_SIZE 16
          #else
              #define SERIAL_BUFFER_SIZE 128
          #endif
    This will change the size of the Hardware and Software Serial buffers from 64 to 128 and ensure that no data is lost in transmission while the Client is sending/receiving
    to/from Cosm Server.
*/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

dwifiTCP CosmClient=dwifiTCP();
/* Setup serial connections. */
void setup()
{ 
  Serial.end();     // Close any previously established connections
  Serial.begin(9600);
  CosmClient.init();
  delay(100);
  CosmClient.ssid="DEXTER";  // SSID of WiFi Network
  CosmClient.ind="1";  // For WEP network, Default=1 (value from 1 to 4).
  CosmClient.key="DEXTERINDUSTRIES";  // Key or Passphrase for WiFi Network
}

void loop()
{
  delay(1000);
  CosmClient.setWpa();
  CosmClient.associate_with_network(true);
  int CosmClientCID = CosmClient.client_connect(CosmClient.dns_lookup("api.cosm.com"),80);
  /* PUT /v2/feeds/<FEED ID>.csv HTTP/1.1\r\nHost: api.cosm.com\r\nX-ApiKey: <API KEY>\r\nUser-Agent: <USER AGENT/ PROJECT NAME>\r\nContent-Length: <Length value>\r\nContent-Type: text/csv\r\nConnection: close\r\n\r\n*/
  /* Sensor1,<value of Analog pin> */
  int pinval = analogRead(1);  // Read value of pin 1 (Can be changed to any value)
  // Request sent using HTTP PUT method
  CosmClient.commandStr = "PUT /v2/feeds/";  // PUT HTTP Method
  CosmClient.commandStr += "82525";  // FEED ID
  CosmClient.commandStr += ".csv HTTP/1.1\r\nHost: api.cosm.com\r\nX-ApiKey: ";  // Request continued
  CosmClient.commandStr += "4c9QGeVwNDTmjTZLRoYhGsWOA8mSAKxSU2UrYnRMTyszST0g"; // API KEY
  CosmClient.commandStr += "\r\n";
  CosmClient.tcp_write(CosmClient.commandStr, CosmClientCID);  // Send partial request uptil now.
  CosmClient.commandStr = "User-Agent: ";
  CosmClient.commandStr += "Cosm Arduino Example (82525)"; // USER AGENT
  CosmClient.commandStr += "\r\nContent-Length: ";
  int thisLength = 8 + intLength(pinval);
  CosmClient.commandStr += thisLength;
  CosmClient.commandStr += "\r\nContent-Type: text/csv\r\nConnection: close\r\n\r\n";
  CosmClient.tcp_write(CosmClient.commandStr, CosmClientCID);  // Send partial request uptil now.
  CosmClient.commandStr = "Sensor1,";    /* Name of the graph (Data stream) plotted in the feed (comma ',' is important at the end). Can be Changed to any value.
                                            If a value greater than 8 characters is used, then variable thisLength will become <number of charactes> + intLength(pinval)
                                            For Example: "PinDexter," is used then-> thisLength = 10 + intLength(pinval); */
  CosmClient.commandStr += pinval;
  CosmClient.commandStr += "\r\n";  // End of HTTP request
  CosmClient.tcp_write(CosmClient.commandStr, CosmClientCID);  // Send value of analog pin.
  while(1);  // Do nothing
}

/* This function is used to get the number of digits in an integer value */
int intLength(int val)
{
  int digits = 1;
  int dividend = val/10;
  while (dividend > 0)
  {
    dividend = dividend/10;
    digits++;
  }
  return digits;
}



