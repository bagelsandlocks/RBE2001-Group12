#include <Arduino.h>
#include <wpi-32u4-lib.h>

#include <IRdecoder.h>
#include <ir_codes.h>

#include <Chassis.h>

#include <Romi32U4.h>

#include <BlueMotor.h>
#include <ServoControl.h>
#include <Rangefinder.h>

Romi32U4ButtonB buttonB;

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
    lineFollowUntilLine = 56,
    turnRight = 54,
    approachTF = 55,

};


// Final Project Task 1

// int orderOfActions[] = {wait, openGripper, goTowardsFF, positionArmFF, approachFF,
//                          closeGripper, liftFromFF, turnToNextLine, positionOnZero, openGripper};

// int orderOfActions[] = {wait, openGripper, goTowardsFF, positionArmFF, approachFF,
//                          closeGripper, positionOnTF, turnToNextLine, positionOnZero, goTowardsBox, openGripper};

//Final Project Task 3
// Line follow to next house
int orderOfActions[] = {wait, lineFollowUntilLine, centerOnLine, turnRight, lineFollowUntilLine, centerOnLine, turnRight, approachTF};

// Final Project Task 3
//int orderOfActions[] = {0, 1, 3, 2, 4, 1, 2, 5, 1, 2};

int numStates = sizeof(orderOfActions) / sizeof(int);

ServoControl servo;
BlueMotor motor;
Rangefinder rangefinder(7, 12);

int fourtyFivePosition = 3300;
int escapeFourtyFivePosition = 3100; // after backing up a lil bit
int twentyFivePosition = 4000;
int zeroPosition = 0;

int approachFFDist = 9.2;
int approachTFDist = 9.2;
int approachBoxDist = 4;

unsigned long long timeToPrint = 0;
const unsigned int printPause = 250;

int distanceToReckon = 10.5; // until robot is centered over black tape line
int motorClickCount = 0;
float romiDistanceTraveled = 0;

int hitLine = 0;
bool linePrevState = 0;
bool lineState = 0;
bool oldButtonState = 0;


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

// 0 CCW, 1 CW returns true if done
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
            return 1;
        } else{
            return 0;
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
        return 1;
        } else{
            return 0;
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
        Serial.println(romiDistanceTraveled);

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
    //if (keyPress == ENTER_SAVE){
    if (buttonB.isPressed() && oldButtonState == 0){
        oldButtonState = 1;
        chassis.setMotorEfforts(0, 0);
        state = (state + 1) % numStates;
        action = orderOfActions[state];
        delay(1000);
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
        //servo.setEffort(0);
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
        oldButtonState = 0;
        //action = int(Serial.write(Serial.parseInt()));
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
            action = wait;
        }

    } else if (action == positionOnZero){
        if (motor.getPosition() > zeroPosition){
            motor.setEffort(400);
        } else{
            motor.setEffort(0);
            action = wait;
        }

    } else if (action == positionArmFF){
        if (motor.getPosition() < fourtyFivePosition){
                    motor.setEffort(-400);
        } else{
            motor.setEffort(0);
            Serial.println("set to 45");
            action = resetEncoders;
        }

    } else if (action == positionOnTF){
        if (motor.getPosition() < twentyFivePosition){
            motor.setEffort(-400);
        } else{
            motor.setEffort(0);
            action = 0;
        }
    } else if (action == goTowardsBox){

        lineFollow();

        if (urfRead < approachBoxDist){
            action = 0;
        }
    } else if (action == turnToNextLine){
        if (turnUntilNextLineHit(0)){
            action = 0;
        };

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
        if (moveUntil(4)){
            action = 0;
        };
    }

    if (action == resetEncoders){
        resetChassisEncoders();
        action = 0;
    }

    if (action == liftFromFF){
        if (abs(motor.moveTo(fourtyFivePosition + 100)) > 10){
            motor.moveTo(fourtyFivePosition + 100);
        } else{
            motor.setEffort(0);
            action = wait;
        }
    }

    if (action == lineFollowUntilLine){
        lineFollow();

        if (leftValue + rightValue > 1400){
            chassis.setMotorEfforts(0, 0);
            resetChassisEncoders();
            action = 0;
        }
    }

    if (action == turnRight){
        if (turnUntilNextLineHit(1)){
            action = wait;
        };
    }

    if (action == approachTF){
        lineFollow();

        // stop moving and wait if we get to distance
        if (urfRead < approachTFDist){
            action = 0;
        }
    }



    // print when state change
    if (lastState != state){
        Serial.print("Current Action: ");
        //Serial.print(actions[orderOfActions[state]]);
        Serial.print("      Next Action: ");
        //Serial.println(actions[orderOfActions[(state + 1) % numStates]]);
        lastState = state;
    }

    if (millis() >= timeToPrint){
        //Serial.println(urfRead);
        // Serial.print(leftValue);
        // Serial.print("            ");
        // Serial.println(rightValue);
        //Serial.println(romiDistanceTraveled);
        //Serial.println(effort);
        //Serial.println(servo.getPosition());
        //Serial.println(motor.getPosition());
        Serial.print(leftValue);
        Serial.print("       ");
        Serial.println(rightValue);
        timeToPrint = millis() + printPause;
    }

}
