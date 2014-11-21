# <img src="https://s3-eu-west-1.amazonaws.com/biipublicimage/lasote/guybrush_point.gif" width="50"> lasote/weather

### What is it?
An Arduino client to check an online [weather service][1].
It uses [`lasote/arduino_http`][2] block for access service, so it can be used with any hardware adaptor.
WeatherClient can check current weather and forecast of X future days.

### How can i use it?
* Instance hardware client and init weather_client
* Setup hardware 
* Call to get_weather


        //Our hardware adapter is Enc28J60 HanRun module
        EthernetClient client;
        
        //Instance a weather client for check weather
        WeatherClient weather_client(client, Serial);
        
        //LOOP
        bool ret = weather_client.get_weather(w, city, future_days);
        if(ret){
            Serial.println(w.description); Serial.print(F("Max temp:"));
            Serial.println(w.max_temp);  Serial.print(F("Min temp:"));
            Serial.println(w.min_temp);
        }
        
Check the full example in examples/main.cpp file!

Enjoy it!!!


  [1]: http://openweathermap.org/
  [2]: https://www.biicode.com/lasote/blocks/lasote/arduino_http/branches/master
