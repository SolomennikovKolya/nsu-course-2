package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.StackException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;

public class PrintCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException {
        if (!commandArgs.isEmpty())
            throw new WrongArgsNumException("The \"PRINT\" command does not needs any argument");
        if (data.stack.empty())
            throw new StackException("To execute \"PRINT\" command, there must be at least 1 elements on the stack");

        System.out.println(data.stack.peek());
    }
}
