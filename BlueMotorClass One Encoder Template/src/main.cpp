#include <Arduino.h>
#include <wpi-32u4-lib.h>

#include <IRdecoder.h>
#include <ir_codes.h>

#include <Chassis.h>

#include <Romi32U4.h>

#include <BlueMotor.h>
#include <ServoControl.h>
#include <Rangefinder.h>

const uint8_t IR_DETECTOR_PIN = 1;
IRDecoder decoder(14);

int rightSense = 21;
int leftSense = 22;

int leftValue = 0;
int rightValue = 0;
int error = 0;
int effort = 0;

volatile float urfRead = 0;



int state = 0;
int action = 0;
int asdf = 0;
int lastState = -1;
String printable = "";

Chassis chassis;

String actions[] = {"Waiting", "Open Gripper", "Close Gripper",
                "Position on Staging Platform", "Position on 45",
                 "Position on 25", "Prepare to Escape 45"};

enum action
{
    wait = 0,
    openGripper = 1,
    closeGripper = 2,
    positionOnZero = 3,
    positionArmFF = 4,
    positionOnTF = 5,
    liftFromFF = 22,
    liftFromTF = 7,
    turnToNextLine = 8,
    goTowardsBox = 9,
    goTowardsFF = 20,
    approachFF = 21,
    goTowardsTF = 11,
    emergencyStop = 99,
    centerOnLine = 10,
    resetEncoders = 50,

};


// Final Project Task 1

// int orderOfActions[] = {wait, openGripper, goTowardsFF, positionArmFF, approachFF,
//                          closeGripper, liftFromFF, turnToNextLine, positionOnZero, closeGripper};

int orderOfActions[] = {wait, openGripper, goTowardsFF, positionArmFF, approachFF,
                         closeGripper, liftFromFF, turnToNextLine, positionOnZero, closeGripper};

// Final Project Task 2
//int orderOfActions[] = {0, 1, 3, 2, 4, 1, 2, 5, 1, 2};

// Final Project Task 3
//int orderOfActions[] = {0, 1, 3, 2, 4, 1, 2, 5, 1, 2};

int numStates = sizeof(orderOfActions) / sizeof(int);

ServoControl servo;
BlueMotor motor;
Rangefinder rangefinder(7, 12);

int fourtyFivePosition = 3400;
int escapeFourtyFivePosition = 3100; // after backing up a lil bit
int twentyFivePosition = 6300;
int zeroPosition = 0;

int approachFFDist = 8.83;
int approachBoxDist = 4;

unsigned long long timeToPrint = 0;
const unsigned int printPause = 250;

int distanceToReckon = 10.5; // until robot is centered over black tape line
int motorClickCount = 0;
float romiDistanceTraveled = 0;

int hitLine = 0;
bool linePrevState = 0;
bool lineState = 0;


void setup()
{
    pinMode(rightSense, INPUT);
    pinMode(leftSense, INPUT);
    decoder.init();
    chassis.init();
    rangefinder.init();
    Serial.begin(9600);
    delay(1000);
    Serial.println("Hello");
    motor.setup();
    motor.reset();
    delay(1000);
    timeToPrint = millis() + printPause;
}

// 0 CCW, 1 CW
bool turnUntilNextLineHit(int direction){
    if (!direction){
        chassis.setMotorEfforts(-50, 50);
        if (analogRead(leftSense) > 620){
            hitLine = 1;
            Serial.println(analogRead(leftSense));
        }
        if (hitLine && analogRead(leftSense) < 50){
            Serial.println(analogRead(leftSense));
            chassis.setMotorEfforts(0, 0);
            return 0;
        } else{
            return 1;
        }
    }
    if (direction){
    chassis.setMotorEfforts(50, -50);
    if (analogRead(rightSense) > 620){
        hitLine = true;
        Serial.println(analogRead(rightSense));
        }
    if (hitLine && analogRead(rightSense) < 120){
        Serial.println(analogRead(rightSense));
        chassis.setMotorEfforts(0, 0);
        return 0;
        } else{
            return 1;
        }
    }
    return 0;
}

void lineFollow(int speed=50){
    rightValue = analogRead(rightSense);
    leftValue = analogRead(leftSense);
    error = leftValue - rightValue;
    effort = error * 0.1;
    chassis.setMotorEfforts(-(speed + effort), -(speed - effort));
}

void resetChassisEncoders(){
    chassis.getLeftEncoderCount(true);
    chassis.getRightEncoderCount(true);
}

// returns TRUE if done approaching
bool moveUntil(float dist){
    // FIND ROMI DISTANCE
        motorClickCount = (chassis.getLeftEncoderCount() + chassis.getRightEncoderCount()) / 2;
        romiDistanceTraveled = (((7) * M_PI / 1440) * motorClickCount) * 2;

        // negative since backwards
        if (romiDistanceTraveled < -dist){
            return true;
            resetChassisEncoders();
        }else{
            return false;
        }
}




void loop()
{
    urfRead = rangefinder.getDistance();


    // Receive ENTER_SAVE keypress
    int keyPress = decoder.getKeyCode();
    if (keyPress == ENTER_SAVE){
        state = (state + 1) % numStates;
        action = orderOfActions[state];
    }
    if (keyPress == NUM_7){
        motor.setEffort(400);
        //servo.setEffort(100);
        //Serial.println(servo.getPosition());
        //Serial.println(motor.getPosition());
    }
    if (keyPress == NUM_9){
        motor.setEffort(-400);
        //servo.setEffort(-100);
        //Serial.println(servo.getPosition());
    }
    if (keyPress == NUM_8){
        //servo.setEffort(100);
        //Serial.println(servo.getPosition());
        motor.setEffort(0);
        Serial.println(motor.getPosition());
    }
    if (keyPress == NUM_5){
        motor.reset();
        Serial.println(motor.getPosition());
    }
    if (keyPress == LEFT_ARROW){
        //action = 9;
        servo.setEffort(0);
        
    }
    if (keyPress == RIGHT_ARROW){
        action = 8;
    }

    if (action == emergencyStop){
        chassis.setMotorEfforts(0, 0);
        Serial.println("Stopped");
        action = wait;
    }


    if (action == wait){
        chassis.setMotorEfforts(0, 0);
        // Waiting state


    } else if (action == openGripper){

        servo.setEffort(-100);
        if(servo.getPosition() < 300){
            servo.setEffort(0);
            action = wait;
        }

    } else if (action == closeGripper){

        servo.setEffort(100);
        if(servo.getPosition() > 850){
            servo.setEffort(0);
            action = 0;
        }

    } else if (action == positionOnZero){
        if (abs(motor.moveTo(zeroPosition)) > 2){
            motor.moveTo(zeroPosition);
        } else{
            motor.setEffort(0);
            action = wait;
        }

    } else if (action == positionArmFF){
        if (abs(motor.moveTo(fourtyFivePosition)) > 2){
                    motor.moveTo(fourtyFivePosition);
        } else{
            action = resetEncoders;
        }

    } else if (action == positionOnTF){
        if (abs(motor.moveTo(twentyFivePosition)) > 10){
            motor.moveTo(twentyFivePosition);
        } else{
            motor.setEffort(0);
            action = 0;
        }
    } else if (action == goTowardsBox){

        lineFollow((urfRead - approachBoxDist) * 10);

        if (urfRead < approachBoxDist){
            action = 0;
        }
    } else if (action == 8){
        chassis.setMotorEfforts(0, 0);
        action = 0;

    } else if (action == centerOnLine){
        lineFollow();

        if (moveUntil(10.4)){
            action = 0;
        }

    } else if (action == 11){
        if (!turnUntilNextLineHit(1)){
            hitLine = 0;
            action = 0;
        }

    } else if(action == 12){
        lineFollow();
    }

    if (action == goTowardsFF){

        // proportionally line follow to correct distance to approach 45
        //lineFollow((urfRead - approachFFDist) * 10);
        lineFollow();

        // stop moving and wait if we get to distance
        if (urfRead < approachFFDist){
            action = 0;
        }
    }

    if (action == approachFF){
        
        lineFollow();
        if (moveUntil(2)){
            action = 0;
        };
    }

    if (action == resetEncoders){
        resetChassisEncoders;
        action = 0;
    }



    // print when state change
    if (lastState != state){
        Serial.print("Current Action: ");
        Serial.print(actions[orderOfActions[state]]);
        Serial.print("      Next Action: ");
        Serial.println(actions[orderOfActions[(state + 1) % numStates]]);
        lastState = state;
    }

    if (millis() >= timeToPrint){
        Serial.println(urfRead);
        // Serial.print(leftValue);
        // Serial.print("            ");
        // Serial.println(rightValue);
        //Serial.println(romiDistanceTraveled);
        //Serial.println(effort);
        //Serial.println(servo.getPosition());
        timeToPrint = millis() + printPause;
    }

}
