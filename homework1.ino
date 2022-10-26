const int pinLedRed = 9;
const int pinLedGreen = 10;
const int pinLedBlue = 11;

const int potRedPin = A5;
const int potGreenPin = A4;
const int potBluePin = A3;

const int maxAnalogValue = 255;
const int minAnalogValue = 0;
const int maxPotValue = 1023;
const int minPotValue = 0;

const int threshold = 3;

int potRedValue = 0;
int potGreenValue = 0;
int potBlueValue = 0;

int ledRedValue = 0;
int ledGreenValue = 0;
int ledBlueValue = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedBlue, OUTPUT);
  pinMode(potRedPin, INPUT);
  pinMode(potRedPin, INPUT);
  pinMode(potRedPin, INPUT);

}

void loop() {
  potRedValue = analogRead(potRedPin);
  potGreenValue = analogRead(potGreenPin);
  potBlueValue = analogRead(potBluePin);


  if(potRedValue > threshold)
    ledRedValue = map(potRedValue, minPotValue, maxPotValue, minAnalogValue, maxAnalogValue);
  else
    ledRedValue = 0;
    
  if(potGreenValue > threshold)
    ledGreenValue = map(potGreenValue, minPotValue, maxPotValue, minAnalogValue, maxAnalogValue);
  else
    ledGreenValue = 0;
    
   if(potBlueValue > threshold)
    ledBlueValue = map(potBlueValue, minPotValue, maxPotValue, minAnalogValue, maxAnalogValue);
  else
    ledBlueValue = 0;
  
  analogWrite(pinLedRed, ledRedValue);
  analogWrite(pinLedGreen,ledGreenValue);
  analogWrite(pinLedBlue, ledBlueValue);
}
