#include <Arduino.h>
#include <wpi-32u4-lib.h>
#include <IRdecoder.h>
#include <ir_codes.h>
#include <Chassis.h>
#include <Romi32U4.h>
#include <BlueMotor.h>
#include <ServoControl.h>
#include <Rangefinder.h>
#include <LineFollow.h>

Rangefinder rangefinder(0, 12);
BlueMotor motor;
Romi32U4ButtonB buttonB;
const uint8_t IR_DETECTOR_PIN = 1;
IRDecoder decoder(14);
ServoControl servo;
LineFollow lineFollow;
long timeToPrint = 0;
long now = 0;
long newPosition = 0;
long oldPosition = 0;
long sampleTime = 100;
int speedInRPM = 0;
int CPR = 270;
int motorEffort = 400;

enum STATE
{
    START = 0, //Start and reset state. Can only be accessed from the STOP state
    APPROACHHOUSE = 1, //Approach house and lift arm to take panel off
    GRABPANELHOUSE = 2, // Grabs panel off of house
    TURN = 3, //Turns around back towards staging area
    APPROACHSTAGING = 4, // Approaches staging area
    PLACEPANELSTAGING = 5, // Places panel at staging area and pauses
    GRABPANELSTAGING = 6, //When a button is pressed grabs panel and turns
    PLACEPANELHOUSE = 7, //Places panel back on house
    LINEFOLLOW = 8, //Line follows to other house
    STOP = 9 //Emergency stop function. Does nothing unless told to go back to previous state or unless told to reset
};
int robotState = 0;
int returnState;
bool taskOneOrFourDone = false;
bool stop = false;
int fourtyFivePosition = 3650;
int escapeFourtyFivePosition = 4500;
int twentyFivePosition = 6300;
int armRaiseAngle = fourtyFivePosition; //armRaiseAngle starts at first house angle and gets changed in state 8 to other house angle
int distanceFromHouse; //Needs actual value
int distanceFromStagingArea; //Needs real value

/*for all set efforts if repeatedly writing to the motors/servos is fucking with it then we can nest the command into an if statement so that it only triggers once per entrance and set the stop contingency to reset the controlling variable*/
/*in a similar vain if the cases with multiple while statements are not triggering we can add a boolean variable to automatically skip any that have already been run to completion*/
//May need to add in some resets to the final if statements that switch states, thinking specifically about encoder counts
void loop(){
    switch(robotState){
        int keyPress = decoder.getKeyCode();
        if(keyPress == NUM_6){
            stop = !stop;
        }
        case START:
        //reset all variables (replacement for setup) includes all initilizations of sensors, servos, and motors
        if(keyPress == NUM_1){
            robotState = 1;
            break;
        }

        break;

        case APPROACHHOUSE:
        if(stop == false && rangefinder.getDistance() > distanceFromHouse){
            lineFollow.lineFollow();
            motor.setEffort(400)
            if(motor.getPosition() > armRaiseAngle){
                motor.setEffort(0);
            }
        }

        if(stop == true){
            returnState = 1;
            robotState = 9;
            motor.setEffort(0);
            chassis.setMotorEfforts(0,0);
            break;
        }

        if(rangefinder.getDistance() < distanceFromHouse && motor.getPosition() > armRaiseAngle && taskOneOrFourDone == true && stop == false){
            motor.setEffort(0);
            chassis.setMotorEfforts(0,0);
            robotState = 7;
            break;
        }else if (rangefinder.getDistance() < distanceFromHouse && motor.getPosition() > armRaiseAngle && stop == false){
            motor.setEffort(0);
            chassis.setMotorEfforts(0,0);
            robotState = 2
            break;
        }

        break;

        case GRABPANELHOUSE:
        if(stop == false && /*jaw is not stuck*/){
            /*grab panel*/
        }

        if(stop == false && motor.getPosition() < armRaiseAngle){
            motor.setEffort(400);
        }

        if(stop == true){
            returnState = 2;
            robotState = 9;
            servo.jawStop();
            motor.setEffort(0);
            break;
        }

        if(/*panel is grabbed (either jaw stuck variable is true or other method)*/ && motor.getPosition() > armRaiseAngle){
            servo.jawStop();
            motor.setEffort(0);
            robotState = 3;
            break;
        }

        break;

        case TURN:
        if(stop == false && /*encoder count is less than a 15 degree turn (so that the robot does not see the line underneath when line following)*/){
            /*point turn set effort*/
            /*update encoder count*/
        }
        /*if needed we can add in code that stops movement here*/
        if(stop == false && /*line not detected*/){
            /*point turn*/
        }

        if(stop == true){
            returnState = 3;
            robotState = 9;
            chassis.setMotorEfforts(0,0);
            break;
        }

        if(stop == false && /*line detected*/){
            chassis.setMotorEfforts(0,0);
            robotState = 4;
            break;
        }

        break;

        case APPROACHSTAGING:
        if(stop == false && rangefinder.getDistance() > distanceFromStagingArea){
            lineFollow.lineFollow();
            /*may need to include contingency for cross*/
        }

        if(stop == true){
            returnState = 4;
            robotState = 9;
            chassis.setMotorEfforts(0,0);
            break;
        }

        if(rangefinder.getDistance() < distanceFromStagingArea){
            chassis.setMotorEfforts(0,0);
            robotState = 5;
            break;
        }

        break;

        case PLACEPANELSTAGING:
        if(stop == false && motor.getPosition() > armRaiseAngle){
            motor.setEffort(-400);
        }

        if(stop == false && /*jaw has not reached amount open we want*/){
            motor.setEffort(0);
            /* set effort to open jaw*/
        }

        if(stop == true){
            returnState = 5;
            robotState = 9;
            servo.jawStop();
            motor.setEffort(0);
            break;
        }

        if(stop == false && /*jaw is at the correct amount open*/){
            servo.jawStop();
            motor.setEffort(0);
            if(keyPress == NUM_2){
                robotState = 6;
                break;
            }
        }

        break;

        case GRABPANELSTAGING:
        if(stop == false && /*jaw is not closed*/){
            /*close jaw*/
        }

        if(stop == false && /*encoder count is less than 15 degree turn*/){
            servo.jawStop();
            /*start point turn*/
            /*update encoder count*/
        }

        if(stop == false && /*line not detected*/){
            /*point turn*/
        }

        if(stop == true){
            returnState = 6;
            robotState = 9;
            servo.jawStop();
            chassis.setMotorEfforts(0,0);
            break;
        }

        if(stop == false && /*line detected*/){
            servo.jawStop();
            chassis.setMotorEfforts(0,0);
            robotState = 1;
            taskOneOrFourDone = true;
            break;
        }

        break;

        case PLACEPANELHOUSE:
        if(stop == false && motor.getPosition() < armRaiseAngle){
            motor.setEffort(-400);
        }

        if(stop == false && /*jaw is not open enough*/){
            motor.setEffort(0);
            /*set jaw to open*/
        }

        if(stop == true){
            returnState = 7;
            robotState = 9;
            servo.jawStop();
            motor.setEffort(0);
            break;
        }

        if(stop == false && /*jaw is open enough*/){
            servo.jawStop();
            motor.setEffort(0);
            if(keyPress == NUM_3){
                robotState = 8;
                break;
            }
        }

        break;

        case LINEFOLLOW:
        if(stop == false && /*encoder count is less than 15 degree turn*/){
            /*start point turn*/
            /*update encoder count*/
        }

        if (stop == false && /*!crossDetected*/){//crossDetected might be better suited to be declared in another class
            lineFollow.lineFollow();
        }
        
        if(stop == false && /*encoderCounts2 is less than a 15 degree turn*/){
            /*point turn*/
            /*update encodercount2*/
        }

        if(stop == false && /*cross2 not detected*/){
            lineFollow.lineFollow();
        }

        if(stop == false && /*encoderCounts3 is less than a 15 degree turn*/){
            /*point turn*/
            /*update encodercount3*/
        }

        if(stop == false && /*line not detected*/){
            /*point turn*/
        }
        
         if(stop == true){
            returnState = 8;
            robotState = 9;
            chassis.setMotorEfforts(0,0);
            break;
        }

        if(stop == false && /*line detected*/){
            chassis.setMotorEfforts(0,0);
            taskOneOrFourDone = false;
            armRaiseAngle = twentyFivePosition;
            //make sure all variable that need it are reset
            if(keyPress == NUM_4){
                robotState = 1;
                break;
            }
        }

        break;

        case STOP:
        if(keyPress == NUM_5){
            robotState = 0;
            break;
        }

        keyPress = decoder.getKeyCode();
        if(keyPress == NUM_6){
            stop = !stop;
        }
        
        if(stop == true){
            break;
        }

        if(stop == false){
            robotState = returnState;
            break;
        }
    }
}