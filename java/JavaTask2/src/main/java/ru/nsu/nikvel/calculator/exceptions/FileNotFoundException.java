package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- FileException <- FileNotFoundException
public class FileNotFoundException extends FileException {
    public FileNotFoundException() {
    }

    public FileNotFoundException(String msg) {
        super(msg);
    }
}
