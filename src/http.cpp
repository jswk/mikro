#include "net.h"

#include "http.h"
#include "Arduino.h"

#include "tcp.h"

#include "debug.h"

void HTTP::handler(TCP_handler_args* args) {
#ifdef DEBUG_HTTP
	Serial.println(args->length);
	uint16_t i = 0;
	while (i < args->length) {
		Serial.print((char)args->data[i]);
		i++;
	}
#endif

	char* data =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=UTF-8\r\n"
			"Content-Length: 82\r\n"
			"\r\n"
			"<html><head><title>Woohoo!</title></head><body><h1>Hello World!</h1></body></html>";

	TCP::send(args->status, data);
}
