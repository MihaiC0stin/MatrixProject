void setup() {
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // Clear the matrix display
  randomSeed(analogRead(A5));
  lcd.begin(16, 2);
  for (int currentButton = 0; currentButton < numberOfButtons; currentButton++) {
    pinMode(button[currentButton], INPUT_PULLUP);
  }
  pinMode(lcdAnode, INPUT);
  analogWrite(lcdAnode, lcdBrightness);
}
