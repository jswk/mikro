#include "net.h"

#include "ICMPv6.h"
#include "Arduino.h"

#include "IPv6.h"

uint8_t* ICMPv6::buffer = IPv6::buffer;

void ICMPv6::packetProcess(uint16_t offset, uint16_t length) {
#ifdef DEBUG_ICMPv6
	Serial.println(F("ICMP message received"));
#endif
}
