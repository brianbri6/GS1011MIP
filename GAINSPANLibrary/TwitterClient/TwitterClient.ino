/*
   Twitter Client
   Created for Dexter Industries by Jasmeet Singh. */
/* This example is used with the dwifiTCP library for Dexter Industries' WiFi Shield to read information of a particular Twitter (www.twitter.com) user's timeline.
   It connects to api.twitter.com and reads information about tweets of a user, like- tweet text, source application of the tweet, user information and more.
   The command uses the REST API version 1 (instead of API version 1.1) of Twitter.
   Command: GET /<APIversion>/statuses/user_timeline.json?screen_name=<user's twitter handle><& parameter=value>&count=<number of tweets you want to see> HTTP/1.1\r\n\r\n
   \r\n\r\n is used to indicate end of request.
   For more information on GET method and various parameters (like trim_user) for twitter user's timeline visit: dev.twitter.com/docs/api/1/get/statuses/user_timeline
   For other methods of api.twitter.com visit: dev.twitter.com/docs/api/1/
   To confirm/check output for:                  GET /1/statuses/user_timeline.json?screen_name=dexterind&trim_user=true&count=1 HTTP/1.1
   In your browser, paste the link:   api.twitter.com/1/statuses/user_timeline.json?screen_name=dexterind&trim_user=true&count=1 HTTP/1.1 
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
   sending/receiving data to/from Twitter Server.
*/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
#include "math.h"
#include "Arduino.h"
#include "dwifiTCP.h"

dwifiTCP twitter=dwifiTCP();
/* Setup serial connections. */
void setup()
{ 
  Serial.end();     // Close any previously established connections
  Serial.begin(9600);
  twitter.init();  // Initialize Software Serial connections to transmit data to/from WiFi Shield
  delay(100);
  twitter.ssid="DEXTER";  // SSID of your WiFi Network
  twitter.ind="1";  // For WEP network, Default=1 (value from 1 to 4).
  twitter.key="DEXTERINDUSTRIES";  // Passphrase(WPA) or Hexadecimal Key(WEP) of your WiFi Network
}

void loop()
{
  delay(1000);
  twitter.setWpa();
  twitter.associate_with_network(true);
  int TwitterClientCID = twitter.client_connect(twitter.dns_lookup("api.twitter.com"),80); // IP address Twitter
  twitter.tcp_write("GET /1/statuses/user_timeline.json?screen_name=dexterind&trim_user=true&count=1 HTTP/1.1\r\n\r\n",TwitterClientCID); // Send command to server
  while(true){ // read response from server
    if(twitter.readall()>0){
      while(1);
    }
  }
}
