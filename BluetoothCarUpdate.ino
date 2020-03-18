#include <SoftwareSerial.h>

#define MAX_SPEED              250
#define MIN_SPEED               20

#define STOP                   '0'
#define FORWARD                '1'
#define BACKWARD               '2'
#define LEFT                   '3'
#define RIGHT                  '4'
#define INCREASE_SPEED         '5'
#define DECREASE_SPEED         '6'
#define SET_MAX_SPEED          '7'
#define SET_ABS                '8'
#define SET_AUTONOMOUS         '9'

#define MAX_DISTANCE            20
#define DELAY_AFTER_OBSTACLE   500

//Right Motor
const int enA = 10;
const int in1 = 9;
const int in2 = 8;

//Left Motor
const int in3 = 7;
const int in4 = 6;
const int enB = 5;

//Bluetooth
const int tx = 3; //TX
const int rx = 2; //RX
SoftwareSerial BTSerial(tx, rx);

//Sonar
const int trigPin = 13;
const int echoPin = 12;

//Utilities
char command;
int vSpeed = 100;
int dir;
boolean isBackward = false;
boolean isMovingForward = false;
boolean isAbsOn = true;

//FORWARD
void handleForward(){
  
  //Activates front motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  isBackward = false;
  isMovingForward = true;
}

//BACKWARD
void handleBackward(){
  
  //Activates rear motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  isBackward = true;
  isMovingForward = false;
}

//LEFT
void handleLeft(){  
  
  //Activates right motors (to go to the opposite direction)
  if(!isBackward){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }
  else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
  isMovingForward = false;
  
}

//RIGHT
void handleRight(){
  
  //Activates left motors
  if(!isBackward){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
  else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
  isMovingForward = false;
  
}

//STOP
void handleStop(){
  
  //Stops motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  isBackward = false;
  isMovingForward = false;
}

//Increase speed by 10
void increaseSpeed(){

  if (vSpeed < MAX_SPEED){
    vSpeed += 10;
    analogWrite(enA, vSpeed);
    analogWrite(enB, vSpeed);
  }    
  
}

//Decrease speed by 10
void decreaseSpeed(){

  if (vSpeed > MIN_SPEED){
    vSpeed -= 10;
    analogWrite(enA, vSpeed);
    analogWrite(enB, vSpeed);
  }    
  
}

//Set the speed to max speed
void setMaxSpeed(){
  
  vSpeed = MAX_SPEED;
  analogWrite(enA, vSpeed);
  analogWrite(enB, vSpeed);
  
} 

//Gets the distance from the closest obstacle
int getDistance(){
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds  
  return pulseIn(echoPin, HIGH) * 0.034/2;
}

//Checks if the radar signals obstacles in front of the car, in case it stops
boolean stopIfObstacle(){

  int safetyDistance = MAX_DISTANCE + vSpeed/20;
  if (getDistance() <= safetyDistance){
    handleStop();
    return true;
  }

  return false;
}

//Makes a manouver to avoid an obstacle
void avoidObstacle(){
  
  handleBackward();
  delay(DELAY_AFTER_OBSTACLE);
  handleLeft();
  delay(DELAY_AFTER_OBSTACLE);
  handleForward();
  
}

//Handlse autonomous driving
void driveAutonomous(){

  handleForward();
  
  while(true){
    if(stopIfObstacle())
      avoidObstacle();

    if(BTSerial.available()){
      command = BTSerial.read();
      
      //Increase speed if command is 5
      if (command == INCREASE_SPEED)
        increaseSpeed();

      //Decrease speed if command is 6
      else if (command == DECREASE_SPEED)
        decreaseSpeed();

      //Set speed equal to maximum value
      else if (command == SET_MAX_SPEED)
        setMaxSpeed();

      //Starts driving autonomously
      else if (command == SET_AUTONOMOUS or command == STOP)
        break;
    }
  }

  handleStop();
  
}

void setup() {
  
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    analogWrite(enA, vSpeed);
    analogWrite(enB, vSpeed);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    Serial.begin(9600);
    BTSerial.begin(9600);
}
 
void loop() {
  
  while(!BTSerial.available()){
    //Checks for obstancle in front of the car in case the car is moving forward
    if(isMovingForward and isAbsOn)
      stopIfObstacle();
  }

  command = BTSerial.read();

  //Stop if command is 0
  if (command == STOP)
    handleStop();
         
  //Send forward if command is 1
  else if (command == FORWARD)
    handleForward();
      
  //Send backward if command is 2
  else if (command == BACKWARD)
    handleBackward();

  //Send left if command is 3
  else if (command == LEFT)
    handleLeft();

  //Send right if command is 4
  else if (command == RIGHT)
    handleRight();

  //Increase speed if command is 5
  else if (command == INCREASE_SPEED)
    increaseSpeed();

  //Decrease speed if command is 6
  else if (command == DECREASE_SPEED)
    decreaseSpeed();

  //Set speed equal to maximum value
  else if (command == SET_MAX_SPEED)
    setMaxSpeed();

  //Set ABS on/off
  else if (command == SET_ABS)
    isAbsOn = !isAbsOn;

  //Starts driving autonomously
  else if (command == SET_AUTONOMOUS)
    driveAutonomous();
}
