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
int linearPotPin = 18;
int servoStop = 1490;  
int servoJawDown = 1300;  
int servoJawUp = 1700;  
int linearPotVoltageADC = 500;
int jawOpenPotVoltageADC = 600;
int jawClosedPotVoltageADC = 940;
int stuckThreshold = 1;
int stuckCount;
bool stuck = 0;
enum STATE
{
    START = 0,
    OPEN = 1,
    CLOSE = 2,
    STUCK = 3
};
int state;

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

void ServoControl:: jawOpen(){
    int lastPosition;
    linearPotVoltageADC = analogRead(linearPotPin);
    Serial.println(linearPotVoltageADC);
    jawServo.writeMicroseconds(servoJawDown);
    while (linearPotVoltageADC > jawOpenPotVoltageADC){
        linearPotVoltageADC = analogRead(linearPotPin);
        Serial.println(linearPotVoltageADC);
        if (printTimer.isExpired()){
            if (abs(lastPosition-linearPotVoltageADC) < stuckThreshold){
                stuckCount ++;
                if(stuckCount > 3){
                    state = 3;
                    jawStop();
                    return;
                }
            }
        }
        lastPosition = linearPotVoltageADC;
    }
    state = 2;
    jawStop();
}

void ServoControl:: jawClose(){
    int lastPosition;
    linearPotVoltageADC = analogRead(linearPotPin);
    Serial.println(linearPotVoltageADC);
    jawServo.writeMicroseconds(servoJawUp);
    while (linearPotVoltageADC < jawClosedPotVoltageADC){
        linearPotVoltageADC = analogRead(linearPotPin);
        Serial.println(linearPotVoltageADC);
        if (printTimer.isExpired()){
            if (abs(lastPosition-linearPotVoltageADC) < stuckThreshold){
                stuckCount ++;
                if(stuckCount > 100){
                    state = 3;
                    jawStop();
                    return;
                }
            }
        }
        lastPosition = linearPotVoltageADC;
    }
    state = 1;
    jawStop();
}

void ServoControl::jawStop(){
    jawServo.writeMicroseconds(servoStop);
    delay(1000);
}

void ServoControl::grabPanel()
{
    Serial.println(state);
    switch(state){
        case START:
        setup();
        jawStop();
        state = 2;
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
            state = 1;
        }else{
            state = 2;
        }
        break;
    }
}