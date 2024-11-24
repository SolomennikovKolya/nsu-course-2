package ru.nsu.nikvel.tetris.view;

public class ViewException extends Exception {
    public ViewException() {
    }

    public ViewException(String msg) {
        super(msg);
    }

    @Override
    public String toString() {
        String s = this.getClass().getSimpleName();
        String message = getLocalizedMessage();
        return (message != null) ? (s + ": " + message) : s;
    }

}
