package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CastException;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;

public class DefineCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException {
        if (commandArgs.size() != 2)
            throw new WrongArgsNumException("The \"DEFINE\" command needs exactly 2 arguments (var name and var value)");

        try {
            final double n = Double.parseDouble(commandArgs.get(0));
            throw new CastException("You cannot define number to a number");
        } catch (NumberFormatException e) {
            try {
                data.vars.put(commandArgs.get(0), Double.parseDouble(commandArgs.get(1)));
            } catch (NumberFormatException e1) {
                throw new CastException("The second parameter for the command should be exactly double");
            }
        }
    }
}