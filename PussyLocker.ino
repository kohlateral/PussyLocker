/*
  TinyDuino WiFi TinyShield Example Sketch

  Just a basic tutorial showing you how to connect to WiFi with the Wifi
  TinyShield

  NOTE: There are a couple things you'll need to change for this to work!

  Written 29 May 2018
  By Laverena Wienclaw
  Modified 07 January 2019
  By Hunter Hykes

  https://TinyCircuits.com
*/

// This library is for the wifi connection
#include <SPI.h>
#include <WiFi101.h>
#include <stdio.h>
#include <stdlib.h>
#include "ardunio_secrets.h"

/*********************** EDIT THIS SECTION TO MATCH YOUR INFO *************************/
char ssid[] = SECRET_SSID;  //  your network SSID (name)
char wifiPassword[] = SECRET_PASS;  // your network password

// Define Serial object based on which TinyCircuits processor board is used.
#if defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#else
  #define SerialMonitorInterface Serial
#endif

int status = WL_IDLE_STATUS;
char server[] = "52.0.236.99";
char lockStatus[] = "Opened";
char currentTemp[] = "25.00";
char thresholdTemp[] = "100.00";
WiFiClient client;

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

void setup() {
  SerialMonitorInterface.begin(9600);
  WiFi.setPins(8, 2, A3, -1); // VERY IMPORTANT FOR TINYDUINO
  while(!SerialMonitorInterface);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Wifi Shield not present");
    while(true);
  }

  while (status != WL_CONNECTED){
    SerialMonitorInterface.print("Attempting to connect to SSID: ");
    SerialMonitorInterface.println(ssid);
    status = WiFi.begin(ssid, wifiPassword);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // Attempt to connect to Wifi network:
  //SerialMonitorInterface.print("Connecting Wifi: ");
  //SerialMonitorInterface.println(ssid);

  // Connect to WiFi, and loop until connection is secured
  //WiFi.begin(ssid, wifiPassword);
  //while (WiFi.status() != WL_CONNECTED){
    //SerialMonitorInterface.print(".");
    //delay(500);
  //}
    

  // Print out the local IP address
  SerialMonitorInterface.println("");
  SerialMonitorInterface.println("WiFi connected");
  printWiFiStatus();

  SerialMonitorInterface.println("\n Starting connection to server...");
}

void loop()
{
  char resp[1000];
  while(client.available()){
    char c = client.read();
    SerialMonitorInterface.write(c);
  }

  if (millis() - lastConnectionTime > postingInterval) {
    update_device_status();
  }
}

void update_device_status() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  
  if (client.connect(server, 80)){
    SerialMonitorInterface.println("Connected to server");
    //HTTP Headers
    client.println("POST /device_status HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Content-Length: 56");
    client.println("Content-Type: text/plain");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: keep-alive");
    client.println();
    //HTTP Body containing data to be sent to server
    client.print("Lock Status:");
    client.println(lockStatus);
    client.print("Temperature:");
    client.println(currentTemp);
    client.print("Threshold:");
    client.println(thresholdTemp);

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    SerialMonitorInterface.println("Connection Failed");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  SerialMonitorInterface.print("SSID: ");
  SerialMonitorInterface.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  SerialMonitorInterface.print("IP Address: ");
  SerialMonitorInterface.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  SerialMonitorInterface.print("signal strength (RSSI):");
  SerialMonitorInterface.print(rssi);
  SerialMonitorInterface.println(" dBm");
}

void appendChar(char *s, char c) {
  int len = strlen(s);
  s[len] = c;
  s[len+1] = '\0';
}

char* mystrcat( char* dest, char* src )
{
     while (*dest) dest++;
     while (*dest++ = *src++);
     return --dest;
}
