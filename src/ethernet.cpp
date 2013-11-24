#include "ethernet.h"
#include "Arduino.h"

#include "enc28j60.h"
#include "net.h"

uint8_t* Ethernet::buffer = ENC28J60::buffer;
uint8_t* Ethernet::MAC = ENC28J60::MAC;

static void setMACs(const uint8_t* destMAC) {
	uint8_t i;
	for (i = 0; i < 6; i++) {
		Ethernet::buffer[ETH_DST_MAC+i] = destMAC[i];
	}

	for (i = 0; i < 6; i++) {
		Ethernet::buffer[ETH_SRC_MAC+i] = Ethernet::MAC[i];
	}
}

static void setTypeLen(uint16_t typelen) {
	Ethernet::buffer[ETH_TYPE_L_P] = (uint8_t) (typelen >> 0);
	Ethernet::buffer[ETH_TYPE_H_P] = (uint8_t) (typelen >> 8);
}

static uint32_t crc_table[] = {
	0x4DBDF21C, 0x500AE278, 0x76D3D2D4, 0x6B64C2B0,
	0x3B61B38C, 0x26D6A3E8, 0x000F9344, 0x1DB88320,
	0xA005713C, 0xBDB26158, 0x9B6B51F4, 0x86DC4190,
	0xD6D930AC, 0xCB6E20C8, 0xEDB71064, 0xF0000000
};

static void setCRC(uint16_t length) {
	uint16_t n;
	uint32_t crc = 0;

	for (n = 0; n < length; n++) {
		crc = (crc >> 4) ^ crc_table[(crc ^ (Ethernet::buffer[n] >> 0)) & 0x0F];
		crc = (crc >> 4) ^ crc_table[(crc ^ (Ethernet::buffer[n] >> 4)) & 0x0F];
	}

	for (n = 0; n < 4; n++) {
		Ethernet::buffer[length+n] = crc >> n;
	}
}

uint16_t Ethernet::packetPrepare(const uint8_t* dest_mac, uint16_t typelen) {
	setMACs(dest_mac);
	setTypeLen(typelen+4);
	return 14;
}

void Ethernet::packetSend(uint16_t length) {
	setCRC(length);
	// CRC has 4 bytes
	ENC28J60::packetSend(length+4);
}

uint16_t Ethernet::packetReceive() {
	return ENC28J60::packetReceive();
}

void Ethernet::getSrcMAC(uint8_t* mac, const uint8_t* buffer) {
	int i;
	for (i = 0; i < 6; i++) {
		mac[i] = buffer[ETH_SRC_MAC+i];
	}
}

uint16_t Ethernet::getTypeLen(const uint8_t* buffer) {
	uint16_t out = 0;
	out |= ((uint16_t) buffer[ETH_TYPE_H_P]) << 8;
	out |= ((uint16_t) buffer[ETH_TYPE_L_P]) << 0;
	return out;
}
