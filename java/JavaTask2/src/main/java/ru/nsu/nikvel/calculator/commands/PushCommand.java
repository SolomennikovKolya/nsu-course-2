package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CastException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;
import ru.nsu.nikvel.calculator.exceptions.CommandException;

import java.util.ArrayList;

public class PushCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException {
        if (commandArgs.size() != 1)
            throw new WrongArgsNumException("The \"PUSH\" command needs exactly 1 argument");

        try {
            data.stack.push(Double.parseDouble(commandArgs.get(0)));
        } catch (NumberFormatException e) {
            final Double n = data.vars.get(commandArgs.get(0));
            if (n == null)
                throw new CastException("The parameter for the command should be exactly double or a variable name");
            data.stack.push(n);
        }
    }
}
