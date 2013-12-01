#include "net.h"

#include "IPv6.h"
#include "Arduino.h"

#include "ethernet.h"
#include "ICMPv6.h"

uint8_t* IPv6::buffer = Ethernet::buffer;
struct IPv6_header* IPv6::header;

void print_ip_to_serial(uint16_t* ip) {
	char sep = 0;
	int i;
	for (i = 0; i < 8; i++) {
		if (sep != 0) Serial.print(sep);
		Serial.print(SWAP_16_H_L(ip[i]), 16);
		sep = ':';
	}
}

void IPv6::packetProcess(uint16_t offset, uint16_t length) {
	IPv6::header = (struct IPv6_header*) (IPv6::buffer + offset);

#ifdef DEBUG_IPv6
	Serial.print("Src address: ");
	print_ip_to_serial((uint16_t*)&IPv6::header->src_ip_h);
	Serial.println();

	Serial.print("Dst address: ");
	print_ip_to_serial((uint16_t*)&IPv6::header->dst_ip_h);
	Serial.println();

	Serial.print("Payload length: ");
	Serial.println(SWAP_16_H_L(IPv6::header->payload_length));
#endif

	switch (header->next_header) {
	case 0x3a: // ICMPv6
		ICMPv6::packetProcess(offset + IPv6_HEADER_LEN, SWAP_16_H_L(IPv6::header->payload_length));
		break;
	}
}
