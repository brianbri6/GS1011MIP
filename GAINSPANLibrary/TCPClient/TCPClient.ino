/*
   TCP Client
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to create a TCP Client on the Shield. Arduino 1.0.1 compatible
   A TCP client is created using the Shield which connects to a TCP Server running on port 100.
   Create a Server on your computer. After the server has been created, use the IP Address and Port Number of the Server and established a connection with the Server.
   After the connection has been established, you can enter data (characters,digits etc.) in the Server and it will be transmitted to the Client.
   The Serial Monitor prints out the data that is received from the Server. This data is put into a String and is availabe for manipulation within the program code.
   The data received from the Server is of the form: <ESC>S<CID of the Client><Text Data Entered at the Server><ESC>E.
   The String saves the <CID of the Client><Text Data Entered at the Server> and discards the ESC sequences depicting start and end of transmission.
   In this file make the following changes:
   In the setup() function change the values of-
     ssid- SSID or Name of the WiFi Network you are connecting to.
     ind- Key Index with value ranging from 1 to 4, used only in WEP network. Depends on your network configurations.
     key- 10 or 26 hexadecimal characters in case of WEP (example- 4A35535535), OR PassPhrase in case of WPA (example- DEXTERINDUSTRIES). Depends on your network configurations.
   In the loop() function-
     Change the value of IP address and port number  in the line String IP="192.168.1.2" and tcpclient.client_connect(IP,100) according to the IP and port number of the TCP Server.
     If you use a WEP network or would like to change the way the Shield connects to the network, refer to the setWpa and setWep functions of the dwifiTCP.cpp file for details on how to do this.
   ---How to Create a TCP Server on Windows 7---
   1. Ensure that the TCP Server functionalities are operational on your computer.
      Check link on how to do this-> technet.microsoft.com/en-us/library/cc770501%28v=ws.10%29.aspx#bkmk_installVista
   2. Click on Start button of Windows and search for- 'services.msc'
      On opening this, Go to 'Telnet' and Right-Click on it.
      Select 'Properties' and ensure that the 'Startup Type' is not set to "Disabled". Change it to "Manual" (preferred).
   3. Click on Start button of Windows and search for 'cmd.exe' (Command Prompt)
      Open it by Right-clicking, and then selecting 'Run as administrator'.
      Type the following commands in the command prompt one by one to ensure that the TCP Server is configured properly:
	tlntadmn  (to see the parameter values)
	tlntadmn config port=100  (to set the TCP Server on port 100 or any other port of your choice)
	tlntadmn config sec +NTLM -passwd
	tlntadmn config mode=stream
	tlntadmn config maxfail=3  (to set maximum number of login failure attempts, can be any number you like)
	tlntadmn config maxconn=2  (to set maximum number of connections, your choice)
	tlntadmn config ctrlakeymap=yes
	tlntadmn start (Start TCP server, and/or to check that there are no errors on TCP server creation)
	tlntadmn stop (Stop TCP server)
      To see more details about the tlntadmn command go to -> technet.microsoft.com/en-us/library/ff698995%28v=ws.10%29.aspx
      OR www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/telnet_commands.mspx?mfr=true
   4. Download the Hercules Setup Utility from: www.hw-group.com/products/hercules/index_en.html
      by dowloading the latest version available under "Downloads & Links".
      OR
      Start the TCP Server on your computer.
   5. If you use Hercules Setup Utility-
	a) Open the Hercules Utility by double cliking the .exe file.
	b) Open the TCP Server tab.
	c) In 'Server Status' enter the port number that you have defined for the TCP Server in Step 3.
	d) Check the Client authorization option box.
	e) Now make relevant changes to TCPClient.ino file and upload the TCPclient.ino and TCP library onto the Arduino with the WiFi Shield.
	f) Click on 'Listen' button in the Hercules Utility next to the port number. The IP address of the Server is the same as that of the computer on which the utility is running.
	g) Click on the Serial Monitor Box button in Arduino to run the program.
	h) The client will now connect to the TCP server and you can see the IP address of the connected Client in the Hercules Utility under 'Client connection status'.
	i) After the statement "Ready to Receive Data" is printed on the Serial Monitor Box you are free to send any data to the Client from the server by entering ASCII data in the 'Send' textbox in the Hercules Utility.
*/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

dwifiTCP tcpclient=dwifiTCP();
/* Setup serial connections. */
void setup()
{ 
  Serial.end();    // Close any previously established connections
  Serial.begin(9600);
  tcpclient.init();  // Initialize Software Serial connections to transmit data to WiFi Shield
  delay(100);
  tcpclient.ssid="DEXTER";  // SSID of WiFi Network
  tcpclient.ind="1";  // For WEP network only, Default=1 (value from 1 to 4).
  tcpclient.key="DEXTERINDUSTRIES";  // Key or Passphrase for WiFi Network
}

void loop()
{
  delay(1000);
  tcpclient.setWpa();  // Configure Shield to connect to WiFi WPA Network
  tcpclient.associate_with_network(true);  // Connect Shield to the WiFi Network and obtain an IP address.
  String IP="192.168.1.2";  // IP Address of Server
  int TCP_ClientCID = tcpclient.client_connect(IP,100); /* Connect to the TCP Server using the IP address and port number of the 
                                                                      machine on which TCP Server is running(depends on your network).*/
  delay(8000);  // To allow user to enter some data.
  Serial.println("Ready to Receive Data");
  while(true){  // Continuously receive data from TCP Server
    tcpclient.commandStr=tcpclient.tcp_read(); // Read and print incoming strings with escape sequence to Serial monitor.
                                               // But return only <CID of Client><Text data> as the string without ESC sequences.
    Serial.print("Data Received: ");
    Serial.println(tcpclient.commandStr);  // Print <CID of Client><Text data> to Serial Monitor
  }
}
