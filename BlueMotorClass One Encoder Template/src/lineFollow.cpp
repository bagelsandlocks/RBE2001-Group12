#include <Arduino.h>
#include <Romi32U4.h>
#include <LineFollow.h>

int leftReflectorPin = 18;
int rightReflectorPin = 22;
int leftWhiteValue = 475;
int rightWhiteValue = 457;
int leftBlackValue = 840;
int rightBlackValue = 814;
int crossCount = 0;

LineFollow::LineFollow(){}

void LineFollow::setup(){

}

void LineFollow::lineFollow(){

}

void LineFollow::findLine(){
    
}

int leftReflectanceAvg() // Takes the average of 6 reflectance readings on the left sensor to improve accuracy
{
  int a = analogRead(leftReflectance);
  int b = analogRead(leftReflectance);
  int c = analogRead(leftReflectance);
  int d = analogRead(leftReflectance);
  int e = analogRead(leftReflectance);
  int f = analogRead(leftReflectance);
  return (a + b + c + d + e + f) / 6;
}

int rightReflectanceAvg() // Takes the average of 6 reflectance readings on the right sensor to improve accuracy
{
  int a = analogRead(rightReflectance);
  int b = analogRead(rightReflectance);
  int c = analogRead(rightReflectance);
  int d = analogRead(rightReflectance);
  int e = analogRead(rightReflectance);
  int f = analogRead(rightReflectance);
  return (a + b + c + d + e + f) / 6;
}