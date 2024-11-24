package ru.nsu.nikvel.tetris.control;

public class ControlException extends Exception {
    public ControlException() {
    }

    public ControlException(String msg) {
        super(msg);
    }

    @Override
    public String toString() {
        String s = this.getClass().getSimpleName();
        String message = getLocalizedMessage();
        return (message != null) ? (s + ": " + message) : s;
    }
}
