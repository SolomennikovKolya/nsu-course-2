package ru.nsu.nikvel.tetris.view.gui;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import ru.nsu.nikvel.tetris.control.gui.scenes.MainMenuController;
import ru.nsu.nikvel.tetris.model.Model;

import java.io.IOException;
import java.util.Objects;

public class Main extends Application {
    @Override
    public void start(Stage stage) throws Exception {
        try {
            // Создание модели
            Model model = new Model();

            // Загрузка новой сцены
            FXMLLoader loader = new FXMLLoader(Main.class.getResource("scenes/mainMenuScene.fxml"));
            Scene mainMenuScene = new Scene(loader.load());

            // Настройка окна
            stage.setScene(mainMenuScene);
            stage.setTitle("Tetris - Main Menu");
            stage.getIcons().add(new Image(Objects.requireNonNull(getClass().getResourceAsStream("/ru/nsu/nikvel/tetris/spr/icon.png"))));
            stage.setResizable(false);
            stage.centerOnScreen();
            stage.show();

            // Привязка модели к контроллеру сцены
            MainMenuController controller = loader.getController();
            controller.setModel(model);

        } catch (IOException e) {
            System.out.println(e.toString());
        }
    }

    public static void main(String[] args) {
        Application.launch(args);
    }
}
