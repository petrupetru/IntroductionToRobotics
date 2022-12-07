#include<LiquidCrystal.h>
#include"LedControl.h"
#include <time.h>
#include <string.h>
#include<EEPROM.h>


//states
const byte sInit = 0;
const byte sGame = 1;
const byte sGameOver = 2;
const byte sMenu = 3;
const byte sLeaderboard = 4;
const byte sAbout = 5;
const byte sHow = 6;

//joystick pins
const int xPin = A0;
const int yPin = A1;
const int pinSW = 2;

//matrix pins
const byte dinPin =12;
const byte clockPin =11;
const byte loadPin =10;
const byte matrixSize =8;
bool matrixChanged = true;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);


//lcd pins
const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;


const byte moveInterval = 100;
unsigned long long lastMoved = 0;
unsigned long long lastBlinkFood = 0;
unsigned long blinkFoodDelay = 400;
const unsigned int debounceDelay = 50;


byte xPos = 0;
byte yPos = 7;
byte brightnessLCDPin = 3;
byte matrixBrightness = 10;
int lcdBrightness = 100;


byte xLastPos = 0;
byte yLastPos = 0;
bool joyMovedX = false;
bool joyMovedY = false;
const int minThreshold = 400;
const int maxThreshold = 600;
byte lastButtonValue = LOW;
byte buttonValue = LOW;
byte buttonState = LOW;
byte lastButtonState = LOW;
unsigned long lastDebounceTime = 0;

byte xFood = 0;
byte yFood = 0;
byte state = 0;

const int nameSize = 3;
char name[nameSize] = "QQQ";
int score = 0;
unsigned int initDuration = 2;
unsigned int gameDuration = 15;
unsigned int timeRemaing = gameDuration;
unsigned long long timeStart = 0;
const int leaderboardSize = 5; 
char leaderboardNames[leaderboardSize][3] = {"DEF", "DEF", "DEF", "DEF", "DEF"};
int leaderboardScores[leaderboardSize] = {5, 4, 3, 2, 1};
int highscore = 0;
const char about[] = "github.com/petrupetru";






/*MENU:
  (0)Play
  (1)Leaderboard -> 1,2,3,4,5
  (2)Settings ->  (0)Name:
                  (1)LCD Brightness
                  (2)Matrix Brightness
                  (3)Sound -> On / Off
  (3)About
  (4)How To Play
*/
const int displayColumns = 16;
const int dispalyRows = 2;
const int menuSize = 5;
String menu[menuSize] = {
  "Play",
  "Leaderboard",
  "Settings",
  "About",
  "How to Play?"
};
byte menuSelector = 0;
byte leaderboardSelector = 0;


LiquidCrystal lcd(RS,enable,d4,d5,d6,d7);
byte selectArrow[] = {
  B00000,
  B10000,
  B11000,
  B11100,
  B11110,
  B11100,
  B11000,
  B10000
};
byte crown[] = {
  B00000,
  B00000,
  B10001,
  B10101,
  B11111,
  B11111,
  B00000,
  B00000
};

byte matrixByte[matrixSize] = {
    B11111111,
    B10000001,
    B10111101,
    B10100101,
    B10110101,
    B10000101,
    B11111101,
    B00000001,
  };

byte matrixByteEmpty[matrixSize] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
};


const uint64_t matrixInit = {0x007f415d457d01ff};
const uint64_t matrixEmpty = {0x0000000000000000}; 
const uint64_t matrixLeaderboard = {0xff80bea2ba82fe00};

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  

  pinMode(brightnessLCDPin, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);

  lcd.begin(16,2);
  lcd.createChar(1, selectArrow);
  lcd.createChar(2, crown);
  srand(time(0));
  /*EEPROM.put(0, leaderboardNames);
  int address_arr2 = sizeof(leaderboardNames);
  EEPROM.put(address_arr2, leaderboardScores);*/

  EEPROM.get(0, leaderboardNames);
  int address_arr2 = sizeof(leaderboardNames);
  EEPROM.get(address_arr2, leaderboardScores);
}

void loop() {

  analogWrite(brightnessLCDPin, lcdBrightness);

  readButton();
  stateSelector(state);
  

  if(matrixChanged == true){
    updateByteMatrix();
    matrixChanged = false;
  }
  lastButtonValue = buttonValue;
}

void stateSelector(byte state) {
  if (state == sInit) {
    stateInit();
  }
  else if (state == sGame) {
    stateGame();
  }
  else if (state == sGameOver) {
    stateGameOver();
  }
  else if (state == sMenu) {
    stateMenu();
  }
  else if (state == sLeaderboard) {
    stateLeaderboard();
  }
  else if (state == sAbout) {
    stateAbout();
  }
  
}

void stateGameOver() {
  highscore = leaderboardScores[0];
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.print("  ");
  lcd.write(2);
  lcd.print(": ");
  lcd.print(highscore);  
  

  if(buttonValue == 1 && lastButtonValue == 0){
    displayImage(matrixEmpty);
    matrixChanged = true;
    lcd.clear();
    state = sMenu;
  }
}



void stateMenu() {
  displayImage(matrixInit);
  updateMenuPosition();
  lcd.setCursor(0, 0);
  if(menuSelector < menuSize - 1){
    String line1 = menu[menuSelector];
    String line2 = menu[menuSelector + 1];
    lcd.write(1);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(' ');
    lcd.print(line2);
  }
  else {
    String line1 = menu[menuSelector - 1];
    String line2 = menu[menuSelector];
    lcd.print(' ');
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.write(1);
    lcd.print(line2);
  }
  if(buttonValue == 1 && lastButtonValue == 0) {
    if (menuSelector == 0){
      lcd.clear();
      displayImage(matrixEmpty);
      timeStart = millis();
      score = 0;
      state = sGame;
    }
    if (menuSelector == 1){
      lcd.clear();
      displayImage(matrixEmpty);
      EEPROM.get(0, leaderboardNames);
      int address_arr2 = sizeof(leaderboardNames);
      EEPROM.get(address_arr2, leaderboardScores);
      state = sLeaderboard;
    }
    if (menuSelector == 3){
      lcd.clear();
      displayImage(matrixEmpty);
      state = sAbout;
    }
  }
}

void stateInit(){
  if (initDuration - millis()/1000 > 0) {
    lcd.setCursor(0, 0);
    lcd.print("Hello!");
  }
  else {
    for (int i; i < matrixSize; i++)
      matrixByte[i] = matrixByteEmpty[i];
    lcd.clear();
    state = sMenu;
  }
}

void stateGame() {
  timeRemaing = gameDuration - (millis() - timeStart) / 1000;
  
 
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (timeRemaing > 9) {
    lcd.print(timeRemaing);
  }
  else{
    lcd.print(' ');
    lcd.print(timeRemaing);
  }
    
  lcd.setCursor(0,1);
  lcd.print("Score: ");
  lcd.print(score);

  updateGamePositions();
  blinkFood();
  if (xPos == xFood && yPos == yFood){
    score++;
    generateFood();
  }

  if (timeRemaing == 0) {
    updateLeaderboard();
    state = sGameOver;
    lcd.clear();
  
    if (score > highscore) {
      const uint64_t smiley = {0x3c4299a581a5423c};
      displayImage(smiley);

    }
    else {
      const uint64_t sad = {0x3c42a59981a5423c};
      displayImage(sad);
    }
  }

}

void stateLeaderboard() {
  displayImage(matrixLeaderboard);
  updateLeaderboardPosition();
  lcd.setCursor(0, 0);
  if (leaderboardSelector < leaderboardSize - 2) {
    int score1 = leaderboardScores[leaderboardSelector];
    int score2 = leaderboardScores[leaderboardSelector + 1];
    char name1[nameSize];
    char name2[nameSize];
    for (int k = 0; k < nameSize; k++) {
      name1[k] = leaderboardNames[leaderboardSelector][k];
      name2[k] = leaderboardNames[leaderboardSelector + 1][k];
    }
 
    lcd.write(1);
    lcd.print(name1);
    lcd.print(": ");
    lcd.print(score1);
    lcd.setCursor(0, 1);
    lcd.print(' ');
    lcd.print(name2);
    lcd.print(": ");
    lcd.print(score2);
  }
  else {
    int score1 = leaderboardScores[leaderboardSelector - 1];
    int score2 = leaderboardScores[leaderboardSelector];
    char name1[nameSize];
    char name2[nameSize];
    for (int k = 0; k < nameSize; k++) {
      name1[k] = leaderboardNames[leaderboardSelector - 1][k];
      name2[k] = leaderboardNames[leaderboardSelector][k];
    }
    lcd.print(' ');
    lcd.print(name1);
    lcd.print(": ");
    lcd.print(score1);
    lcd.setCursor(0, 1);
    lcd.write(1);
    lcd.print(name2);
    lcd.print(": ");
    lcd.print(score2);
  }

  if (buttonValue == 1 && lastButtonValue == 0) {
    lcd.clear();
    displayImage(matrixEmpty);
    state = sMenu;
  }

}

void stateAbout() {
  lcd.setCursor(0, 0);
  displayImage(matrixInit);
  lcd.print(about);
  if (buttonValue == 1 && lastButtonValue == 0) {
    lcd.clear();
    displayImage(matrixEmpty);
    state = sMenu;
  }
}

void readButton() {
  buttonState = digitalRead(pinSW);
  if(buttonState != lastButtonState) {
    lastDebounceTime = millis();
    lastButtonState = buttonState;
  }
  if(millis() - lastDebounceTime > debounceDelay){
    buttonValue = !buttonState;
  }
}

void updateLeaderboardPosition() {
  int xValue = analogRead(xPin);
  //xLastPos = xPos;
  if(xValue >= minThreshold && xValue <= maxThreshold){
    joyMovedX = false;
  }
  else if(xValue > maxThreshold && joyMovedX == false){
    if(leaderboardSelector > 0) {
      leaderboardSelector--;
    }
    else {
      leaderboardSelector = leaderboardSize - 2;
    }
    joyMovedX = true;  
  }
  else if(xValue < minThreshold && joyMovedX == false){
    if(leaderboardSelector < leaderboardSize - 2) {
      leaderboardSelector++;
    }
    else {
      leaderboardSelector = 0;
    }
    joyMovedX = true;
  }
  if (joyMovedX){
    lcd.clear();
  }
}

void updateLeaderboard() {
  for (int i = 0; i < nameSize; i++){
    leaderboardNames[leaderboardSize - 1][i] = name[i];
  }
  
  leaderboardScores[leaderboardSize - 1] = score;
  for (int i = 0; i < leaderboardSize; i++) {
    for(int j = i + 1; j < leaderboardSize; j++) {
      if (leaderboardScores[j] > leaderboardScores[i]){
        int aux = leaderboardScores[j];
        char auxName[nameSize];
        for (int k = 0; k < nameSize; k++){
          auxName[k] = leaderboardNames[j][k];
        }
        leaderboardScores[j] = leaderboardScores[i];
        leaderboardScores[i] = aux;
        for (int k = 0; k < nameSize; k++){
          leaderboardNames[j][k] = leaderboardNames[i][k];
        }
        for (int k = 0; k < nameSize; k++){
          leaderboardNames[i][k] = auxName[k];
        }
      }
    }
  }

  EEPROM.put(0, leaderboardNames);
  int address_arr2 = sizeof(leaderboardNames);
  EEPROM.put(address_arr2, leaderboardScores);

}

void updateByteMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, matrixByte[row]);
  }
}

void updateMenuPosition() {
  int xValue = analogRead(xPin);
  //xLastPos = xPos;
  if(xValue >= minThreshold && xValue <= maxThreshold){
    joyMovedX = false;
  }
  else if(xValue > maxThreshold && joyMovedX == false){
    if(menuSelector > 0) {
      menuSelector--;
    }
    else {
      menuSelector = menuSize - 1;
    }
    joyMovedX = true;  
  }
  else if(xValue < minThreshold && joyMovedX == false){
    if(menuSelector < menuSize - 1) {
      menuSelector++;
    }
    else {
      menuSelector = 0;
    }
    joyMovedX = true;
  }
  if (joyMovedX){
    lcd.clear();
  }

}

void updateGamePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  xLastPos = xPos;
  yLastPos = yPos;
  if(xValue >= minThreshold && xValue <= maxThreshold){
    joyMovedX = false;
  }
  else if(xValue > maxThreshold && joyMovedX == false){
    if(xPos > 0) {
      xPos--;
    }
    else {
      xPos = matrixSize - 1;
    }
    joyMovedX = true;  
  }
  else if(xValue < minThreshold && joyMovedX == false){
    if(xPos < matrixSize - 1) {
      xPos++;
    }
    else {
      xPos = 0;
    }
    joyMovedX = true;
  }
  if (joyMovedX == false){
    if(yValue >= minThreshold && yValue <= maxThreshold){
      joyMovedY = false;
    }
    else if(yValue > maxThreshold && joyMovedY == false){
      if(yPos > 0) {
        yPos--;
      }
      else {
        yPos = matrixSize - 1;
      }
      joyMovedY = true;  
    }
    else if(yValue < minThreshold && joyMovedY == false){
      if(yPos < matrixSize - 1) {
        yPos++;
      }
      else {
        yPos = 0;
      }
      joyMovedY = true;
    }
  }
  
  if (xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrixByte[xLastPos] = 0;
    matrixByte[xPos] = 1 << yPos;
  }
}

void blinkFood() {
  if(millis() - lastBlinkFood >= blinkFoodDelay){
    lastBlinkFood = millis();
    matrixByte[xFood] = matrixByte[xFood] ^ (1 << yFood);
    matrixChanged = true;
  }
}

void generateFood() {
  xFood = rand() % 8;
  yFood = rand() % 8;
  while (xFood == xPos && yFood == yPos) {
    xFood = rand() % 8;
    yFood = rand() % 8;
  }

}

void displayImage(uint64_t image) {
  for (int i = 0; i < matrixSize; i++) {
    byte row = (image >> i * 8) & 0xFF;
    matrixByte[i] = row;
  }
  matrixChanged = true;
}
