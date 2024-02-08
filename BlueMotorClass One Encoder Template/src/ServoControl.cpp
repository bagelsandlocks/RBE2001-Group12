#include <Arduino.h>
#include <Romi32U4.h>
#include "Timer.h"
#include <servo32u4.h>
#include <ServoControl.h>
Romi32U4ButtonB buttonB1;

/*
The FS90R continuous rotation servo converts 
standard RC servo position pulses into continuous
rotation speed. The default rest point is 1.5 ms, but
this can be adjusted by using a small screwdriver
to turn the middle-point adjustment potentiometer.
Pulse widths above the rest point result in counterclockwise 
rotation, with speed increasing as the pulse width increases;
pulse widths below the rest point result in clockwise rotation, 
with speed increasing as the pulse width decreases.
*/

//int servoPin = 5;
int printDelay = 500;
int linearPotPin = 18;
int servoStop = 1490;  
int servoJawDown = 1300;  
int servoJawUp = 1700;  
int linearPotVoltageADC = 500;
int jawOpenPotVoltageADC = 600;
int jawClosedPotVoltageADC = 940;

Servo32U4 jawServo;

Timer printTimer(printDelay);

ServoControl::ServoControl()
{
}

void ServoControl::setup()
{
  Serial.begin(9600);
  jawServo.attach();
}

void ServoControl::grabPanel()
{
  // Stop servo
  jawServo.writeMicroseconds(servoStop);
  delay(2000);
  // Get Pot Value
  linearPotVoltageADC = analogRead(linearPotPin);
  Serial.print("Initial linearPotVoltageADC:   ");
  Serial.println(linearPotVoltageADC);

  while (buttonB1.isPressed())
  {
  
  // Move Jaw Down
  jawServo.writeMicroseconds(servoJawDown);

  while (linearPotVoltageADC > jawOpenPotVoltageADC)
  {
    linearPotVoltageADC = analogRead(linearPotPin);
    if (printTimer.isExpired()){
      Serial.print("linearPotVoltageADC:    ");
      Serial.println(linearPotVoltageADC);
    }
  }

  // Stop servo
  jawServo.writeMicroseconds(servoStop);

  linearPotVoltageADC = analogRead(linearPotPin);
  Serial.print("Bottom linearPotVoltageADC Before Delay:    ");
  Serial.println(linearPotVoltageADC);
  delay(5000);
  linearPotVoltageADC = analogRead(linearPotPin);
  Serial.print("Bottom linearPotVoltageADC After Delay:     ");
  Serial.println(linearPotVoltageADC);
  delay(5000);


  // Move Jaw Up
  jawServo.writeMicroseconds(servoJawUp);

  while (linearPotVoltageADC < jawClosedPotVoltageADC)
  {
    linearPotVoltageADC = analogRead(linearPotPin);
    if (printTimer.isExpired()){
      Serial.print("linearPotVoltageADC:     ");
      Serial.println(linearPotVoltageADC);
    }
  }
  
  // Stop servo
  jawServo.writeMicroseconds(servoStop);

  linearPotVoltageADC = analogRead(linearPotPin);
  Serial.print("Final linearPotVoltageADC Before Delay:      ");
  Serial.println(linearPotVoltageADC);
  delay(5000);
  linearPotVoltageADC = analogRead(linearPotPin);
  Serial.print("Final linearPotVoltageADC After Delay:      ");
  Serial.println(linearPotVoltageADC);
  delay(5000);
  
  }
  // Stop servo
  jawServo.writeMicroseconds(servoStop);

}