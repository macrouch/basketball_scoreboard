#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

#define PIN 1
#define     IR_LED                 13
#define     IR_SENSOR              4

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("Basketball Scoreboard!");
#endif

  matrix.begin(0x70);

  pinMode(IR_LED, OUTPUT);
  pinMode(IR_SENSOR, INPUT);
}

void loop() {
  uint16_t baskets = 0;
  uint16_t print_time = 0;
  boolean in_hoop = false;
  boolean start_game = false;
  printMatrix(30, baskets);

  // start game when basket is scored
  if (isBallInHoop()) {
    start_game = true;
  }

  if (start_game) {
    // time = 1000 * 30s
    for(uint16_t time = 30000; time > 0; time -= 10) {
      // if time == an even second update matrix
      if (time % 1000 == 0) {
        print_time = time / 1000;
        printMatrix(print_time, baskets);
      } 

      if (in_hoop) {
        if (!isBallInHoop()) {
          in_hoop = false;
        }
      }

      if (!in_hoop && isBallInHoop()) {
        in_hoop = true;
        baskets++;
        printMatrix(print_time, baskets);
        Serial.println("basketball is in the way!");
        Serial.println(baskets);
      }      

      delay(10);
    }
    printMatrix(0, baskets);
    delay(5000);
  }
  delay(100);
}

void printMatrix(uint16_t time, uint16_t count) {
  matrix.writeDigitNum(0, time / 10);
  matrix.writeDigitNum(1, time % 10);
  matrix.writeDigitNum(3, (count / 10) % 10);
  matrix.writeDigitNum(4, count % 10);
  matrix.writeDisplay();
}

///////////////////////////////////////////////////////
// isBallInHoop function
//
// Returns true if a ball is blocking the sensor.
///////////////////////////////////////////////////////
boolean isBallInHoop() {
  // Pulse the IR LED at 38khz for 1 millisecond
  pulseIR(1000);

  // Check if the IR sensor picked up the pulse (i.e. output wire went to ground).
  if (digitalRead(IR_SENSOR) == LOW) {
    return false; // Sensor can see LED, return false.
  }

  return true; // Sensor can't see LED, return true.
}

///////////////////////////////////////////////////////
// pulseIR function
//
// Pulses the IR LED at 38khz for the specified number
// of microseconds.
///////////////////////////////////////////////////////
void pulseIR(long microsecs) {
  // 38khz IR pulse function from Adafruit tutorial: http://learn.adafruit.com/ir-sensor/overview
  // we'll count down from the number of microseconds we are told to wait
  cli();  // this turns off any background interrupts

  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
    digitalWrite(IR_LED, HIGH);  // this takes about 3 microseconds to happen
    delayMicroseconds(9);        // hang out for 10 microseconds, you can also change this to 9 if its not working
    digitalWrite(IR_LED, LOW);   // this also takes about 3 microseconds
    delayMicroseconds(9);        // hang out for 10 microseconds, you can also change this to 9 if its not working

    // so 26 microseconds altogether
    microsecs -= 26;
  }

  sei();  // this turns them back on
}
