package ru.nsu.nikvel.tetris.model;

import java.util.ArrayList;
import java.util.TreeMap;

public interface ModelForViewer {
    abstract public int getGridWidth();

    abstract public int getGridHeight();

    abstract public int getCell(int x, int y);

    abstract public int[][] getActiveFigureCells();

    abstract public GameMode getGameMode();

    abstract public int getGamePeriod();

    abstract public int getScore();

    abstract public ArrayList<Pair<Integer, String>> getScoreboard();
}
