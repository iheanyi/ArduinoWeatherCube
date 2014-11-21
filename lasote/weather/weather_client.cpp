/*
 * weather_reader.cpp
 *
 *  Created on: 05/03/2014
 *      Author: laso
 */
#include "lasote/weather/weather_client.h"
#include "lasote/stream_processor/stream_json_processor.h"
#include <stdlib.h>     /* atoi */

#define LOG_OUTPUT false

namespace lasote {

	WeatherClient::WeatherClient(Client& aClient, Stream& serial) : stream(serial) {
		http_client = new HttpClient(aClient, serial);
	}

	WeatherClient::~WeatherClient() {
		delete http_client;
	}

	bool WeatherClient::get_weather(weather& w, const char* city, int future_days){
		if(future_days < 1){
			return get_current_weather(w, city);
		}
		else{
			return get_forecast(w, city, future_days);
		}
	}

	bool WeatherClient::get_current_weather(weather& we, const char* city){
		 /**
		  * {"coord":{"lon":-3.7,"lat":40.42},
		  *  "sys":{"message":0.0447,"country":"ES","sunrise":1396245552,"sunset":1396291108},
		  *  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04d"}],
		  *  "base":"cmc stations",
		  *  "main":{"temp":12.1,"humidity":63,"pressure":1016,"temp_min":11,"temp_max":14.44},
		  *  "wind":{"speed":2.06,"gust":4.11,"deg":153},
		  *  "clouds":{"all":56},
		  *  "dt":1396263911,
		  *  "id":3117735,
		  *  "name":"Madrid",
		  *  "cod":200}
		  */
		 char tmp[strlen(city) + 35];
		 tmp[0] = '\0';
		 strcat_P(tmp, PSTR("/data/2.5/weather?q="));
	  	 strcat(tmp, city);
		 strcat_P(tmp, PSTR("&units=metric"));
		 const char* queries[] = { "weather.0.description", "main.temp_min", "main.temp_max" };
		 return call_and_receive(tmp, queries, we);
	};

	bool WeatherClient::get_forecast(weather& we, const char* city, int future_days){
			 /**
			  * {
				   "cod":"200",
				   "message":0.0047,
				   "city":{
					  "id":1851632,
					  "name":"Shuzenji",
					  "coord":{
						 "lon":138.933334,
						 "lat":34.966671
					  },
					  "country":"JP",
					  "population":0
				   },
				   "cnt":10,
				   "list":[
					  {
						 "dt":1396317600,
						 "temp":{
							"day":287.73,
							"min":275.35,
							"max":287.73,
							"night":275.35,
							"eve":283.86,
							"morn":287.73
						 },
						 "pressure":986.76,
						 "humidity":37,
						 "weather":[
							{
							   "id":801,
							   "main":"Clouds",
							   "description":"few clouds",
							   "icon":"02d"
							}
						 ],
						 "speed":2.06,
						 "deg":121,
						 "clouds":24
					  },
				   ]
				}
			  */

			 char tmp_days[4];
			 sprintf(tmp_days, "%d", future_days);

			 char uri[strlen(city) + 50];
			 uri[0] = '\0';
			 strcat_P(uri, PSTR("/data/2.5/forecast/daily?q="));
		  	 strcat(uri, city);
			 strcat_P(uri, PSTR("&units=metric"));
			 strcat_P(uri, PSTR("&cnt="));
			 strcat(uri, tmp_days);



			 sprintf(tmp_days, "%d", future_days - 1); //for array

			 const char* queries[3];
			 char query_0[27 + strlen(tmp_days) + 1]; query_0[0] = '\0';
			 strcat_P(query_0, PSTR("list."));
			 strcat(query_0, tmp_days);
			 strcat_P(query_0, PSTR(".weather.0.description"));

			 char query_1[14 + strlen(tmp_days) + 1]; query_1[0] = '\0';
			 strcat_P(query_1, PSTR("list."));
			 strcat(query_1, tmp_days);
			 strcat_P(query_1, PSTR(".temp.min"));

			 char query_2[14 + strlen(tmp_days) + 1]; query_2[0] = '\0';
			 strcat_P(query_2, PSTR("list."));
			 strcat(query_2, tmp_days);
			 strcat_P(query_2, PSTR(".temp.max"));


			 //Serial.println(uri);

			 queries[0] = query_0;
			 queries[1] = query_1;
			 queries[2] = query_2;

			 return call_and_receive(uri, queries, we);
		};

	bool WeatherClient::call_and_receive(char uri[], const char* queries[], weather& we){

		http_client->processors.reset();
		http_client->reset();

		StreamJsonProcessor processor(queries, 3, 20, 30, 30);
		http_client->processors.add_item(&processor);

		uint8_t connected = http_client->get("api.openweathermap.org", uri);
		if(connected){
			stream.println(F("Connected ok!"));
			int http_code = http_client->receive();
			stream.println(http_code);
			if(http_code==200){ //OK!
				if(processor.finished()){
					strcpy(we.description, processor.results[0]);
					we.min_temp = atof(processor.results[1]);
					we.max_temp = atof(processor.results[2]);
					return true;
				}
				else{
					stream.println(F("Return JSON not detected or queries not found!"));
					return false;
				}
			}
			else{
				stream.println(F("Error receiving!"));
				return false;
			}
		}
		else{
			stream.println(F("Error connecting weather client"));
			return false;
		}
		return false;
	}

} /* namespace weather_reader */
