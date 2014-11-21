#include "Arduino.h"
#include "ntruchsess/arduino_uip/uipethernet.h"
#include "lasote/arduino_http/http_client.h"

#include "lasote/stream_finder_processor/stream_finder_processor.h"
#include "lasote/stream_recorder/stream_recorder.h"
#include "Arduino.h"

//For cc3000 adapter
#include <adafruit/cc3000_library/adafruit_cc3000.h>
#include "diego/ardunet/cc3000client.h"
#include "diego/ardunet/cc3000utils.h"

//This file, located in your hive root folder must #define WLAN_SSID , WLAN_PASS and WLAN_SECURITY
//of your wifi. It is not published in your block for obvious reasons. Example:
#define WLAN_SSID "XXXXXXXXXXX"
#define WLAN_PASS "XXXXXXXXXXX"
#define WLAN_SECURITY WLAN_SEC_WPA2

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10


using namespace lasote;

//create CC3000 instance, and an SDK compatible Client
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS,
        ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);

CC3000Client client(cc3000);

char response_buffer[20];
StreamRecorderProcessor response_recorder(response_buffer, 20); //Capture response in buffer
HttpClient http_client(client, Serial);
int led = 12;

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	Serial.println("************* SETUP ****************");
    pinMode(led, OUTPUT);     // initialize the digital pin as an output.
    startConnection(cc3000, WLAN_SSID, WLAN_PASS, WLAN_SECURITY);

    http_client.processors.add_item(&response_recorder);

}

// the loop routine runs over and over again forever:
void loop() {

	delay(5000);
    Serial.println("--- SENDING POST---");

    http_client.reset(); //Reset processors and custom headers
    //We can add custom headers to the request
    http_client.add_custom_header("Content-Type", "application/json");

    int sent = http_client.post(IPAddress(192, 168, 1, 100), "/", "{\"uno\":1, \"dos\":[2,2,2,2]}", 8080);
    if(sent){
    	if(short int ret_code = http_client.receive()){
    		if(ret_code == 200){
    			Serial.println("Code 200 OK!");
    			Serial.println(response_buffer);
    		}
    		else{
    			Serial.print("Http Error:");
    			Serial.println(ret_code);
    		}
    	}
    	else{
    		Serial.println(F("Error receiving!"));
    	}
    }
    else{
    	Serial.println(F("ERROR CONNECTING... retrying"));
    }
}
