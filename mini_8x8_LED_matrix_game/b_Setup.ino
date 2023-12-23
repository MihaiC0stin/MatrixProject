void setup() {
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false);                 // turn off power saving, enables display
  EEPROM.get(eepromAdressMatrixBrightness,matrixBrightness);      //get the brightness value from EEPROM for matrix
  if(matrixBrightness > 15){                                      // in case EEPROM address was used for something else in another project and value is out of range
    matrixBrightness = 2;                                         // set default matrixBrightness
  }
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // Clear the matrix display
  randomSeed(analogRead(A5));
  lcd.begin(16, 2);
  for (int currentButton = 0; currentButton < numberOfButtons; currentButton++) {
    pinMode(button[currentButton], INPUT_PULLUP);
  }
  pinMode(lcdAnode, INPUT);
  EEPROM.get(eepromAdressLcdBrightness,lcdBrightness);            //same, like matrixBrightness
  if(lcdBrightness > 255){
    lcdBrightness = 180;
  }
  analogWrite(lcdAnode, lcdBrightness);
  EEPROM.get(eepromAdressSoundStatus, sound);           // same, but here no need for default, because it is bool so only false = 0, true = anything else than 0

  lcd.createChar(0, lcdStickmanMove);                    // create custom characters for startup animation and highscore
  lcd.createChar(1, lcdStickmanStay);
  lcd.createChar(2, highscoresAnimationLcd);
}
