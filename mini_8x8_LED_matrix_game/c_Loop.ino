void loop() {
  debounce();       //default debounce
  iWantOrderInTheHighscores();    // function that compare and swap highscores (some king of sort)
  if (positionAnimationY < 2) {   // this is my startup animation, it should have look like a monster, but now it looks more like a cowboy, whatever.        
    if (millis() - lastStartupAnimationTime >= startupAnimationDelay) {     //delay made with millis()
      lcd.clear();
      lcd.setCursor(positionAnimationX, positionAnimationY);                //I made the animation to move from position 1 row 1 to position 2 row 2 (not sure if to talk like a human in comments and to use 1 instead of 0)
      if (positionAnimationX % 2 == 0) {                                    //Alternate between custom characters, one is a "cowboy" standing, other is a "cowboy" running, it looks a little more "fun" like an animation in 2 fps
        lcd.write(byte(0));                                                 
        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, matrixStickmanMove[row]);
        }
      } else {
        lcd.write(byte(1));
        for (byte row = 0; row < matrixSize; row++) {
          lc.setColumn(0, row, matrixStickmanStay[row]);
        }
      }
      positionAnimationX++;
      if (positionAnimationX > 15) {
        positionAnimationX = 0;
        positionAnimationY++;
      }
      lastStartupAnimationTime = millis();
    }
  } else {
    if (player.getInSaveName() == false) {      //this if, if it is true, will pause the game, and will show me a screen where to put my name.
      if (inGame == true) {                     // we start the game
        game.startGame();
      } else if (inMenu == true) {              // we scroll in menu
        menu.interactiveMenu();
      }
    } else {
      player.savePlayerName();
    }
  }
   resetButtons();        // reset the status of buttons (buttonPressed = false)
}