module ru.nsu.nikvel.tetris {
    requires javafx.controls;
    requires javafx.fxml;

    exports ru.nsu.nikvel.tetris.view;
    exports ru.nsu.nikvel.tetris.view.gui;
    exports ru.nsu.nikvel.tetris.model;
    exports ru.nsu.nikvel.tetris.control.gui;
    opens ru.nsu.nikvel.tetris.control.gui.scenes to javafx.fxml;
}