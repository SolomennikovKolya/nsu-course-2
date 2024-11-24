package ru.nsu.nikvel.tetris.model.figures;

public class FigureO extends Figure {
    private static final int[][][] states = {
            {{1, 0}, {1, 1}, {2, 0}, {2, 1}},
            {{1, 0}, {1, 1}, {2, 0}, {2, 1}},
            {{1, 0}, {1, 1}, {2, 0}, {2, 1}},
            {{1, 0}, {1, 1}, {2, 0}, {2, 1}}};

    FigureO(int x0, int y0) {
        super(x0, y0, states);
    }

    FigureO() {
        super(states);
    }
}
