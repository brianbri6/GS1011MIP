/*
   Web Server
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to create a simple Web Server on the Shield. Arduino 1.0.1 compatible
   When a client connects to the Shield, and issues a request, the web server shows the value of the analog input pins.
   Request: <anything> \r\n\r\n             Eg: GET / HTTP/1.1\r\n\r\n
   \r\n\r\n is used to indicate end of request. \r\n is similar to pressing the "Enter" key on keyboard once.
   To see the output of the code in a browser-
    1. Click on the browser of your choice (Mozilla Firefox, Google Chrome, Internet Explorer etc).
    2. Execute the code and observe the output displayed in SerialUSB Monitor.
    3. When the Shield acquires an IP Address, note down this Address.
    4. After the Server has started, wait 1-2 seconds, enter the IP Address obtained in step 3 in the Address bar of the Browser and press "Enter" key.
    5. You will observe in the browser that the value of the pins is displayed. But this value remains only till the page is refreshed. After 7 seconds, the page is automatically refreshed
       and the browser issues the request again and now the new current values for the analog pins is seen in the browser. This continues indefinitely till the server is closed.
       You can also use several browsers or clients together (At max fifteen clients can be used at a time).
   It is also possible to issue a request from a TCP Client created using PuTTYtel, Hercules Setup Utility etc.
   PuTTYtel: www.chiark.greenend.org.uk/~sgtatham/putty/download.html
   Hercules Setup Utility: www.hw-group.com/products/hercules/index_en.html
   Before executing this code, go to the directory wherever you have installed the Arduino 1.0.1 files and do this-
   a) In folder "arduino-1.0.1\hardware\arduino\cores\arduino"            b) In folder "arduino-1.0.1\libraries\SoftwareSerialUSB"             
      go to file "HardwareSerialUSB.cpp"                                        go to file "SoftwareSerialUSB.h"
      find the line where it is written-                                     find the line where it is written-
          #if (RAMEND < 1000)                                                     #define _SS_MAX_RX_BUFF 64 // RX buffer size     
              #define SerialUSB_BUFFER_SIZE 16                                  and change it to-
          #else                                                                   #define _SS_MAX_RX_BUFF 128 // RX buffer size
              #define SerialUSB_BUFFER_SIZE 64
          #endif                                                          c) In library file "dwifiTCP.h"
      and change it to-                                                      find the line where it is written-
          #if (RAMEND < 1000)                                                     char buffer[128];
              #define SerialUSB_BUFFER_SIZE 16                                  and change it to-
          #else                                                                   char buffer[448];
              #define SerialUSB_BUFFER_SIZE 128
          #endif
   This will change the size of the Hardware and Software SerialUSB buffers from 64 to 128 and ensure that no data is lost in transmission while the Server is 
   sending/receiving data to/from Web Client/ Browser. The size of internal buffer is changed to 448 to ensure that the complete request header from a browser can be stored.
*/
#include "string.h"
#include "ctype.h"
//#include "SoftwareSerialUSB.h"
//#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"
#include "SamNonDuePin.h"
#include "variant.h"
#include <Arduino_Due_SD_HSCMI.h> // This creates the object SD


dwifiTCP webserver=dwifiTCP();

const int Blue =  5;         // the number of the LED pin
const int RST =  30;         // the number of the LED pin XB MULI5
const int PRM =  X4;  

void setup()
{ 
  SerialUSB.end();      // Close any previously established connections
  Serial.begin(9600);
 SerialUSB.begin(115200);
   pinModeNonDue(PRM, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWriteNonDue(PRM, LOW);
  digitalWrite(RST, HIGH);
  pinMode(Blue, OUTPUT);
  digitalWrite(Blue, LOW);

  digitalWrite(RST, LOW);
  delay(2000);
  digitalWrite(RST, HIGH);
  delay(1000);
  
  
  webserver.init();  // Initialize Software SerialUSB connections to transmit data to WiFi Shield
  delay(100);
  webserver.ssid="mywifi"; // SSID of your WiFi Network
  webserver.key="12345678";  // Passphrase(WPA) or Hexadecimal Key(WEP) of your WiFi Network
}

void loop()
{
delay(1000);
 
  webserver.setWpa2();  // Setup default WPA
  webserver.associate_with_network(true);  // Associate with network
  int WebServerCID = webserver.server_start(80);  // Start a Server on port 80
  while(1){  // Run indefinitely till server has available WiFi connection
    delay(4000);  // To wait for a client to connect
    webserver.output=webserver.tcp_read();  // Example String when client connects: CONNECT <Server CID> <Client CID>
    delay(200);  // To wait for a GET request
    webserver.readall();  // Read GET request from Client/Browser. Example: <ESC>S<ClientCID><Request><ESC>E
    boolean BlankLine = false;  // Used to check for end of request
    int WebClientCID;  // To store the CID of the Client/Browser that issues the request
    for (int i=0;i<strlen(webserver.buffer);i++)
    { if (webserver.buffer[i]==27 && webserver.buffer[i+1]=='S'){  // Check for <ESC>S to get the CID of the Client/Browser issuing the request
         WebClientCID=webserver.hex2int(webserver.buffer[i+2]); SerialUSB.println(WebClientCID);}  // get the value of the Client/Browser CID from GET request
      if (webserver.buffer[i] == '\n' && BlankLine){              // Blank Line if \n followed by \r\n (End of Request)
        webserver.tcp_write("HTTP/1.1 200 OK\n",WebClientCID);    // Send Data to Client/Browser
        webserver.tcp_write("Connection: close\n",WebClientCID);
        webserver.tcp_write("Content-Type: text/html\r\n\r\n",WebClientCID);
        webserver.tcp_write("<!DOCTYPE html>\n",WebClientCID);
        webserver.tcp_write("<html>\n<head>\n",WebClientCID);
        webserver.tcp_write("<meta http-equiv=\"refresh\" content=\"7\">\n",WebClientCID);  // Automatically refresh browser page after every 7 seconds
      
          webserver.commandStr+="<br />\n";
      
        
        webserver.tcp_write("</head>\n</html>\n",WebClientCID);  // Close HTML tag
        break;
      }
      if (webserver.buffer[i] == '\n') {BlankLine = true;}    // If new line is starting
      else if (webserver.buffer[i] != '\r') {BlankLine = false;}  // If line is not blank
    }
    delay(1);
    webserver.cid_disconnect(WebClientCID);  // Disconnect the Client
    SerialUSB.println("Client Disconnected");
  }
}
