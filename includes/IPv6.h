#ifndef IPv6_H
#define IPv6_H

#define IPv6_H

#include <Arduino.h>

struct IPv6_header {
	uint32_t first4octets;
	uint16_t payload_length;
	uint8_t next_header;
	uint8_t hop_limit;
	uint64_t src_ip_h;
	uint64_t src_ip_l;
	uint64_t dst_ip_h;
	uint64_t dst_ip_l;
};

class IPv6 {
public:
	static uint8_t* buffer;
	static uint16_t* address;

	static void packetProcess(uint16_t offset, uint16_t length);

};

#endif
