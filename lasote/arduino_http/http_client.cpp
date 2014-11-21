/*
 * http_client.cpp
 *
 *  Created on: 28/03/2014
 *      Author: laso
 */

#include "lasote/arduino_http/http_client.h"
#include "lasote/stringutils/cstringutils.h"
#include <stdio.h>
#include <avr/pgmspace.h>

using namespace cstringutils;

//#define DEBUGGING 1
#define GET "GET"
#define POST "POST"

#ifdef DEBUGGING
	#define print_line(...)  stream.println (__VA_ARGS__)
	#define print_string(...) stream.print (__VA_ARGS__)
#else
	#define print_line(...) 0
	#define print_string(...) 0
#endif

namespace lasote {


	Header::Header(const char *name, const char* value) : name(name), value(value){

	}
	Header::~Header(){

	}


	HttpClient::HttpClient(Client& aClient, Stream& stream) : aClient(aClient), stream(stream){
	}

	HttpClient::~HttpClient() {
		reset();
	}

	void HttpClient::reset(){
		//Deletes custom headers and reset processors
		for(int i=0; i < custom_headers.length; i++){
			delete custom_headers[i];
		}
		custom_headers.reset(); //empty list
		for(int i=0; i < processors.length; i++){
			processors[i]->reset();
		}
	}


	void HttpClient::add_custom_header(const char* name, const char* value){
		Header* custom_header = new Header(name, value);
		custom_header->name = name;
		custom_header->value = value;
		custom_headers.add_item(custom_header);
	}

	void HttpClient::send_header(const char* header_name,const char* header_value){
		aClient.print(header_name);
		aClient.print(": ");
		aClient.println(header_value);
	}

	void HttpClient::send_all_headers(const char* method, const char* host, const char* uri){

		const char http[] = " HTTP/1.1";
		const char labelhost[] = "Host: ";
		const char conn[] = "Connection: close";
		print_string((char*) method);
		aClient.print(method);
		print_string(" ");
		aClient.print(" ");
		print_string((char*)uri);
		aClient.print(uri);
		print_line(http);
		aClient.println(http);
		print_string(labelhost);
		aClient.print(labelhost);
		print_line((char*)host);
		aClient.println(host);
		print_line((char*)conn);
		aClient.println(conn);

		//Add custom headers
		for(int i=0; i < custom_headers.length; i++){
			Header* header = custom_headers[i];
			print_string(header->name);
			print_string(": ");
			print_line(header->value);
			send_header(header->name, header->value);
		}

		print_line("Headers sent");
	}

	void HttpClient::end_headers(){
		aClient.write(0x0D); aClient.write(0x0A);
	}

	void HttpClient::end_request(){
		aClient.write(0x0D); aClient.write(0x0A);
		aClient.flush();
	}

	uint8_t HttpClient::get(IPAddress ip, const char *uri, uint16_t port){
		uint8_t connected = aClient.connect(ip, port);
		if(aClient.connected()){
			char host[17];
			ip_to_string(ip, host);
			send_all_headers(GET, host, uri);
			end_request();
		}
		return connected;
	}

	uint8_t HttpClient::get(const char *host, const char* uri, uint16_t port){
		uint8_t connected = aClient.connect(host, port);
		if(aClient.connected()){
			send_all_headers(GET, host, uri);
			end_request();
		}
		return connected;
	}

	uint8_t HttpClient::post(IPAddress ip, const char *uri, const char *body, uint16_t port){
		uint8_t connected = aClient.connect(ip, port);
		if(aClient.connected()){
			char host[17];
			ip_to_string(ip, host);
			send_all_headers(POST, host, uri);
			send_body(body);
			print_line(F("enviado"));
		}
		return connected;
	}

	uint8_t HttpClient::post(const char *host, const char* uri, const char *body, uint16_t port){
		uint8_t connected = aClient.connect(host, port);
		if(aClient.connected()){
			send_all_headers(POST, host, uri);
			send_body(body);
			print_line(F("enviado"));
		}
		return connected;
	}


	void HttpClient::send_body(const char* body){
		char buff[10];
		int_to_string(strlen(body), buff);
		send_header("Content-Length", buff);
		end_headers();
		aClient.write((const uint8_t*) body, strlen(body));
		//end_request();
	}

	void HttpClient::send_body(Stream& body){
		//Chunk size fixed to 20, TODO make configurable
		//Write header chunked before body
		send_header("Transfer-Encoding", "chunked");
		end_headers();
		static short int chunk_max_size = 20;

		char buff[chunk_max_size];
		char chunk_size_hex[5];

		while(1){
		    short int received = body.readBytes((char *)buff, chunk_max_size);
		    sprintf(&chunk_size_hex[0], "%04x", received);

		    aClient.write((const uint8_t*) chunk_size_hex, 4); aClient.write(0x0D); aClient.write(0x0A);
		    aClient.write((const uint8_t*) buff, received); aClient.write(0x0D); aClient.write(0x0A);
			if(received < chunk_max_size){
				break;
			}
		}
		aClient.write((const uint8_t*) "0", 1); aClient.write(0x0D); aClient.write(0x0A);
		end_request();
	}


	void HttpClient::ip_to_string(IPAddress ip, char* buffer){
		buffer[0] = '\0';
		for(int i=0; i<4; i++){
			char tmp[4];
			int_to_string(ip[i], (char*) &tmp);
			if(i > 0){
				append_to_string(buffer, '.', 16);
			}
			for(int j=0; j < strlen(tmp); j++){
				append_to_string(buffer, tmp[j], 16);
			}
		}
	}

	uint8_t HttpClient::wait_for_response(unsigned int miliseconds){
		int next = millis() + miliseconds;

		while(aClient.connected() && !aClient.available()){
		  int tmp = next - millis();
		  if (tmp < 0){
			return RESPONSE_TIMEOUT;
		  }
		}
		if(!aClient.connected()){
			print_line(F("Not connected!"));
			return LOST_CONNECTION;
		}
		return true;
	}


	short int HttpClient::receive(){
		unsigned char c[10];
		//TODO let configure params
		unsigned int network_delay = 3 * 1000; // 3 seconds
		unsigned int response_delay = 1000 * 4; // 4 seconds
		// Whilst we haven't timed out & haven't reached the end of the body
		print_line(F("Wait for receive..."));
		aClient.flush();
		if(!wait_for_response(response_delay)){
			print_line(F("Response timeout!"));
			aClient.stop();
			return RESPONSE_TIMEOUT;
		}

		HttpResponseProcessor http_response_processor;

		while(aClient.connected()){

			if(!wait_for_response(response_delay)){
				print_line(F("Response timeout on BODY!"));
				return RESPONSE_TIMEOUT;
			}
			int size = aClient.read((uint8_t*) c, 10);
			for(int i=0;i<size;i++){
				http_response_processor.process_char(c[i]);
				if(http_response_processor.finished()){
					break;
				}
				else if(http_response_processor.body_char){

					//Body char, run the processors
					//print_string((char)c);
					for(int j=0; j < processors.length; j++){
						IStreamProcessor* processor = processors[j];
						processor->process_char(c[i]);
					}
				}
				else{
					print_string((char)c[i]);
				}
			}

			if(http_response_processor.finished()){
				break;
			}
		}

		if(!http_response_processor.finished()){
			print_line(F("**Bad request"));
			aClient.stop();
			return -1;
		}

		aClient.stop();

		return http_response_processor.status_code;
	}
} /* namespace arduino_http_client */
