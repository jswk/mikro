#ifndef HTTP_H
#define HTTP_H

#include <Arduino.h>
#include "tcp.h"

class HTTP {
public:

	static void handler(TCP_handler_args* args);
};

#endif
