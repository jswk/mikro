#include "net.h"

#include "ndp.h"
#include "ICMPv6.h"
#include "Arduino.h"

#include "debug.h"

static uint8_t broadcast[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

struct ndp_pair NDP::pairs[NDP_CACHE_LEN];

int cmp_ip(uint8_t *ip1, uint8_t *ip2) {
	int i;
	for (i = 0; i < 16; i++) {
		if (ip1[i] < ip2[i]) {
			return -1;
		} else if (ip1[i] > ip2[i]) {
			return 1;
		}
	}
	return 0;
}

uint8_t* NDP::getMAC(uint8_t *ip) {
	int i;
	for (i = 0; i < NDP_CACHE_LEN; i++) {
		if(cmp_ip(ip, NDP::pairs[i].ip) == 0) {
			return NDP::pairs[i].mac;
		}
	}
	return 0;
}

void NDP::handleAdvertisment(struct ICMPv6_header *header) {
	uint8_t *src_ip = &header->body;
	uint8_t *src_mac = &header->body + 16 + 2;

	NDP::savePairing(src_ip, src_mac);

#ifdef DEBUG_NDP
	int i;
	for (i = 0; i < NDP_CACHE_LEN; i++) {
		if (NDP::pairs[i].created == 0) continue;
		Serial.print(NDP::pairs[i].created);
		Serial.print(F(": "));
		print_ip_to_serial(NDP::pairs[i].ip);
		Serial.print(F(" -> "));
		print_mac_to_serial(NDP::pairs[i].mac);
		Serial.println();
	}
#endif
}

void NDP::savePairing(uint8_t *ip, uint8_t *mac) {
	uint8_t i = 0;
	uint32_t best = 0;
	uint8_t best_i = 0;

	while (i < NDP_CACHE_LEN) {
		if (cmp_ip(ip, NDP::pairs[i].ip) == 0) {
			NDP::pairs[i].created = millis();
			return;
		}
		if (NDP::pairs[i].created == 0) {
			memcpy(NDP::pairs[i].ip, ip, 16);
			memcpy(NDP::pairs[i].mac, mac, 6);
			NDP::pairs[i].created = millis();
			return;
		}
		if (best == 0 || NDP::pairs[i].created < best) {
			best = NDP::pairs[i].created;
			best_i = i;
		}
		i++;
	}

	memcpy(NDP::pairs[best_i].ip, ip, 16);
	memcpy(NDP::pairs[best_i].mac, mac, 6);
	NDP::pairs[best_i].created = millis();
}
