package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.CalculatorData;

import java.util.ArrayList;

public class HelpCommand implements Command {
    @Override
    public void execute(CalculatorData data, ArrayList<String> commandArgs) {
        System.out.println("""
                Here are possible commands for calculator:
                # - line with a comment.
                POP - remove the number from the stack.
                PUSH - put the number on the stack.
                +, -, *, /, SQRT - arithmetic operations. Uses one or two top elements of the stack, remove them from the stack, placing the result back.
                PRINT - prints the top element of the stack (without deleting it).
                DEFINE - set the value of the parameter. In the future, use this value instead of the parameter everywhere.
                """);
    }
}
