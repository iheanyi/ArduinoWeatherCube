#include "Arduino.h"
#include "ntruchsess/arduino_uip/uipethernet.h"
#include <lasote/weather/weather_client.h>
#include <LiquidCrystal.h>

#define STATUS_PIN 13
#define DELAY_SECONDS 5//Time between calls to api

using namespace lasote;

//Our hardware adapter is Enc28J60 HanRun module
EthernetClient client;
//Instance a weather client for check weather
WeatherClient weather_client(client, Serial);
//Init lcd
LiquidCrystal lcd(2, 3, 8, 9, 10, 11);

bool get_weather(const char* city, int future_days){
	//reset previous status for a clean request
	Serial.println(F("Lets check weather!"));

	//weather object
	weather w;
	bool ret = weather_client.get_weather(w, city, future_days);
	if(ret){
		Serial.println(w.description); Serial.print(F("Max temp:"));
		Serial.println(w.max_temp);  Serial.print(F("Min temp:"));
		Serial.println(w.min_temp);

		lcd.clear();
		lcd.setCursor(0, 0);
		if(strlen(w.description) > 16){
			w.description[15] = '\0';
		}
		lcd.print(w.description);
		lcd.setCursor(0, 1);
		lcd.print(w.min_temp);
		lcd.print("-");
		lcd.print(w.max_temp);
	}
	else{
		lcd.print(w.description);
		lcd.setCursor(0, 1);
		lcd.print("ERROR");
	}
}



void setup() {
	Serial.begin(9600);
	Serial.println(F("***********SETUP****************"));

	//MAC init
	uint8_t mac[6] = {0x10,0x31,0x02,0x33,0x04,0x65};
	Ethernet.begin(mac);

	pinMode(STATUS_PIN, OUTPUT);

	// initialize the library with the numbers of the interface pins
	lcd.begin(16, 2);
	delay(4000);
}


void loop() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.write("Now...");
	//For today
	get_weather("Madrid", 0);

	delay(DELAY_SECONDS * 1000);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.write("Tomorrow...");
	//For tomorrow
	get_weather("Madrid", 1); //Tomorrow


	delay(DELAY_SECONDS * 1000);

}

