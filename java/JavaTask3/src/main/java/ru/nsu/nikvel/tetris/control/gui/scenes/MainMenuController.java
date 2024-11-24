package ru.nsu.nikvel.tetris.control.gui.scenes;

import java.io.IOException;
import java.net.URL;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.fxml.FXMLLoader;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import javafx.scene.Parent;
import javafx.scene.Scene;
import ru.nsu.nikvel.tetris.control.ControlException;
import ru.nsu.nikvel.tetris.model.Model;

public class MainMenuController {

    private Model model; // модель

    @FXML
    private ResourceBundle resources;

    @FXML
    private URL location;

    @FXML
    private Button buttonNewGame, buttonHighScores, buttonAbout, buttonExit;

    /**
     * Привязка модели к контроллеру сцены
     **/
    public void setModel(Model model) {
        this.model = model;
    }

    /**
     * Переход в режим игры inProcess (в том же окне)
     **/
    @FXML
    void clickButtonNewGame(ActionEvent event) throws ControlException {
        try {
            // Загрузка новой сцены
            final String path = "/ru/nsu/nikvel/tetris/view/gui/scenes/inProgressScene.fxml";
            FXMLLoader loader = new FXMLLoader(getClass().getResource(path));
            if (loader.getLocation() == null)
                throw new ControlException("FXML resource not found or path is incorrect");
            Scene scene = new Scene(loader.load());

            // Настройка окна
            Stage stage = (Stage) buttonNewGame.getScene().getWindow();
            stage.setScene(scene);
            stage.centerOnScreen();
            stage.setTitle("Tetris - Game");

            // Настройка контроллера для новой сцены
            InProgressController controller = loader.getController();
            controller.setModel(model);
            controller.start();

        } catch (IOException e) {
            throw new ControlException("Failed to load FXML file");
        }
    }

    /**
     * Открытие окна с таблицей рекордов
     **/
    @FXML
    void clickButtonHighScores(ActionEvent event) throws ControlException {
        try {
            // Загрузка новой сцены
            String path = "/ru/nsu/nikvel/tetris/view/gui/scenes/highScoresScene.fxml";
            FXMLLoader loader = new FXMLLoader(getClass().getResource(path));
            if (loader.getLocation() == null)
                throw new ControlException("FXML resource not found or path is incorrect");
            Scene scene = new Scene(loader.load());

            // Настройка окна
            Stage stage = new Stage(); // создание нового окна
            stage.setScene(scene); // загрузка сцены в новое окно
            stage.setTitle("Tetris - High Scores");
            stage.getIcons().add(new Image(Objects.requireNonNull(getClass().getResourceAsStream("/ru/nsu/nikvel/tetris/spr/icon.png"))));
            stage.setResizable(false);
            stage.show();

            // Настройка контроллера для новой сцены
            HighScoresController controller = loader.getController();
            controller.setModel(model);
            controller.start();

        } catch (IOException e) {
            throw new ControlException("Failed to load FXML file.");
        }
    }

    /**
     * Открытие окна с информацией об игре
     **/
    @FXML
    void clickButtonAbout(ActionEvent event) throws ControlException {
        try {
            // Загрузка новой сцены
            String path = "/ru/nsu/nikvel/tetris/view/gui/scenes/aboutScene.fxml";
            FXMLLoader loader = new FXMLLoader(getClass().getResource(path));
            if (loader.getLocation() == null)
                throw new ControlException("FXML resource not found or path is incorrect");
            Scene scene = new Scene(loader.load());

            // Настройка окна
            Stage stage = new Stage(); // создание нового окна
            stage.setScene(scene); // загрузка сцены в новое окно
            stage.setTitle("Tetris - About");
            stage.getIcons().add(new Image(Objects.requireNonNull(getClass().getResourceAsStream("/ru/nsu/nikvel/tetris/spr/icon.png"))));
            stage.setResizable(false);
            stage.show();

            // Настройка контроллера для новой сцены
            AboutSceneController controller = loader.getController();
            controller.start();

        } catch (IOException e) {
            throw new ControlException("Failed to load FXML file.");
        }
    }

    /**
     * Выход из игры
     **/
    @FXML
    void clickButtonExit(ActionEvent event) {
        System.exit(0);
    }
}
