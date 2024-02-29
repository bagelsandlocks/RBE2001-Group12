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
int printDelay = 1000;
int linearPotPin = 19;
int servoStop = 1490;  
int servoJawDown = 1300;  
int servoJawUp = 1700;  
int linearPotVoltageADC = 500;
int jawOpenPotVoltageADC = 200;
int jawClosedPotVoltageADC = 880;
int stuckThreshold = 1;
int stuckCount = 0;
bool stuck = 0;
enum STATE
{
    START = 0,
    OPEN = 1,
    CLOSE = 2,
    STUCK = 3
};
int servoState;

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

int ServoControl::getPosition(){
    return analogRead(linearPotPin);
}

void ServoControl::setEffort(int effort){
    effort = map(effort,-100,100,servoJawDown,servoJawUp);
    jawServo.writeMicroseconds(effort);
}

void ServoControl::jawOpen(){
    stuckCount = 0;
    int lastPosition;
    linearPotVoltageADC = analogRead(linearPotPin);
    //Serial.println(linearPotVoltageADC);
    jawServo.writeMicroseconds(servoJawDown);
    while (linearPotVoltageADC > jawOpenPotVoltageADC){
        linearPotVoltageADC = analogRead(linearPotPin);
        if (printTimer.isExpired()){
            if (abs(lastPosition-linearPotVoltageADC) < stuckThreshold){
                stuckCount++;
                Serial.println(stuckCount);
                if(stuckCount > 100){
                    servoState = 3;
                    jawStop();
                    return;
                }
            }
        }
        lastPosition = linearPotVoltageADC;
    }
    Serial.println(linearPotVoltageADC);
    jawStop();
}

void ServoControl:: jawClose(){
    stuckCount = 0;
    int lastPosition;
    linearPotVoltageADC = analogRead(linearPotPin);
    //Serial.println(linearPotVoltageADC);
    jawServo.writeMicroseconds(servoJawUp);
    while (linearPotVoltageADC < jawClosedPotVoltageADC){
        linearPotVoltageADC = analogRead(linearPotPin);
        //Serial.println(linearPotVoltageADC);
        if (printTimer.isExpired()){
            if (abs(lastPosition-linearPotVoltageADC) < stuckThreshold){
                stuckCount++;
                Serial.println(stuckCount);
                if(stuckCount > 100){
                    servoState = 3;
                    jawStop();
                    return;
                }
            }
        }

        lastPosition = linearPotVoltageADC;
    }
    Serial.println(linearPotVoltageADC);
    jawStop();
}

void ServoControl::jawStop(){
    jawServo.writeMicroseconds(servoStop);
    delay(50);
}

void ServoControl::grabPanel()
{
    Serial.println(servoState);
    switch(servoState){
        case START:
        setup();
        jawStop();
        servoState = 2;
        break;

        case OPEN:
            Serial.println("in open");
            jawOpen();
        break;

        case CLOSE:
            Serial.println("in close");
            jawClose();
        break;

        case STUCK:
        if (linearPotVoltageADC > jawOpenPotVoltageADC){
            servoState = 1;
        }else{
            servoState = 2;
        }
        break;
    }
}