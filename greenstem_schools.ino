//Wake up for 2 hours each day and film video, take 6 sensor readings 20 minutes apart. Sleep for 22 hours.

// Libraries
#include "SleepyPi.h"
#include <Time.h>
#include <LowPower.h>
#include <DS1374RTC.h>
#include <Wire.h>
#include <math.h>

const int LED_PIN = 13;

void setup() {
  // Configure "Standard" LED pin
  pinMode(LED_PIN, OUTPUT);		
  digitalWrite(LED_PIN,LOW);		// Switch off LED
  pinMode(5, OUTPUT);  //soil sensor on digital pin 5
  pinMode(10,OUTPUT);  //temp sensor on digital pin 10
  pinMode(12, OUTPUT); //IR LED on digital pin 12
  Serial.begin(57600); // initialize serial communication: In Arduino IDE use "Serial Monitor"
}

void loop() {
  
  // Take sensor readings, turn on LED
  SleepyPi.enablePiPower(true);
  SleepyPi.enableExtPower(true);
  
  digitalWrite(LED_PIN,HIGH);		// Switch on LED
  delay(500);  
  digitalWrite(LED_PIN,LOW);		// Switch off LED 
  
  delay(60000); //wait a minute to let Pi start up
    
  for (int j=0; j<6; j++) {            // Take 6 sets of readings

  digitalWrite(5, HIGH);   //provide power to soil sensor
  digitalWrite(10, HIGH);  //provide power to temperature sensor
  digitalWrite(12, HIGH);  //provide power to IR LED
  delay(500);  //short delay to settle pin voltage
  
  //VH400 soil sensor
  float soil1 = analogRead(A1);
  
  //TMP36 temperature sensor
  int reading = analogRead(A4);
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100;   //converting from 10 mv per degree wit 500 mV offset to degrees ((voltage - 500mV) times 100)
  float temperature = (temperatureC * 9.0 / 5.0) + 32.0; // now convert to Fahrenheit
  
  // print out the value you read:
  Serial.print("soil1");
  Serial.print(',');
  Serial.print("N/A");
  Serial.print(',');
  Serial.print("N/A");
  Serial.print(',');
  Serial.print(temperature);
  Serial.print(',');
  Serial.println("4900");
  
  digitalWrite(5, LOW); 
  digitalWrite(10, LOW); 
  digitalWrite(12, LOW); 
  
  for (int i=0; i<20; i++) {
  delay(60000); // Wait 20 minutes between readings 
  }
  }
  digitalWrite(11, LOW);  //turn off power to IR LED
  
  SleepyPi.enablePiPower(false);
  
    // Enter idle state for 8 s with the rest of peripherals turned off. 225 in loop represents 30 min sleep time. 8265 is 9893 - 2 hrs + 97 minutes.
  for (int i=0; i<8265; i++) {  
  SleepyPi.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
				SPI_OFF, USART0_OFF, TWI_OFF);
  } 

}
