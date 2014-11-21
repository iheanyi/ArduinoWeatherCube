/*
 * weather_reader.h
 *
 *  Created on: 05/03/2014
 *      Author: laso
 */
#include "http_client.h"

#ifndef WEATHER_READER_H_
#define WEATHER_READER_H_

namespace lasote {

struct weather{
	char description[30];
	float min_temp;
	float max_temp;
};


class WeatherClient {
	public:
		WeatherClient(Client& aClient, Stream& serial);
		virtual ~WeatherClient();
		bool get_weather(weather&, const char* city, int future_days=0);
	private:
		bool get_current_weather(weather&, const char* city);
		bool get_forecast(weather& we, const char* city, int future_days);
		char* get_current_weather_uri(const char* city);
		bool call_and_receive(char uri[], const char* queries[], weather& we);
		HttpClient* http_client;
		Stream& stream;

};

} /* namespace weather_reader */
#endif /* WEATHER_READER_H_ */
