package ru.nsu.nikvel.tetris.view.console;

import ru.nsu.nikvel.tetris.control.ControlException;
import ru.nsu.nikvel.tetris.control.console.Controller;
import ru.nsu.nikvel.tetris.model.Model;
import ru.nsu.nikvel.tetris.model.ModelException;
import ru.nsu.nikvel.tetris.view.ViewException;

public class Main {
    public static void main(String[] args) {
        try {
            Model model = new Model();
            Viewer viewer = new Viewer(model);
            Controller controller = new Controller(model, viewer);

            while (controller.isActive()) {
                controller.update();
                viewer.draw();
            }

        } catch (ModelException | ViewException | ControlException e) {
            System.out.println(e.toString());
        }
    }
}
