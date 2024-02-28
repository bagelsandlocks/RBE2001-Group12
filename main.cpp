#include <Arduino.h>
#include <wpi-32u4-lib.h>
#include <Romi32U4.h>


int trig = 12;
int echo =0;

volatile unsigned int startTime = 0;
volatile unsigned int deltaTime = 50;
volatile unsigned int count = 0;

unsigned int printDelay = 500;
unsigned int trigDelay = 0;
unsigned int nextPrint = 500;
unsigned int nextTrig = 0;

float distanceCm = 0;

unsigned int storeDeltaTime = 0;







void echoISR(){
    if (digitalRead(echo) == HIGH){
        startTime = micros();
    } else if (digitalRead(echo) == LOW)
    {
        deltaTime = micros() - startTime;
    }
}

void setup()
{
    pinMode(echo, INPUT);
    pinMode(trig, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(echo), echoISR, CHANGE);
    Serial.begin(9600);
    delay(1000);
    Serial.println("Hello");
    delay(1000);

    nextPrint = millis() + printDelay;
    nextTrig = millis() + trigDelay;
}
/*
void loop(){
    digitalWrite(trig, LOW);
    delay(2);
    digitalWrite(trig, HIGH);
    delay(10);
    digitalWrite(trig, LOW);
    int duration = pulseIn(echo, HIGH);
    int distance = duration*0.034/2;
    Serial.println("Distance: ");
    Serial.println(distance);
    delay(100);

}

*/








void loop()
{
    storeDeltaTime = deltaTime;

    if(millis() >= nextTrig){
        digitalWrite(trig,LOW);
        delay(10);
        digitalWrite(trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig, LOW);
        nextTrig = millis() + trigDelay;
    }

    
    if (millis() >= nextPrint){
        distanceCm = (storeDeltaTime / 2) * 0.0343;
        /*
        if(distanceCm<3){
            distanceCm = (sqrt(pow(distanceCm,2)-2.25));
        }
        */
        Serial.println(distanceCm);
        nextPrint = millis() + printDelay;
    }
}
