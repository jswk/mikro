#include "net.h"

#include "ICMPv6.h"
#include "Arduino.h"

#include "IPv6.h"

uint8_t* ICMPv6::buffer = IPv6::buffer;

uint16_t generateChecksum(struct ICMPv6_header *header) {
	unsigned long checksum = 0UL;

	unsigned int length = SWAP_16_H_L(IPv6::header->payload_length);
	unsigned int* tmp = (uint16_t*) &IPv6::header->src_ip_h;
	unsigned int* end;

	checksum += SWAP_16_H_L(tmp[0]);
	checksum += SWAP_16_H_L(tmp[1]);
	checksum += SWAP_16_H_L(tmp[2]);
	checksum += SWAP_16_H_L(tmp[3]);
	checksum += SWAP_16_H_L(tmp[4]);
	checksum += SWAP_16_H_L(tmp[5]);
	checksum += SWAP_16_H_L(tmp[6]);
	checksum += SWAP_16_H_L(tmp[7]);

	tmp = (unsigned int*) &IPv6::header->dst_ip_h;

	checksum += SWAP_16_H_L(tmp[0]);
	checksum += SWAP_16_H_L(tmp[1]);
	checksum += SWAP_16_H_L(tmp[2]);
	checksum += SWAP_16_H_L(tmp[3]);
	checksum += SWAP_16_H_L(tmp[4]);
	checksum += SWAP_16_H_L(tmp[5]);
	checksum += SWAP_16_H_L(tmp[6]);
	checksum += SWAP_16_H_L(tmp[7]);

	checksum += length;

	checksum += 58UL;

	for (tmp = (unsigned int*) header, end = tmp + (length+1)/2; tmp < end; tmp++) {
		checksum += SWAP_16_H_L(*tmp);
	}

	checksum -= SWAP_16_H_L(header->checksum);

	checksum += (checksum >> 16);

	return (uint16_t) ~checksum;
}

void ICMPv6::packetProcess(uint16_t offset, uint16_t length) {
	struct ICMPv6_header *header = (struct ICMPv6_header*) (ICMPv6::buffer+offset);
#ifdef DEBUG_ICMPv6
	Serial.print("ICMPv6 Type: ");
	Serial.println(header->type);

	Serial.print("Code: ");
	Serial.println(header->code);

	Serial.print("Checksum:           0x");
	Serial.println(SWAP_16_H_L(header->checksum), 16);

	Serial.print("Generated checksum: 0x");
	Serial.println(generateChecksum(header), 16);
#endif
}
