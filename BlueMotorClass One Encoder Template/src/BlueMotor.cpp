#include <Arduino.h>
#include <BlueMotor.h>
#include <Romi32U4.h>

long oldValue = 0;
long newValue;
long count = 0;
unsigned time = 0;
int a;
int b;// those are statues of encoders, 1 for high, 0 for low

// No argument constructor
BlueMotor::BlueMotor(){
}
/***********************
 * note: this code is running on 540 counts per blue motor round, and I think that's the count max used to measure the level position.
 * 
 * Dead band correction is included
 * 
 * 
 * 
 */











void BlueMotor::setup()
{
    pinMode(PWMOutPin, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    TCCR1A = 0xA8; //0b10101000; //gcl: added OCR1C for adding a third PWM on pin 11
    TCCR1B = 0x11; //0b00010001;
    ICR1 = 400;
    OCR1C = 0;
    int a = digitalRead(ENCA);
    int b = digitalRead(ENCB);
    

    attachInterrupt(digitalPinToInterrupt(ENCA), isr1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCB), isr2, CHANGE);
    reset();
}

long BlueMotor::getPosition()
{
    long tempCount = 0;
    noInterrupts();
    tempCount = count;
    interrupts();
    return tempCount;
}

void BlueMotor::reset()
{
    noInterrupts();
    count = 0;
    interrupts();
}


void BlueMotor::isr1()
{
    // if 
    if(a == 1){
        a = 0;
        if(b == 1){
            count++;
        }
        else{
            count--;
        }
    }
    else{
         a = 1;
        if(b == 0){
            count++;
        }
        else{
            count--;
        }

    }

    
}

void BlueMotor::isr2()
{
    if(b == 1){
        b = 0;
        if(a == 0){
            count++;
        }
        else{
            count--;
        }
    }
    else{
         b = 1;
        if(a == 1){
            count++;
        }
        else{
            count--;
        }

    }

}


void BlueMotor::setEffort(int effort)
{
    if (effort < 0)
    {
        setEffort(-effort, true);
    }
    else
    {
        setEffort(effort, false);
    }
}

void BlueMotor::setEffortTuned(int effort){
        if (effort < 0)
    {
        setEffort(map(-effort, 0, 400, deadBandAjust, 400), false);
    }
    else
    {
        setEffort(map(effort, 0, 400, deadBandAjust, 400), true);
    }

}

void BlueMotor::setEffort(int effort, bool clockwise)
{
    if (clockwise)
    {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
    }
    else
    {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
    }
    OCR1C = constrain(effort, 0, 400);
}


// non blocking implementation
int BlueMotor::moveTo(long target)
{
    // calculate effort to send to motor
    float error = target - getPosition();
    float effortAmount =(Kp * error);
    setEffortTuned(effortAmount);
    // Serial.println(effortAmount);
    // delay(10);
    return error;
}