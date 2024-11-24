package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- CommandException
public class CommandException extends CalculatorException {
    public CommandException() {
    }

    public CommandException(String msg) {
        super(msg);
    }
}
