/*
   Echo Server
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to create a Server on the Shield on port 100
   that sends back the data it receives from the client.
   When a client, created using PuTTYtel,Hercules Setup Utility etc., connects to the Shield, and sends some data,
   the server reads the data into a string and addes a linefeed(\n) to the end of the
   string and sends the data back to the client.
   PuTTYtel: www.chiark.greenend.org.uk/~sgtatham/putty/download.html
   Hercules Setup Utility: www.hw-group.com/products/hercules/index_en.html
   The code runs indefinitely till the shield has a connection to the WiFi Network.
   Fifteen Clients can be connected to this Server at a time.
   Before executing this code, go to the directory wherever you have installed the Arduino 1.0.1 files and do this (RECOMMENDED, NOT COMPULSORY for this example)-
   a) In folder "arduino-1.0.1\hardware\arduino\cores\arduino"          b) In folder "arduino-1.0.1\libraries\SoftwareSerial"             
      go to file "HardwareSerial.cpp"                                      go to file "SoftwareSerial.h"
      find the line where it is written-                                   find the line where it is written-
          #if (RAMEND < 1000)                                                   #define _SS_MAX_RX_BUFF 64 // RX buffer size     
              #define SERIAL_BUFFER_SIZE 16                                and change it to-
          #else                                                                 #define _SS_MAX_RX_BUFF 128 // RX buffer size
              #define SERIAL_BUFFER_SIZE 64
          #endif
      and change it to-
          #if (RAMEND < 1000)
              #define SERIAL_BUFFER_SIZE 16
          #else
              #define SERIAL_BUFFER_SIZE 128
          #endif
   This will change the size of the Hardware and Software Serial buffers from 64 to 128 and ensure that no data is lost in transmission while
   the Server is sending/receiving data to/from Client.
*/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

dwifiTCP echoserver=dwifiTCP();

void setup()
{ 
  Serial.end();      // Close any previously established connections
  Serial.begin(9600);
  echoserver.init();  // Initialize Software Serial connections to transmit data to WiFi Shield
  delay(100);
  echoserver.ssid="DEXTER"; // SSID of your WiFi Network
  echoserver.ind="1";  // For WEP network, Default=1 (value from 1 to 4).
  echoserver.key="DEXTERINDUSTRIES";  // Passphrase(WPA) or Hexadecimal Key(WEP) of your WiFi Network
}

void loop()
{
  echoserver.setWpa();  // Setup default WPA
  echoserver.associate_with_network(true);  // Associate with network
  int EchoServerCID = echoserver.server_start(100);  // Create Echo Server on Port 100
  delay(4000);    // Wait for a client to connect to the Server
  echoserver.output = echoserver.tcp_read();  // Example String when client connects: CONNECT <Server CID> <Client CID>
  while(true){      // Run indefinitely till the Shield has a connection to the WiFi Network
    delay(1000);
    Serial.println("Data Received by Server:");
    echoserver.output = echoserver.tcp_read();  // Read incoming data from Client as: <Client CID><data>. The string returned ignores the <ESC>S and <ESC>E sequences 
    Serial.println("Data Sent by Server:");
    echoserver.output += "\n";  // append a newline at the end of the String (or change it in any other way)
    echoserver.writeesc('S',echoserver.output,'E');  // Send back data to Client as: <Client CID><data>\n
    /*('S' and 'E' are used in TCP Server and Client connections to indicate the start and end of the data transmission)*/
  }
}
