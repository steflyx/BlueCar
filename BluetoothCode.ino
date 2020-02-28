#include <SoftwareSerial.h>
#include <DHT.h>
#include "DHT.h"
#include "pitches.h"

//Right Motor
const int enA = 10;
const int in1 = 9;
const int in2 = 8;

//Left Motor
const int in3 = 7;
const int in4 = 6;
const int enB = 5;

//Led pins
const int leftRed = 4;
const int whites = 12;
const int rightRed = 13;
const int analogRed = A3;
const int analogGreen = A4;
const int analogBlue = A5;

//Sensor pin
#define DHTPIN 11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Buzzer pin and melody arrays
const int buzzer = A0;
int melody[] = {
  NOTE_C4, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_F4, NOTE_C5,
  NOTE_AS4, NOTE_A4, NOTE_A4, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,
  NOTE_AS4, NOTE_C5, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_G4
};

int noteDuration[] = {
  4, 4, 8, 8, 4, 4, 3, 8, 3, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 3
};

int noteNumber = 21;

//Bluetooth
const int tx = 3; //TX
const int rx = 2; //RX
SoftwareSerial BTSerial(tx, rx);

//Messages
String messages[12] = {
  "Forward!",
  "Backward!",
  "Left!",
  "Right!",
  "Stop!",
  "Sound!",
  "Siren On!",
  "Siren Off!",
  "Type the new speed:",
  "New speed set!",
  "Type the message:",
  "Clacson!"
};

//Utilities
String state;
boolean isBlinkerRightOn;
boolean isBlinkerLeftOn;
boolean isLedHigh = false;
boolean isSirenOn = false;
boolean isSirenRed;
int timeSiren;
int timeBlinker; 
int vSpeed = 100;
const int maxMessageLength = 256;

//FORWARD
void handleForward(){
  //Activates front motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  //Activates front lights
  digitalWrite(leftRed, LOW);
  digitalWrite(whites, HIGH);
  digitalWrite(rightRed, LOW);  

  //Deactivates blinkers
  isBlinkerLeftOn = false;
  isBlinkerRightOn = false;
  
  //Prints "Forward!"
  BTSerial.println(messages[0]);
}

//BACKWARD
void handleBackward(){
  //Activates rear motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  //Activates rear lights
  digitalWrite(leftRed, HIGH);
  digitalWrite(whites, LOW);
  digitalWrite(rightRed, HIGH);

  //Deactivates blinkers
  isBlinkerLeftOn = false;
  isBlinkerRightOn = false;
  
  //Prints "Backward!"
  BTSerial.println(messages[1]);
}

//LEFT
int handleLeft(){  
  //Activates right motors (to go to the opposite direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  //Activates Left lights
  digitalWrite(leftRed, HIGH);
  digitalWrite(whites, HIGH);
  digitalWrite(rightRed, LOW);
  isBlinkerLeftOn = true;
  isBlinkerRightOn = false;
  isLedHigh = true;
  
  //Prints "Left!"
  BTSerial.println(messages[2]);

  return millis();
}

//RIGHT
int handleRight(){
  //Activates left motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  //Activates right lights
  digitalWrite(leftRed, LOW);
  digitalWrite(whites, HIGH);
  digitalWrite(rightRed, HIGH);
  isBlinkerRightOn = true;
  isBlinkerLeftOn = false;
  isLedHigh = true;
  
  //Pritns "Right!"  
  BTSerial.println(messages[3]);

  return millis();
}

//STOP
void handleStop(){
  //Stops motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  //Turns off all lights
  digitalWrite(leftRed, LOW);
  digitalWrite(whites, LOW);
  digitalWrite(rightRed, LOW);
  
  //Deactivates blinkers
  isBlinkerLeftOn = false;
  isBlinkerRightOn = false;
  
  //Prints "Stop!"
  BTSerial.println(messages[4]);
}

//TEMPERATURE
void handleTemperature(){
  //Reads temperature from sensor
  int t = dht.readTemperature();

  //Outputs the result
  String output = "There's ";
  output = output + t + " degrees here";
  BTSerial.println(output);
}

//HUMIDITY
void handleHumidity(){
  //Reads humidity from sensor
  int h = dht.readHumidity();

  //Outputs the result
  String output = "There's ";
  output = output + h + "% humidity here";
  BTSerial.println(output);
}

//HELP
void handleHelp(){
  String output = "The available commands are:\n - Forward\n - Backward\n - Left\n - Right\n - Stop\n - Temperature\n - Humidity\n - Sound\n - Siren\n - Speed\n - Morse\n - Clacson\n - Commands";
  BTSerial.println(output);
}

//SOUND
void handleSound(){
  //Prints "Sound!"
  BTSerial.println(messages[5]);
  
  //Plays the melody
  for(int i = 0; i < noteNumber; i++){
    int duration = 1500 / noteDuration[i];
    tone(buzzer, melody[i], duration);
    delay(duration * 1.3);
  }
}

//SIREN
//Used to light up the RGB led
void lightSiren(int red, int green, int blue){
  analogWrite(analogRed, red);
  analogWrite(analogBlue, blue);
  analogWrite(analogGreen, green);
}

//Starts the siren blinking
int startSiren(){
  //Starts the siren
  if(!isSirenOn){
    BTSerial.println(messages[6]);
    lightSiren(255, 0, 0);
    isSirenRed = true;
  }
  //Stops the siren
  else{
    BTSerial.println(messages[7]);
    lightSiren(0,0,0);
  }

  isSirenOn = !isSirenOn;
  return millis();
}

//Handles the switch from red to blue and viceversa, returning the time
int handleSiren(){
  //If siren is red switches to blue and viceversa
  if (isSirenRed)
    lightSiren(0, 0, 255);
  else
    lightSiren(255, 0, 0);
    
  isSirenRed = !isSirenRed;
  return millis();
}

//BLINKER
int handleBlinker(){

    //Handles right blinker
    if(isBlinkerRightOn){
        if (isLedHigh)
          digitalWrite(rightRed, LOW);
        else digitalWrite(rightRed, HIGH);
    }

    //Handles left blinker
    if(isBlinkerLeftOn){
      if (isLedHigh)
          digitalWrite(leftRed, LOW);
      else digitalWrite(leftRed, HIGH);
    }
    
    isLedHigh = !isLedHigh;
    return millis();
}

//SPEED
//Sets the new speed according to the user's input
void handleSpeed(){
   //Prints request for speed
   BTSerial.println(messages[8]);

   //Waits for input
   while(!BTSerial.available());

   //Reads speed from input
   String inputSpeed;
   inputSpeed = BTSerial.readString();
   vSpeed = inputSpeed.toInt();

   //Checks the speed limit
   if (vSpeed > 255)
    vSpeed = 255;

   //Prints new speed
   BTSerial.println(messages[9]);
}

//MORSE
//Translates a string in Morse code through the buzzer
void handleMorse(){
  //Prints request for message
  BTSerial.println(messages[10]);

  //Waits for input
  while(!BTSerial.available());

  //Copies input in a char array
  String userInput;
  userInput = BTSerial.readString();
  userInput.toLowerCase();
  //char message[maxMessageLength];
  //userInput.toCharArray(message, max(userInput.length(), maxMessageLength));

  //Converts each letter into the correspondig Morse code
  //int messageToInt[maxMessageLength];
  //String output = "";
  for (int i=0; i<userInput.length(); i++){
    char letter = userInput.charAt(i);
    if (letter == 'a'){
      playShort();
      playLong();
    }
    else if (letter == 'b'){
      playLong();
      playShort();
      playShort();
      playShort(); 
    }      
    else if (letter == 'c'){
      playLong();
      playShort();
      playLong();
      playShort();
    }
    else if (letter == 'd'){
      playLong();
      playShort();
      playShort();
    }
    else if (letter == 'e'){
      playShort();
    }
    else if (letter == 'f'){
      playShort();
      playShort();
      playLong();
      playShort();
    }
    else if (letter == 'g'){
      playLong();
      playLong();
      playShort();
    }
    else if (letter == 'h'){
      playShort();
      playShort();
      playShort();
      playShort();
    }
    else if (letter == 'i'){      
      playShort();
      playShort();
    }
    else if (letter == 'j'){
      playShort();
      playLong();
      playLong();
      playLong();
    }
    else if (letter == 'k'){
      playLong();
      playShort();
      playLong();
    }
    else if (letter == 'l'){
      playShort();
      playLong();
      playShort();
      playShort();
    }
    else if (letter == 'm'){
      playLong();
      playLong();
    }
    else if (letter == 'n'){
      playLong();      
      playShort();
    }
    else if (letter == 'o'){
      playLong();
      playLong();
      playLong();
    }
    else if (letter == 'p'){      
      playShort();
      playLong();
      playLong();
      playShort();      
    }
    else if (letter == 'q'){
      playLong();
      playLong();
      playShort();
      playLong();
    }
    else if (letter == 'r'){      
      playShort();
      playLong();
      playShort();
    }
    else if (letter == 's'){
      playShort();
      playShort();
      playShort();      
    }
    else if (letter == 't'){
      playLong();
    }
    else if (letter == 'u'){
      playShort();
      playShort();
      playLong();
    }
    else if (letter == 'v'){
      playShort();
      playShort();
      playShort();
      playLong();
    }
    else if (letter == 'w'){
      playShort();
      playLong();
      playLong();
    }
    else if (letter == 'x'){
      playLong();
      playShort();
      playShort();
      playLong();
    }
    else if (letter == 'y'){
      playLong();
      playShort();
      playLong();
      playLong();
    }
    else if (letter == 'z'){
      playLong();
      playLong();
      playShort();
      playShort();
    }
    else if (letter == ' ')
      delay(1000);

    delay(300);
  }
}

//Makes a short sound
void playShort(){
  tone(buzzer, 1000, 500);
  delay(600);
}

//Makes a long sound
void playLong(){
  tone(buzzer, 1000, 1000);
  delay(1100);
}

//CLACSON
void handleClacson(){
  playShort();
}

void setup() {

    pinMode(leftRed, OUTPUT);
    pinMode(whites, OUTPUT);
    pinMode(rightRed, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(analogRed, OUTPUT);
    pinMode(analogBlue, OUTPUT);
    pinMode(analogGreen, OUTPUT);
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    Serial.begin(9600);
    BTSerial.begin(9600);
}
 
void loop() {

  analogWrite(enA, vSpeed);
  analogWrite(enB, vSpeed);
  
  if(BTSerial.available()){
    state = BTSerial.readString();
  }

  //Send forward if letter is F
  if (state == "f" or state == "F" or state == "Forward" or state == "forward")
    handleForward();
         
  //Send forward if letter is B
  else if (state == "b" or state == "B" or state == "Backward" or state == "backward")
    handleBackward();
      
  //Send forward if letter is L
  else if (state == "l" or state == "L" or state == "Left" or state == "left")
    timeBlinker = handleLeft();

  //Send forward if letter is R
  else if (state == "r" or state == "R" or state == "Right" or state == "right")
    timeBlinker = handleRight();

  //Stop if letter is S
  else if (state == "s" or state == "S" or state == "Stop" or state == "stop")
    handleStop();

  //Read temperature if letter is T
  else if (state == "t" or state == "T" or state == "Temp" or state == "temp" or state == "Temperature" or state == "temperature")
    handleTemperature();

  //Read humidity if letter is H
  else if (state == "h" or state == "H" or state == "Humidity" or state == "humidity")
    handleHumidity();

  //Display available commands if letter is C
  else if (state == "Commands" or state == "commands" or state == "help" or state == "Help")
    handleHelp();

  //Make sound if letter is P
  else if (state == "p" or state == "Sound" or state == "sound")
    handleSound();

  //Light up the siren if command is "Siren"
  else if (state == "Siren" or state == "siren")
    timeSiren = startSiren();

  //Set the speed
  else if (state == "Speed" or state == "speed")
    handleSpeed();

  //Translate a message in Morse code
  else if (state == "Morse" or state == "morse")
    handleMorse();

  //Makes just a short sound as clacson
  else if (state == "Clacson" or state == "clacson" or state == "c" or state == "C" )
    handleClacson();
   
  while(!BTSerial.available()){
    
    //To make blinkers work
    if((isBlinkerRightOn or isBlinkerLeftOn) and (millis() - timeBlinker) > 500)
      timeBlinker = handleBlinker();

    //To sound the siren
    if(isSirenOn and (millis() - timeSiren) > 500)
      timeSiren = handleSiren();
  }
}
