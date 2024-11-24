package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- FileException <- ConfigException
public class ConfigException extends FileException {
    public ConfigException() {
    }

    public ConfigException(String msg) {
        super(msg);
    }
}
