//LIBRARIES ########################################################################################################################################
#include "LedControl.h"  // Include LedControl library for controlling the LED matrix
#include <LiquidCrystal.h>

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
const int maxThreshold = 700;
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

const byte lcdAnode = 3;
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

const int maxQueueSize = 254;
const byte maxNumberOfMonsters = 56;

const byte maxMatrixBrightness = 15;
const int maxLcdBrightness = 255;

const byte numberOfCharacters = 53;
const byte maxNameLength = 10;
const byte numberOfHighscores = 5;
//VARIABLES #########################################################################################################################################
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

int money = 0;
int score = 0;
int lastScore = 0;
int highscores[numberOfHighscores];
char highscoresNames[numberOfHighscores][maxNameLength + 1];

bool gameReseted = false;


//LIBRARY OBJECTS ##################################################################################################################################
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//PROTOTYPE FUNCTIONS ##########################################################################################################################
bool checkObject(byte, byte, byte);
void checkDirection(byte);

//CLASSES ######################################################################################################################################
class GameEntity {
protected:
  byte id = 0;
  byte health = 0;
  unsigned long lastBlink;
  int blinkRate;
  bool blinked;
public:
  GameEntity(byte id, byte health, int blinkRate) {
    this->id = id;
    this->health = health;
    this->blinkRate = blinkRate;
    this->lastBlink = 0;
    bool blinked = true;
  };

  void setId(byte id) {
    this->id = id;
  }

  byte getId() {
    return this->id;
  }

  void blink() {
    if (millis() - lastBlink >= blinkRate) {
      this->blinked = !blinked;
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
  int speed = 0;
  byte numberOfMonsters = 0;
  byte scoreValue = 0;
  byte moneyValue = 0;

public:
  Monster(byte id, byte health, int blinkRate, int speed)
    : GameEntity(id, health, blinkRate) {
    this->speed = speed;
  }

  void spawnMonsters(byte iteratorStart, byte numberOfMonsters) {
    this->numberOfMonsters = this->numberOfMonsters + numberOfMonsters;
    for (byte currentMonster = iteratorStart; currentMonster < numberOfMonsters; currentMonster++) {
      MonstersHealth[currentMonster] = health;
      xPosMonsters[currentMonster] = random(0, mapSize);
      yPosMonsters[currentMonster] = random(0, mapSize);
      if (outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] != 0) {
        currentMonster--;
      } else {
        outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] = id;
      }
    }
  }

  void ManageMonsterHealth(byte xPos, byte yPos, byte damageReceived) {
    for (byte currentMonster = 0; currentMonster < numberOfMonsters; currentMonster++) {
      if (xPosMonsters[currentMonster] == xPos && yPosMonsters[currentMonster] == yPos) {
        if ((MonstersHealth[currentMonster] - damageReceived) <= 0) {
          outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] = 0;
          score = score + scoreValue;
          if (currentMonster != numberOfMonsters - 1) {
            for (byte iterator = currentMonster; iterator < numberOfMonsters - 1; iterator++)
              MonstersHealth[currentMonster] = MonstersHealth[currentMonster + 1];
            xPosMonsters[currentMonster] = xPosMonsters[currentMonster + 1];
            yPosMonsters[currentMonster] = yPosMonsters[currentMonster + 1];
          } else {
            MonstersHealth[currentMonster] = 0;
            xPosMonsters[currentMonster] = 0;
            yPosMonsters[currentMonster] = 0;
          }
          numberOfMonsters--;
        } else {
          MonstersHealth[currentMonster] = MonstersHealth[currentMonster] - damageReceived;
        }
      }
    }
  }

  void updateMonstersPosition(byte xPosMonsters, byte yPosMonsters, byte currentMonster) {
    outputMatrix[this->xPosMonsters[currentMonster]][this->yPosMonsters[currentMonster]] = 0;
    this->xPosMonsters[currentMonster] = xPosMonsters;
    this->yPosMonsters[currentMonster] = yPosMonsters;
    outputMatrix[this->xPosMonsters[currentMonster]][this->yPosMonsters[currentMonster]] = id;
    matrixChanged = true;
  }

  void boostMonsterStatus() {
    health = health + 1;
    speed = speed - 300;
    damage = damage + 1;
  }

  void reset() {
    health = 5;
    speed = 3000;
    damage = 5;
    if (numberOfMonsters != 0) {
      for (byte currentMonster = 0; currentMonster < numberOfMonsters; currentMonster++) {
        outputMatrix[xPosMonsters[currentMonster]][yPosMonsters[currentMonster]] = 0;
        MonstersHealth[currentMonster] = 0;
        xPosMonsters[currentMonster] = 0;
        yPosMonsters[currentMonster] = 0;
      }
      numberOfMonsters = 0;
      blinked = false;
    }
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

Monster monster = Monster(6, 5, 200, 3000);


class Player : public GameEntity {
  char name[maxNameLength + 1] = "Player";
  char nameCharacters[numberOfCharacters] = { ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                              'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
  byte cursorPosition = 0;
  byte lastCursorPosition = 0;
  byte letter[maxNameLength];
  byte positionX;
  byte positionY;
  byte lastPositionX;
  byte lastPositionY;
  unsigned long lastMoved = 0;
  int speed;
  bool joystickMovedLeftRight = false;
  bool joystickMovedUpDown = false;
  bool updateLCD = true;
  bool inSaveName = false;



public:
  Player(byte id, byte health, int blinkrate, byte positionX, byte positionY, byte lastPositionX, byte lastPositionY, int speed)
    : GameEntity(id, health, blinkrate) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->lastPositionX = lastPositionX;
    this->lastPositionY = lastPositionY;
    this->speed = speed;
  }

  void setPosition(byte positionX, byte positionY) {
    this->positionX = positionX;
    this->positionY = positionY;
  }

  void SetUpdateLcd(bool updateLCD) {
    this->updateLCD = updateLCD;
  }
  void updateLastPosition() {
    this->lastPositionX = this->positionX;
    this->lastPositionY = this->positionY;
  }

  bool checkInRange(byte option, byte numberOfOptions) {
    if (option < 0 || option >= numberOfOptions) {
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
        updateLCD = true;
      }
    } else if (xValue > maxThreshold && joystickMovedLeftRight == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMovedLeftRight = true;
        updateLCD = true;
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
        updateLCD = true;
      }
    } else if (yValue > maxThreshold && joystickMovedUpDown == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMovedUpDown = true;
        updateLCD = true;
      }
    } else if (yValue >= minThreshold && yValue <= maxThreshold) {
      joystickMovedUpDown = false;
    }
    return option;
  }

  void savePlayerName() {
    cursorPosition = moveLeftRight(cursorPosition, maxNameLength);
    letter[cursorPosition] = moveUpDown(letter[cursorPosition], numberOfCharacters);
    if (updateLCD == true) {
      lcd.setCursor(lastCursorPosition, 1);
      lcd.print(" ");
      lcd.setCursor(cursorPosition, 0);
      lastCursorPosition = cursorPosition;
      lcd.print(nameCharacters[letter[cursorPosition]]);
      lcd.setCursor(cursorPosition, 1);
      lcd.print("^");
      updateLCD = false;
    }
    if (pressedButton[ok] == true) {
      for (byte iterator = 0; iterator < maxNameLength; iterator++) {
        name[iterator] = nameCharacters[letter[iterator]];
      }

      strcpy(highscoresNames[numberOfHighscores - 1], name);
      inSaveName = false;
      inMenu = true;
      inGame = false;
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
  void updatePlayerPosition() {
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

  void managePlayerHealth(byte damageReceived) {
    if ((health - damageReceived) <= 0) {
      gameOver = true;
      health = 0;
    } else {
      health = health - damageReceived;
    }
  }

  void reset() {
    outputMatrix[positionX][positionY] = 0;
    blinked = false;
    updateMatrix();
    name[maxNameLength + 1] = "Player";
    health = 20;
    positionX = 5;
    positionY = 5;
    lastPositionX = 5;
    lastPositionY = 5;
  }
};

Player player = Player(2, 20, 400, 5, 5, 5, 5, 200);

class Weapons {
protected:
  byte id;
  byte damage;
  unsigned long lastAnimation = 0;
  bool monsterHit = false;
  byte range = 0;
  byte currentCycle = 0;
public:
  Weapons(byte id, byte damage, byte range) {
    this->id = id;
    this->damage = damage;
    this->range = range;
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
  byte bulletSpeed;
public:
  Pistol(byte id, byte damage, byte range, byte bulletSpeed)
    : Weapons(id, damage, range) {
    this->bulletSpeed = bulletSpeed;
  }

  void shoot() {
    CheckToShoot();
    bulletAnimation();
  }

  void CheckToShoot() {
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
        updateBulletPosition();
        lastAnimation = millis();
      } else if (currentCycle == range) {
        clearBullet();
        matrixChanged = true;
        currentCycle++;
      } else if (currentCycle > range) {
        for (int currentDirection = 0; currentDirection < numberOfDirections; currentDirection++) {
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
          if (checkObject(xPosBullet, yPosBullet - 1, monster.getId()) == false && checkObject(xPosBullet, yPosBullet - 1, wall) == false) {
            yPosBullet--;
          } else if (checkObject(xPosBullet, yPosBullet - 1, monster.getId()) == true) {
            monster.ManageMonsterHealth(xPosBullet, yPosBullet - 1, damage);
            monsterHit = true;
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
      if ((xPosBullet != xLastPosBullet || yPosBullet != yLastPosBullet) && monsterHit == false) {
        matrixChanged = true;
        outputMatrix[xLastPosBullet][yLastPosBullet] = nothing;
        outputMatrix[xPosBullet][yPosBullet] = bullet;
      } else if (monsterHit == true) {
        matrixChanged = true;
        outputMatrix[xLastPosBullet][yLastPosBullet] = nothing;
      }
    } else {
      if ((xPosBullet != xLastPosBullet || yPosBullet != yLastPosBullet) && monsterHit == false) {
        matrixChanged = true;
        outputMatrix[xPosBullet][yPosBullet] = bullet;
      } else if (monsterHit == true) {
        matrixChanged = true;
      }
    }
  }
};

Pistol pistol = Pistol(3, 2, 3, 50);

class GameDifficulty {
  byte difficulty = 1;
  bool updateLvl = true;
  unsigned int increaseDifficultyDelay = 1000 * 60 * 1;
  unsigned long lastDifficultyIncrease = 0;
  byte maxDifficulty = 7;

public:
  GameDifficulty(byte difficulty) {
    this->difficulty = difficulty;
  }
  void setGameDifficulty(byte difficulty) {
    this->difficulty = difficulty;
    updateLvl = true;
  }
  void updateTheLvl() {
    if (updateLvl == true) {
      if (difficulty == 1) {
        outputMatrix[player.getPositionX()][player.getPositionY()] = player.getId();
      }
      monster.spawnMonsters(monster.getNumberOfMonsters(), difficulty * 2);
      monster.setScoreValue(difficulty * 10);
      if (difficulty > 1) {
        monster.boostMonsterStatus();
      }
      Serial.println(monster.getNumberOfMonsters());
      matrixChanged = true;
      updateLvl = false;
    }
  }

  void finishLvl() {
    if (difficulty < maxDifficulty) {
      if ((monster.getNumberOfMonsters() == 0) || (millis() - lastDifficultyIncrease >= increaseDifficultyDelay)) {
        setGameDifficulty(difficulty + 1);
        lastDifficultyIncrease = millis();
      }
    }
  }

  void finishGame() {
    if (difficulty == maxDifficulty && monster.getNumberOfMonsters() == 0) {
      gameFinished = true;
    }
  }
};

GameDifficulty gameDifficulty = GameDifficulty(1);

// class Queue {
//   byte queueArray[maxQueueSize];
//   int front;
//   int rear;

// public:
//   Queue() {
//     front = -1;
//     rear = -1;
//   }

//   bool isFull() {
//     if (front == 0 && rear == maxQueueSize - 1) {
//       return true;
//     }
//     return false;
//   }
//   bool isEmpty() {
//     if (front == -1) {
//       return true;
//     }
//     return false;
//   }

//   void addElement(byte value) {
//     if (isFull() == false) {
//       if (front == -1) {
//         front = 0;
//       }
//       rear++;
//       queueArray[rear] = value;
//     }
//   }

//   void deleteElement() {
//     byte value;
//     if (isEmpty() == false) {
//       value = queueArray[front];
//     }
//   }
// };


class Game {
  unsigned long monsterLastMoved;
  bool updateLcd = true;
  unsigned long lastTimePrinted = 0;
  int victoryMessageDelay = 6000;

  void playerGotHit() {
    player.managePlayerHealth(monster.getMonsterDamage());
    updateLcd = true;
  }
public:

  void resetGame() {
    if (pressedButton[ok] == true) {
      for (byte iterator = 0; iterator < numberOfHighscores; iterator++) {
        if (score > highscores[iterator]) {
          player.setInSaveName(true);
          lcd.clear();
          player.SetUpdateLcd(true);
          highscores[numberOfHighscores - 1] = score;
        }
      }
      if (player.getInSaveName() == false) {
        score = 0;
        player.reset();
      }
      monster.reset();
      gameDifficulty.setGameDifficulty(1);
      inGame = false;
      inMenu = true;
      pressedButton[ok] = false;
    }
  }

  void startGame() {
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
    } else if (gameFinished == true && gameOver == false) {
      lcd.setCursor(0, 0);
      lcd.print("Congratulations ");
      lcd.setCursor(0, 1);
      lcd.print("Your score:");
      lcd.print(score);
      resetGame();
    } else if (gameFinished == false && gameOver == true) {
      lcd.setCursor(0, 0);
      lcd.print("    You Died     ");
      lcd.setCursor(0, 1);
      lcd.print("Your score:");
      lcd.print(score);
      resetGame();
    }
  }

  void checkScoreChange() {
    if (score != lastScore) {
      updateLcd = true;
      lastScore = score;
    }
  }

  void printGameInfo() {
    if (updateLcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HP:");
      lcd.print(player.getHealth());
      lcd.setCursor(8, 0);
      lcd.print("$ ");
      lcd.print(money);
      lcd.setCursor(0, 1);
      lcd.print("score: ");
      lcd.print(score);
      updateLcd = false;
    }
  }

  void monsterAI() {
    if (millis() - monsterLastMoved >= monster.getSpeed()) {
      for (int currentMonster = 0; currentMonster < monster.getNumberOfMonsters(); currentMonster++) {
        bool left = false;
        bool up = false;
        bool right = false;
        bool down = false;

        if (abs(player.getPositionX() - monster.getMonsterPositionX(currentMonster)) > abs(player.getPositionY() - monster.getMonsterPositionY(currentMonster))) {
          if (player.getPositionX() < monster.getMonsterPositionX(currentMonster)) {
            up = true;
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

        if (left == true) {
          if (checkObject(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) - 1, wall) == false) {
            if (checkObject(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) - 1, player.getId()) == false) {
              monster.updateMonstersPosition(monster.getMonsterPositionX(currentMonster), monster.getMonsterPositionY(currentMonster) - 1, currentMonster);
            } else {
              playerGotHit();
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
  bool joystickMoved = false;
  bool updateLCD = true;
  byte numberOfOptionsMain = 4;
  byte numberOfOptionsSettings = 3;
  byte numberOfOptionsHighscores = 5;
  byte numberOfOptionsAbout = 2;
  bool changeMatrixIntensity = false;
  bool changeLcdIntensity = false;
  bool toggleTheSound = false;

public:
  Menu(byte optionMainMenu, byte optionSettings, byte optionAbout, byte optionHighscores) {
    this->optionMainMenu = optionMainMenu;
    this->optionSettings = optionSettings;
    this->optionAbout = optionAbout;
    this->optionHighscores = optionHighscores;
  }

  bool checkInRange(byte option, byte numberOfOptions) {
    if (option <= 0 || option > numberOfOptions) {
      return false;
    } else {
      return true;
    }
  }

  byte moveLeftRight(byte option, byte numberOfOptions) {
    int xValue = analogRead(xPin);
    if (xValue < minThreshold && joystickMoved == false) {
      if (checkInRange(option - 1, numberOfOptions) == true) {
        option--;
        joystickMoved = true;
        updateLCD = true;
      }
    } else if (xValue > maxThreshold && joystickMoved == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMoved = true;
        updateLCD = true;
      }
    } else if (xValue >= minThreshold && xValue <= maxThreshold) {
      joystickMoved = false;
    }
    // Serial.println(option);
    return option;
  }


  byte moveUpDown(byte option, byte numberOfOptions) {
    int yValue = analogRead(yPin);
    if (yValue < minThreshold && joystickMoved == false) {
      if (checkInRange(option - 1, numberOfOptions) == true) {
        option--;
        joystickMoved = true;
        updateLCD = true;
      }
    } else if (yValue > maxThreshold && joystickMoved == false) {
      if (checkInRange(option + 1, numberOfOptions) == true) {
        option++;
        joystickMoved = true;
        updateLCD = true;
      }
    } else if (yValue >= minThreshold && yValue <= maxThreshold) {
      joystickMoved = false;
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
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("   Play Game"));
        lcd.setCursor(0, 1);
        lcd.print(F(" > Settings"));
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("   Settings"));
        lcd.setCursor(0, 1);
        lcd.print(F(" > Highscores"));
        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Highscores");
        lcd.setCursor(0, 1);
        lcd.print(" > About");
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
          pressedButton[ok] = false;
        }
        break;
      case 2:
        if (pressedButton[ok] == true) {
          optionSettings = 1;
          updateLCD = true;
          pressedButton[ok] = false;
        }

        break;
      case 3:
        if (pressedButton[ok] == true) {
          optionHighscores = 1;
          updateLCD = true;
          pressedButton[ok] = false;
        }
        break;
      case 4:
        if (pressedButton[ok] == true) {
          optionAbout = 1;
          updateLCD = true;
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
        lcd.print(F(" > Set Matrix Brightness"));
        lcd.setCursor(0, 1);
        lcd.print(F("  Set LCD Brightness"));
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(" > Set LCD Brightness"));
        lcd.setCursor(0, 1);
        lcd.print(F("   Toggle sound ON/OFF"));
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("   Set LCD Brightness"));
        lcd.setCursor(0, 1);
        lcd.print(F(" > Toggle sound ON/OFF"));
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
          pressedButton[ok] = false;
        }
        if (pressedButton[back] == true) {
          optionSettings = 0;
          updateLCD = true;
          pressedButton[back] = false;
        }
        break;
      case 2:
        if (pressedButton[ok] == true) {
          changeLcdIntensity = true;
          pressedButton[ok] = false;
        }
        if (pressedButton[back] == true) {
          optionSettings = 0;
          updateLCD = true;
          pressedButton[back] = false;
        }
        break;
      case 3:
        if (pressedButton[ok] == true) {
          toggleTheSound = true;
          pressedButton[ok] = false;
        }
        if (pressedButton[back] == true) {
          optionSettings = 0;
          updateLCD = true;
          pressedButton[back] = false;
        }
        break;
      default:
        break;
    }
  }


  void changeMatrixBrightness() {
    matrixBrightness = moveUpDown(matrixBrightness, maxMatrixBrightness);
    lc.setIntensity(0, matrixBrightness);
    Serial.println(matrixBrightness);
    if (pressedButton[back] == true) {
      changeMatrixIntensity = false;
      updateLCD = true;
      pressedButton[back] = false;
    }
  }

  void changeLcdBrightness() {
    lcdBrightness = 10 * moveUpDown(lcdBrightness / 10, maxLcdBrightness / 10);
    analogWrite(lcdAnode, lcdBrightness);
    Serial.println(lcdBrightness);
    if (pressedButton[back] == true) {
      changeLcdIntensity = false;
      updateLCD = true;
      pressedButton[back] = false;
    }
  }

  void toggleSound() {
    if (pressedButton[ok]) {
      sound = !sound;
      Serial.println(sound);
      pressedButton[ok] = false;
    }
    if (pressedButton[back] == true) {
      toggleTheSound = false;
      updateLCD = true;
      pressedButton[back] = false;
    }
  }

  void printHighscoresMenu() {
    switch (optionHighscores) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        lcd.print(highscores[optionHighscores - 1]);
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        lcd.print(highscores[optionHighscores - 1]);
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        lcd.print(highscores[optionHighscores - 1]);
        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        lcd.print(highscores[optionHighscores - 1]);
        break;
      case 5:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(highscoresNames[optionHighscores - 1]);
        lcd.setCursor(0, 1);
        lcd.print(highscores[optionHighscores - 1]);
      default:
        break;
    }
  }

  void highscoresMenu() {
    if (pressedButton[back] == true) {
      optionHighscores = 0;
      updateLCD = true;
      pressedButton[back] = false;
    }
  }

  void printAboutMenu() {
    switch (optionAbout) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("> Game Name: Survive the Night"));
        lcd.setCursor(0, 1);
        lcd.print(F("  Github: https://github.com/MihaiC0stin"));
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("  Game Name: Survive the Night"));
        lcd.setCursor(0, 1);
        lcd.print(F("> Github: https://github.com/MihaiC0stin"));
        break;
      default:
        break;
    }
  }

  void aboutMenu() {
    if (pressedButton[back] == true) {
      optionAbout = 0;
      updateLCD = true;
      pressedButton[back] = false;
    }
  }

  void CheckMenuRestarted() {
    if (gameOver == true || gameFinished == true) {
      updateLCD = true;
      gameOver = false;
      gameFinished = false;
    }
  }

  void interactiveMenu() {
    CheckMenuRestarted();
    if (optionMainMenu != 0 && optionSettings == 0 && optionHighscores == 0 && optionAbout == 0) {
      optionMainMenu = moveUpDown(optionMainMenu, numberOfOptionsMain);
      if (updateLCD == true) {
        printMainMenu();
        updateLCD = false;
      }
      mainMenu();
    } else if (optionMainMenu != 0 && optionSettings != 0 && optionHighscores == 0 && optionAbout == 0) {
      if (changeMatrixIntensity == false && changeLcdIntensity == false && toggleTheSound == false) {
        optionSettings = moveUpDown(optionSettings, numberOfOptionsSettings);
        if (updateLCD == true) {
          printSettingsMenu();
          updateLCD = false;
        }
        settingsMenu();
      } else if (changeMatrixIntensity == true) {
        changeMatrixBrightness();
      } else if (changeLcdIntensity == true) {
        changeLcdBrightness();
      } else if (toggleTheSound == true) {
        toggleSound();
      }
    } else if (optionMainMenu != 0 && optionSettings == 0 && optionHighscores != 0 && optionAbout == 0) {
      optionHighscores = moveUpDown(optionHighscores, numberOfOptionsHighscores);
      if (updateLCD == true) {
        printHighscoresMenu();
        updateLCD = false;
      }
      highscoresMenu();
    } else if (optionMainMenu != 0 && optionSettings == 0 && optionHighscores == 0 && optionAbout != 0) {
      optionAbout = moveUpDown(optionAbout, numberOfOptionsAbout);
      if (updateLCD == true) {
        printAboutMenu();
        updateLCD = false;
      }
      aboutMenu();
    }
  }
};

Menu menu = Menu(1, 0, 0, 0);

//FUNCTIONS #############################################################################################################################

bool checkObject(byte positionX, byte postionY, byte objectToCheck) {
  if (outputMatrix[positionX][postionY] == objectToCheck) {
    return true;
  } else {
    return false;
  }
}


void checkDirection(byte directionToCheck) {
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

void makeFovMatrix() {
  // Serial.println();
  for (int row = player.getPositionX() - 4; row < player.getPositionX() + 4; row++) {
    for (int col = player.getPositionY() - 4; col < player.getPositionY() + 4; col++) {
      if (row >= 0 && row < mapSize && col >= 0 && col < mapSize) {
        fovMatrix[row + 4 - player.getPositionX()][col + 4 - player.getPositionY()] = outputMatrix[row][col];
        // Serial.print(fovMatrix[row + 4 - player.getPositionX()][col + 4 - player.getPositionY()]);
        // Serial.print(" ");
      } else {
        fovMatrix[row + 4 - player.getPositionX()][col + 4 - player.getPositionY()] = 0;
        // Serial.print(fovMatrix[row + 4 - player.getPositionX()][col + 4 - player.getPositionY()]);
        // Serial.print(" ");
      }
    }
    // Serial.println();
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
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

void ledStateMatrix(int row, int col, byte ledState) {
  lc.setLed(displayLed, col, row, ledState);  // set each led individually
}

void iWantOrderInTheHighscores() {
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

void debounce() {
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