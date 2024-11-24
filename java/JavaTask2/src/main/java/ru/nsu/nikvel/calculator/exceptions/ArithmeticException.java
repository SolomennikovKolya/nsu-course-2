package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- CommandException <- ArithmeticException
public class ArithmeticException extends CommandException {
    public ArithmeticException() {
    }

    public ArithmeticException(String msg) {
        super(msg);
    }
}
