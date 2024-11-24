package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.ArithmeticException;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.StackException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;

import static java.lang.Math.sqrt;

public class SqrtCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException {
        if (!commandArgs.isEmpty())
            throw new WrongArgsNumException("The \"SQRT\" command does not needs any argument");
        if (data.stack.isEmpty())
            throw new StackException("To execute \"SQRT\" command, there must be at least 1 elements on the stack");

        final double n = data.stack.pop();
        if (n < 0)
            throw new ArithmeticException("The number under the square root must not be less than zero");
        data.stack.push(sqrt(n));
    }
}