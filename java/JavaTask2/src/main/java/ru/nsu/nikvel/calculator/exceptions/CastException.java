package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- CommandException <- CastException
public class CastException extends CommandException {
    public CastException() {
    }

    public CastException(String msg) {
        super(msg);
    }
}
