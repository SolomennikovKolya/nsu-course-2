package ru.nsu.nikvel.tetris.model.figures;

import static ru.nsu.nikvel.tetris.model.Utils.copy2DArr;
import static ru.nsu.nikvel.tetris.model.Utils.copy3DArr;
import static ru.nsu.nikvel.tetris.model.Utils.mod;

public abstract class Figure {
    // текущие координаты фигуры
    private int x;
    private int y;
    // текущий поворот фигуры (в пределах [0, 3])
    private int turn;
    // все возможные положения клеток фигуры [4 варианта поворота][4 клетки][2 координаты]
    private final int[][][] states;

    Figure(int x0, int y0, int[][][] states) {
        this.x = x0;
        this.y = y0;
        this.turn = 0;
        this.states = copy3DArr(states);
    }

    Figure(int[][][] states) {
        this(0, 0, states);
    }

    /**
     * получить абсолутные координаты клеток фигуры (4 пары чисел)
     **/
    public int[][] getCoordsOfCells() {
        int[][] coords = copy2DArr(states[turn]);
        for (int i = 0; i < 4; ++i) {
            coords[i][0] += x;
            coords[i][1] += y;
        }
        return coords;
    }

    /**
     * подвинуть фигуру
     **/
    public void shift(int dx, int dy) {
        x += dx;
        y += dy;
    }

    /**
     * повернуть фигуру по часовой
     **/
    public void rotateRight() {
        turn = mod(turn + 1, 4);
    }

    /**
     * повернуть фигуру против часовой
     **/
    public void rotateLeft() {
        turn = mod(turn - 1, 4);
    }
}
