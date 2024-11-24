package ru.nsu.nikvel.tetris.model;

public interface ModelForController {
    abstract public void moveFigureLeft();

    abstract public void moveFigureRight();

    abstract public void moveFigureAllTheWayDown();

    abstract public void rotateFigureLeft();

    abstract public void rotateFigureRight();

    abstract public GameMode getGameMode();

    abstract public void startMainMenu();

    abstract public void startGame();

    abstract public void endGame();

}
