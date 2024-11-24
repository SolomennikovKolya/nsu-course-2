package ru.nsu.nikvel.calculator.commands;

import org.junit.Before;
import org.junit.Test;
import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.StackException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertThrows;
import static org.junit.Assert.fail;

public class PopCommandTest {
    private final CalculatorData data = new CalculatorData();
    private final Command command = new PopCommand();

    @Before
    public void setUp() {
        data.stack.clear();
        data.vars.clear();
    }

    @Test
    public void executeTest() {
        try {
            data.stack.push(10.0);
            command.execute(data, new ArrayList<String>(List.of()));

            data.stack.push(20.0);
            command.execute(data, new ArrayList<String>(List.of()));

            data.stack.push(30.0);
            data.stack.push(40.0);
            command.execute(data, new ArrayList<String>(List.of()));
            command.execute(data, new ArrayList<String>(List.of()));
        } catch (CommandException e) {
            fail();
        }
    }

    @Test
    public void exceptionsTest() {
        assertThrows(WrongArgsNumException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("10")));
        });

        assertThrows(StackException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of()));
        });
    }
}