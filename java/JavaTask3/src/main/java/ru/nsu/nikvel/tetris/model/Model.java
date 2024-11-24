package ru.nsu.nikvel.tetris.model;

import javafx.application.Platform;
import ru.nsu.nikvel.tetris.control.gui.UpdatesListener;
import ru.nsu.nikvel.tetris.model.figures.Figure;
import ru.nsu.nikvel.tetris.model.figures.FigureFactory;

import java.io.*;
import java.util.*;

enum CollisionType {
    noCollision,
    collisionWithLeftWall,
    collisionWithRightWall,
    collisionWithFloor,
    collisionWithCells,
}

public class Model implements ModelForController, ModelForViewer {

    private GameMode gameMode; // игровой режим

    private final Grid grid; // сетка клеток
    private static final int defaultGridWidth = 10; // ширина поля по умолчанию
    private static final int defaultGridHeight = 20; // высота поля по умолчанию

    private Figure activeFigure = null; // активная фигура (фигура, которой можно управлять в данный момент)
    private final static FigureFactory figureFactory = new FigureFactory(); // фабрика создания фигур
    private final static Random figureRandomizer = new Random(); // генератор случайных чисел

    private int gamePeriod; // количество периодов с начала запуска игры
    private final int periodDT = 1000; // длительность одного периода игры
    private UpdatesListener updatesListener; // слушатель обновлений таймера или движений фигуры
    private final Timer gameTimer = new Timer(); // таймер для отсчёта периодов
    private final TimerTask fallDownTask = new TimerTask() {
        // действие, которое будет выполняться каждый период (сдвиг фигуры вниз)
        public void run() {
            if (Platform.isFxApplicationThread()) {
                Platform.runLater(() -> {
                    if (gameMode == GameMode.inProgress) {
                        moveFigureDown();
                        gamePeriod++;
                        if (updatesListener != null)
                            updatesListener.handleUpdate();
                    }
                });
            } else {
                if (gameMode == GameMode.inProgress) {
                    moveFigureDown();
                    gamePeriod++;
                    if (updatesListener != null)
                        updatesListener.handleUpdate();
                }
            }
        }
    };

    private int score; // количество очков (количество удалённых клеток в ходе стирания полных линий)
    private ArrayList<Pair<Integer, String>> scoreboard; // таблица рекордов

    /**
     * Создаёт модель с размером поля gridWidth на gridHeight.
     **/
    public Model(int gridWidth, int gridHeight) throws ModelException {
        if (gridWidth < 4 || gridHeight < 4)
            throw new ModelException("The grid size is too small");
        this.grid = new Grid(gridWidth, gridHeight);
        this.gameMode = GameMode.mainMenu;
        gameTimer.scheduleAtFixedRate(fallDownTask, 0, periodDT);
        readHighScores();
    }

    /**
     * Создаёт модель с размером поля по умолчанию.
     **/
    public Model() throws ModelException {
        this(defaultGridWidth, defaultGridHeight);
    }

    /**
     * Задать слушателя событий
     **/
    public void setUpdatesListener(UpdatesListener updatesListener) {
        this.updatesListener = updatesListener;
    }

    /**
     * Переход в главное меню.
     **/
    @Override
    public void startMainMenu() {
        gameMode = GameMode.mainMenu;
    }

    /**
     * Запустить или перезапустить игру.
     **/
    @Override
    public void startGame() {
        gameMode = GameMode.inProgress;
        score = 0;
        grid.clear();
        spawnNewFigure();
        gamePeriod = 0;
    }

    /**
     * Завершить игру (насовсем выйти из приложения)
     **/
    @Override
    public void endGame() {
        gameMode = GameMode.ended;
        fallDownTask.cancel();
        gameTimer.cancel();
    }

    /**
     * Создаёт случайную новую фигуру сверху поля. Если новая фигура пересекается с клетками поля, то
     * игра заканчивается (проигрыш). При этом очки записываются в таблицу рекордов.
     **/
    private void spawnNewFigure() {
        final int figureNum = figureRandomizer.nextInt(7);
        this.activeFigure = figureFactory.getFigure(figureNum, grid.getWidth() / 2 - 2, 0);
        if (checkCollision() != CollisionType.noCollision) {
            // проигрыш (фигуры достигли верхней границы поля)
            gameMode = GameMode.gameOver;
            writeHighScores();
        }
    }

    /**
     * Добавляет клетки активной фигуры на поле.
     **/
    private void placeActiveFigure() {
        final int[][] coordsOfCells = activeFigure.getCoordsOfCells();
        for (int i = 0; i < 4; ++i) {
            grid.set(coordsOfCells[i][0], coordsOfCells[i][1], 1);
        }
        deleteFullLines();
    }

    /**
     * Десериализация таблицы рекордов
     **/
    @SuppressWarnings("unchecked")
    private void readHighScores() {
        String filename = "src/main/resources/ru/nsu/nikvel/tetris/highScores.txt";
        try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(filename))) {
            Object obj = in.readObject();
            scoreboard = (ArrayList<Pair<Integer, String>>) (ArrayList<?>) obj;
            return;
        } catch (IOException | ClassNotFoundException e) {
            System.out.println("Failed to deserialize HighScores");
        }
        // если не получилось получить scoreboard нормальным путём
        scoreboard = new ArrayList<>();
    }

    /**
     * Запись и сериализация таблицы рекордов
     **/
    private void writeHighScores() {
        scoreboard.add(new Pair<>(score, (new Date()).toString()));
        final String filename = "src/main/resources/ru/nsu/nikvel/tetris/highScores.txt";
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(filename))) {
            oos.writeObject(scoreboard);
        } catch (InvalidClassException e) {
            System.out.println("InvalidClassException");
        } catch (NotSerializableException e) {
            System.out.println("NotSerializableException");
        } catch (IOException e) {
            System.out.println("Failed to serialize HighScores");
        }
    }

    /**
     * Сдвигает все клетки поля, которые выше строки с номером lineY на единицу вниз.
     * Сама строка lineY при этом удалиться.
     **/
    private void moveTopOfTheGridDown(int lineY) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            for (int y = lineY; y > 0; --y) {
                grid.set(x, y, grid.get(x, y - 1));
            }
        }
    }

    /**
     * Если на поле есть полностью заполненные горизонтальные линии, то удаляет их и
     * сдвигает верхнюю часть поля вниз. При удалении линии добавляются очки.
     **/
    private void deleteFullLines() {
        for (int y = 0; y < grid.getHeight(); ++y) {
            int isFull = 1;
            for (int x = 0; x < grid.getWidth(); ++x) {
                if (grid.get(x, y) == 0) {
                    isFull = 0;
                    break;
                }
            }
            if (isFull == 1) {
                moveTopOfTheGridDown(y);
                score += grid.getWidth();
            }
        }
    }

    /**
     * Проверяет коллизии активной фигуры с краями поля и другими клетками поля.
     * Сначала проверяется коллизия со стенами, а потом с занятыми клетками поля.
     **/
    private CollisionType checkCollision() {
        final int[][] coordsOfCells = activeFigure.getCoordsOfCells();
        for (int i = 0; i < 4; ++i) {
            final int x = coordsOfCells[i][0];
            final int y = coordsOfCells[i][1];
            if (x < 0)
                return CollisionType.collisionWithLeftWall;
            else if (x >= grid.getWidth())
                return CollisionType.collisionWithRightWall;
            else if (y >= grid.getHeight())
                return CollisionType.collisionWithFloor;

        }
        for (int i = 0; i < 4; ++i) {
            final int x = coordsOfCells[i][0];
            final int y = coordsOfCells[i][1];
            if (grid.get(x, y) != 0)
                return CollisionType.collisionWithCells;
        }
        return CollisionType.noCollision;
    }

    /**
     * Сдвинуть фигуру налево, если возможно.
     **/
    @Override
    public void moveFigureLeft() {
        activeFigure.shift(-1, 0);
        if (checkCollision() != CollisionType.noCollision)
            activeFigure.shift(1, 0);
        if (updatesListener != null)
            updatesListener.handleUpdate();
    }

    /**
     * Сдвинуть фигуру направо, если возможно.
     **/
    @Override
    public void moveFigureRight() {
        activeFigure.shift(1, 0);
        if (checkCollision() != CollisionType.noCollision)
            activeFigure.shift(-1, 0);
        if (updatesListener != null)
            updatesListener.handleUpdate();
    }

    /**
     * Передвинуть фигуру вниз. Если фигура находилась вплотную к нижней границы поля или
     * к заполненным клеткам поля, то появляется новая фигура, а предыдущая отпечатывается на поле.
     **/
    public void moveFigureDown() {
        activeFigure.shift(0, 1);
        if (checkCollision() != CollisionType.noCollision) {
            activeFigure.shift(0, -1);
            placeActiveFigure();
            spawnNewFigure();
        }
        if (updatesListener != null)
            updatesListener.handleUpdate();
    }

    /**
     * Опускает фигуру до самого низа, а затем генерирует новую фигуру
     **/
    @Override
    public void moveFigureAllTheWayDown() {
        CollisionType collision = checkCollision();
        while (collision == CollisionType.noCollision) {
            activeFigure.shift(0, 1);
            collision = checkCollision();
        }
        activeFigure.shift(0, -1);
        placeActiveFigure();
        spawnNewFigure();
        if (updatesListener != null)
            updatesListener.handleUpdate();
    }

    /**
     * Пытается разрешить коллизию после поворота. Если разрешить получилось, то возвращает true,
     * иначе возвращает false.
     **/
    private boolean resolveCollisionAfterRotation() {
        final CollisionType collision = checkCollision();
        if (collision == CollisionType.collisionWithLeftWall) {
            activeFigure.shift(1, 0);
            if (checkCollision() != CollisionType.noCollision) {
                activeFigure.shift(1, 0);
                if (checkCollision() != CollisionType.noCollision) {
                    // не получилось разрешить коллизию с левой стеной
                    activeFigure.shift(-2, 0);
                    return false;
                }
            }
        } else if (collision == CollisionType.collisionWithRightWall) {
            activeFigure.shift(-1, 0);
            if (checkCollision() != CollisionType.noCollision) {
                activeFigure.shift(-1, 0);
                if (checkCollision() != CollisionType.noCollision) {
                    // не получилось разрешить коллизию с правой стеной
                    activeFigure.shift(2, 0);
                    return false;
                }
            }
        } else if (collision != CollisionType.noCollision) {
            // сложный случай коллизии
            return false;
        }
        return true;
    }

    /**
     * Повернуть фигуру против часовой стрелки, если это возможно.
     * Если фигура находилась возле боковой стенки поля, то фигура поворачивается и сдвигается в бок так,
     * чтобы не было коллизий. Если поворот повлечёт за собой коллизию с другими клетками поля,
     * то поворот не делается.
     **/
    @Override
    public void rotateFigureLeft() {
        activeFigure.rotateLeft();
        if (!resolveCollisionAfterRotation())
            activeFigure.rotateRight();
        if (updatesListener != null)
            updatesListener.handleUpdate();
    }

    /**
     * Повернуть фигуру по часовой стрелки, если это возможно.
     * Если фигура находилась возле боковой стенки поля, то фигура поворачивается и сдвигается в бок так,
     * чтобы не было коллизий. Если поворот повлечёт за собой коллизию с другими клетками поля,
     * то поворот не делается.
     **/
    @Override
    public void rotateFigureRight() {
        activeFigure.rotateRight();
        if (!resolveCollisionAfterRotation())
            activeFigure.rotateLeft();
        if (updatesListener != null)
            updatesListener.handleUpdate();
    }

    /**
     * Получить режим игры
     **/
    @Override
    public GameMode getGameMode() {
        return gameMode;
    }

    /**
     * Получить ширину поля.
     **/
    @Override
    public int getGridWidth() {
        return grid.getWidth();
    }

    /**
     * Получить высоту поля.
     **/
    @Override
    public int getGridHeight() {
        return grid.getHeight();
    }

    /**
     * Получить значение клетки поля с координатами (x, y).
     **/
    @Override
    public int getCell(int x, int y) {
        return grid.get(x, y);
    }

    /**
     * Получить координаты клеток активной фигуры.
     **/
    @Override
    public int[][] getActiveFigureCells() {
        return activeFigure.getCoordsOfCells();
    }

    /**
     * Получение игрового периода
     **/
    @Override
    public int getGamePeriod() {
        return gamePeriod;
    }

    /**
     * Текущее количество очков
     **/
    @Override
    public int getScore() {
        return score;
    }

    /**
     * Получить таблицу очков
     **/
    @Override
    public ArrayList<Pair<Integer, String>> getScoreboard() {
        return scoreboard;
    }
}
