package ru.nsu.nikvel.tetris.control.gui.scenes;

import java.net.URL;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.scene.text.Text;

public class AboutSceneController {

    @FXML
    private ResourceBundle resources;
    @FXML
    private URL location;
    @FXML
    private Text textPlate;

    public void start() {
        textPlate.setText("""              
                Элементы управления в игре:
                    "a" - подвинуть фигуру влево
                    "d" - подвинуть фигуру вправо
                    "q" - повернуть фигуру против часовой стрелки
                    "e" - повернуть фигуру по часовой стрелки
                    "s" - сбросить фигуру в самый низ
                    "ESCAPE" - завершить игру и перейти в главное меню""");
    }
}

