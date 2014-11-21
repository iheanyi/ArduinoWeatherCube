#include "Arduino.h"
#include "ntruchsess/arduino_uip/uipethernet.h"
#include "lasote/arduino_http/http_client.h"
#include "lasote/stream_finder_processor/stream_finder_processor.h"
#include "lasote/stream_recorder/stream_recorder.h"

using namespace lasote;

EthernetClient client;
char response_buffer[20];
StreamRecorderProcessor response_recorder(response_buffer, 20); //Capture response in buffer
HttpClient http_client(client, Serial);
int led = 12;

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	Serial.println("************* SETUP ****************");
    pinMode(led, OUTPUT);     // initialize the digital pin as an output.
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    Ethernet.begin(mac);
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
