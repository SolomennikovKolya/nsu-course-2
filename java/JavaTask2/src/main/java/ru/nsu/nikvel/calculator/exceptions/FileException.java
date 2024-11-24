package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- FileException
public class FileException extends CalculatorException {
    public FileException() {
    }

    public FileException(String msg) {
        super(msg);
    }
}
