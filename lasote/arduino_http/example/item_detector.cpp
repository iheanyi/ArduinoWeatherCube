#include "Arduino.h"
#include "ntruchsess/arduino_uip/uipethernet.h"
#include "lasote/arduino_http/http_client.h"
#include "lasote/stream_finder_processor/stream_finder_processor.h"

using namespace lasote;


int led = 13;
EthernetClient client;
HttpClient http_client(client, Serial);
StreamFinderProcessor item_finder("MicroSD", "64GB", 50);



// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	Serial.println(F("************* SETUP ****************"));
    pinMode(led, OUTPUT);     // initialize the digital pin as an output.
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    Ethernet.begin(mac);

    http_client.processors.add_item(&item_finder);

    Serial.print(F("localIP: "));
    Serial.println(Ethernet.localIP());
    Serial.print(F("subnetMask: "));
    Serial.println(Ethernet.subnetMask());
    Serial.print(F("gatewayIP: "));
    Serial.println(Ethernet.gatewayIP());
    Serial.print(F("dnsServerIP: "));
    Serial.println(Ethernet.dnsServerIP());

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

