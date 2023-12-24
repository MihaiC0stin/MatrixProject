//LIBRARIES ########################################################################################################################################
#include "LedControl.h"  // Include LedControl library for controlling the LED matrix
#include <LiquidCrystal.h>
#include <EEPROM.h>

//CONSTANT VARIABLES ###############################################################################################################################
//microcontroller pins
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
//joystick pins
const byte xPin = A0;
const byte yPin = A1;
//joystick threshold
const int minThreshold = 200;
const int maxThreshold = 800;
//matrix sizes
const byte matrixSize = 8;  // Size of the LED matrix
const byte mapSize = 16;
//buttons
const byte numberOfButtons = 2;
const byte button[numberOfButtons] = { 2, 13 };  // 2 is OK, 3 is BACK !
const byte ok = 0;
const byte back = 1;
//
const byte nothing = 0;  //literally nothing, no wall, no player. Just nothing.
const byte wall = 1;
const byte bullet = 3;
//
const byte numberOfDirections = 4;
const byte left = 0;
const byte right = 1;
const byte up = 2;
const byte down = 3;
//
const byte lcdAnode = 3;
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
//
const byte maxNumberOfMonsters = 56;  //calculated exactly for 7 lvls full of monsters

const byte maxMatrixBrightness = 15;
const int maxLcdBrightness = 255;
//
const byte numberOfCharacters = 53;
const byte maxNameLength = 10;
const byte numberOfHighscores = 5;
//
const byte eepromAdressMatrixBrightness = 10;
const byte eepromAdressLcdBrightness = 20;
const byte eepromAdressSoundStatus = 30;
//

byte matrixStickmanMove[matrixSize] = {
  B00011000,
  B00111100,
  B00011000,
  B00011111,
  B00011000,
  B00011000,
  B00100100,
  B01000010
};
byte matrixStickmanStay[matrixSize] = {
  B00011000,
  B00111100,
  B00011000,
  B00011111,
  B00011000,
  B00011000,
  B00011000,
  B00011000
};
byte lcdStickmanMove[matrixSize] = {
  B00100,
  B01110,
  B00100,
  B00111,
  B00100,
  B00100,
  B01010,
  B10001
};
byte lcdStickmanStay[matrixSize] = {
  B00100,
  B01110,
  B00100,
  B00111,
  B00100,
  B00100,
  B00100,
  B00100
};

byte playAnimation[matrixSize] = {
  B00100000,
  B00110000,
  B00111000,
  B00111100,
  B00111000,
  B00110000,
  B00100000,
  B00000000
};
byte settingsAnimation[matrixSize] = {
  B00000000,
  B10010010,
  B01010100,
  B00111000,
  B11101110,
  B00111000,
  B01010100,
  B10010010
};
byte highscoresAnimation[matrixSize] = {
  B01111100,
  B01111100,
  B01111100,
  B00111000,
  B00010000,
  B00010000,
  B00111000,
  B01111100
};
byte highscoresAnimationLcd[matrixSize] = {
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00100,
  B01110,
  B11111
};
byte aboutAnimation[matrixSize] = {
  B00000000,
  B00010000,
  B00000000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00010000
};
byte howToPlayAnimation[matrixSize] = {
  B00111000,
  B01000100,
  B00000100,
  B00001000,
  B00010000,
  B00010000,
  B00000000,
  B00010000
};

int startupAnimationDelay = 250;
byte positionAnimationX = 0;
byte positionAnimationY = 0;
//VARIABLES #########################################################################################################################################
unsigned long lastStartupAnimationTime = 0;
byte matrixBrightness = 2;
int lcdBrightness = 180;
bool sound = true;

bool matrixChanged = true;  // Flag to track if the matrix display needs updating
byte outputMatrix[mapSize][mapSize] = {
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};
byte fovMatrix[matrixSize][matrixSize];

byte buttonState[numberOfButtons] = { LOW, LOW };
byte reading[numberOfButtons] = { HIGH, HIGH };
byte lastReading[numberOfButtons] = { HIGH, HIGH };
bool pressedButton[numberOfButtons] = { false, false };
unsigned long lastDebounceTime[numberOfButtons] = { 0, 0 };
byte debounceDelay = 50;
bool direction[numberOfDirections] = { false, false, false, false };  // LEFT, RIGHT, UP, DOWN
byte displayLed = 0;

bool gameFinished = false;
bool inMenu = true;
bool inGame = false;
bool gameOver = false;

int score = 0;
int lastScore = 0;
int highscores[numberOfHighscores];
char highscoresNames[numberOfHighscores][maxNameLength + 1];

bool gameReseted = false;
bool updateLcd = true;
bool newHighscore = false;
bool highscoreChecked = false;


//LIBRARY OBJECTS ##################################################################################################################################
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//PROTOTYPE FUNCTIONS ##########################################################################################################################
bool checkObject(byte, byte, byte);
void checkDirection(byte);

//CLASSES ######################################################################################################################################
class GameEntity {
protected:
  byte id;
  byte health;
  unsigned long lastBlink = 0;
  int blinkRate;
  bool blinked = true;
public:
  GameEntity(byte id, byte health, int blinkRate) {
    this->id = id;
    this->health = health;
    this->blinkRate = blinkRate;
  };

  void setId(byte id) {
    this->id = id;
  }

  byte getId() {
    return this->id;
  }

  void blink() {
    if (millis() - lastBlink >= blinkRate) {
      this->blinked = !blinked;  //blinked is what is shown on matrix (0 and 1).
      matrixChanged = true;
      lastBlink = millis();
    }
  }

  bool getBlinkStatus() {
    return this->blinked;
  }


  byte getHealth() {
    return this->health;
  }
};

class Monster : public GameEntity {
  byte damage = 5;
  byte MonstersHealth[maxNumberOfMonsters];
  byte xPosMonsters[maxNumberOfMonsters];
  byte yPosMonsters[maxNumberOfMonsters];
  int speed = 3000;
  byte numberOfMonsters = 0;
  byte scoreValue = 0;
  byte moneyValue = 0;

public:
  Monster(byte id, byte health, int blinkRate)
    : GameEntity(id, health, blinkRate) {
  }

  void spawnMonsters(byte iteratorStart, byte numberOfMonsters) {                                    //iteratorStart is the actual numberOfMonsters, numberOfMonsters here is what amount of monsters is added.
    this->numberOfMonsters = this->numberOfMonsters + numberOfMonsters;                              // update the number of monsters
    for (int currentMonster = iteratorStart; currentMonster < numberOfMonsters; currentMonster++) {  //update the arrays
      MonstersHealth[currentMonster] = health;
      xPosMonsters[currentMonster] = random(0, mapSize);  //monsters should be spawned random
      yPosMonsters[currentMonster] = random(0, mapSize);
      if (outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] != 0) {  // but I don't want it to be spawned into a wall, or on the player
        currentMonster--;
      } else {
        outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] = id;
      }
    }
  }

  void ManageMonsterHealth(byte xPos, byte yPos, byte damageReceived) {  // takes coordinates of the monster that got hit, and search in array what monster has these coordinates
    for (byte currentMonster = 0; currentMonster < numberOfMonsters; currentMonster++) {
      if (xPosMonsters[currentMonster] == xPos && yPosMonsters[currentMonster] == yPos) {
        if ((MonstersHealth[currentMonster] - damageReceived) <= 0) {  // if health is 0 or lower, remove the monster from array and increase score of the player
          score = score + scoreValue;
          for (byte iterator = currentMonster; iterator < numberOfMonsters; iterator++) {
            if (iterator != numberOfMonsters - 1) {  // if is not the last monster, then I shift to the left all monsters after the current monster
              MonstersHealth[iterator] = MonstersHealth[iterator + 1];
              xPosMonsters[iterator] = xPosMonsters[iterator + 1];
              yPosMonsters[iterator] = yPosMonsters[iterator + 1];
            } else {
              outputMatrix[xPosMonsters[iterator]][yPosMonsters[iterator]] = 0;
              MonstersHealth[iterator] = 0;  // if is the last monster then I update his status to 0;
              xPosMonsters[iterator] = 0;
              yPosMonsters[iterator] = 0;
              Serial.print("pew");
            }
          }
          numberOfMonsters--;
          updateLcd = true;
          matrixChanged = true;
        } else {
          MonstersHealth[currentMonster] = MonstersHealth[currentMonster] - damageReceived;  // if remaining hp is more than 0, then update his health into array.
        }
      }
    }
  }

  void updateMonstersPosition(byte xPosMonsters, byte yPosMonsters, byte currentMonster) {  // This is used in monsterAI, to make the monster move to the player in a clean way.
    outputMatrix[this->xPosMonsters[currentMonster]][this->yPosMonsters[currentMonster]] = 0;
    this->xPosMonsters[currentMonster] = xPosMonsters;
    this->yPosMonsters[currentMonster] = yPosMonsters;
    outputMatrix[this->xPosMonsters[currentMonster]][this->yPosMonsters[currentMonster]] = id;
    matrixChanged = true;
  }

  void boostMonsterStatus() {  // increase difficulty = increase status for monsters
    health = health + 1;       // also as crazy as it sounds, -300 speed is a buff, because speed here means interval after the monster can move
    speed = speed - 300;
    damage = damage + 1;
  }

  void reset() {  // the name of the method tells everything
    health = 5;
    speed = 3000;
    damage = 5;
    for (byte currentMonster = 0; currentMonster < maxNumberOfMonsters; currentMonster++) {
      outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] = 0;
      MonstersHealth[currentMonster] = 0;
      xPosMonsters[currentMonster] = 0;
      yPosMonsters[currentMonster] = 0;
    }
    numberOfMonsters = 0;
    blinked = false;
  }

  void setScoreValue(byte scoreValue) {
    this->scoreValue = scoreValue;
  }

  void setMoneyValue(byte moneyValue) {
    this->moneyValue = moneyValue;
  }

  byte getNumberOfMonsters() {
    return numberOfMonsters;
  }

  int getSpeed() {
    return speed;
  }

  byte getMonsterPositionX(byte currentMonster) {
    return xPosMonsters[currentMonster];
  }

  byte getMonsterPositionY(byte currentMonster) {
    return yPosMonsters[currentMonster];
  }

  byte getMonsterDamage() {
    return damage;
  }

  byte getScoreValue() {
    return scoreValue;
  }

  byte getMoneyValue() {
    return moneyValue;
  }
};

Monster monster = Monster(6, 5, 50);


class Player : public GameEntity {
  char name[maxNameLength + 1];
  char nameCharacters[numberOfCharacters] = { ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                              'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
  byte letter[maxNameLength];
  byte positionX = 6;
  byte positionY = 6;
  byte lastPositionX = 6;
  byte lastPositionY = 6;
  unsigned long lastMoved = 0;
  int speed = 200;
  bool joystickMovedLeftRight = false;
  bool joystickMovedUpDown = false;
  bool inSaveName = false;
  byte cursorPosition = 0;
  byte lastCursorPosition = 0;

public:
  Player(byte id, byte health, int blinkrate)
    : GameEntity(id, health, blinkrate) {
    for (byte iterator; iterator < maxNameLength; iterator++) {
      letter[iterator] = 0;
    }
  }

  void setPosition(byte positionX, byte positionY) {
    this->positionX = positionX;
    this->positionY = positionY;
  }

  void updateLastPosition() {
    this->lastPositionX = this->positionX;
    this->lastPositionY = this->positionY;
  }

  bool checkInRange(byte option, byte numberOfOptions) {  // check if the option is in range of 0, numberOfOptions
    if (option < 0 || option >= numberOfOptions) {
      return false;
    } else {
      return true;
    }
  }

  byte moveLeftRight(byte option, byte numberOfOptions) {  //read joystick on X axis, and change the option depending on the joystick movement
    int xValue = analogRead(xPin);                         // method first used for menu, to change what option it shows
    if (xValue < minThreshold && joystickMovedLeftRight == false) {
      if (checkInRange(option - 1, numberOfOptions) == true) {
        option--;
        joystickMovedLeftRight = true;
        updateLcd = true;
      }
    } else if (xValue > maxThreshold && joystickMovedLeftRight == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMovedLeftRight = true;
        updateLcd = true;
      }
    } else if (xValue >= minThreshold && xValue <= maxThreshold) {
      joystickMovedLeftRight = false;
    }
    // Serial.println(option);
    return option;
  }


  byte moveUpDown(byte option, byte numberOfOptions) {  //same thing but for Y axis
    int yValue = analogRead(yPin);
    if (yValue < minThreshold && joystickMovedUpDown == false) {
      if (checkInRange(option - 1, numberOfOptions) == true) {
        option--;
        joystickMovedUpDown = true;
        updateLcd = true;
      }
    } else if (yValue > maxThreshold && joystickMovedUpDown == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMovedUpDown = true;
        updateLcd = true;
      }
    } else if (yValue >= minThreshold && yValue <= maxThreshold) {
      joystickMovedUpDown = false;
    }
    return option;
  }

  void savePlayerName() {                                           //it shows a screen where the player, after achieved a highscore, can change letters and position of cursor by moving the joystick
    cursorPosition = moveLeftRight(cursorPosition, maxNameLength);  // after entering the name and saving it, it continues with reset and return to the interactive menu
    letter[cursorPosition] = moveUpDown(letter[cursorPosition], numberOfCharacters);
    if (updateLcd == true) {
      lcd.setCursor(lastCursorPosition, 1);
      lcd.print(F(" "));
      lcd.setCursor(cursorPosition, 0);
      lastCursorPosition = cursorPosition;
      lcd.print(nameCharacters[letter[cursorPosition]]);
      lcd.setCursor(cursorPosition, 1);
      lcd.print(F("^"));
      updateLcd = false;
    }
    if (pressedButton[ok] == true) {
      for (byte iterator = 0; iterator < maxNameLength; iterator++) {
        name[iterator] = nameCharacters[letter[iterator]];
      }
      strcpy(highscoresNames[numberOfHighscores - 1], name);
      highscores[numberOfHighscores - 1] = score;
      inSaveName = false;
      inMenu = true;
      inGame = false;
      updateLcd = true;
      reset();
      pressedButton[ok] = false;
    }
  }

  byte getPositionX() const {
    return this->positionX;
  }
  byte getPositionY() const {
    return this->positionY;
  }
  byte getLastPositionX() const {
    return this->lastPositionX;
  }
  byte getLastPositionY() const {
    return this->lastPositionY;
  }
  bool getInSaveName() {
    return inSaveName;
  }
  void setInSaveName(bool inSaveName) {
    this->inSaveName = inSaveName;
  }
  void updatePlayerPosition() {  //basically just move the player depending on the joystick movement. It also checks for obstacles and stops movement if it hit 1.
    //Read Joystick values left-right up-down
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    // Store the last positions of the LED
    this->lastPositionX = this->positionX;
    this->lastPositionY = this->positionY;

    // Update xPos based on joystick movement (X-axis)
    if (xValue < minThreshold) {
      checkDirection(left);
      if ((checkObject(this->positionX, this->positionY - 1, wall) == false) && (checkObject(this->positionX, this->positionY - 1, monster.getId()) == false)) {
        this->positionY--;
      }
    }
    if (xValue > maxThreshold) {
      checkDirection(right);
      if ((checkObject(this->positionX, this->positionY + 1, wall) == false)
          && (checkObject(this->positionX, this->positionY + 1, monster.getId()) == false)) {
        this->positionY++;
      }
    }
    // Update yPos based on joystick movement (Y-axis)
    if (yValue < minThreshold) {
      checkDirection(up);
      if ((checkObject(this->positionX - 1, this->positionY, wall) == false)
          && (checkObject(this->positionX - 1, this->positionY, monster.getId()) == false)) {
        this->positionX--;
      }
    }
    if (yValue > maxThreshold) {
      checkDirection(down);
      if ((checkObject(this->positionX + 1, this->positionY, wall) == false)
          && (checkObject(this->positionX + 1, this->positionY, monster.getId()) == false)) {
        this->positionX++;
      }
    }

    // Check if the position has changed and update the matrix if necessary
    if (this->positionX != this->lastPositionX || this->positionY != lastPositionY) {
      matrixChanged = true;
      outputMatrix[this->lastPositionX][this->lastPositionY] = nothing;
      outputMatrix[this->positionX][this->positionY] = this->id;
    }
  }
  void playerMovement() {
    if (millis() - lastMoved > speed) {  // Check if it's time to move the LED
      updatePlayerPosition();            // Update the position of the LED based on joystick input
      lastMoved = millis();              // Update the time of the last move
    }
  }

  void managePlayerHealth(byte damageReceived) {  //if the hp of the player is 0, the game is over.
    if ((health - damageReceived) <= 0) {
      gameOver = true;
      updateLcd = true;
      health = 0;
    } else {
      health = health - damageReceived;  //if the hp is still > 0, after being hit, update the health of the player.
    }
  }

  void reset() {
    outputMatrix[positionX][positionY] = 0;
    blinked = false;
    health = 20;
    positionX = 6;
    positionY = 6;
    lastPositionX = 6;
    lastPositionY = 6;
    score = 0;
    cursorPosition = 0;
    lastCursorPosition = 0;
    for (byte iterator = 0; iterator < maxNameLength; iterator++) {
      letter[iterator] = 0;
    }
  }
};

Player player = Player(2, 20, 400);

class Weapons {
protected:
  byte id;
  byte damage;
  unsigned long lastAnimation = 0;
  bool monsterHit = false;
  byte range;  //range of the weapon, at least for the pistol, is the number cycles a bullet will do if it doesn't hit something
  byte currentCycle;
public:
  Weapons(byte id, byte damage, byte range) {
    this->id = id;
    this->damage = damage;
    this->range = range;  // currentcycle have to be > than range to not enter in the loop
    this->currentCycle = range + 1;
  }
  byte getId() const {
    return this->id;
  }
  int getDamage() const {
    return this->damage;
  }
};

class Pistol : public Weapons {
  byte xPosBullet = 0;
  byte yPosBullet = 0;
  byte xLastPosBullet = 0;
  byte yLastPosBullet = 0;
  byte currentDirectionBullet = 0;
  byte bulletSpeed = 50;
public:
  Pistol(byte id, byte damage, byte range)
    : Weapons(id, damage, range) {
  }

  void shoot() {
    CheckToShoot();
    bulletAnimation();
  }

  void CheckToShoot() {  //check if the player pressed the button, if yes, currentcycle = 0 and will enter in the bullet animation loop
    if (pressedButton[ok] == true) {
      xPosBullet = player.getPositionX();
      yPosBullet = player.getPositionY();
      monsterHit = false;
      currentCycle = 0;
      pressedButton[ok] = false;
    }
  }

  void bulletAnimation() {
    if (millis() - lastAnimation >= bulletSpeed && monsterHit == false) {
      if (currentCycle < range) {
        updateBulletPosition();  //method used to move the bullet
        lastAnimation = millis();
      } else if (currentCycle == range) {
        clearBullet();
        matrixChanged = true;
        currentCycle++;  //stops the loop
      } else if (currentCycle > range) {
        for (int currentDirection = 0; currentDirection < numberOfDirections; currentDirection++) {  //check for the direction of the player when it is out of loop, and updates the direction of the bullet
          if (direction[currentDirection] == 1) {
            currentDirectionBullet = currentDirection;
          }
        }
      }
    }
  }

  void clearBullet() {
    outputMatrix[xPosBullet][yPosBullet] = 0;
  }

  // Function to read joystick input and update the position of the LED
  void updateBulletPosition() {
    xLastPosBullet = xPosBullet;
    yLastPosBullet = yPosBullet;
    currentCycle++;
    switch (currentDirectionBullet) {
      case 0:
        if (monsterHit == false) {
          if (checkObject(xPosBullet, yPosBullet - 1, monster.getId()) == false && checkObject(xPosBullet, yPosBullet - 1, wall) == false) {  //checks if it hits an object, if not, the bullet moves
            yPosBullet--;
          } else if (checkObject(xPosBullet, yPosBullet - 1, monster.getId()) == true) {  // if hits a monster, call the method to update his hp
            monster.ManageMonsterHealth(xPosBullet, yPosBullet - 1, damage);
            monsterHit = true;  //this stops the bullet animation
          }
        }
        break;
      case 1:
        if (monsterHit == false) {
          if (checkObject(xPosBullet, yPosBullet + 1, monster.getId()) == false && checkObject(xPosBullet, yPosBullet + 1, wall) == false) {
            yPosBullet++;
          } else if (checkObject(xPosBullet, yPosBullet + 1, monster.getId()) == true) {
            monster.ManageMonsterHealth(xPosBullet, yPosBullet + 1, damage);
            monsterHit = true;
          }
        }
        break;
      case 2:
        if (monsterHit == false) {
          if (checkObject(xPosBullet - 1, yPosBullet, monster.getId()) == false && checkObject(xPosBullet - 1, yPosBullet, wall) == false) {
            xPosBullet--;
          } else if (checkObject(xPosBullet - 1, yPosBullet, monster.getId()) == true) {
            monster.ManageMonsterHealth(xPosBullet - 1, yPosBullet, damage);
            monsterHit = true;
          }
        }
        break;
      case 3:
        if (monsterHit == false) {
          if (checkObject(xPosBullet + 1, yPosBullet, monster.getId()) == false && checkObject(xPosBullet + 1, yPosBullet, wall) == false) {
            xPosBullet++;
          } else if (checkObject(xPosBullet + 1, yPosBullet, monster.getId()) == true) {
            monster.ManageMonsterHealth(xPosBullet + 1, yPosBullet, damage);
            monsterHit = true;
          }
        }
        break;
      default:
        break;
    }
    if (currentCycle != 1) {
      if ((xPosBullet != xLastPosBullet || yPosBullet != yLastPosBullet) && monsterHit == false) {  //update the bullet position on matrix
        matrixChanged = true;
        outputMatrix[xLastPosBullet][yLastPosBullet] = nothing;
        outputMatrix[xPosBullet][yPosBullet] = bullet;
      } else if (monsterHit == true) {
        matrixChanged = true;
        outputMatrix[xLastPosBullet][yLastPosBullet] = nothing;
      }
    } else {
      if ((xPosBullet != xLastPosBullet || yPosBullet != yLastPosBullet) && monsterHit == false) {  // this was put because in first stage of animation the last position of the bullet is the player position
        matrixChanged = true;
        outputMatrix[xPosBullet][yPosBullet] = bullet;
      } else if (monsterHit == true) {
        matrixChanged = true;
      }
    }
  }
};

Pistol pistol = Pistol(3, 2, 3);

class GameDifficulty {
  byte difficulty = 1;
  bool updateLvl = true;
  unsigned int increaseDifficultyDelay = 1000 * 60 * 1;  // 1 minute
  unsigned long lastDifficultyIncrease = 0;
  byte maxDifficulty = 7;

public:
  GameDifficulty() {
  }
  void setGameDifficulty(byte difficulty) {
    this->difficulty = difficulty;
    updateLvl = true;
  }
  byte getGameDifficulty() {
    return difficulty;
  }
  void updateTheLvl() {
    if (updateLvl == true) {
      if (difficulty == 1) {                                                          //is set to 1 only when begin a new game
        outputMatrix[player.getPositionX()][player.getPositionY()] = player.getId();  //put the id of the player on array of the map
      }
      if (difficulty > 1) {
        monster.boostMonsterStatus();
      }
      monster.spawnMonsters(monster.getNumberOfMonsters(), difficulty * 2);  //spawn monsters when difficulty increase
      monster.setScoreValue(difficulty * 10);                                //also increase their value
      // Serial.println(monster.getNumberOfMonsters());
      updateLcd = true;
      lastDifficultyIncrease = millis();  //reset the timer for difficulty increase
      matrixChanged = true;
      updateLvl = false;
    }
  }

  void finishLvl() {
    if (difficulty < maxDifficulty) {
      if ((monster.getNumberOfMonsters() == 0) || (millis() - lastDifficultyIncrease >= increaseDifficultyDelay)) {  //checks if all monsters on the actual lvl are dead or the timer reached 1 minute
        setGameDifficulty(difficulty + 1);                                                                           //increase the difficulty (it doesn't spawn monsters till next loop)
        updateLcd = true;
      }
    }
  }

  void finishGame() {
    if (difficulty == maxDifficulty + 1 && monster.getNumberOfMonsters() == 0) {  //if difficulty is set to 8 and are 0 monsters (from the previous lvl, 7) then the player won.
      gameFinished = true;
    }
  }
};

GameDifficulty gameDifficulty = GameDifficulty();


class Game {
  unsigned long monsterLastMoved = 0;
  unsigned long lastTimePrinted = 0;
  int delayHighscore = 5000;
  int delayReset = 10000;


  void playerGotHit() {
    player.managePlayerHealth(monster.getMonsterDamage());  //call the method from player class that update the player's health
    updateLcd = true;
  }
public:
  void resetGame() {
    if (pressedButton[ok] == true) {  //you need to press button to reset the game after victory/defeat
      if (newHighscore == true) {
        player.setInSaveName(true);
        newHighscore = false;
      }
      if (player.getInSaveName() == false) {
        player.reset();  // here resets the score too
      }
      monster.reset();  //resets the members of player and monster classes, also switch to menu
      gameDifficulty.setGameDifficulty(1);
      inGame = false;
      inMenu = true;
      updateLcd = true;
      lcd.clear();
      pressedButton[ok] = false;
    }
  }

  void clearMatrix() {
    for (int row = 0; row < matrixSize; row++) {
      for (int col = 0; col < matrixSize; col++) {
        ledStateMatrix(row, col, 0);
      }
    }
  }

  void checkHighscore() {  //check if the actual score is a highscore
    if (newHighscore == false) {
      for (byte iterator = 0; iterator < numberOfHighscores; iterator++) {
        if (score > highscores[iterator]) {
          newHighscore = true;
          updateLcd = true;
        }
      }
      printNewHighscore();
    }
  }

  void printNewHighscore() {
    if (updateLcd == true && newHighscore == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(F("New Highscore"));
      lcd.setCursor(6, 1);
      lcd.write(byte(2));
      lcd.print(score);
      lcd.write(byte(2));
      updateLcd = false;
    }
  }

  void startGame() {  //the control center of the game
    if (gameFinished == false && gameOver == false) {
      gameDifficulty.updateTheLvl();
      gameDifficulty.finishLvl();
      mapDisplay();
      player.playerMovement();
      pistol.shoot();
      player.blink();
      monster.blink();
      monsterAI();
      checkScoreChange();
      printGameInfo();
      gameDifficulty.finishGame();
      checkGameOverOrFinished();
    } else if (gameFinished == true) {
      printWinMessage();
      if ((millis() - lastTimePrinted >= delayHighscore) && (millis() - lastTimePrinted <= delayReset)) {  //we have a little delay of 10 seconds after victory of defeat, so highscore message can be seen
        checkHighscore();
      }
      if (millis() - lastTimePrinted >= delayReset) {
        resetGame();
      }
    } else if (gameOver == true) {
      printLoseMessage();
      if ((millis() - lastTimePrinted >= delayHighscore) && (millis() - lastTimePrinted <= delayReset)) {
        checkHighscore();
      }
      if (millis() - lastTimePrinted >= delayReset) {
        resetGame();
      }
    }
  }

  void checkGameOverOrFinished() {
    if (gameOver == true || gameFinished == true) {
      updateLcd = true;
    }
  }

  void printWinMessage() {
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Congratulations "));
      lcd.setCursor(0, 1);
      lcd.print(F("Your score:"));
      lcd.print(score);
      lastTimePrinted = millis();
      updateLcd = false;
      clearMatrix();
    }
  }

  void printLoseMessage() {
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("    You Died     "));
      lcd.setCursor(0, 1);
      lcd.print(F("Your score:"));
      lcd.print(score);
      lastTimePrinted = millis();
      updateLcd = false;
      clearMatrix();
    }
  }

  void checkScoreChange() {
    if (score != lastScore) {
      updateLcd = true;
      lastScore = score;
    }
  }

  void printGameInfo() {  //this is what the lcd screen shows while playing the game
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("HP:"));
      lcd.print(player.getHealth());
      lcd.setCursor(6, 0);
      lcd.print(F("Score:"));
      lcd.print(score);
      lcd.setCursor(0, 1);
      lcd.print(F("Diff:"));
      lcd.print(gameDifficulty.getGameDifficulty());
      lcd.setCursor(7, 1);
      lcd.print(F("Alive:"));
      lcd.print(monster.getNumberOfMonsters());
      updateLcd = false;
    }
  }

  void monsterAI() {  // not even a pathfinder :(
    if (millis() - monsterLastMoved >= monster.getSpeed()) {
      for (int currentMonster = 0; currentMonster < monster.getNumberOfMonsters(); currentMonster++) {
        bool left = false;
        bool up = false;
        bool right = false;
        bool down = false;

        if (abs(player.getPositionX() - monster.getMonsterPositionX(currentMonster)) > abs(player.getPositionY() - monster.getMonsterPositionY(currentMonster))) {  //at least I learned something from all these A* tutorials
          if (player.getPositionX() < monster.getMonsterPositionX(currentMonster)) {                                                                                //decides if to move on X or Y axis depending on the distance
            up = true;                                                                                                                                              //difference between player and monster
          } else if (player.getPositionX() > monster.getMonsterPositionX(currentMonster)) {
            down = true;
          }
        } else if (abs(player.getPositionX() - monster.getMonsterPositionX(currentMonster)) <= abs(player.getPositionY() - monster.getMonsterPositionY(currentMonster))) {
          if (player.getPositionY() < monster.getMonsterPositionY(currentMonster)) {
            left = true;
          } else if (player.getPositionY() > monster.getMonsterPositionY(currentMonster)) {
            right = true;
          }
        }

        if (left == true) {                                                                                                                //here is the monster movement, if the next position monster moves, is hold
          if (checkObject(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) - 1, wall) == false) {  //by the player, then monster gets angry and hits the player.
            if (checkObject(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) - 1, player.getId()) == false) {
              monster.updateMonstersPosition(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) - 1, currentMonster);
            } else {
              playerGotHit();  //calls the method that updates player hp.
            }
          }
        } else if (up == true) {
          if (checkObject(monster.getMonsterPositionX(currentMonster) - 1, monster.getMonsterPositionY(currentMonster), wall) == false) {
            if (checkObject(monster.getMonsterPositionX(currentMonster) - 1, monster.getMonsterPositionY(currentMonster), player.getId()) == false) {
              monster.updateMonstersPosition(monster.getMonsterPositionX(currentMonster) - 1, monster.getMonsterPositionY(currentMonster), currentMonster);
            } else {
              playerGotHit();
            }
          }
        } else if (right == true) {
          if (checkObject(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) + 1, wall) == false) {
            if (checkObject(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) + 1, player.getId()) == false) {
              monster.updateMonstersPosition(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) + 1, currentMonster);
            } else {
              playerGotHit();
            }
          }
        } else if (down == true) {
          if (checkObject(monster.getMonsterPositionX(currentMonster) + 1, monster.getMonsterPositionY(currentMonster), wall) == false) {
            if (checkObject(monster.getMonsterPositionX(currentMonster) + 1, monster.getMonsterPositionY(currentMonster), player.getId()) == false) {
              monster.updateMonstersPosition(monster.getMonsterPositionX(currentMonster) + 1, monster.getMonsterPositionY(currentMonster), currentMonster);
            } else {
              playerGotHit();
            }
          }
        }
        monsterLastMoved = millis();
      }
    }
  }
};

Game game = Game();

class Menu {
  byte optionMainMenu = 1;
  byte optionSettings = 0;
  byte optionHighscores = 0;
  byte optionAbout = 0;
  byte optionHowToPlay = 0;
  bool joystickMovedLeftRight = false;
  bool joystickMovedUpDown = false;
  byte numberOfOptionsMain = 5;
  byte numberOfOptionsSettings = 3;
  byte numberOfOptionsHighscores = 5;
  byte numberOfOptionsAbout = 3;
  byte numberOfOptionsHowToPlay = 11;
  bool changeMatrixIntensity = false;
  bool changeLcdIntensity = false;
  bool toggleTheSound = false;

public:
  Menu() {
  }

  bool checkInRange(byte option, byte numberOfOptions) {  //same thing from the player class, I took it from here
    if (option <= 0 || option > numberOfOptions) {
      return false;
    } else {
      return true;
    }
  }

  byte moveLeftRight(byte option, byte numberOfOptions) {
    int xValue = analogRead(xPin);
    if (xValue < minThreshold && joystickMovedLeftRight == false) {
      if (checkInRange(option - 1, numberOfOptions) == true) {
        option--;
        joystickMovedLeftRight = true;
        updateLcd = true;
      }
    } else if (xValue > maxThreshold && joystickMovedLeftRight == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMovedLeftRight = true;
        updateLcd = true;
      }
    } else if (xValue >= minThreshold && xValue <= maxThreshold) {
      joystickMovedLeftRight = false;
    }
    // Serial.println(option);
    return option;
  }


  byte moveUpDown(byte option, byte numberOfOptions) {
    int yValue = analogRead(yPin);
    if (yValue < minThreshold && joystickMovedUpDown == false) {
      if (checkInRange(option - 1, numberOfOptions) == true) {
        option--;
        joystickMovedUpDown = true;
        updateLcd = true;
      }
    } else if (yValue > maxThreshold && joystickMovedUpDown == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMovedUpDown = true;
        updateLcd = true;
      }
    } else if (yValue >= minThreshold && yValue <= maxThreshold) {
      joystickMovedUpDown = false;
    }
    // Serial.println(option);
    return option;
  }

  void printMainMenu() {
    switch (optionMainMenu) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(" > Play Game"));
        lcd.setCursor(0, 1);
        lcd.print(F("   Settings"));

        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, playAnimation[row]);
        }
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("   Play Game"));
        lcd.setCursor(0, 1);
        lcd.print(F(" > Settings"));

        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, settingsAnimation[row]);
        }
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("   Settings"));
        lcd.setCursor(0, 1);
        lcd.print(F(" > Highscores"));

        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, highscoresAnimation[row]);
        }
        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Highscores");
        lcd.setCursor(0, 1);
        lcd.print(" > About");

        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, aboutAnimation[row]);
        }
        break;
      case 5:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   About");
        lcd.setCursor(0, 1);
        lcd.print(" > How to play");

        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, howToPlayAnimation[row]);
        }
        break;
      default:
        break;
    }
  }

  void mainMenu() {
    switch (optionMainMenu) {
      case 1:
        if (pressedButton[ok] == true) {
          lcd.clear();
          inMenu = false;
          inGame = true;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        break;
      case 2:
        if (pressedButton[ok] == true) {
          optionSettings = 1;
          updateLcd = true;
          pressedButton[ok] = false;
        }

        break;
      case 3:
        if (pressedButton[ok] == true) {
          optionHighscores = 1;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        break;
      case 4:
        if (pressedButton[ok] == true) {
          optionAbout = 1;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        break;
      case 5:
        if (pressedButton[ok] == true) {
          optionHowToPlay = 1;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        break;
      default:
        break;
    }
  }

  void printSettingsMenu() {
    switch (optionSettings) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(">Mtrx Brightness"));
        lcd.setCursor(0, 1);
        lcd.print(F(" LCD Brightness"));
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(">LCD Brightness"));
        lcd.setCursor(0, 1);
        lcd.print(F(" Toggle sound"));
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(" LCD Brightness"));
        lcd.setCursor(0, 1);
        lcd.print(F(">Toggle sound"));
        break;
      default:
        break;
    }
  }

  void settingsMenu() {
    switch (optionSettings) {
      case 1:
        if (pressedButton[ok] == true) {
          changeMatrixIntensity = true;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        if (pressedButton[back] == true) {
          optionSettings = 0;
          updateLcd = true;
          pressedButton[back] = false;
        }
        break;
      case 2:
        if (pressedButton[ok] == true) {
          changeLcdIntensity = true;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        if (pressedButton[back] == true) {
          optionSettings = 0;
          updateLcd = true;
          pressedButton[back] = false;
        }
        break;
      case 3:
        if (pressedButton[ok] == true) {
          toggleTheSound = true;
          updateLcd = true;
          pressedButton[ok] = false;
        }
        if (pressedButton[back] == true) {
          optionSettings = 0;
          updateLcd = true;
          pressedButton[back] = false;
        }
        break;
      default:
        break;
    }
  }


  void changeMatrixBrightness() {
    matrixBrightness = moveUpDown(matrixBrightness, maxMatrixBrightness);
    EEPROM.put(eepromAdressMatrixBrightness, matrixBrightness);
    lc.setIntensity(0, matrixBrightness);
    printMatrixBrightness();
    if (pressedButton[back] == true) {
      changeMatrixIntensity = false;
      updateLcd = true;
      pressedButton[back] = false;
    }
  }

  void printMatrixBrightness() {
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Matrix Brightness"));
      lcd.setCursor(7, 1);
      lcd.print(matrixBrightness);
      updateLcd = false;
    }
  }

  void changeLcdBrightness() {
    lcdBrightness = 10 * moveUpDown(lcdBrightness / 10, maxLcdBrightness / 10);
    EEPROM.put(eepromAdressLcdBrightness, lcdBrightness);
    analogWrite(lcdAnode, lcdBrightness);
    printLcdBrightness();
    if (pressedButton[back] == true) {
      changeLcdIntensity = false;
      updateLcd = true;
      pressedButton[back] = false;
    }
  }

  void printLcdBrightness() {
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(F("LCD Brightness"));
      lcd.setCursor(7, 1);
      lcd.print(lcdBrightness);
      updateLcd = false;
    }
  }

  void toggleSound() {
    printSoundStatus();
    if (pressedButton[ok] == true) {
      sound = !sound;
      updateLcd = true;
      EEPROM.put(eepromAdressSoundStatus, sound);
      pressedButton[ok] = false;
    }
    if (pressedButton[back] == true) {
      toggleTheSound = false;
      updateLcd = true;
      pressedButton[back] = false;
    }
  }

  void printSoundStatus() {
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(F("Status Sound"));
      lcd.setCursor(7, 1);
      if (sound == true) {
        lcd.print(F("ON"));
      } else {
        lcd.print(F("OFF"));
      }
      updateLcd = false;
    }
  }

  void printHighscoresMenu() {
    switch (optionHighscores) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(optionHighscores);
        lcd.print(".");
        lcd.setCursor(3, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        printHighscore();
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(optionHighscores);
        lcd.print(".");
        lcd.setCursor(3, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        printHighscore();
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(optionHighscores);
        lcd.print(".");
        lcd.setCursor(3, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        printHighscore();
        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(optionHighscores);
        lcd.print(".");
        lcd.setCursor(3, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        printHighscore();
        break;
      case 5:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(optionHighscores);
        lcd.print(".");
        lcd.setCursor(3, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        printHighscore();
      default:
        break;
    }
  }

  void printHighscore() {
    if (highscores[optionHighscores - 1] == 0) {
      lcd.print(" ");
    } else {
      lcd.print(highscores[optionHighscores - 1]);
    }
  }

  void highscoresMenu() {
    if (pressedButton[back] == true) {
      optionHighscores = 0;
      updateLcd = true;
      pressedButton[back] = false;
    }
  }

  void printAboutMenu() {
    switch (optionAbout) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Game: Survive"));
        lcd.setCursor(0, 1);
        lcd.print(F("the Night"));
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Author: Belu"));
        lcd.setCursor(0, 1);
        lcd.print(F("Mihai Costin"));
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Github Username:"));
        lcd.setCursor(0, 1);
        lcd.print(F("MihaiC0stin"));
        break;
      default:
        break;
    }
  }

  void aboutMenu() {
    if (pressedButton[back] == true) {
      optionAbout = 0;
      updateLcd = true;
      pressedButton[back] = false;
    }
  }

  void printHowToPlayMenu() {
    switch (optionHowToPlay) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Move joystick"));
        lcd.setCursor(0, 1);
        lcd.print(F("left, right,"));
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("up, down to move"));
        lcd.setCursor(0, 1);
        lcd.print(F("and click the"));
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("'ok' button to"));
        lcd.setCursor(0, 1);
        lcd.print(F("attack."));
        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Killing monsters"));
        lcd.setCursor(0, 1);
        lcd.print(F("will award you"));
        break;
      case 5:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("score points"));
        lcd.setCursor(0, 1);
        lcd.print(F("depending on"));
        break;
      case 6:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("difficulty of"));
        lcd.setCursor(0, 1);
        lcd.print(F("the game."));
        break;
      case 7:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("The game has 7"));
        lcd.setCursor(0, 1);
        lcd.print(F("difficulty lvls"));
        break;
      case 8:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("that rises,"));
        lcd.setCursor(0, 1);
        lcd.print(F("every minute or"));
        break;
      case 9:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("if all monsters"));
        lcd.setCursor(0, 1);
        lcd.print(F("on the map are"));
        break;
      case 10:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("dead."));
        lcd.setCursor(0, 1);
        lcd.print(F("Avoid to get"));
        break;
      case 11:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("hit and killed"));
        lcd.setCursor(0, 1);
        lcd.print(F("by monsters :)"));
        break;
      default:
        break;
    }
  }

  void howToPlayMenu() {
    if (pressedButton[back] == true) {
      optionHowToPlay = 0;
      updateLcd = true;
      pressedButton[back] = false;
    }
  }

  void CheckMenuRestarted() {
    if (gameOver == true || gameFinished == true) {
      gameOver = false;
      gameFinished = false;
    }
  }

  void interactiveMenu() {  //this is my interactive menu that just calls methods depending on the flags I set and options.
    CheckMenuRestarted();
    if (optionMainMenu != 0 && optionSettings == 0 && optionHighscores == 0 && optionAbout == 0 && optionHowToPlay == 0) {
      optionMainMenu = moveUpDown(optionMainMenu, numberOfOptionsMain);
      if (updateLcd == true) {
        printMainMenu();
        updateLcd = false;
      }
      mainMenu();
    } else if (optionSettings != 0) {
      if (changeMatrixIntensity == false && changeLcdIntensity == false && toggleTheSound == false) {
        optionSettings = moveUpDown(optionSettings, numberOfOptionsSettings);
        if (updateLcd == true) {
          printSettingsMenu();
          updateLcd = false;
        }
        settingsMenu();
      } else if (changeMatrixIntensity == true) {
        changeMatrixBrightness();
      } else if (changeLcdIntensity == true) {
        changeLcdBrightness();
      } else if (toggleTheSound == true) {
        toggleSound();
      }
    } else if (optionHighscores != 0) {
      optionHighscores = moveUpDown(optionHighscores, numberOfOptionsHighscores);
      if (updateLcd == true) {
        printHighscoresMenu();
        updateLcd = false;
      }
      highscoresMenu();
    } else if (optionAbout != 0) {
      optionAbout = moveUpDown(optionAbout, numberOfOptionsAbout);
      if (updateLcd == true) {
        printAboutMenu();
        updateLcd = false;
      }
      aboutMenu();
    } else if (optionHowToPlay != 0) {
      optionHowToPlay = moveUpDown(optionHowToPlay, numberOfOptionsHowToPlay);
      if (updateLcd == true) {
        printHowToPlayMenu();
        updateLcd = false;
      }
      howToPlayMenu();
    }
  }
};

Menu menu = Menu();

//FUNCTIONS #############################################################################################################################

bool checkObject(byte positionX, byte postionY, byte objectToCheck) {  //used almost everywhere in the game checks for an "id" on the parsed coordinates.
  if (outputMatrix[positionX][postionY] == objectToCheck) {
    return true;
  } else {
    return false;
  }
}


void checkDirection(byte directionToCheck) {  //checks the direction the player moves, used for the bullet animation
  for (int currentDirection = 0; currentDirection < numberOfDirections; currentDirection++) {
    if (currentDirection == directionToCheck) {
      direction[currentDirection] = true;
    } else {
      direction[currentDirection] = false;
    }
  }
}


void mapDisplay() {
  if (matrixChanged == true) {  // Check if the matrix display needs updating
    makeFovMatrix();
    updateMatrix();         // Update the LED matrix display
    matrixChanged = false;  // Reset the update flag
  }
}

void makeFovMatrix() {                                                                 //copy the big matrix into small matrix, with the player as it's center. If the row or col of bigger matrix
  for (int row = player.getPositionX() - 4; row < player.getPositionX() + 4; row++) {  //gets out of range (example < 0) then fill that row with 0;
    for (int col = player.getPositionY() - 4; col < player.getPositionY() + 4; col++) {
      if (row >= 0 && row < mapSize && col >= 0 && col < mapSize) {
        fovMatrix[row + 4 - player.getPositionX()][col + 4 - player.getPositionY()] = outputMatrix[row][col];
      } else {
        fovMatrix[row + 4 - player.getPositionX()][col + 4 - player.getPositionY()] = 0;
      }
    }
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {    //update the matrix using blinked member of these 2 classes, (basically it changes every time
    for (int col = 0; col < matrixSize; col++) {  // from false to true and true to false at a set rate, that I named blink rate (blinkRate).
      if (fovMatrix[row][col] == player.getId()) {
        ledStateMatrix(row, col, player.getBlinkStatus());
      } else if (fovMatrix[row][col] == monster.getId()) {
        ledStateMatrix(row, col, monster.getBlinkStatus());
      } else {
        ledStateMatrix(row, col, fovMatrix[row][col]);
      }
    }
  }
}

void ledStateMatrix(int row, int col, byte ledState) {  //I have BS
  lc.setLed(displayLed, col, row, ledState);            // set each led individually
}

void iWantOrderInTheHighscores() {  //sort my highscores and names using the scores
  for (byte iterator = 0; iterator < numberOfHighscores - 1; iterator++) {
    if (highscores[iterator] < highscores[iterator + 1]) {
      byte temp = highscores[iterator];
      highscores[iterator] = highscores[iterator + 1];
      highscores[iterator + 1] = temp;
      char tempName[numberOfHighscores + 1];
      strcpy(tempName, highscoresNames[iterator]);
      strcpy(highscoresNames[iterator], highscoresNames[iterator + 1]);
      strcpy(highscoresNames[iterator + 1], tempName);
    }
  }
}

void debounce() {  //the classic debounce, but for 2 buttons using arrays
  for (int currentButton = 0; currentButton < numberOfButtons; currentButton++) {
    reading[currentButton] = digitalRead(button[currentButton]);
    if (reading[currentButton] != lastReading[currentButton]) {
      lastDebounceTime[currentButton] = millis();
    }
    if ((millis() - lastDebounceTime[currentButton]) > debounceDelay) {
      if (reading[currentButton] != buttonState[currentButton]) {
        buttonState[currentButton] = reading[currentButton];
        if (buttonState[currentButton] == LOW) {
          pressedButton[currentButton] = true;
        }
      }
    }
    lastReading[currentButton] = reading[currentButton];
  }
}

void resetButtons() {
  for (int currentButton = 0; currentButton < numberOfButtons; currentButton++) {
    pressedButton[currentButton] = false;
  }
}