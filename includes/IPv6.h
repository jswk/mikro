#ifndef IPv6_H
#define IPv6_H

#include <Arduino.h>

struct IPv6_header {
	uint32_t first4octets;
	uint16_t payload_length;
	uint8_t next_header;
	uint8_t hop_limit;
	uint16_t src_ip[8];
	uint16_t dst_ip[8];
};

class IPv6 {
public:

	static uint8_t* buffer;
	static uint16_t* address;
	static struct IPv6_header* header;

	static void packetProcess(uint16_t offset, uint16_t length);
	static uint16_t generateChecksum(uint16_t correction);
	static uint16_t packetPrepare(uint16_t *dst_ip, uint8_t next_header, uint16_t length);
	static void packetSend(uint16_t length);
	static void prepareAnswer();
};

#endif
