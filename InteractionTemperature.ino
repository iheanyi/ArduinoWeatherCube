
/*

 This example connects to an unencrypted Wifi network.
 Then it prints the  MAC address of the Wifi shield,
 the IP address obtained, and other network details.
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
#include <SPI.h>
#include <WiFi.h>
#include <JsonParser.h>
#include <b64.h>
#include <HttpClient.h>
#include <weather_client.h>
//#include "lasote/weather/weather_client.h"


//using namespace ArduinoJson::Parser;
//using namespace lasote;

char ssid[] = "ND-guest";     // the name of your network
int status = WL_IDLE_STATUS;     // the Wifi radio's status

const char server[] = "api.openweathermap.org";
char server2[] = "www.google.com";
IPAddress ip(192,168,0,177);

const char path[] = "/data/2.5/weather?q=South%20Bend,IN&mode=json";
const int networkDelay = 5000;

//IPAddress server2(74,125,232,128);  // numeric IP for Google (no DNS)

// Initialize client library
WiFiClient client;
//


unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000l;
//JsonParser<16> parser;

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;


char *response_buffer;
int _read = 0;

String result = "";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  response_buffer = new char[16000];
  
  String fv = WiFi.firmwareVersion();
  
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(5000);
  }





  printCurrentNet();
  printWifiData();
  //delay(1000);
}

void loop() {

}


void httpRequest() {
  client.stop();
  Serial.println("Making a request!");
  if (client.connect(server, 80)) {
    Serial.println("connected...");
    // Send HTTP request
    client.println("GET /data/2.5/weather?q=South%20Bend,IN&mode=json HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    //client.println("User-Agent: ArduinoWifi/1.1");
    //client.println("Connection: close");
    client.println();

    lastConnectionTime = millis();
  } 
  else {
    Serial.println("Connection Failed."); 
  }
}

void httpRequestStuff() {
  _read = 0;
  result = "";
  while (client.available()) {
    char c = client.read();
    //response_buffer[_read] = c;
    result = result + c;
    /*if(_read >= 3 && response_buffer[_read-3] == 'r' 
       && response_buffer[_read-2] == '\n' 
       && response_buffer[_read-1] == '\r' && response_buffer[_read] == '\n') {   
      
       Serial.println(_read);
     }*/
     
    //Serial.println(_read);
    
   Serial.print(c);
   _read++;
  }
  
  //Serial.print(result);

  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}
void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
}

void connectClient() {
  
}

void alternateHTTP() {
 int err = 0;
 HttpClient http(client);
 
 err = http.get(server, path);
 if(err == 0) {
  Serial.println("Requested stated just fine.");
  err = http.responseStatusCode(); 
  
  if (err >= 0) {
   Serial.println("Got status code: ");
   Serial.println(err);
  
    err = http.skipResponseHeaders();
   
   if (err >= 0) {
    int bodyLen = http.contentLength();
   } 
  }
 }
}
void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
}

  
