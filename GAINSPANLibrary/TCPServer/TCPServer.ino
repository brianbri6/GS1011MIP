/*
   TCP Server
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to create a TCP Server on the Shield. Arduino 1.0.1 compatible
   A TCP Server is created in port 100, using the Shield which, can be connected to TCP clients.
   Create a Server on your shield. After the server has been created, use the IP Address and Port Number of the Server and established a connection with a client.
   After the connection has been established, you can enter data (characters,digits etc.) in the Client and it will be transmitted to the Server.
   The Serial Monitor prints out the data that is received from the Client. This data is put into a String and is availabe for manipulation within the program code.
   The data received from the Client is of the form: <ESC>S<CID of the Client><Text Data Entered at the Client><ESC>E.
   The String saves the <CID of the Client><Text Data Entered at the Server> and discards the ESC sequences depicting start and end of transmission.
   In this file make the following changes:
   In the setup() function change the values of-
     ssid- SSID or Name of the WiFi Network you are connecting to.
     ind- Key Index with value ranging from 1 to 4, used only in WEP network. Depends on your network configurations.
     key- 10 or 26 hexadecimal characters in case of WEP (example- 4A35535535), OR PassPhrase in case of WPA (example- DEXTERINDUSTRIES). Depends on your network configurations.
   In the loop() function-
     If you use a WEP network or would like to change the way the Shield connects to the network, refer to the setWpa and setWep functions of the dwifiTCP.cpp file for details on how to do this.
   After the server has been created and Serial Monitor prints "Ready to Receive Data" you can connect TCP Clients to it via telnet using PuTTYtel, Hercules Setup Utility etc.
   PuTTYtel: www.chiark.greenend.org.uk/~sgtatham/putty/download.html
   Hercules Setup Utility: www.hw-group.com/products/hercules/index_en.html
   IP Address of the Server is printed in the Serial Monitor after successful connection to the WiFi Network. Port Number of Server can be changed in the loop() function below.
   After connection has been established with the Server, using the IP address and the port number, you can enter data (characters,digits etc.) in the Client window and it will be transmitted to the Server and printed on the Serial Monitor.
   The returned String is devoid of the <Esc>S and <Esc>E characters and only prints the CID of the Client connected to the server (1 character) followed by the data received from that client.
   Fifteen Clients can be connected to this Server at maximum, at a time.
*/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

dwifiTCP tcpserver=dwifiTCP();
/* Setup serial connections. */
void setup()
{ 
  Serial.end();                  // Close any previously established connections
  Serial.begin(9600);
  tcpserver.init();  // Initialize Software Serial connections to transmit data to WiFi Shield
  delay(100);
  tcpserver.ssid="DEXTER";  // SSID of WiFi Network
  tcpserver.ind="1";  // For WEP network, Default=1 (value from 1 to 4).
  tcpserver.key="DEXTERINDUSTRIES";  // Key or Passphrase for WiFi Network
}

void loop()
{
  delay(1000);
  tcpserver.setWpa();  // Configure Shield to connect to WiFi WPA Network
  tcpserver.associate_with_network(true);  // Connect Shield to the WiFi Network and obtain an IP address.
  int TCP_serverCID = tcpserver.server_start(100);  // Create TCP Server on Shield on Port Number 100
  delay(8000);  // Wait for user to open PuTTYtel or Hercules utility and enter the IP address and Port number of the Server.
  Serial.println("Ready to Receive Data");
  while(true){  // Receive data from Client
    tcpserver.commandStr=tcpserver.tcp_read();  //Read and print all incoming data and return it into a String in the form: <CID of Client><Text data>
    Serial.print("Data Received: ");
    Serial.println(tcpserver.commandStr);
  }
}
