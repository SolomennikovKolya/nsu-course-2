package ru.nsu.nikvel.calculator.exceptions;

public class CalculatorException extends Exception {
    public CalculatorException() {
    }

    public CalculatorException(String msg) {
        super(msg);
    }

    @Override
    public String toString() {
        String s = this.getClass().getSimpleName();
        String message = getLocalizedMessage();
        return (message != null) ? (s + ": " + message) : s;
    }
}

