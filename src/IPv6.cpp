#include "net.h"

#include "IPv6.h"
#include "Arduino.h"

#include "ethernet.h"
#include "ICMPv6.h"
#include "ndp.h"

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

	switch (header->next_header) {
	case 0x3a: // ICMPv6
		
		ICMPv6::packetProcess(offset + IPv6_HEADER_LEN, SWAP_16_H_L(IPv6::header->payload_length));
		break;
	}
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

uint16_t IPv6::packetPrepare(uint16_t *dst_ip, uint8_t next_header, uint16_t length) {
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
	uint16_t* tmp = IPv6::header->src_ip;
	uint16_t* end;

	checksum += SWAP_16_H_L(tmp[0]);
	checksum += SWAP_16_H_L(tmp[1]);
	checksum += SWAP_16_H_L(tmp[2]);
	checksum += SWAP_16_H_L(tmp[3]);
	checksum += SWAP_16_H_L(tmp[4]);
	checksum += SWAP_16_H_L(tmp[5]);
	checksum += SWAP_16_H_L(tmp[6]);
	checksum += SWAP_16_H_L(tmp[7]);

	tmp = IPv6::header->dst_ip;

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

	for (tmp = payload, end = tmp + (length+1)/2; tmp < end; tmp++) {
		checksum += SWAP_16_H_L(*tmp);
	}

	checksum -= SWAP_16_H_L(correction);

	checksum += (checksum >> 16);

	return (uint16_t) ~checksum;
}
