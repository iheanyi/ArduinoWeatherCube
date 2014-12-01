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

int rPin = 9; //red
int gPin = 6; //green
int bPin = 10; //blue

int temperature = 100;
boolean hasPrecipitation = false;
//JsonParser<64> parser;

void setup() {
  Bridge.begin();
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);
  setupLightPins();
  
  Serial.println("Testing Yun");
  //delay(1000);
  
  //runCurl();
}

void runCurl() {
  Process p;
  p.begin("curl");
  p.addParameter("http://api.openweathermap.org/data/2.5/weather?q=South%20Bend,IN"); // Add the URL parameter to "curl"
  //p.addParameter("http://google.com");
  String weatherString = "";
  p.run();  while (p.available()>0) {
    char c = p.read();
    Serial.print(c);
    //weatherString += c;
  }
  
  Serial.println(weatherString);
  Serial.println(weatherString.length());
  Serial.flush();
  
  /*char jsonBuffer[1024];
  
  weatherString.toCharArray(jsonBuffer, 1024);
  Serial.println(jsonBuffer);*/
  Serial.println("Buffer printed CURL.");
}

void getWeatherInfo() {
  HttpClient weatherClient;
  weatherClient.get("http://api.openweathermap.org/data/2.5/weather?q=South%20Bend,IN");
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
  Serial.println("Attempting to print out jsonBuffer.");
  
  
  weatherString.toCharArray(jsonBuffer, 600);
  weatherString = "";
  Serial.println(jsonBuffer);
  Serial.println("Buffer printed.");
  
  Serial.println("Attempting to parse JSON.");
  
  StaticJsonBuffer<600> weatherBuffer;
  JsonObject& root = weatherBuffer.parseObject(jsonBuffer);
  
  if(!root.success()) {
   Serial.println("Parsing failed.");
   return;
  } else {
    //free(jsonBuffer);
  }
  
  Serial.println("Parsing successful!");
  const char* weather = root["weather"];
  
  root.prettyPrintTo(Serial);
  //free(weatherString);
  //parseJSON(weatherString);

}

/*void parseJSON(String jsonString) {
  char jsonBuffer[1024];

  Serial.println(jsonString);
  Serial.println("IN the parsing of the JSON.");
  jsonString.toCharArray(jsonBuffer, 1024);
  JsonObject root = parser.parse(jsonBuffer);
  Serial.println(jsonBuffer);
  if(!root.success()) {
   Serial.println("Error parsing JSON."); 
  }
  Serial.println("made it.");
  Serial.println(jsonBuffer);
  Serial.println("Buff");
  
  for (JsonObjectIterator i=root.begin(); i!=root.end(); ++i) {
    Serial.println("Parsing the iterator.");
    Serial.println(i.key());
    Serial.println((char*)i.value());
  }
  
}*/


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
  analogWrite(gPin, green);
  analogWrite(bPin, blue);
  
}

void setupLightPins() {
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  setColor(0,0,0);
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
