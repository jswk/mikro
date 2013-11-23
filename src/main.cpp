/*
 * main.cpp
 *
 *  Created on: 23 lis 2013
 *      Author: Savi
 */

#include <Arduino.h>

void setup()   {
  pinMode(13, OUTPUT);
}


void loop()
{

	digitalWrite(13, HIGH);
	delay(100);
	digitalWrite(13, LOW);
	delay(1000);

}


