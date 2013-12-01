#ifndef ETH_H
#define ETH_H

#define ETH_H

#include <Arduino.h>

class Ethernet {
public:
	static uint8_t* buffer;
	static uint8_t* MAC;
	static uint16_t packetPrepare(const uint8_t* dest_mac, uint16_t typelen);
	static void packetSend(uint16_t length);
	static void packetProcess(uint16_t length);

	static void getSrcMAC(uint8_t* mac);

	static uint16_t getTypeLen();
};

#endif
