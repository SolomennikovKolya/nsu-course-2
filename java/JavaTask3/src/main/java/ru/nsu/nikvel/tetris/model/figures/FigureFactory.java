package ru.nsu.nikvel.tetris.model.figures;

public class FigureFactory {
    public Figure getFigure(int figureNum) {
        return switch (figureNum) {
            case 0 -> new FigureI();
            case 1 -> new FigureT();
            case 2 -> new FigureJ();
            case 3 -> new FigureL();
            case 4 -> new FigureS();
            case 5 -> new FigureZ();
            case 6 -> new FigureO();
            default -> new FigureI();
        };
    }

    public Figure getFigure(int figureNum, int x0, int y0) {
        return switch (figureNum) {
            case 0 -> new FigureI(x0, y0);
            case 1 -> new FigureT(x0, y0);
            case 2 -> new FigureJ(x0, y0);
            case 3 -> new FigureL(x0, y0);
            case 4 -> new FigureS(x0, y0);
            case 5 -> new FigureZ(x0, y0);
            case 6 -> new FigureO(x0, y0);
            default -> new FigureI(x0, y0);
        };
    }
}
