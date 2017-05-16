/*
   Web Client
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to read the response html data from Google(www.google.com) for a particular search query.
   It connects to www.google.com and reads the page returned about the query.
   Command: GET /search?q=<the query terms to search for (separated by +)> HTTP/1.0\r\n\r\n
   \r\n\r\n is used to indicate end of request.
   To confirm/check output for:         GET /search?q=dexter+industries HTTP/1.0
   In your browser, enter:   www.google.com/search?q=dexter+industries 
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
   This will change the size of the Hardware and Software Serial buffers from 64 to 128 and ensure that no data is lost in transmission while the Client is 
   sending/receiving data to/from Google Server.
*/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

dwifiTCP webclient=dwifiTCP();
/* Setup serial connections. */
void setup()
{ 
  Serial.end();      // Close any previously established connections
  Serial.begin(9600);
  webclient.init();
  delay(100);
  webclient.ssid="DEXTER"; // SSID of your WiFi Network
  webclient.ind="1";  // For WEP network, Default=1 (value from 1 to 4).
  webclient.key="DEXTERINDUSTRIES";  // Passphrase(WPA) or Hexadecimal Key(WEP) of your WiFi Network
}

void loop()
{
  delay(1000);
  webclient.setWpa();
  webclient.associate_with_network(true);
  int WebClientCID = webclient.client_connect(webclient.dns_lookup("www.google.com"),80); // Connect to IP address of Google on port 80.
  webclient.tcp_write("GET /search?q=dexter+industries HTTP/1.0\r\n\r\n",WebClientCID);  // Send Request
  while(true){
    if(webclient.readall()>0){  // Read response
      while(1);
    }
  }
}
