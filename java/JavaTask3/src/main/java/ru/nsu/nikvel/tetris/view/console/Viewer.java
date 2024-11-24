package ru.nsu.nikvel.tetris.view.console;

import ru.nsu.nikvel.tetris.model.GameMode;
import ru.nsu.nikvel.tetris.model.ModelForViewer;
import ru.nsu.nikvel.tetris.model.Pair;
import ru.nsu.nikvel.tetris.view.ViewException;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Map;
import java.util.TreeMap;

class ComparatorForScoreboard implements Comparator<Pair<Integer, String>> {
    @Override
    public int compare(Pair<Integer, String> o1, Pair<Integer, String> o2) {
        return o2.key().compareTo(o1.key());
    }
}

public class Viewer {

    private final ModelForViewer model; // модель, на которую опирается наблюдатель
    // буферизированный вывод, чтобы быстро выводить текст на экран
    private final BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(System.out));

    private GameMode lastGameMode; // Режим игры который был 1 итерацию назад
    private final int[][] lastFieldCells; // Последнее нарисованное поле
    private final int[][] lastFigureCells; // Последняя нарисованная активная фигура
    // wasChanges := true, если актуальное состояние модели отличается от предыдущего
    // wasChanges := false, если актуальное состояние модели не отличается от предыдущего
    // если wasChanges == true, то модель стоит отрисовать
    private boolean wasChanges;

    /**
     * Создание наблюдателя (привязка наблюдатель к модели)
     **/
    public Viewer(ModelForViewer model) throws ViewException {
        if (model == null)
            throw new ViewException("The model passed to the parameters must not be null");

        this.model = model;
        this.lastFieldCells = new int[model.getGridWidth()][model.getGridHeight()];
        this.lastFigureCells = new int[4][2]; // 4 клетки по 2 координаты
        this.wasChanges = true;
    }

    /**
     * Отрисовать всё поле и активную фигуру
     **/
    public void draw() throws ViewException {
        try {
            // Проверка на смену игрового режима
            GameMode gameMode = model.getGameMode();
            if (gameMode != lastGameMode) {
                // отрисовка проигрышного поля, если режим сменился на GameMode.gameOver
                if (gameMode == GameMode.gameOver) {
                    updateChanges();
                    drawGameScreen();
                }

                switch (gameMode) {
                    case GameMode.mainMenu -> writer.write("<< Main Menu >>\n");
                    case GameMode.inProgress -> writer.write("<< Game >>\n");
                    case GameMode.gameOver -> writer.write("<< Game Over >>\n");
                }
                lastGameMode = gameMode;
            }

            // Отрисовка модели в зависимости от режима игры
            if (gameMode == GameMode.inProgress) {
                updateChanges();
                if (wasChanges)
                    drawGameScreen();
            }
            writer.flush();

        } catch (IOException e) {
            throw new ViewException("Writer exception");
        }
    }

    /**
     * Записывает в lastFieldCells и lastFigureCells актуальное состояние поля
     * и актуальное состояние активной фигуры соответственно.
     * Если новое состояние отличается от предыдущего (то есть хотя бы одна клетка поля изменилась,
     * либо подвинулась фигура), то возвращается true, иначе false.
     **/
    private void updateChanges() {
        wasChanges = false;
        for (int y = 0; y < model.getGridHeight(); ++y) {
            for (int x = 0; x < model.getGridWidth(); ++x) {
                if (lastFieldCells[x][y] != model.getCell(x, y)) {
                    lastFieldCells[x][y] = model.getCell(x, y);
                    wasChanges = true;
                }
            }
        }

        final int[][] currentFigureCells = model.getActiveFigureCells();
        for (int i = 0; i < 4; ++i) {
            if (lastFigureCells[i][0] != currentFigureCells[i][0]) {
                lastFigureCells[i][0] = currentFigureCells[i][0];
                wasChanges = true;
            }
            if (lastFigureCells[i][1] != currentFigureCells[i][1]) {
                lastFigureCells[i][1] = currentFigureCells[i][1];
                wasChanges = true;
            }
        }
    }

    /**
     * Возвращает true, если клетка с координатами (x, y) принадлежит активной фигуре
     **/
    private boolean isCellInFigure(int x, int y) {
        return lastFigureCells[0][0] == x && lastFigureCells[0][1] == y ||
                lastFigureCells[1][0] == x && lastFigureCells[1][1] == y ||
                lastFigureCells[2][0] == x && lastFigureCells[2][1] == y ||
                lastFigureCells[3][0] == x && lastFigureCells[3][1] == y;
    }

    /**
     * Отрисовка основного режима игры (собственно самого тетриса).
     * Информацию о клетках поля и фигуры берёт из lastFieldCells и lastFigureCells.
     **/
    private void drawGameScreen() throws IOException {
        writer.write("\nScore: " + model.getScore() + "\n");
        for (int y = 0; y < model.getGridHeight(); ++y) {
            for (int x = 0; x < model.getGridWidth(); ++x) {
                if (model.getCell(x, y) == 0) {
                    if (isCellInFigure(x, y))
                        writer.write("@");
                    else
                        writer.write(".");
                } else {
                    if (isCellInFigure(x, y))
                        writer.write("X");
                    else
                        writer.write("#");
                }
                writer.write(" ");
            }
            writer.write("\n");
        }
    }

    /**
     * Отрисовка таблицы рекордов
     **/
    public void drawScoreboard() {
        final ArrayList<Pair<Integer, String>> scoreboard = model.getScoreboard();
        scoreboard.sort(new ComparatorForScoreboard());

        try {
            writer.write("Рекорды: \n");
            writer.write("Очки:\tДата:\n");
            for (Pair<Integer, String> record : scoreboard) {
                writer.write(record.key() + "\t\t" + record.value() + "\n");
            }
            writer.flush();
        } catch (IOException e) {
            System.out.println("Writer error");
        }
    }

    /**
     * Просто выводит текст в консоль с переводом на следующую строку.
     **/
    public void printText(String text) {
        System.out.println(text);
    }
}
