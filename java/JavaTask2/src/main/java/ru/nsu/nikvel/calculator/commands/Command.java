package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CommandException;

import java.util.ArrayList;

public interface Command {
    public void execute(CalculatorData data, ArrayList<String> commandArgs) throws CommandException;
}
