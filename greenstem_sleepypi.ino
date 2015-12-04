// Libraries
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
  
  pinMode(5,OUTPUT);    //Soil moisture or temp sensor on digital pin 5
  pinMode(11, OUTPUT);  //IR LED on digital pin 10
  
  // initialize serial communication: In Arduino IDE use "Serial Monitor"
  Serial.begin(57600);
}

void loop() {
  
  // Enter idle state for 8 s with the rest of peripherals turned off. 225 in loop represents 30 min sleep time
  for (int i=0; i<=80; i++) {
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
  
  for (int j=0; j<=3; j++) {

  digitalWrite(5, HIGH);    //provide power to sensor
  digitalWrite(11, HIGH);  //provide power to IR LED
  delay(500);
  
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
  
  digitalWrite(5, LOW); 
  
  delay(60000);// Wait a minute before next reading 
  }
  
  
  SleepyPi.enablePiPower(false);

}
