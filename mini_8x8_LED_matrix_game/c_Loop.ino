void loop() {
  iWantOrderInTheHighscores();
  debounce();
  if (player.getInSaveName() == false) {
    if (inGame == true) {
      game.startGame();
    } else if (inMenu == true) {
      menu.interactiveMenu();
    }
  } else {
    player.savePlayerName();
  }
}