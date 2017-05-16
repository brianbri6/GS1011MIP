/* dwifiTCP.h
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

#ifndef dwifitcp_h
#define dwifitcp_h

#define RTS 10
#define CTS 11
#define CTS_PIN_SHIFT 3
#define checkOK(buff) memcmp(buff+2,"OK",2)==0?true:false /* Check if OK received in Buffer when Output is <CR><LF>OK (buff[2]=='O' && buff[3]=='K') */
#define RTS_check() !digitalRead(RTS)?true:false          /* Check to see RTS. */
#define CTS_start() PORTB=PORTB &~(1 << CTS_PIN_SHIFT)    /* Set CTS pin low. Clear to Send. */
#define CTS_stop() PORTB=PORTB | (1 << CTS_PIN_SHIFT)     /* Set CTS pin high.  Not Clear to Send. */
#define writech(ch) ((int(ch)!=10) && (int(ch)!=13) && (int(ch)!=32))?Serial.write(ch):SerialUSB.print("") /* Writes all characters in command except <CR>, <LF> and <Space>. */

class dwifiTCP
{
        public: 
                String ssid;  /* SSID of the WEP/WPA Network. */
                String ind;   /* Index value (1 to 4) used only for WEP network. */
                String key;   /* 10 or 26 character Hexadecimal Key for WEP OR PassPhrase for WPA. */
                char buffer[128];
                String commandStr;  /* Generally used for commands to be sent to WiFi Shield. */
                String output;  /* Generally used for characters received from WiFi Shield. */
                int bytewifi;
                void init();
		String getstr();      
                void writeStrNoEnd(String);
		void writeStr(String);
                void writeesc(char, String, char);
                void Receive(boolean);
                int readall();
		void clear_read_buffer();
		inline int inString(char*, char);
		boolean verbose_on(boolean);
		boolean echo_on(boolean);
		boolean soft_flow_control(boolean);
		boolean hard_flow_control(boolean);
		boolean infra_mode(boolean);
		boolean set_network_config(unsigned int);
		boolean keep_alive(unsigned int);
		boolean set_transmit_power(int);
                inline void disassociate();
		boolean wifi_auth_mode(int);
                boolean DHCP_enable(boolean);
                void associate_with_network(boolean);
		void setWep(boolean);
		void setWpa();
           void setWpa2();
                String dns_lookup(String);
		int server_start(unsigned int);
		int client_connect(String, unsigned int);
		void tcp_write(String, int);
		String tcp_read();
		inline boolean cid_disconnect(int);
		inline void cid_check();
                inline int hex2int(char);
};

/* Function to find first position of a character in an char array.
   If character not found, returns value -1. */
/* Example Call to function: int position = inString("data",'t'); =>returns 2. */
inline int dwifiTCP::inString(char *buf, char ch)
{
  int pos=-1;
  for(int i=0; i<strlen(buf); i++){
    if(ch==buf[i]){
      pos=i;
      break;
    }
  }
  return pos;    
}

/* Function to disassociate from network. */
/* Example Call to function: disassociate(); */
inline void dwifiTCP::disassociate()
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+WD";
  writeStr(commandStr);  // Send command AT+WD.
  delay(10);
  Receive(true);
  commandStr="";
}

/* Disconnect TCP Server or Client.
   CID- Connection Identifier of Server or Client to disconnect (0 to 15). 
   Returns true on successful completion, false otherwise. */
/* Example Preceding Code: int CID_Server = server_start(2000);
   Example call to function: boolean check = cid_disconnect(CID_Server); */
inline boolean dwifiTCP::cid_disconnect(int CID)
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+NCLOSE=";
  commandStr += String(CID,HEX);
  commandStr.toUpperCase();
  writeStr(commandStr); // Send command AT+NCLOSE=<CID>.
  delay(10);
  Receive(true);
  commandStr="";
  return checkOK(buffer);
}

/* Get details of all TCP Server/Client connections.
   Prints CID number, CID type, Protocol, Local port, Remote port, Remote IP address */
/* Example call to function: cid_check(); */
inline void dwifiTCP::cid_check()
{
  delay(100);
  clear_read_buffer();
  commandStr = "AT+CID=?";
  writeStr(commandStr); // Send command AT+CID=?
  delay(10);
  Receive(true);
  commandStr="";
}

/* Function to convert hexadecimal value in char to integer value. */
/* Example call to function: int value = hex2int('A'); */
inline int dwifiTCP::hex2int(char hex)
{
  int x;
  if (hex>=48 && hex<=57){  // For values between 0 to 9
    x=hex-48;
  }
  else {
    if (hex>=65 && hex<=70){ // For values between A to F
    x=hex-55;}
  }
  return x;
}

#endif
