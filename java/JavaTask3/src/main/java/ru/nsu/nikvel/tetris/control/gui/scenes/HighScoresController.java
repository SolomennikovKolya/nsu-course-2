package ru.nsu.nikvel.tetris.control.gui.scenes;

import java.io.Serializable;
import java.net.URL;
import java.util.*;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import ru.nsu.nikvel.tetris.control.ControlException;
import ru.nsu.nikvel.tetris.model.ModelForViewer;
import ru.nsu.nikvel.tetris.model.Pair;

class ComparatorForScoreboard implements Comparator<Pair<Integer, String>> {
    @Override
    public int compare(Pair<Integer, String> o1, Pair<Integer, String> o2) {
        return o2.key().compareTo(o1.key());
    }
}

public class HighScoresController {

    private ModelForViewer model; // модель

    @FXML
    private ResourceBundle resources;
    @FXML
    private URL location;
    @FXML
    private Text textPlate;

    public void setModel(ModelForViewer model) {
        this.model = model;
    }

    public void start() throws ControlException {
        if (this.model == null)
            throw new ControlException("Model must not be null");

        final ArrayList<Pair<Integer, String>> scoreboard = model.getScoreboard();
        scoreboard.sort(new ComparatorForScoreboard());

        StringBuilder text = new StringBuilder();
        text.append("Очки:\tДата:\n");
        for (Pair<Integer, String> record : scoreboard) {
            text.append(record.key()).append("\t\t").append(record.value()).append("\n");
        }
        textPlate.setText(text.toString());
    }
}
