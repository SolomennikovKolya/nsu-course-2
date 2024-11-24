package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.StackException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;

public class PopCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException {
        if (!commandArgs.isEmpty())
            throw new WrongArgsNumException("The \"POP\" command does not needs any argument");
        if (data.stack.empty())
            throw new StackException("To execute \"POP\" command, there must be at least 1 elements on the stack");

        data.stack.pop();
    }
}
