package ru.nsu.nikvel.calculator.exceptions;

// CalculatorException <- CommandException <- NotEnoughArgsException
public class WrongArgsNumException extends CommandException {
    public WrongArgsNumException() {
    }

    public WrongArgsNumException(String msg) {
        super(msg);
    }
}
