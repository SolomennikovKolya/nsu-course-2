package ru.nsu.nikvel.calculator.commands;

import org.junit.Before;
import org.junit.Test;
import ru.nsu.nikvel.calculator.CalculatorData;
import ru.nsu.nikvel.calculator.exceptions.CastException;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.exceptions.WrongArgsNumException;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.*;

public class PushCommandTest {
    private final CalculatorData data = new CalculatorData();
    private final Command command = new PushCommand();

    @Before
    public void setUp() {
        data.stack.clear();
        data.vars.clear();
    }

    @Test
    public void executeTest() {
        try {
            command.execute(data, new ArrayList<String>(List.of("10")));
            assertEquals(new Double(10), data.stack.pop());

            command.execute(data, new ArrayList<String>(List.of("20")));
            assertEquals(new Double(20), data.stack.pop());

            data.vars.put("qwe", 30.0);
            command.execute(data, new ArrayList<String>(List.of("qwe")));
            assertEquals(new Double(30), data.stack.pop());
        } catch (CommandException e) {
            fail();
        }
    }

    @Test
    public void exceptionsTest() {
        assertThrows(WrongArgsNumException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of()));
        });

        assertThrows(WrongArgsNumException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("10", "20")));
        });

        assertThrows(CastException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("qwe")));
        });
    }
}