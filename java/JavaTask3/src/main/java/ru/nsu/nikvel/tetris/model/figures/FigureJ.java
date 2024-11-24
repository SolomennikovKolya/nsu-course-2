package ru.nsu.nikvel.tetris.model.figures;

public class FigureJ extends Figure {
    private static final int[][][] states = {
            {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
            {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
            {{0, 2}, {1, 0}, {1, 1}, {1, 2}},
            {{0, 0}, {0, 1}, {1, 1}, {2, 1}}};

    FigureJ(int x0, int y0) {
        super(x0, y0, states);
    }

    FigureJ() {
        super(states);
    }
}
