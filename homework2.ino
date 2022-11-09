const int carGreenPin = 5;
const int carYellowPin = 6;
const int carRedPin = 7;
const int pedGreenPin = 11;
const int pedRedPin = 10;
const int buttonPin = 12;
const int buzzerPin = 3;
const int state3BlinkInterval = 400;
const int state4BlinkInterval = 250;
const int state1Duration = 8000; //8000
const int state2Duration = 3000; //3000
const int state3Duration = 8000; //8000
const int state4Duration = 4000; //4000
const int untilState2 = state1Duration;
const int untilState3 = state1Duration + state2Duration;
const int untilState4 = state1Duration + state2Duration + state3Duration;
const int untilState1 = state1Duration + state2Duration + state3Duration + state4Duration;
const int frequency = 250;

byte carGreenValue = LOW;
byte carYellowValue = LOW;
byte carRedValue = LOW;
byte pedGreenValue = LOW;
byte pedRedValue = LOW;
byte buttonState = LOW;
byte buttonValue = LOW;
byte lastButtonState = LOW;
byte trafficLightTrigger = false;
byte blinkState3 = true;
byte blinkState4 = true;
unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 50;
unsigned int triggerTime = 0;
unsigned int currentTime = 0;
unsigned int lastMillisState3 = 0;
unsigned int lastMillisState4 = 0;


void setup() {
  pinMode(carGreenPin, OUTPUT);
  pinMode(carYellowPin, OUTPUT);
  pinMode(carRedPin, OUTPUT);
  pinMode(pedGreenPin, OUTPUT);
  pinMode(pedRedPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  currentTime = millis();

  //select the state based on button and elapsed time 
  if(trafficLightTrigger == false || (trafficLightTrigger == true && (currentTime - triggerTime) <= untilState2 )){
    defaultState();
  }
  else if(trafficLightTrigger == true && (currentTime - triggerTime) > untilState2 && (currentTime - triggerTime) <= untilState3){
    state2();
  }
  else if(trafficLightTrigger == true && (currentTime - triggerTime) > untilState3 && (currentTime - triggerTime) <= untilState4){
    state3();
  }
  else if(trafficLightTrigger == true && (currentTime - triggerTime) > untilState4 && (currentTime - triggerTime) <= untilState1){
    state4();
  }
  else {
    trafficLightTrigger = false;
  }
  
  

  //read the button with debounce
  buttonState = digitalRead(buttonPin);
  if(buttonState != lastButtonState) {
    lastDebounceTime = millis();
    lastButtonState = buttonState;
  }
  if( millis() - lastDebounceTime > debounceDelay) {
    buttonValue = !buttonState; //pullup button so buttonValue = pressed when buttonState = LOW
  }

  if(buttonValue == HIGH && trafficLightTrigger == false){
    trafficLightTrigger = true;
    triggerTime = millis();
    
  }

}


void defaultState() {
  noTone(buzzerPin);
  digitalWrite(carGreenPin, HIGH);
  digitalWrite(carRedPin, LOW);
  digitalWrite(carYellowPin, LOW);
  digitalWrite(pedGreenPin, LOW);
  digitalWrite(pedRedPin, HIGH);
}

void state2() {
  digitalWrite(carGreenPin, LOW);
  digitalWrite(carRedPin, LOW);
  digitalWrite(carYellowPin, HIGH);
  digitalWrite(pedGreenPin, LOW);
  digitalWrite(pedRedPin, HIGH);
}

void state3() {
  
  if(millis() - lastMillisState3 >= state3BlinkInterval){
    lastMillisState3 = millis();
    if(blinkState3 == true){  
      blinkState3 = false;
      tone(buzzerPin, frequency);
      digitalWrite(carGreenPin, LOW);
      digitalWrite(carRedPin, HIGH);
      digitalWrite(carYellowPin, LOW);
      digitalWrite(pedGreenPin, HIGH);
      digitalWrite(pedRedPin, LOW);
    }
    else{
      blinkState3 = true;
      noTone(buzzerPin);
      digitalWrite(carGreenPin, LOW);
      digitalWrite(carRedPin, HIGH);
      digitalWrite(carYellowPin, LOW);
      digitalWrite(pedGreenPin, HIGH);
      digitalWrite(pedRedPin, LOW);
    }
  }
}
void state4() {
  if(millis() - lastMillisState4 >= state4BlinkInterval){
    lastMillisState4 = millis();
    if(blinkState4 == true){
      blinkState4 = false;
      tone(buzzerPin, frequency);
      digitalWrite(carGreenPin, LOW);
      digitalWrite(carRedPin, HIGH);
      digitalWrite(carYellowPin, LOW);
      digitalWrite(pedGreenPin, LOW);
      digitalWrite(pedRedPin, LOW);
    }
    else{
      blinkState4 = true;
      noTone(buzzerPin);
      digitalWrite(carGreenPin, LOW);
      digitalWrite(carRedPin, HIGH);
      digitalWrite(carYellowPin, LOW);
      digitalWrite(pedGreenPin, HIGH);
      digitalWrite(pedRedPin, LOW);
    }
  }
}
