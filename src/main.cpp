/*
 * main.cpp
 *
 *  Created on: 23 lis 2013
 *      Author: Savi
 */

#include "net.h"
#include <Arduino.h>
#include "enc28j60.h"
#include "ethernet.h"
#include "ndp.h"

byte ENC28J60::buffer[700];

uint8_t ENC28J60::MAC[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte destmac[] = { 0xb8,0x70,0xf4,0xaa,0x9d,0x62 };
//static byte destmac[] = { 0xff,0xff,0xff,0xff,0xff,0xff };

uint8_t NDP::IP[] = { 0xFE, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF};


void setup()   {
	ENC28J60::initialize(sizeof ENC28J60::buffer, ENC28J60::MAC);
	ENC28J60::disableMulticast();
	Serial.begin(9600);
}


void loop()
{
	uint16_t len;

	do {
		len = ENC28J60::packetReceive();
	} while (len == 0);

	Ethernet::packetProcess(len);
}

int main(void) {

  init();
  setup();
/*
  delay(100);

  uint16_t off = Ethernet::packetPrepare(destmac, 100);
  memset(Ethernet::buffer+off, 0x42, 100);
  Ethernet::packetSend(100+off);
*/
  while(true) {
    loop();
  }
}
