/*
 * DS1374RTC.h - library for DS1374 RTC
 * This library is intended to be used with Arduino Time.h library functions
 */

#ifndef DS1374RTC_h
#define DS1374RTC_h

#include "Arduino.h"
#include <Time.h>
#include <Wire.h>

union timeDate_t{
  time_t time; 
  uint8_t bytes[4];
};

typedef enum {
	eALARM = 0,
	eWATCHDOG 
}eCTRType_t;

typedef enum {
	eRESET_PIN = 0,
	eINT_PIN 
}eWDTOutput_t;

typedef enum {
	e1Hz	= 0,
	e4kHz,
	e8kHz,
	e32kHz
}eSQWRate_t;

typedef struct {
	bool			disableOsc;		// Enable / Disable the RTC OScillator
	bool			enableCTR;		// Enable / Disable the Watchdog or Alarm Counters
	eCTRType_t		CTRType;		// Select either the Watchdog or the Alarm as the Counter
	bool			enableSQW;		// Enable / Disable the square wave output
	eWDTOutput_t	WDToutput;		// Select where the reset pulse goes when the WDT expires
	eSQWRate_t		SQWRate;		// Selects the frequency of the square wave output			
	bool			enableAlarmInt;	// Enable / Disable the Alarm for setting the INT pin when it expires
}RTCConfig_t;

union RTCConfigU_t {
   uint8_t  	configByte;
   RTCConfig_t  config;
};

// library interface description
class DS1374RTC
{
  // user-accessible "public" interface
  public:
    DS1374RTC();

	bool readAlarm(time_t &t);
	bool readAlarm(tmElements_t &tm);

	bool setAlarm(time_t t);
	bool setAlarm(tmElements_t &tm);

	bool setTime(time_t t);
	bool setTime(tmElements_t &tm);

	bool readTime(time_t &t);
	bool readTime(tmElements_t &tm);

	bool setConfig(RTCConfig_t &config);
    bool chipPresent() { return exists; }

  private:
    static bool exists;
};

extern DS1374RTC RTC;

#endif
 
