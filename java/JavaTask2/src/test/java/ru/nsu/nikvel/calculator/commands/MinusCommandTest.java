package ru.nsu.nikvel.calculator.commands;

import org.junit.Before;
import org.junit.Test;
import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.StackException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.*;

public class MinusCommandTest {
    private final CalculatorData data = new CalculatorData();
    private final Command command = new MinusCommand();

    @Before
    public void setUp() {
        data.stack.clear();
        data.vars.clear();
    }

    @Test
    public void executeTest() {
        try {
            data.stack.push(10.0);
            data.stack.push(20.0);
            command.execute(data, new ArrayList<String>(List.of()));
            assertEquals(data.stack.peek(), new Double(-10));

            data.stack.push(-20.0);
            command.execute(data, new ArrayList<String>(List.of()));
            assertEquals(data.stack.peek(), new Double(10));
        } catch (CommandException e) {
            fail();
        }
    }

    @Test
    public void exceptionsTest() {
        assertThrows(WrongArgsNumException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("123")));
        });

        assertThrows(StackException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of()));
        });

        data.stack.push(10.0);
        assertThrows(StackException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of()));
        });
    }
}