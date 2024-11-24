package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.ArithmeticException;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.StackException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;

public class DivideCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException {
        if (!commandArgs.isEmpty())
            throw new WrongArgsNumException("The \"/\" command does not needs any argument");
        if (data.stack.size() < 2)
            throw new StackException("To execute \"/\" command, there must be at least 2 elements on the stack");

        final double n1 = data.stack.pop();
        final double n2 = data.stack.pop();
        if (n1 == 0)
            throw new ArithmeticException("The divisor must not be zero");
        data.stack.push(n2 / n1);
    }
}