package ru.nsu.nikvel.tetris.control.console;

import ru.nsu.nikvel.tetris.control.ControlException;
import ru.nsu.nikvel.tetris.model.GameMode;
import ru.nsu.nikvel.tetris.model.ModelForController;
import ru.nsu.nikvel.tetris.view.console.Viewer;

public class Controller {

    private final ModelForController model; // Модель, на которую опирается контроллер
    private final Viewer viewer; // Иногда используется, чтобы вывести на экран информацию

    private boolean isActive; // Активен ли контроллер (он активен всегда, только если игра не закончена)
    private final CommandReader commandReader; // Класс для считывания команд из стандартного IO (отдельный поток)
    private final CommandLine commandLine; // Общий с CommandReader ресурс

    /**
     * Создание контроллера
     **/
    public Controller(ModelForController model, Viewer viewer) throws ControlException {
        if (model == null)
            throw new ControlException("The model passed to the parameters must not be null");
        if (viewer == null)
            throw new ControlException("The viewer passed to the parameters must not be null");

        this.model = model;
        this.viewer = viewer;
        this.isActive = true;
        this.commandLine = new CommandLine();
        this.commandReader = new CommandReader("CommandReaderThread", commandLine);
        commandReader.start(); // запуск потока
    }

    /**
     * Обновляет состояние контроллера, который в свою очередь может изменить состояние модели.
     * Если введена команда, то она исполняется.
     **/
    public void update() throws ControlException {
        if (!isActive)
            throw new ControlException("Attempt to update an inactive controller");

        GameMode gameMode = model.getGameMode();
        switch (gameMode) {
            case GameMode.mainMenu -> controlMainMenu();
            case GameMode.inProgress -> controlGameInProcess();
            case GameMode.gameOver -> controlGameOver();
        }
    }

    /**
     * Считывает и обрабатывает команды для режима главного меню (когда игра только что запущена)
     **/
    private void controlMainMenu() {
        final String command = commandLine.get();
        switch (command) {
            case "New Game" -> {
                model.startGame();
            }
            case "Exit" -> {
                viewer.printText("Нажмите enter чтобы выйти");
                model.endGame();
                commandReader.disable();
                isActive = false;
            }
            case "About" -> viewer.printText("""
                    Доступные команды:
                        "New Game" - запустить игру
                        "High Scores" - посмотреть таблицу рекордов
                        "About" - посмотреть информацию об игре
                        "Exit" - выйти из игры
                                        
                    Элементы управления в игре:
                        "a" - подвинуть фигуру влево
                        "d" - подвинуть фигуру вправо
                        "q" - повернуть фигуру против часовой стрелки
                        "e" - повернуть фигуру по часовой стрелки
                        "s" - сбросить фигуру в самый низ
                        "r" - завершить игру и перейти в главное меню""");

            case "High Scores" -> {
//                System.out.println("<< High Scores >>");
                viewer.drawScoreboard();
            }
            case null -> {
                // Ничего не делает, т.к. новая команда ещё не введена.
            }
            default -> viewer.printText("""
                    Неправильная команда
                    Напишите "About" чтобы посмотреть список доступных команд""");
        }
    }

    /**
     * Функциональность основного режима игры (собственно сама игра)
     **/
    private void controlGameInProcess() {
        final String command = commandLine.get();
        switch (command) {
            case "a" -> model.moveFigureLeft();
            case "s" -> model.moveFigureAllTheWayDown();
            case "d" -> model.moveFigureRight();
            case "q" -> model.rotateFigureLeft();
            case "e" -> model.rotateFigureRight();
            case "r" -> model.startMainMenu();
            case null -> {
                // Ничего не делает, т.к. новая команда ещё не введена.
            }
            default -> {
                // viewer.printText("Неправильная команда");
            }
        }
    }

    /**
     * Логика для состояния проигрыша.
     **/
    private void controlGameOver() {
        final String command = commandLine.get();
        switch (command) {
            case "Restart" -> model.startGame();
            case "Main Menu" -> model.startMainMenu();
            case null -> {
                // Ничего не делает, т.к. новая команда ещё не введена.
            }
            default -> viewer.printText("""
                    Неправильная команда
                    Доступные команды:
                        "Restart" - начать игру заново
                        "Main Menu" - выйти в главное меню""");
        }
    }

    /**
     * Активен ли сейчас контроллер (он может быть не активен только, если пользователь ввёл команду выхода из игры)
     **/
    public boolean isActive() {
        return isActive;
    }
}
