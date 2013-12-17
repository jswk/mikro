#include "net.h"

#include "IPv6.h"
#include "Arduino.h"

#include "ethernet.h"
#include "ICMPv6.h"
#include "ndp.h"

#include "debug.h"

uint8_t* IPv6::buffer = Ethernet::buffer;
struct IPv6_header* IPv6::header;

void cp_ip(uint16_t *to, uint16_t *from) {
	memcpy(to, from, 16);
}

void IPv6::packetProcess(uint16_t offset, uint16_t length) {
	IPv6::header = (struct IPv6_header*) (IPv6::buffer + offset);

#ifdef DEBUG_IPv6
	Serial.print("Src address: ");
	print_ip_to_serial(IPv6::header->src_ip);
	Serial.println();

	Serial.print("Dst address: ");
	print_ip_to_serial(IPv6::header->dst_ip);
	Serial.println();

	Serial.print("Payload length: ");
	Serial.println(SWAP_16_H_L(IPv6::header->payload_length));
#endif

	if (IPv6::filter(IPv6::header->dst_ip)) {
		return;
	}

	switch (header->next_header) {
	case 0x3a: // ICMPv6
		
		ICMPv6::packetProcess(offset + IPv6_HEADER_LEN, SWAP_16_H_L(IPv6::header->payload_length));
		break;
	}
}

bool IPv6::filter(uint8_t *ip) {
	if (ip[0] == 0xFF) {
		return false;
	}

	uint8_t i;
	for (i = 0; i < 16; i++) {
		if (ip[i] != NDP::IP[i]) {
			return true;
		}
	}

	return false;
}

void IPv6::prepareAnswer() {
	//IPv6::header->dst_ip_h = IPv6::header->src_ip_h;//ustawienie nadawcy solicitation jako odbiorcy tego
	//IPv6::header->dst_ip_l = IPv6::header->src_ip_l;
	
	//IPv6::header->src_ip_h = this_ip_h;
	//IPv6::header->src_ip_l = this_ip_l;
	
	uint8_t* dest = (uint8_t*)malloc(sizeof(uint8_t));
	Ethernet::getSrcMAC(dest);
	Ethernet::packetPrepare(dest, Ethernet::getTypeLen());
	Ethernet::packetSend(Ethernet::getTypeLen());
}

uint16_t IPv6::packetPrepare(uint8_t *dst_ip, uint8_t next_header, uint16_t length) {
	memcpy(header->dst_ip, dst_ip, 16);
	memcpy(header->src_ip, NDP::IP, 16);
	header->payload_length = SWAP_16_H_L(length);
	header->next_header = next_header;
	uint8_t *dst_mac = NDP::getMAC(dst_ip);
	return Ethernet::packetPrepare(dst_mac, 0x86DD) + IPv6_HEADER_LEN;
}

void IPv6::packetSend(uint16_t length) {
	Ethernet::packetSend(length);
}


uint16_t IPv6::generateChecksum(uint16_t correction) {
	uint32_t checksum = 0UL;

	uint16_t *payload = (uint16_t *) (((uint8_t *) IPv6::header) + IPv6_HEADER_LEN);
	uint16_t length = SWAP_16_H_L(IPv6::header->payload_length);
	uint16_t* tmp;
	uint8_t* ip = IPv6::header->src_ip;
	uint16_t* end;
	uint8_t i;

	for (i = 0; i < 8; i++) {
		checksum += BIG_ENDIAN_JOIN(ip[2*i], ip[2*i+1]);
	}

	ip = IPv6::header->dst_ip;

	for (i = 0; i < 8; i++) {
		checksum += BIG_ENDIAN_JOIN(ip[2*i], ip[2*i+1]);
	}

	checksum += length;

	checksum += IPv6::header->next_header;

	for (tmp = payload, end = tmp + (length+1)/2; tmp < end; tmp++) {
		checksum += SWAP_16_H_L(*tmp);
	}

	checksum -= SWAP_16_H_L(correction);

	checksum += (checksum >> 16);

	return (uint16_t) ~checksum;
}
