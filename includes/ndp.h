#ifndef NDP_H
#define NDP_H

#include <Arduino.h>

class NDP {
public:
	static uint16_t *IP;
	static uint8_t* getMAC(uint16_t *ip);
};

#endif
