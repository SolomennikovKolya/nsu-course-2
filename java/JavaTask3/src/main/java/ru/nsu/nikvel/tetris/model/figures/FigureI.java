package ru.nsu.nikvel.tetris.model.figures;

public class FigureI extends Figure {
    private static final int[][][] states = {
            {{1, 0}, {1, 1}, {1, 2}, {1, 3}},
            {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
            {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
            {{0, 2}, {1, 2}, {2, 2}, {3, 2}}};

    FigureI(int x0, int y0) {
        super(x0, y0, states);
    }

    FigureI() {
        super(states);
    }
}
