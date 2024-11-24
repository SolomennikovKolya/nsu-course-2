package ru.nsu.nikvel.tetris.model.figures;

public class FigureT extends Figure {
    private static final int[][][] states = {
            {{0, 1}, {1, 0}, {1, 1}, {2, 1}},
            {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
            {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
            {{0, 1}, {1, 0}, {1, 1}, {1, 2}}};

    FigureT(int x0, int y0) {
        super(x0, y0, states);
    }

    FigureT() {
        super(states);
    }
}
