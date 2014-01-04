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
prog_char http_e_404[] PROGMEM =
		"<html>"
			"<head>"
				"<title>Arduino Board</title>"
			"</head>"
			"<body><h1>404</h1></body>"
		"</html>";

prog_char http_newline[] PROGMEM = "\r\n";
prog_char http_200[] PROGMEM = "HTTP/1.1 200 OK\r\n";
prog_char http_404[] PROGMEM = "HTTP/1.1 404 Not Found\r\n";
prog_char http_h_content_type[] PROGMEM = "Content-Type: text/html; charset=UTF-8\r\n";
prog_char http_h_content_length[] PROGMEM = "Content-Length: %i\r\n";

prog_char http_r_index[] PROGMEM = "GET / ";
prog_char http_r_init[] PROGMEM = "POST /init/";
prog_char http_r_read[] PROGMEM = "GET /pin/";
prog_char http_r_write[] PROGMEM = "POST /pin/";

char HTTP::buffer[240];

void serveIndex(struct TCP_handler_args* args) {
	char* s;
	char* buffer = HTTP::buffer;

	buffer[0] = 0;
	strcat_P(buffer, http_200);
	strcat_P(buffer, http_h_content_type);
	s = buffer;
	while (*++s);
	sprintf_P(s, http_h_content_length, strlen_P(http_index));
	strcat_P(buffer, http_newline);
	strcat_P(buffer, http_index);

	TCP::send(args->status, (uint8_t*)buffer, strlen(buffer));
}

void serveInit(struct TCP_handler_args* args) {}
void serveWrite(struct TCP_handler_args* args) {}
void serveRead(struct TCP_handler_args* args) {}

void serve404(struct TCP_handler_args* args) {
	char* s;
	char* buffer = HTTP::buffer;

	buffer[0] = 0;
	strcat_P(buffer, http_404);
	strcat_P(buffer, http_h_content_type);
	s = buffer;
	while (*++s);
	sprintf_P(s, http_h_content_length, strlen_P(http_e_404));
	strcat_P(buffer, http_newline);
	strcat_P(buffer, http_e_404);

	TCP::send(args->status, (uint8_t*)buffer, strlen(buffer));
}

void HTTP::handler(struct TCP_handler_args* args) {
#ifdef DEBUG_HTTP
	Serial.println(args->length);
	uint16_t i = 0;
	while (i < args->length) {
		Serial.print((char)args->data[i]);
		i++;
	}
#endif

	if (memcmp_P(args->data, http_r_index, strlen_P(http_r_index)) == 0) {
		serveIndex(args);
	} else if (memcmp_P(args->data, http_r_init, strlen_P(http_r_init)) == 0) {
		serveInit(args);
	} else if (memcmp_P(args->data, http_r_read, strlen_P(http_r_read)) == 0) {
		serveRead(args);
	} else if (memcmp_P(args->data, http_r_write, strlen_P(http_r_write)) == 0) {
		serveWrite(args);
	} else {
		serve404(args);
	}

}
