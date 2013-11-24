/*
 * main.cpp
 *
 *  Created on: 23 lis 2013
 *      Author: Savi
 */

#include <Arduino.h>
#include "enc28j60.h"
#include "ethernet.h"

byte ENC28J60::buffer[1024];
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte destmac[] = { 0xb8,0x70,0xf4,0xaa,0x9d,0x62 };
//static byte destmac[] = { 0xff,0xff,0xff,0xff,0xff,0xff };

void setup()   {
	ENC28J60::initialize(1024, mymac);
	ENC28J60::disableMulticast();
	Serial.begin(9600);
}


void loop()
{
	uint16_t len;

	do {
		len = Ethernet::packetReceive();
	} while (len == 0);

	uint8_t mac[6];
	int i;
	char sep = ' ';
	uint16_t typelen;

	Ethernet::getSrcMAC(mac, Ethernet::buffer);
	typelen = Ethernet::getTypeLen(Ethernet::buffer);

	Serial.print("Src MAC:");
	for (i = 0; i < 6; i++) {
		Serial.print(sep);
		Serial.print(mac[i], HEX);
		sep = ':';
	}
	Serial.println();
	Serial.print("Type/length: ");
	Serial.println(typelen, HEX);
	Serial.println(len);

}

int main(void) {

  init();
  setup();

  delay(100);

  uint16_t off = Ethernet::packetPrepare(destmac, 100);
  memset(Ethernet::buffer+off, 0x42, 100);
  Ethernet::packetSend(100+off);

  while(true) {
    loop();
  }
}
