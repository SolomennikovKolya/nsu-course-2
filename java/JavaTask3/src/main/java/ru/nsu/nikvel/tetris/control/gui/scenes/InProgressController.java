package ru.nsu.nikvel.tetris.control.gui.scenes;

import java.io.IOException;
import java.net.URL;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.event.ActionEvent;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.fxml.FXML;
import javafx.scene.layout.RowConstraints;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.stage.Stage;
import ru.nsu.nikvel.tetris.control.ControlException;
import ru.nsu.nikvel.tetris.control.gui.UpdatesListener;
import ru.nsu.nikvel.tetris.model.GameMode;
import ru.nsu.nikvel.tetris.model.Model;

public class InProgressController implements UpdatesListener {

    private Model model; // модель

    @FXML
    private ResourceBundle resources;
    @FXML
    private URL location;
    @FXML
    private GridPane gridPane;
    @FXML
    private VBox LoseBox;
    @FXML
    private Button buttonMainMenu, buttonRestart, buttonLose;

    /**
     * Привязка модели к контроллеру сцены
     **/
    public void setModel(Model model) {
        this.model = model;
    }

    /**
     * Инициализация контроллера, если он ни разу не создавался
     **/
    public void start() throws ControlException {
        if (this.model == null)
            throw new ControlException("Model must not be null");

        // Создание прямоугольников внутри GridPane
        int cellSize = 40;
        for (int y = 0; y < model.getGridHeight(); y++) {
            for (int x = 0; x < model.getGridWidth(); x++) {
                Rectangle cell = new Rectangle(cellSize, cellSize);
                if ((y + x) % 2 == 0)
                    cell.setFill(new Color(0.13, 0.13, 0.13, 1));
                else
                    cell.setFill(new Color(0.11, 0.11, 0.11, 1));
                cell.setStroke(Color.BLACK);

                gridPane.add(cell, x, y);
            }
        }

        model.startGame(); // Запуск игры
        model.setUpdatesListener(this); // Привязка слушателя для модели

        // Привязка обработчика нажатий клавиш на клавиатуре
        Scene scene = gridPane.getScene();
        scene.setOnKeyPressed(this::handleKeyPress);

        // LosePane должен появляться только при проигрыше
        LoseBox.setVisible(false);

        renderModel();
    }

    /**
     * Возвращает true, если клетка с координатами (x, y) принадлежит фигуре
     **/
    private boolean isCellInFigure(int[][] figure, int x, int y) {
        return figure[0][0] == x && figure[0][1] == y ||
                figure[1][0] == x && figure[1][1] == y ||
                figure[2][0] == x && figure[2][1] == y ||
                figure[3][0] == x && figure[3][1] == y;
    }

    /**
     * Рендер сцены
     **/
    private void renderModel() {
        final int[][] figure = model.getActiveFigureCells();

        for (int y = 0; y < model.getGridHeight(); y++) {
            for (int x = 0; x < model.getGridWidth(); x++) {
                Rectangle cell = (Rectangle) gridPane.getChildren().get(y * gridPane.getColumnCount() + x);

                if (model.getCell(x, y) == 0) {
                    if (isCellInFigure(figure, x, y))
                        cell.setFill(new Color(0.28, 0.28, 0.7, 1));
                    else if ((y + x) % 2 == 0)
                        cell.setFill(new Color(0.13, 0.13, 0.13, 1));
                    else
                        cell.setFill(new Color(0.11, 0.11, 0.11, 1));
                } else {
                    if (isCellInFigure(figure, x, y))
                        cell.setFill(new Color(0.75, 0.22, 0.22, 1));
                    else
                        cell.setFill(new Color(0.18, 0.18, 0.6, 1));
                }
            }
        }
    }

    /**
     * Обновление сцены при обновлении модели
     **/
    @Override
    public void handleUpdate() {
        renderModel();
        // Условие появление проигрышной панели
        if (model.getGameMode() == GameMode.gameOver) {
            LoseBox.setVisible(true);
            buttonLose.setText("Score: " + model.getScore());
        }
    }

    /**
     * Переход в главное меню (в том же окне)
     **/
    private void startMainMenuScene() {
        try {
            // Загрузка новой сцены
            final String pathToScene = "/ru/nsu/nikvel/tetris/view/gui/scenes/mainMenuScene.fxml";
            FXMLLoader loader = new FXMLLoader(getClass().getResource(pathToScene));
            if (loader.getLocation() == null)
                System.out.println("FXML resource not found or path is incorrect");
            Scene newScene = new Scene(loader.load());

            // Настройка окна
            Stage stage = (Stage) gridPane.getScene().getWindow();
            stage.setScene(newScene);
            stage.centerOnScreen();
            stage.setTitle("Tetris - Main Menu");

            // Настройка контроллера для новой сцены
            MainMenuController controller = loader.getController();
            controller.setModel(model);

            // Переход в новый режим игры
            model.startMainMenu();

        } catch (IOException e) {
            System.out.println("Failed to load FXML file.");
        }
    }

    /**
     * Метод-обработчик нажатия клавиш
     **/
    private void handleKeyPress(KeyEvent event) {
        if (model.getGameMode() != GameMode.inProgress)
            return;
        switch (event.getCode()) {
            case KeyCode.A -> model.moveFigureLeft();
            case KeyCode.D -> model.moveFigureRight();
            case KeyCode.S -> model.moveFigureAllTheWayDown();
            case KeyCode.Q -> model.rotateFigureLeft();
            case KeyCode.E -> model.rotateFigureRight();
            case KeyCode.ESCAPE -> startMainMenuScene();
        }
    }

    @FXML
    void clickButtonMainMenu(ActionEvent event) {
        startMainMenuScene();
        LoseBox.setVisible(false);
    }

    @FXML
    void clickButtonRestart(ActionEvent event) {
        model.startGame();
        renderModel();
        LoseBox.setVisible(false);
    }
}
