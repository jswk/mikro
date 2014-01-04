#include "net.h"

#include "http.h"
#include "Arduino.h"

#include "tcp.h"

#include "debug.h"

prog_char http_index[] PROGMEM =
		"<html>"
			"<head>"
				"<title>Arduino Board</title>"
				"<script type=\"text/javascript\" src=\"http://jswk.pl/projekty/arduino/main.js\"></script>"
			"</head>"
			"<body></body>"
		"</html>";

prog_char http_newline[] PROGMEM = "\r\n";
prog_char http_200[] PROGMEM = "HTTP/1.1 200 OK\r\n";
prog_char http_h_content_type[] PROGMEM = "Content-Type: text/html; charset=UTF-8\r\n";
prog_char http_h_content_length[] PROGMEM = "Content-Length: %i\r\n";

char HTTP::buffer[240];

void HTTP::handler(TCP_handler_args* args) {
#ifdef DEBUG_HTTP
	Serial.println(args->length);
	uint16_t i = 0;
	while (i < args->length) {
		Serial.print((char)args->data[i]);
		i++;
	}
#endif

	char* s;

	memset(buffer, 0, sizeof(buffer));
	strcat_P(buffer, http_200);
	strcat_P(buffer, http_h_content_type);
	s = buffer;
	while (*++s);
	sprintf_P(s, http_h_content_length, strlen_P(http_index));
	strcat_P(buffer, http_newline);
	strcat_P(buffer, http_index);

	TCP::send(args->status, (uint8_t*)buffer, strlen(buffer));
}
