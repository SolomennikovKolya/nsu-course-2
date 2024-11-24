package ru.nsu.nikvel.tetris.model;

public class ModelException extends Exception {
    public ModelException() {
    }

    public ModelException(String msg) {
        super(msg);
    }

    @Override
    public String toString() {
        String s = this.getClass().getSimpleName();
        String message = getLocalizedMessage();
        return (message != null) ? (s + ": " + message) : s;
    }
}
