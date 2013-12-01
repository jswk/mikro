/*
 * main.cpp
 *
 *  Created on: 23 lis 2013
 *      Author: Savi
 */

#include <Arduino.h>
#include "enc28j60.h"
#include "ethernet.h"
#include "net.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

byte ENC28J60::buffer[700];
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte destmac[] = { 0xb8,0x70,0xf4,0xaa,0x9d,0x62 };
//static byte destmac[] = { 0xff,0xff,0xff,0xff,0xff,0xff };


void setup()   {
	ENC28J60::initialize(sizeof ENC28J60::buffer, mymac);
	ENC28J60::disableMulticast();
	Serial.begin(9600);
}


void loop()
{
	uint16_t len;

	do {
		len = ENC28J60::packetReceive();
	} while (len == 0);

	Ethernet::packetProcess(ENC28J60::buffer, len);
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
