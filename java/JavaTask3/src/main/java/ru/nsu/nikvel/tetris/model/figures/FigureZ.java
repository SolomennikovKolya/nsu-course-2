package ru.nsu.nikvel.tetris.model.figures;

public class FigureZ extends Figure {
    private static final int[][][] states = {
            {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
            {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
            {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
            {{0, 1}, {0, 2}, {1, 0}, {1, 1}}};

    FigureZ(int x0, int y0) {
        super(x0, y0, states);
    }

    FigureZ() {
        super(states);
    }
}
