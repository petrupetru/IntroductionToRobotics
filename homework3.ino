// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output
// declare all the segments pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;
const int numPossibleMoves = 4;
const int segSize = 8;// modify if you have common anode
const bool commonAnode = false; 
const unsigned int debounceDelay = 50;
const unsigned int blinkDelay = 700;
const int longPressTime = 3000;

byte state = false;
int index = 0;
int currentState = 1;
int xValue = 0;
int yValue = 0;
int currentSegment = 7;
int lastSegment = 7;
bool joyMovedX = false;
bool joyMovedY = false;
int minThreshold = 400;
int maxThreshold = 600;
byte lastButtonValue = LOW;
byte buttonValue = LOW;
byte buttonState = LOW;
byte lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
unsigned long pressDuration = 0;
byte blinkState = LOW;
bool longPress = false;
bool shortPress = false;



int displayMapping[segSize][numPossibleMoves] = {
//up, down, left, right
  {0, 6, 5, 1}, //a
  {0, 6, 5, 1}, //b
  {6, 3, 4, 7}, //c
  {6, 3, 4, 2}, //d
  {6, 3, 4, 2}, //e
  {0, 6, 5, 1}, //f
  {0, 3, 6, 1}, //g
  {7, 7, 2, 7}  //dp
};

int segments[segSize] = { 
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
byte segmentStates[segSize] = {
  0, 0, 0, 0, 0, 0, 0, 0
};
byte dpState = LOW; //decimal point

void setup() {
  Serial.begin(9600);
  // initialize all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  if (commonAnode == true) {
    state = !state;
  }
}

void loop() {
  longPress = false;
  shortPress = false;
  //read the button with debounce
  buttonState = digitalRead(pinSW);
  if(buttonState != lastButtonState){
    lastDebounceTime = millis();
    lastButtonState = buttonState;
  }
  if(millis() - lastDebounceTime> debounceDelay){
    buttonValue = !buttonState;
  }
  //check button value and detect long press
  if(buttonValue == 1 && lastButtonValue == 0){
    pressedTime = millis();
   }
  else if(buttonValue == 0 && lastButtonValue == 1){
    releasedTime = millis();
    pressDuration = releasedTime - pressedTime;
    if(pressDuration >= longPressTime)
    {
      //long press detected
      longPress = true;
    }
    else{
      //short press detected
      shortPress = true;
      //currentState = 2;
    }
  }


  //state 1
  if(currentState == 1)
  {
    //check joystick movement
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    if(xValue >= minThreshold && xValue <= maxThreshold){
      joyMovedX = false;
    }
    else if(xValue > maxThreshold && joyMovedX == false){
      lastSegment = currentSegment;
      currentSegment = displayMapping[currentSegment][3];
      joyMovedX = true;
      
    }
    else if(xValue < minThreshold && joyMovedX == false){
      lastSegment = currentSegment;
      currentSegment = displayMapping[currentSegment][2];
      joyMovedX = true;
    }

    if(yValue >= minThreshold && yValue <= maxThreshold && joyMovedX == false){
      joyMovedY = false;
    }
    else if(yValue > maxThreshold && joyMovedY == false && joyMovedX == false){
      lastSegment = currentSegment;
      currentSegment = displayMapping[currentSegment][1];
      joyMovedY = true;
      
    }
    else if(yValue < minThreshold && joyMovedY == false && joyMovedX == false){
      lastSegment = currentSegment;
      currentSegment = displayMapping[currentSegment][0];
      joyMovedY = true;
    }
    if(longPress){
      currentSegment = 7;
      for (int i = 0; i < segSize; i++){
        segmentStates[i] = 0;
      }
    }
    if(shortPress){
      currentState = 2;
    }
    
    //display current configuration
    for (int i = 0; i < segSize; i++){
      digitalWrite(segments[i], segmentStates[i]);
    }
    blink(currentSegment);

    
    
  }

  //state2
  else if(currentState == 2){
    //check joystick movement on X axis
    xValue = analogRead(pinX);
    if(xValue >= minThreshold && xValue <= maxThreshold){
      joyMovedX = false;
    }
    else if((xValue > maxThreshold || xValue < minThreshold) && joyMovedX == false){
      joyMovedX = true;
      segmentStates[currentSegment] = !segmentStates[currentSegment];
    }
    //display current configuration
    for (int i = 0; i < segSize; i++){
      digitalWrite(segments[i], segmentStates[i]);
    }

    //check button value 
    if(shortPress){
      currentState = 1;
    }
    
  }
  lastButtonValue = buttonValue;
}



void blink(int segment){
  if(millis() - lastBlinkTime >= blinkDelay){
    lastBlinkTime = millis();
    blinkState = !blinkState;
  }
  digitalWrite(segments[segment], blinkState);

}


