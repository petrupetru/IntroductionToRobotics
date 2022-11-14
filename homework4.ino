// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int encodingsNumber = 16;
const int displayCount = 4;
const byte regSize = 8; // 1 byte aka 8 bits

const int state1 = 1;
const int state2 = 2;
const unsigned int debounceDelay = 50;
const int longPressTime = 3000;

const int defaultDisplayConfig[] = {0, 0, 0, 0};

int xValue = 0;
int yValue = 0;
bool joyMovedX = false;
bool joyMovedY = false;
int minThreshold = 400;
int maxThreshold = 600;
byte lastButtonValue = LOW;
byte buttonValue = LOW;
byte buttonState = LOW;
byte lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
unsigned long pressDuration = 0;
bool longPress = false;
bool shortPress = false;

int currentDigit = 0;
int currentState = state1;
int currentDisplay = 0;
int displayConfig[]  = {0, 0, 0, 0};

unsigned long lastBlinkTime = 0;
bool blinkState = false;
const unsigned int blinkDelay = 400;


int displayDigits[] = {
  segD1, segD2, segD3, segD4
};


byte registers[regSize];


byte byteEncodings[encodingsNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};

byte byteEncodingsDP[encodingsNumber] = {
//A B C D E F G DP 
  B11111101, // 0.
  B01100001, // 1.
  B11011011, // 2.
  B11110011, // 3.
  B01100111, // 4.
  B10110111, // 5.
  B10111111, // 6.
  B11100001, // 7.
  B11111111, // 8.
  B11110111, // 9.
  B11101111, // A.
  B00111111, // b.
  B10011101, // C.
  B01111011, // d.
  B10011111, // E.
  B10001111  // F.
};

void setup() {
  // put your setup code here, to run once:
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, OUTPUT); 
  pinMode(pinSW, INPUT_PULLUP);
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  Serial.begin(9600);
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
    }
  }

  if(currentState == state1){
    //check joystick movement
    xValue = analogRead(pinX);
    if(xValue >= minThreshold && xValue <= maxThreshold){
      joyMovedX = false;
    }
    else if(xValue > maxThreshold && joyMovedX == false){
      if(currentDisplay > 0){
        currentDisplay -= 1; 
      }
      joyMovedX = true;  
    }
    else if(xValue < minThreshold && joyMovedX == false){
      if(currentDisplay < (displayCount-1)){
        currentDisplay += 1; 
      }
      joyMovedX = true;
    }
    printDisplayState1(displayConfig);

    if(longPress){
      currentDisplay = 0;
      for (int i = 0; i < displayCount; i++){
        displayConfig[i] = defaultDisplayConfig[i];
      }
    }
    if(shortPress){
      currentState = state2;
    }
  }
  else if(currentState == state2){
    //check joystick movement on Y axis
    yValue = analogRead(pinY);
    if(yValue >= minThreshold && yValue <= maxThreshold && joyMovedX == false){
      joyMovedY = false;
    }
    else if(yValue > maxThreshold && joyMovedY == false && joyMovedX == false){
      if(displayConfig[displayCount - 1 - currentDisplay] > 0){
        displayConfig[displayCount - 1 - currentDisplay]--;
      }
      joyMovedY = true;
    }
    else if(yValue < minThreshold && joyMovedY == false && joyMovedX == false){
      if(displayConfig[displayCount - 1 - currentDisplay] < (encodingsNumber - 1)){
        displayConfig[displayCount - 1 - currentDisplay]++;
      }
      joyMovedY = true;
    }


    if(shortPress){
      currentState = state1;
    }
    printDisplayState2(displayConfig);
  }
  lastButtonValue = buttonValue;
}

//display config with DP blink on currentDisplay
void printDisplayState1(int config[displayCount]) {
  for(int i = 0; i < displayCount; i++){
    if(i == currentDisplay){
      writeDisplayblinkDP(config[displayCount - 1 - i]);
    }
    else{
      writeDisplay(config[displayCount - 1 - i]);
    }
    activateDisplay(i);
    delay(5);
  }
}
//display config with DP on currentDisplay
void printDisplayState2(int config[displayCount]){
  for(int i = 0; i < displayCount; i++){
    if(i == currentDisplay){
      writeDisplayDP(config[displayCount - 1 - i]);
    }
    else{
      writeDisplay(config[displayCount - 1 - i]);
    }
    activateDisplay(i);
    delay(5);
  }
}

void activateDisplay(int displayNumber) {

  for (auto display:displayDigits) {
    digitalWrite(display, HIGH);
  }

  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeReg(byte encoding) {
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}
void writeDisplay(int displayNumber){
  writeReg(byteEncodings[displayNumber]);
}
void writeDisplayDP(int displayNumber){
  writeReg(byteEncodingsDP[displayNumber]);
}
void writeDisplayblinkDP(int displayNumber){
  if(millis() - lastBlinkTime >= blinkDelay){
    lastBlinkTime = millis();
    blinkState = !blinkState;
  }
  if(blinkState){
    writeReg(byteEncodings[displayNumber]);
  }
  else{
    writeReg(byteEncodingsDP[displayNumber]);
  }
}
