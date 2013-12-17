#include "net.h"

#include "ICMPv6.h"
#include "Arduino.h"

#include "IPv6.h"
#include "ndp.h"

uint8_t* ICMPv6::buffer = IPv6::buffer;

void ICMPv6::packetProcess(uint16_t offset, uint16_t length) {
	struct ICMPv6_header *header = (struct ICMPv6_header*) (ICMPv6::buffer+offset);
#ifdef DEBUG_ICMPv6
	Serial.print("ICMPv6 Type: ");
	Serial.println(header->type);

	Serial.print("Code: ");
	Serial.println(header->code);

	Serial.print("Type: ");
	Serial.println(header->type);

	Serial.print("Checksum:           0x");
	Serial.println(SWAP_16_H_L(header->checksum), 16);

	Serial.print("Generated checksum: 0x");
	Serial.println(IPv6::generateChecksum(header->checksum), 16);
#endif
	switch (header->type) {
	// Neighbour Advertisment
	case ICMPv6_NBR_ADVERT:
		NDP::handleAdvertisment(header);
		break;
	// Neighbour Solicitation
	case ICMPv6_NBR_SOLICIT:
		NDP::handleSolicitation(header);
		break;
	}

}

