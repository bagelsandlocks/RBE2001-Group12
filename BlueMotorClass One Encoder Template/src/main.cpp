#include <Arduino.h>
#include <Romi32U4.h>
#include "BlueMotor.h"

<<<<<<< Updated upstream

BlueMotor motor;
Romi32U4ButtonB buttonB;
long timeToPrint = 0;
long now = 0;
long newPosition = 0;
long oldPosition = 0;
long sampleTime = 100;
int speedInRPM = 0;
int CPR = 270;
int motorEffort = 400;

void setup()
{
  Serial.begin(9600);
  motor.setup();
  motor.reset();
  delay(3000);
  Serial.print("Time (ms)");
  Serial.print("   ");
  Serial.print("Position");
  Serial.print("    ");
  Serial.println("speedInRPM");
  delay(3000);
}


void loop()
{
  timeToPrint = millis() + sampleTime;
  oldPosition = motor.getPosition();
  while (buttonB.isPressed())
  {
    // The button is currently pressed.
    motor.setEffort(motorEffort);
    if ((now = millis()) > timeToPrint)
    {
      timeToPrint = now + sampleTime;
      newPosition = motor.getPosition();
      speedInRPM = ((newPosition-oldPosition)/CPR) / (timeToPrint/60000) ;
      Serial.print(now);
      Serial.print("          ");
      Serial.print(newPosition);
      Serial.print("          ");
      Serial.println(speedInRPM);
      oldPosition = newPosition;
      
    }
    
  }
    
  motor.setEffort(0);
=======
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
//armRaiseAngle starts at first house angle and gets changed in state 8 to other house angle
/*bool crossDetected = false;*/

/*potential alternative may be to check for button press instead of stop == false*/
/*for all set efforts if repeatedly writing to the motors/servos is fucking with it then we can nest the command into an if statement so that it only triggers once per entrance and set the stop contingency to reset the controlling variable*/
/*in a similar vain if the cases with multiple while statements are not triggering we can add a boolean variable to automatically skip any that have already been run to completion*/
//May need to add in some resets to the final if statements that switch states, thinking specifically about encoder counts
void loop(){
    switch(robotState){
        case START:
        //reset all variables (replacement for setup)
        if(/*buttonX is pressed on IR*/){
            robotState = 1;
            break;
        }

        break;

        case APPROACHHOUSE:
        while(stop == false && /*call ultrasonic sensor as an analogue read or as a variable thats updated at the bottom of this loop*/ > /*whatever distance the robot needs to be away from the house*/){
            /*check for emergency stop, could make our lives a lot easier if we assign an interrupt to the stop button, otherwise we need to include this line a lot*/
            /*line follow forward*/
            /*depending on if arm read position is faster assigning to a variable here or if an analogue read below is better check arm here*/
            if(/*arm is less than correct raise angle*/){
                /*set effort for raising the arm*/
            }
            /*check US sensor here if not analogue reading in while statement*/
        }

        if(stop == true){
            returnState = 1;
            robotState = 9;
            /*stop arm and wheel movement*/
            break;
        }

        if(/*ultrasonic is less than X inches away*/ && /*arm is in position*/ && taskOneOrFourDone == true && stop == false){
            /*stop arm and wheel movement*/
            robotState = 7;
            break;
        }else if (/*ultrasonic is less than X inches away*/ && /*arm is in position*/ && stop == false){
            /*stop arm and wheel movement*/
            robotState = 2
            break;
        }

        break;

        case GRABPANELHOUSE:
        while(stop == false && /*jaw is not stuck*/){
            /*grab panel*/
        }

        while(stop == false && /*arm is not in position*/){
            /*set arm effort*/
            /*potentially check arm position*/
        }

        if(stop == true){
            returnState = 2;
            robotState = 9;
            /*stop jaw and arm movement*/
            break;
        }

        if(/*panel is grabbed (either jaw stuck variable is true or other method)*/ && /*arm is in position*/){
            /*stop jaw and arm movement*/
            robotState = 3;
            break;
        }

        break;

        case TURN:
        while(stop == false && /*encoder count is less than a 15 degree turn (so that the robot does not see the line underneath when line following)*/){
            /*point turn set effort*/
            /*update encoder count*/
        }
        /*if needed we can add in code that stops movement here*/
        while(stop == false && /*line not detected*/){
            /*point turn*/
        }

        if(stop == true){
            returnState = 3;
            robotState = 9;
            /*stop wheel movement*/
            break;
        }

        if(stop == false && /*line detected*/){
            /*stop wheel movement*/
            robotState = 4;
            break;
        }

        break;

        case APPROACHSTAGING:
        while(stop == false && /*ultrasonic sensor states that we're more than the distance required to place panel on staging area*/){
            /*line follow forward*/
            /*may need to include contingency for cross*/
        }

        if(stop == true){
            returnState = 4;
            robotState = 9;
            /*stop wheel movement*/
            break;
        }

        if(/*ultrasonic reading is at right distance*/){
            /*stop wheel movement*/
            robotState = 5;
            break;
        }

        break;

        case PLACEPANELSTAGING:
        while(stop == false && /*arm is not in position*/){
            /*set arm effort so it lowers*/
        }

        /*maybe add arm movement stop here*/
        while(stop == false && /*jaw has not reached amount open we want*/){
            /*stop arm movement, if not stopped above*/
            /* set effort to open jaw*/
        }

        if(stop == true){
            returnState = 5;
            robotState = 9;
            /*stop arm and jaw movement*/
            break;
        }

        if(stop == false && /*jaw is at the correct amount open*/){
            /*stop jaw and arm movement*/
            if(/*button has been pressed, whatever button we want. probable easiest to choose a button on the romi and check for debounce*/){
                robotState = 6;
                break;
            }
        }

        break;

        case GRABPANELSTAGING:
        while(stop == false && /*jaw is not closed*/){
            /*close jaw*/
        }

        /*potential stop jaw code here*/
        while(stop == false && /*encoder count is less than 15 degree turn*/){
            /*potential stop jaw code*/
            /*start point turn*/
            /*update encoder count*/
        }

        while(stop == false && /*line not detected*/){
            /*point turn*/
        }

        if(stop == true){
            returnState = 6;
            robotState = 9;
            /*stop wheel and jaw movement*/
            break;
        }

        if(stop == false && /*line detected*/){
            /*stop jaw and wheel movement*/
            robotState = 1;
            taskOneOrFourDone = true;
            break;
        }

        break;

        case PLACEPANELHOUSE:
        while(stop == false && /*arm is not in position*/){
            /*set arm effort to lower*/
        }

        /*potential stop arm movement here*/
        while(stop == false && /*jaw is not open enough*/){
            /*potential stop arm movement here*/
            /*set jaw to open*/
        }

        if(stop == true){
            returnState = 7;
            robotState = 9;
            /*stop arm and jaw movement*/
            break;
        }

        if(stop == false && /*jaw is open enough*/){
            /*stop arm and jaw movement*/
            if(/*button is pressed*/){
                robotState = 8;
                break;
            }
        }

        break;

        case LINEFOLLOW:
        while(stop == false && /*encoder count is less than 15 degree turn*/){
            /*start point turn*/
            /*update encoder count*/
        }

        while (stop == false && /*!crossDetected*/){//crossDetected might be better suited to be declared in another class
            /*line follow forward*/
        }
        
        while(stop == false && /*encoderCounts2 is less than a 15 degree turn*/){
            /*point turn*/
            /*update encodercount2*/
        }

        while(stop == false && /*cross2 not detected*/){
            /*line follow forward*/
        }

        while(stop == false && /*encoderCounts3 is less than a 15 degree turn*/){
            /*point turn*/
            /*update encodercount3*/
        }

        while(stop == false && /*line not detected*/){
            /*point turn*/
        }
        
         if(stop == true){
            returnState = 8;
            robotState = 9;
            /*stop wheel movement*/
            break;
        }

        if(stop == false && /*line detected*/){
            /*stop wheel movement*/
            taskOneOrFourDone = false;
            /*armRaiseAngle = task5 house angle*/
            //make sure all variable that need it are reset
            if(/*button is pressed*/){
                robotState = 1;
                break;
            }
        }

        break;

        case STOP:
        if(/*button is pressed*/){
            robotState = 0;
            break;
        }

        /*check for button press here*/
        while(stop == true){
            break;
        }

        if(stop == false){
            robotState = returnState;
            break;
        }
    }
>>>>>>> Stashed changes
}