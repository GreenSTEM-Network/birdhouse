// 
// Simple example showing how to set the Watchdog Time to wake up the Arduino
//

// **** INCLUDES *****
#include "SleepyPi.h"
#include <Time.h>
#include <LowPower.h>
#include <DS1374RTC.h>
#include <Wire.h>

const int LED_PIN = 13;

void setup() {
  
  // Configure "Standard" LED pin
  pinMode(LED_PIN, OUTPUT);		
  digitalWrite(LED_PIN,LOW);		// Switch off LED

  // initialize serial communication: In Arduino IDE use "Serial Monitor"
  Serial.begin(57600);
}

void loop() {
  
  // Enter idle state for 8 s with the rest of peripherals turned off
  for (int i=0; i<=225; i++) {
  SleepyPi.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
				SPI_OFF, USART0_OFF, TWI_OFF);
  } 
  
  // Do something here
  // Example: Read sensor, data logging, data transmission.
  SleepyPi.enablePiPower(true);
  SleepyPi.enableExtPower(true);
  
  digitalWrite(LED_PIN,HIGH);		// Switch on LED
  delay(500);  
  digitalWrite(LED_PIN,LOW);		// Switch off LED
  
  for (int j=0; j<=5; j++) {
  int sensorValue = analogRead(A0);
  
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(',');
  Serial.print(sensorValue);
  Serial.print(',');
  Serial.print(sensorValue);
  Serial.print(',');
  Serial.print("60");
  Serial.print(',');
  Serial.println("4900");
  
  delay(60000);// Wait a minute before next reading 
  }
  
  SleepyPi.enablePiPower(false);

}
