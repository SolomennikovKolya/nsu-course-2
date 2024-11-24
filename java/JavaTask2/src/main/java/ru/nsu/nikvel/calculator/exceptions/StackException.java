package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- CommandException <- StackException
public class StackException extends CommandException {
    public StackException() {
    }

    public StackException(String msg) {
        super(msg);
    }
}
