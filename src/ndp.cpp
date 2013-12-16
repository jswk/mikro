#include "net.h"

#include "ndp.h"
#include "Arduino.h"

static uint8_t broadcast[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

uint8_t* NDP::getMAC(uint16_t *ip) {
	return broadcast;
}
