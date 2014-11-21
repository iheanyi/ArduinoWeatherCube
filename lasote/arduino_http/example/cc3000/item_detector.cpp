#include "Arduino.h"
#include "lasote/arduino_http/http_client.h"
#include "lasote/stream_finder_processor/stream_finder_processor.h"

//For cc3000 adapter
#include <adafruit/cc3000_library/adafruit_cc3000.h>
#include "diego/ardunet/cc3000client.h"
#include "diego/ardunet/cc3000utils.h"

using namespace lasote;

//This file, located in your hive root folder must #define WLAN_SSID , WLAN_PASS and WLAN_SECURITY
//of your wifi. It is not published in your block for obvious reasons. Example:
#define WLAN_SSID "XXXXXXXX"
#define WLAN_PASS "XXXXXXXXXXXXXXXXX"
#define WLAN_SECURITY WLAN_SEC_WPA2

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

int led = 13;


//create CC3000 instance, and an SDK compatible Client
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS,
        ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);

CC3000Client client(cc3000);

HttpClient http_client(client, Serial);
StreamFinderProcessor item_finder("MicroSD", "64GB", 50);


// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	Serial.println(F("************* SETUP ****************"));
    pinMode(led, OUTPUT);     // initialize the digital pin as an output.
    startConnection(cc3000, WLAN_SSID, WLAN_PASS, WLAN_SECURITY);

    http_client.processors.add_item(&item_finder);

}


void loop() {

	//Reset status and processors for new clean website check
	http_client.reset();
	delay(4000);
    Serial.println(F("---Checking website!---"));
    int ret = http_client.get("bargainshop.es", "/");
    if(ret){
    	Serial.println("Connected");
    	int ret_code = http_client.receive();
    	if(ret_code==200){
    		Serial.println(F("Page loaded ok..."));
    		if(item_finder.found){
    			Serial.println(F("MicroSD 64GB Found!"));
    		}
    		else{
    			Serial.println(F("No MicroSD 64GB today"));
    		}

    	}
    	else{
    		Serial.println(F("Error code!"));
     	}
    }
    else{
    	Serial.println(F("ERROR CONNECTING... retrying"));
    }
}

