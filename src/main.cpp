/*
 * main.cpp
 *
 *  Created on: 23 lis 2013
 *      Author: Savi
 */

#include <Arduino.h>
#include "enc28j60.h"

byte ENC28J60::buffer[1024];
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

void setup()   {
	ENC28J60::initialize(1024, mymac);
	ENC28J60::disableMulticast();
	Serial.begin(9600);
}


void loop()
{
	uint16_t len;

	do {
		len = ENC28J60::packetReceive();
	} while (len == 0);

	Serial.println(len);

}

int main(void) {

  init();
  setup();

  while(true) {
    loop();
  }
}
