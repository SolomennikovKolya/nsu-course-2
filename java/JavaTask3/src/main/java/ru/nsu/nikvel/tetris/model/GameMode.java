package ru.nsu.nikvel.tetris.model;

/**
 * Режимы игры
 **/
public enum GameMode {
    mainMenu,   // главное меню
    inProgress, // игра в процессе
    gameOver,   // проигрыш
    ended,      // игра завершилась (приложение должно закрыться)
}
