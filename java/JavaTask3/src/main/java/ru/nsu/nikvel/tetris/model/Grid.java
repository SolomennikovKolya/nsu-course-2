package ru.nsu.nikvel.tetris.model;

import static ru.nsu.nikvel.tetris.model.Utils.mod;

public class Grid {
    private final int width;
    private final int height;
    private final int[][] grid;

    public Grid(int width, int height) {
        this.width = width;
        this.height = height;
        this.grid = new int[width][height];
    }

    /**
     * Отчистить поле
     **/
    public void clear() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                grid[x][y] = 0;
            }
        }
    }

    /**
     * положить значение в клетку
     **/
    public void set(int x, int y, int val) {
        grid[mod(x, width)][mod(y, height)] = val;
    }

    /**
     * прочитать значение из клетки
     **/
    public int get(int x, int y) {
        return grid[mod(x, width)][mod(y, height)];
    }

    /**
     * получить ширину поля
     **/
    public int getWidth() {
        return width;
    }

    /**
     * получить высоту поля
     **/
    public int getHeight() {
        return height;
    }
}
