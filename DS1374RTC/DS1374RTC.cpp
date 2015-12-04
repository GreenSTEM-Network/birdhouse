/*
 * DS1374RTC.cpp - library for DS1374 RTC
  
  Copyright (c) Jon Watkins 2013
  This library is intended to be uses with Arduino Time.h library functions
  This library is derived from the work of Michael Margolis on the 
  DS1307RTC.cpp code.
  http://www.pjrc.com/teensy/td_libs_DS1307RTC.html

  Note:
  ======
  Internally the DS1374 differs from other RTC's in that the counter
  is stored as a 32-bit raw binary count rather than bcd encoded sec,
  minutes, hours etc of RTC's like the ds1307.

  The 32-bit binary counters are designed to continuously count time in seconds
  which is accumulated in a 4-byte register as the number of seconds since some 
  arbitrary reference date. The date used in this code is the same used with the 
  UNIX operating system, the reference date January 1, 1970, often referred to 
  as Unix Epoch. Note: Because of the reference date used and the use of a 32-bit 
  counter, this algorithm rolls over on Tuesday, January 19, 03:14:07, 2038.

  License
  =======
  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Releases
  ========
  V1_0 - 10 Aug 2013 - Initial release

  TODO:
  Status and trickle charger

 */
#include <Wire.h>
#include "DS1374RTC.h"

#define DS1374_CTRL_ID 0x68 

DS1374RTC::DS1374RTC()
{
  Wire.begin();
}

// PUBLIC FUNCTIONS
bool DS1374RTC::readAlarm(time_t &t)
{
  // TODO get rid of the unnecessary time conversions here
  tmElements_t tm;
  if (readAlarm(tm) == false) return false;
  t = makeTime(tm);
  return true;
}
bool DS1374RTC::readAlarm(tmElements_t &tm)
{
  uint8_t sec;
  timeDate_t timeDate;

  Wire.beginTransmission(DS1374_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x04);	// Set to alarm register	
#else
  Wire.send(0x04);				// Set to alarm register
#endif  
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;

  // request the 4 TOD Counts 
  Wire.requestFrom(DS1374_CTRL_ID, 3);// TODO replace magic number here
  if (Wire.available() < 3) return false;
#if ARDUINO >= 100
  timeDate.bytes[0] = Wire.read();
  timeDate.bytes[1] = Wire.read();
  timeDate.bytes[2] = Wire.read();

#else
  timeDate.bytes[0] = Wire.receive();
  timeDate.bytes[1] = Wire.receive();
  timeDate.bytes[2] = Wire.receive();
#endif
  breakTime(timeDate.time, tm);

//if (sec & 0x80) return false; // clock is halted TODO - look at an equivalent here
  return true;
}
bool DS1374RTC::setAlarm(time_t t)
{
  timeDate_t timeDate;
  
  timeDate.time = t; 

  Wire.beginTransmission(DS1374_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x04);		 // reset register pointer to start of alarm 
  Wire.write(timeDate.bytes[0]);	 // Write Alarm byte 0	 
  Wire.write(timeDate.bytes[1]);	 // Write Alarm byte 1
  Wire.write(timeDate.bytes[2]);	 // Write Alarm byte 2
#else  
  Wire.send(0x04);					// reset register pointer to start of alarm   
  Wire.send(timeDate.bytes[0]);		// Write TOD byte 1
  Wire.send(timeDate.bytes[1]);		// Write TOD byte 2
  Wire.send(timeDate.bytes[2]);		// Write TOD byte 3
#endif
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;
  return true;
}
bool DS1374RTC::setAlarm(tmElements_t &tm)
{
  timeDate_t	timeDate;
  
  // convert to 32-bit raw time
  timeDate.time = (time_t)makeTime(tm);

  Wire.beginTransmission(DS1374_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x04);		 // reset register pointer to start of alarm 
  Wire.write(timeDate.bytes[0]);	 // Write Alarm byte 0	 
  Wire.write(timeDate.bytes[1]);	 // Write Alarm byte 1
  Wire.write(timeDate.bytes[2]);	 // Write Alarm byte 2
#else  
  Wire.send(0x04);					// reset register pointer to start of alarm   
  Wire.send(timeDate.bytes[0]);		// Write TOD byte 1
  Wire.send(timeDate.bytes[1]);		// Write TOD byte 2
  Wire.send(timeDate.bytes[2]);		// Write TOD byte 3
#endif
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;
  return true;
}



bool DS1374RTC::setTime(time_t t)
{
  tmElements_t tm;

  // TODO - don't need a break time here could setup another write routine
  // that uses the time_t value

  breakTime(t, tm);
//  tm.Second |= 0x80;  // stop the clock 
  setTime(tm); 
//  tm.Second &= 0x7f;  // start the clock
//  write(tm); 
}
bool DS1374RTC::setTime(tmElements_t &tm)
{
  timeDate_t	timeDate;
  
  // convert to 32-bit raw time
  timeDate.time = (time_t)makeTime(tm);

  Wire.beginTransmission(DS1374_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00);		 // reset register pointer  
  Wire.write(timeDate.bytes[0]);	 // Write TOD byte 0	 
  Wire.write(timeDate.bytes[1]);	 // Write TOD byte 1
  Wire.write(timeDate.bytes[2]);	 // Write TOD byte 2
  Wire.write(timeDate.bytes[3]);	 // Write TOD byte 3	  
//  Wire.write(0);			 // Write Alarm byte 0 - TODO Alarm
//  Wire.write(0);			 // Write Alarm byte 1
//  Wire.write(0);			 // Write Alarm byte 2 
//  Wire.write(0);			 // Write Control Register - TODO allow selection of options
#else  
  Wire.send(0x00);			// reset register pointer  
  Wire.send(timeDate.bytes[0]);		// Write TOD byte 1
  Wire.send(timeDate.bytes[1]);		// Write TOD byte 2
  Wire.send(timeDate.bytes[2]);		// Write TOD byte 3
  Wire.send(timeDate.bytes[3]);		// Write TOD byte 4	
//  Wire.send(0);				// Write Alarm byte 0 
//  Wire.send(0);				// Write Alarm byte 1	
//  Wire.send(0);				// Write Alarm byte 2
//  Wire.send(0);				// Write Control Register
 
#endif
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;
  return true;
}
// Aquire data from buffer and convert to time_t
// Returns true if successful
bool DS1374RTC::readTime(time_t &t)   
{
  // TODO get rid of the unnecessary time conversions here
  tmElements_t tm;
  if (readTime(tm) == false) return false;
  t = makeTime(tm);
  return true;
}

// Aquire data from the RTC chip in BCD format
// Returns true if successful
bool DS1374RTC::readTime(tmElements_t &tm)
{
  uint8_t value;
  timeDate_t timeDate;

  Wire.beginTransmission(DS1374_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); 
#else
  Wire.send(0x00);
#endif  
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;

  // request the 4 TOD Counts 
  Wire.requestFrom(DS1374_CTRL_ID, 9);// TODO replace magic number here
  if (Wire.available() < 9) return false;
#if ARDUINO >= 100
  timeDate.bytes[0] = Wire.read();
  timeDate.bytes[1] = Wire.read();
  timeDate.bytes[2] = Wire.read();
  timeDate.bytes[3] = Wire.read();
  value = Wire.read();			// Alarm registers
  value = Wire.read();
  value = Wire.read();
  value = Wire.read();			// Control
  value = Wire.read();			// Status
#else
  timeDate.bytes[0] = Wire.receive();
  timeDate.bytes[1] = Wire.receive();
  timeDate.bytes[2] = Wire.receive();
  timeDate.bytes[3] = Wire.receive();
  value = Wire.receive();		// Alarm registers
  value = Wire.receive();
  value = Wire.receive();
  value = Wire.receive();		// Control
  value = Wire.receive();		// Status
#endif
  breakTime(timeDate.time, tm);

  if (value & 0x80) return false; // clock is halted TODO
  return true;
}

bool DS1374RTC::setConfig(RTCConfig_t &config)
{
  uint8_t	configByte = 0;
  RTCConfigU_t	testConfig;
  
  // ******* Compile the config byte **********
  // ...Enable / Disable the RTC OScillator
  if(config.disableOsc > 0){
	configByte |= 0x80;
  }
  else {
	configByte &= ~0x80;
  }
  // ...Enable / Disable the Watchdog or Alarm Counters
  if(config.enableCTR > 0){
	configByte |= 0x40;
  }
  else {
	configByte &= ~0x40;
  }
  // ...Select either the Watchdog or the Alarm as the Counter
  if(config.CTRType == eWATCHDOG){
	configByte |= 0x20;
  }
  else {
    // Alarm
	configByte &= ~0x20;
  }	
  // ...Enable / Disable the square wave output
  if(config.enableSQW > 0){
	configByte |= 0x10;
  }
  else {
	configByte &= ~0x10;
  }
  // ...Select where the reset pulse goes when the WDT expires
  if(config.WDToutput == eINT_PIN){
	configByte |= 0x08;
  }
  else {
    // Alarm
	configByte &= 0x08;
  }
  // ...Selects the frequency of the square wave output
  switch (config.SQWRate){
	  case e4kHz:
		  configByte |= 0x02;
		  break;
	  case e8kHz:
		  configByte |= 0x04;
		  break;
	  case e32kHz:
		  configByte |= 0x06;
		  break;
	  case e1Hz:	// Drop-thru case nothing set here
	  default:
		  break;
  }
  // Enable / Disable the Alarm for setting the INT pin when it expires		
  if(config.enableAlarmInt > 0){
	configByte |= 0x01;
  }
  else {
	configByte &= ~0x01;
  }  

  testConfig.configByte = configByte;
  config = testConfig.config;	  

  // ******* Write the config byte **********
  Wire.beginTransmission(DS1374_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x07);		// reset register pointer to Control register 
  Wire.write(configByte);		// Write Control Register
  Wire.write(0);			// Reset Status Register
#else  
  Wire.send(0x07);			// reset register pointer to Control register 
  Wire.send(configByte);		// Write Control Register
  Wire.send(0);				// Reset Status Register 
#endif
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;
  return true;
}

// PRIVATE FUNCTIONS

bool DS1374RTC::exists = false;

DS1374RTC RTC = DS1374RTC(); // create an instance for the user
