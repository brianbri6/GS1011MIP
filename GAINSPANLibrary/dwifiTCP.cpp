/* dwifiTCP.cpp
   For use with the Dexter Industries WiFi Shield
   The WiFi Shield can be found here:  dexterindustries.com/wifi/ 
   Created for Dexter Industries by Jasmeet Singh.
   Compatible with Arduino 1.0.1 */
/* This library (dwifiTCP) can be used to- Initialize software serial connection to WiFi Shield; Get commands from textbox of serial monitor; Write string commands to WiFi Shield;
   Receive response from WiFi Shield; Find first instance of a character in a chararray; Turn on/off - Verbose, Echo, Software Control, Hardware Control, DHCP of WiFi Shield;
   Select between infrastructure and adhoc mode; Set- Auto Associate timer, Keep Alive timer, Transmit Power, Network Type(WEP/WPA/OPEN); Associate/Disassociate from a network;
   If you want to choose predefined values for WiFi Shield parameters of WPA/WEP network you can use setWEP and setWPA functions; Find IP address of a hostname;
   Start/Stop TCP Servers on a particular port; Start/Stop TCP Clients to connect to Servers on other machines;
   Write/Read data to/from TCP Servers and Clients; Check all TCP connections currently running. */
#include "string.h"
#include "ctype.h"
//#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

// UNO Board, pins set to first configuration. This will depend on your board; your setup.
//#define SoftRxPin 13
//#define SoftTxPin 12

/* Initialize Software Serial connection. */ 
//SoftwareSerial wifiSerial=SoftwareSerial(SoftRxPin, SoftTxPin);

/* Function to start Software Serial connection and initialize buffer. */
/* Example call to function : init(); */  
void dwifiTCP::init()
{ 
  memset(buffer, ' ', sizeof(buffer));
  Serial.begin(9600);
  Serial.read();
 // pinMode(CTS, OUTPUT);
  delay(100);
  commandStr="";      // String used for passing commands to WiFi Sheild
  bytewifi=-1;           // Bytes to be Received, read as integer.
}

/* Function to read string data from Serial Monitor Box.
   String ends with Carriage Return(CR) or LineFeed(LF) character (ASCII value=13 or 10).
   Returns data String. */
/* Example Call to function: String STR = getstr(); */
String dwifiTCP::getstr()
{
  output="";
  char c;
  delay(8000);                       // Delay of 8 seconds to ensure you can type data in the Serial Monitor.
  while (SerialUSB.available()==0){;}   // If nothing available in serial buffer, wait and do nothing.
  while (SerialUSB.available()) {       // Read data from serial buffer.
    delay(5);  
    if (SerialUSB.available() >0) {
      c = SerialUSB.read();      // Read character from serial buffer and
      if((int(c)!=10) && (int(c)!=13)){
        output += c;}                    // concatenate it to existing string.
      }
    }
  return output;
}

/* Function to write commands to WiFi Shield without <CR> at the end.
   Used only to send portion of a command.
   Must use writeStr() at the end of the each command. */
/* Example Preceding code: String commandStr = "ATE";
   Example Call to function: writeStrNoEnd(commandStr); */
void dwifiTCP::writeStrNoEnd(String str)
{
  SerialUSB.print(str);
  char ch;
  for(int i=0; i<str.length(); i++){
    ch=str.charAt(i);
    writech(ch);
  }
}

/* Function to write variable commands to WiFi Shield with <CR> at the end. */
/* Example Preceding code: String commandStr = "ATE0";
   Example Call to function: writeStr(commandStr); */
void dwifiTCP::writeStr(String str)
{
  SerialUSB.print(str);
 // CTS_stop();
  char ch;
  for(int i=0; i<str.length(); i++){
    ch=str.charAt(i);
    writech(ch);
  }
  Serial.write(13);         // Send Carriage Return <CR>.
  delay(5);
 // CTS_start();
}

/* Function to send data to TCP Server or Client.
   Uses <Esc> characters before start and finish to show start and ending of data.
   For TCP connections start='S' and finish='E' . */
/* Example Call to function: writeesc('S', "Dexter Industries", 'E'); */
void dwifiTCP::writeesc(char start, String data, char finish)
{
  SerialUSB.println(data);
 // CTS_stop();
  Serial.write(27);  // ESC
  Serial.write(start);  // Start character
   for(int i=0; i<data.length(); i++){  // data
    Serial.write(data.charAt(i));}
  Serial.write(27);  // ESC
  Serial.write(finish);   // Stop character
  //CTS_start();
}

/* Function used to receive serial data from WiFi shield which has "OK<CR><LF>" at the end. */
/* Must be preceded by a delay statement:
   Example Preceding code: delay(10);
   Example Calls to function: Receive(true); OR Receive(); */
void dwifiTCP::Receive(boolean wait=false)
{
  int torec=0;
  int i=0;
  torec=Serial.available();   // Number of bytes ready to be received.
  // Wait till there is data available to receive.
  if (wait){
    while(torec==0){
      delay(5);
      torec=Serial.available();}
  }
  int bytewifi2=-1;
  // Receive incoming data.
  while(Serial.available()){
    SerialUSB.print("");
    if(Serial.available()>0){    // If more data available and ready to be received.
      SerialUSB.print("");
      bytewifi=Serial.read();    // Read a byte as an integer.
      buffer[i]=char(bytewifi);      // Convert integer to character.
      // If OK is received (check: K<CR><LF>)
      if(buffer[i]=='K'){
        bytewifi2=Serial.read();        // Read character.
        if(bytewifi2==13){
          buffer[++i]=char(bytewifi2);      // If <CR> Put in buffer.
          bytewifi2= Serial.read();     // Read character.
          if(bytewifi2==10){
            buffer[++i]=char(bytewifi2);    // If <LF> Put in buffer and stop reading further characters.
            break;
          }
          else {buffer[++i]=char(bytewifi2);}  // If not <LF> put in buffer.
        }
        else {buffer[++i]=char(bytewifi2);}  // If not <CR> put in buffer.
      }
      // If ERROR is received (check: E-R-R)
      if(buffer[i]=='E'){
        buffer[++i]=char(Serial.read()); // Read and put character in buffer.
        bytewifi2=Serial.read();         // Read character.
        if (char(bytewifi2)=='R'){           
          buffer[++i]=char(bytewifi2);       // If 'R' Put in buffer.
          buffer[++i]=char(Serial.read()); // Read and put character in buffer.
          bytewifi2=char(Serial.read());  // Read character.
          if (char(bytewifi2)='R'){           
            buffer[++i]=char(bytewifi2);      // If 'R' Put in buffer and stop reading further characters.
            //SerialUSB.println("Error Encountered");
            break;
          }
          else {buffer[++i]=char(bytewifi2); }   // If not 'R' put in buffer.
        }
        else {buffer[++i]=char(bytewifi2); }  // If not 'R' put in buffer.
      }
      i++;
    }
    //CTS_start();
  }
  // Print received data to Serial Monitor Box if, wait = true.
  if (wait==true){
     SerialUSB.print(" : ");
     SerialUSB.println(buffer); }
}

/* Function to read data without checking for error.
   Used when receiving data from TCP servers and clients.
   Returns number of characters received. */
/* Example Call to function: int chars_received = readall(); */
int dwifiTCP::readall()
{
  int i=0;
  while(Serial.available()){ // Keep reading 128 bytes till data is incoming
    memset(buffer,' ', sizeof(buffer));
    i += Serial.readBytes(buffer,sizeof(buffer));
    SerialUSB.print(buffer);
  }
  return i;
}

/* Function to clear data in buffer. */
/* Example Call to function: clear_read_buffer(); */
void dwifiTCP::clear_read_buffer()
{
  delay(100);
  memset(buffer, ' ', sizeof(buffer));  // Clear buffer memory.
 // CTS_start();
  Serial.write(13);
  delay(10);
  while(Serial.available()>0){  // Probably an error. 
      Serial.read();            // Read to clear out any left over incoming data from WiFi shield.
    }
  delay(100);
}

/* Function to turn Verbose ON or OFF.
   onoff= true  - Verbose ON. Characters will be received from WiFi. (default+recommended)
          false - Verbose OFF. Data received from WiFi will be integer values. 
   Returns true on successful completion, false otherwise. */
/* Example Call to function to enable Verbose: boolean check = verbose_on(true); */
boolean dwifiTCP::verbose_on(boolean onoff=true)
{
  delay(100);
  clear_read_buffer();
  commandStr = "ATV";
  commandStr += onoff;
  writeStr(commandStr);    // Send command ATV<onoff> (true=1, false=0)
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to turn Echo OFF or ON. 
   onoff= false - Echo OFF. WiFi will NOT echo back the commands that are sent to it. (default+recommended)
          true  - Echo ON. WiFi will echo back the commands that are sent to it.
   Returns true on successful completion, false otherwise. */
/* Example Call to function to disable Echo: boolean check = echo_on(false); */
boolean dwifiTCP::echo_on(boolean onoff=false)
{
  delay(100);
  clear_read_buffer();
  commandStr = "ATE";
  commandStr += onoff;
  writeStr(commandStr);    // Send command ATE<onoff>(true=1, false=0)
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to enable/disable Software flow control.
   onoff= true  - Enable Software Flow Control. (recommended)
          false - Disable Software Flow Control.
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = soft_flow_control(true); */
boolean dwifiTCP::soft_flow_control(boolean onoff)
{
  delay(100);
  clear_read_buffer();
  commandStr="AT&K";
  commandStr += onoff;
  writeStr(commandStr);    // Send command AT&K<onoff> (true=1, false=0)
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to enable/disable Hardware flow control.
   onoff= true  - Enable Hardware Flow Control. (recommended)
          false - Disable Hardware Flow Control.
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = hard_flow_control(true); */
boolean dwifiTCP::hard_flow_control(boolean onoff)
{
  delay(100);
  clear_read_buffer();
  commandStr="AT&R";
  commandStr += onoff;
  writeStr(commandStr);    // Send command AT&R<onoff> (true=1, false=0)
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to Set Wireless mode.
   val= false - Infrastructure mode
        true  - Ad hoc mode
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = infra_mode(false); */
boolean dwifiTCP::infra_mode(boolean val)
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+WM=";
  commandStr += val;
  writeStr(commandStr);  // Send command AT+WM=<val> (true=1, false=0)
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to set Auto Associate Timeout value.
   timer= 0 to 65535 in 10ms value (Eg. 500= 5 secs).
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = set_network_config(1000); */ 
boolean dwifiTCP::set_network_config(unsigned int aatimer)
{
  delay(100);
  clear_read_buffer();
  commandStr = "ATS1=";
  commandStr += aatimer;
  writeStr(commandStr); // Send command ATS1=<aatimer>
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to set Keep Alive Timer value from 0 to 65535 seconds.
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = keep_alive(1200); */
boolean dwifiTCP::keep_alive(unsigned int katimer)
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+";
  writeStrNoEnd(commandStr); // Send command AT+
  commandStr = "PSPOLLINTRL=";
  commandStr += katimer;
  writeStr(commandStr); // Send command PSPOLLINTRL=<katimer>
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to set Transmit Power value from 0 to 7.
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = set_transmit_power(7); */
boolean dwifiTCP::set_transmit_power(int pow)
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+WP=";
  commandStr += constrain(pow, 0, 7); // Limits value of variable //between 0 and 7.
  writeStr(commandStr);  // Send command AT+WP=<pow>
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Function to set the authentication mode for WiFi. 
   mode= 0 - WPA
         1 - Open
         2 - WEP
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = wifi_auth_mode(0); */
//boolean dwifiTCP::wifi_auth_mode(int mode)
//{
//  delay(100);
//  clear_read_buffer();
//  commandStr = "AT+WAUTH=";
//  commandStr += mode;
//  writeStr(commandStr);  // Send command AT+WAUTH=<mode>
//  delay(10);
//  Receive(true);
//  commandStr="";
//  return checkOK(buffer);/
//}

/* Function to enable/disable DHCP.
   onoff = true  - DHCP enabled.
         = false - DHCP disabled.
   Returns true on successful completion, false otherwise. */
/* Example Call to function: boolean check = DHCP_enable(true); */
boolean dwifiTCP::DHCP_enable(boolean onoff)
{
  delay(100);
  clear_read_buffer();  
  commandStr = "AT+NDHCP=";
  commandStr += onoff;
  writeStr(commandStr); // Send command AT+NDHCP=<onoff>. true=1, false=0.
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Associate with network.
   val = false - Try association only once.
       = true  - Try association till connection established. */
/* Example Call to function: associate_with_network(true); */
void dwifiTCP::associate_with_network(boolean val)
{
  delay(100);
  // Loop to try association atleast once.
  do{
    clear_read_buffer();
    commandStr = "AT+WA=";  // Send command AT+WA=<ssid>
    commandStr += ssid;
    writeStr(commandStr);
    delay(10);
    Receive(true);
    // If OK received. Check 'K' if verbose mode ON check zero if verbose mode OFF.
    if ((inString(buffer,'K')>-1)||(inString(buffer,'0')>-1)){
      val=false;}
    commandStr="";
    delay(100);
  }while (val==true);
}

/* Function to setup WEP network. 
   network = true - Shared with WEP
           = false- Open */
/* Example Call to function: setWep(true); */
void dwifiTCP::setWep(boolean network)
{
  delay(100);
  SerialUSB.println("Setup WEP: ");
  echo_on(false);
  soft_flow_control(false);
  hard_flow_control(false);
  verbose_on(true);
  infra_mode(false);
  disassociate();
  //set_network_config(1000);
  //keep_alive(1200);
  //set_transmit_power(7);
  //wifi_auth_mode(network+1); // if network=true =>2, if false =>1;
  DHCP_enable(true);
  clear_read_buffer();
  delay(100);
  // Set Wep key
  commandStr = "AT+WWEP";
  commandStr += ind;
  commandStr += "=";
  writeStrNoEnd(commandStr);
  commandStr = key;
  writeStr(commandStr); // Send command AT+WWEP<ind>=<key>
  delay(10);
  Receive(true);
  commandStr="";
}

/* Function to setup WPA network. */
/* Example Call to function: setWpa(); */
void dwifiTCP::setWpa()
{
  delay(100);
  SerialUSB.println("Setup WPA: ");
  echo_on(false);
  soft_flow_control(false);
  hard_flow_control(false);
  verbose_on(true);
  infra_mode(true);
  disassociate();
  //set_network_config(1000);
  //keep_alive(1200);
  //set_transmit_power(7);
 // wifi_auth_mode(0);
  DHCP_enable(false);
  clear_read_buffer();
  delay(100);    
  // Set WPA SSID and Passphrase
  commandStr = "AT+WWPA=";
    commandStr = key;
  writeStr(commandStr);  // Send command AT+WWPA=<key>
  delay(10);
  Receive(true);
  commandStr="";
}


/* Function to setup WPA network. */
/* Example Call to function: setWpa2(); */
void dwifiTCP::setWpa2()
{
  delay(100);
  SerialUSB.println("Setup WPA2: ");
  echo_on(true);
  soft_flow_control(false);
  hard_flow_control(false);
  verbose_on(true);
  infra_mode(true);
  
  set_network_config(1000);
  keep_alive(1200);
  set_transmit_power(7);
  //wifi_auth_mode(0);
  DHCP_enable(true);
  clear_read_buffer();
  delay(100);    
  // Set WPA SSID and Passphrase
  commandStr = "AT+WWPA=";
    commandStr += key;
  writeStr(commandStr);  // Send command AT+WWPA=<key>

  delay(10);
  Receive(true);
  commandStr="";
}



/* Function to find IP address of a hostname. 
   Returns IP Address of hostname as a String. */
/* Example Call to function: String DomainIP = dns_lookup("www.dexterindustries.com"); */
String dwifiTCP::dns_lookup(String URL)
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+DNSLOOKUP=";
  writeStrNoEnd(commandStr);
  commandStr = URL;
  writeStr(commandStr);    // Send command AT+DNSLOOKUP=<URL>.
  delay(10);
  Receive(true);
  commandStr="";
  // Example output returned-><CR><LF>IP:123.123.12.123<CR><LF>OK<CR><LF>
  output="";
  int j;
  if (buffer[2]=='I' && buffer[3]=='P'){    // Find "IP:" in buffer.
    j=5;
    while(int(buffer[j])!=13){  // Store IP address values till Space or <CR> is encountered.
       output += buffer[j];
       j++;
    }
  }
  return output;
}

/* Function to start TCP server.
   port- port number value from 1 to 65535.
   Returns CID of the Server as an integer between 0 to 15. */
/* Example call to function: int CID_TCPServer = server_start(2000); */
int dwifiTCP::server_start(unsigned int port)
{
  delay(100);
  char CID;
  clear_read_buffer();
  SerialUSB.print("TCP Server: ");
  commandStr = "AT+NSTCP=";  // Send command AT+NSTCP=<port>
  commandStr += port;
  writeStr(commandStr);
  delay(10);
  Receive(true);
  commandStr="";
  // Example output returned-> CONNECT<Space><CID>...OK<CR>
  int buflen=strlen(buffer);
  int j;
  for (int i=0;i<buflen;i++){  //Check buffer.
    j=i+2;
    if (inString(buffer,'T')==i){  // Find "CONNECT" in buffer.
      if (buffer[j]!=13 || buffer[j]!=32){  // Store hexadecimal CID value till Space or <CR> is encountered.
        CID = buffer[j];
        break;
      }
    }
  }
  return hex2int(CID);
}

/* Connect TCP Client to a Server.
   serverip - IP of TCP Server.
   port - port number used (1 to 65535).
   Returns CID of Client as an integer between 0 to 15. */
/* Example call to function: int CID_TCPClient = client_connect("192.168.1.2",2000); */
int dwifiTCP::client_connect(String serverip, unsigned int port)
{
  delay(100);
  char CID;
  clear_read_buffer();
  SerialUSB.print("TCP Client: ");
  commandStr = "AT+NCTCP=";  // Send command AT+NCTCP=<serverip>,<port>.
  commandStr += serverip;
  commandStr += ",";
  commandStr += port;
  writeStr(commandStr); // Send command AT+NCTCP=<serverip>,<port>.
  delay(10);
  Receive(true);
  commandStr="";
  // Example output returned-> CONNECT<Space><CID>...OK<CR>
  int buflen=strlen(buffer);
  int j;
  for (int i=0;i<buflen;i++){  //Check buffer.
    j=i+2;
    if (inString(buffer,'T')==i){  // Find "CONNECT" in buffer.
      if (buffer[j]!=13 || buffer[j]!=32){  // Store hexadecimal CID unless Space or <CR> is encountered.
        CID = buffer[j];
        break;
      }
    }
  }
  return hex2int(CID);
}

/* Send data to TCP Server or TCP Client
   Connection type- TCP Server or TCP Client.
   data- data to send a string(character array).
   CID- Connection Identifier of current connection (0 to 15).
   Send data: <Esc>S<CID><data><Esc>E  */
/* Example call to function: tcp_write("Dexter Industries",2); */  
void dwifiTCP::tcp_write(String data, int CID)
{
  delay(100);
  commandStr = String(CID,HEX);
  commandStr.toUpperCase();
  commandStr += data; // <CID><data>
  writeesc('S',commandStr,'E');
}

/* Receive data from TCP Server or TCP Client.
   Connection type- TCP Server or TCP Client.
   CID- Connection identifier from which to read.
   Sequence received: <Esc>S<CID><data><Esc>E 
   Returns received data as a String (without<ESC>S and <ESC>E). */
/* Example call to function: String tcpData = tcp_read(); */  
String dwifiTCP::tcp_read()
{
  clear_read_buffer();
  int i=0;
  while(i==0){
    i=readall();
  }
  output="";
  int buflen=strlen(buffer);
  for(i=0;i<buflen;i++){ //Check buffer for <ESC>S and <ESC>E.
    if(buffer[i]==char(27) && buffer[i+1]=='S'){
      i=i+2;}
    if(buffer[i]==char(27) && buffer[i+1]=='E'){
      break;}
    output += buffer[i];
  }
  SerialUSB.println("");
  return output;
}
