#include <SPI.h>
#include <WiFi.h>
//#include <JsonParser.h>
#include <HttpClient.h>
#include <Process.h>
#include <ArduinoJson.h>


//#define COMMON_ANODE

//using namespace ArduinoJson;
//using namespace lasote;


/* &APPID=53d2069076934085f5c518c513aee867 */


// Initialize client library

//int rPin = 9; //red
int rPin = 3; // Red Pin for Max Temp. 6 on board.

//int gPin = 6; //green
//int bPin = 10; //blue. 

int bPin = 5; // 9 on board.

int rPin2 = 6; // Red Pin for Current Temp; 14 on board
int bPin2 = 9; // Blue Pin for Current Temp; 17 on board

int rPin3 = 10; // Red Pin for Min Temp
int bPin3 = 11; // Blue Pin for Min Temp

int temperature = 100;
boolean hasPrecipitation = false;

//JsonParser<64> parser;
void setup() {
  Bridge.begin();
  //Initialize serial and wait for port to open:
  setupLightPins();
  Serial.begin(9600);
  while (!Serial);
  
  
  Serial.println("Testing Yun");
  //delay(1000);
}

void getWeatherInfo() {
  HttpClient weatherClient;
  weatherClient.get("http://api.openweathermap.org/data/2.5/weather?q=South%20Bend,IN&units=imperial&APPID=53d2069076934085f5c518c513aee867");
  String weatherString = "";
  
  while(weatherClient.available()) {
    char c = weatherClient.read();
    weatherString += c;
  }
  
  Serial.flush();
  Serial.println(weatherString);
  Serial.println(weatherString.length());
  int buffer_size = weatherString.length();
  
  char jsonBuffer[buffer_size];
  Serial.println(F("Attempting to print out jsonBuffer."));
  
  
  weatherString.toCharArray(jsonBuffer, buffer_size);
  weatherString = "";
  Serial.println(jsonBuffer);
  Serial.println(F("Buffer printed."));
  
  Serial.println(F("Attempting to parse JSON."));
  
  StaticJsonBuffer<500> weatherBuffer;
  JsonObject& root = weatherBuffer.parseObject(jsonBuffer);

  if(!root.success()) {
   Serial.println(F("Parsing failed."));
   return;
  } else {
    //free(jsonBuffer);
  }
  
  JsonVariant &main = root.at("main");
  JsonVariant &weather = root.at("weather");
  
  Serial.println(F("Parsing successful!"));  
  //root.prettyPrintTo(Serial);
  weather[0].prettyPrintTo(Serial);
  Serial.println();
  main.prettyPrintTo(Serial);
  Serial.println();
  const char* weatherDescription = weather[0]["description"];
  Serial.println(weatherDescription);
  
  float weatherTemperature = (float) main["temp"];
  float maxTemperature = (float) main["temp_max"] * 1.0;
  float minTemperature = (float) main["temp_min"] * 1.0;

  Serial.println(maxTemperature);
  Serial.println(weatherTemperature);
  Serial.println(minTemperature);
  
  if(maxTemperature <= 40) {
    setMaxColor(0, 255);
  } else if(maxTemperature > 40 && maxTemperature < 70) {
    setMaxColor(255, 255);
  } else {
    setMaxColor(255, 0); 
  }
  
  if(weatherTemperature <= 40) {
    setCurrentColor(0, 255);
  } else if(weatherTemperature > 40 && weatherTemperature < 70) {
    setCurrentColor(255, 255);
  } else {
    setCurrentColor(255, 0); 
  }
  
  
  if(minTemperature <= 40) {
    setMinColor(0, 255);
  } else if(minTemperature > 40 && minTemperature < 70) {
    setMinColor(255, 255);
  } else {
    setMinColor(255, 0); 
  }
}

void loop() {
  getWeatherInfo();
  delay(5000);
  
  //colorSwitchingStuff();
}

void setColor(int red, int green, int blue) {
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  
  analogWrite(rPin, red);
  //analogWrite(gPin, green);
  analogWrite(bPin, blue);
  
}

void colorTest() {
 setColor(0,0,0);
 delay(500);
 setColor(255,0,0);
 delay(500);
 setColor(0,255,0);
 delay(500);
 setColor(0,0,255); 
}
void updateColors(double maxTemp, double currentTemp, double minTemp) {
  
  
}

void setupLightPins() {
  pinMode(rPin, OUTPUT);  
  pinMode(bPin, OUTPUT);
  
  pinMode(rPin2, OUTPUT);
  pinMode(bPin2, OUTPUT);
  
  pinMode(rPin3, OUTPUT);
  pinMode(bPin3, OUTPUT);

  
  setMaxColor(255, 0);
  setCurrentColor(255, 0);
  setMinColor(255,0);
}

void setMaxColor(int red, int blue) {
  analogWrite(rPin, red);
  analogWrite(bPin, blue);
}

void setCurrentColor(int red, int blue) {
  analogWrite(rPin2, red);
  analogWrite(bPin2, blue);
}

void setMinColor(int red, int blue) {
  analogWrite(rPin3, red);
  analogWrite(bPin3, blue);
}

void colorSwitchingStuff() {
  
  if (temperature <= 32) {
   setColor(0, 0, 255); 
  } else if (temperature > 32 && temperature < 75) {
   setColor(255, 255, 0); 
  } else {
   setColor(255, 0, 0); 
  }
  
  if(hasPrecipitation) {
   delay(300);
   setColor(0, 0, 0); 
   delay(100);
  }
  
  delay(500);
  /*if(temperature > 0) {
  }*/
  
  temperature -= 10;

  if(temperature <= 32) {
    hasPrecipitation = true;
  }
  
  if(temperature < -50) {
    temperature = 100;
    hasPrecipitation = false; 
  }
  
  delay(500);
}
