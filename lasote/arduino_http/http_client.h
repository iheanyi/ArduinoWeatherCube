/*
 * http_client.h
 *
 *  Created on: 28/03/2014
 *      Author: laso
 */

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <Arduino.h>
#include <IPAddress.h>
#include "Client.h"
#include "lasote/linked_list/linked_list.h"
#include "lasote/http_response_processor/http_response_processor.h"

#define CANT_CONNECT -1
#define RESPONSE_TIMEOUT -2
#define LOST_CONNECTION -3
#define BAD_BODY -4
#define OK 1


namespace lasote {

	class Header{
	public:
		Header(const char *name,const char* value);
		~Header();
		const char* name;
		const char* value;
	};


	class HttpClient {
		public:
			HttpClient(Client& aClient, Stream& stream);
			~HttpClient();
			uint8_t get(const char *host, const char *uri="/", uint16_t port=80);
			uint8_t get(IPAddress ip, const char *uri="/", uint16_t port=80);

			//Body direct
			uint8_t post(const char *host, const char *uri, const char* body, uint16_t port=80);
			uint8_t post(IPAddress ip, const char *uri,  const char* body, uint16_t port=80);

			//Body streamed
			uint8_t post(const char *host, const char *uri, Stream& body, uint16_t port=80);
			uint8_t post(IPAddress ip, const char *uri, Stream& body, uint16_t port=80);

			//Custom headers helper
			void add_custom_header(const char* name, const char* value);

			//Receive response
			short int receive();

			//Reset all
			void reset();

			Client& aClient;
			LinkedList<IStreamProcessor*> processors;
			LinkedList<Header*> custom_headers;

		protected:
			uint8_t wait_for_response(unsigned int miliseconds);
			Stream& stream;

			void send_header(const char* header_name, const char* header_value);
			void send_all_headers(const char* method, const char* host, const char* header_value);
			void send_body(const char* body);
			void send_body(Stream& body);
			void end_request();
			void end_headers();


		private:
			void ip_to_string(IPAddress ip, char* buffer);
	};

} /* namespace arduino_http_client */
#endif /* HTTP_CLIENT_H_ */
