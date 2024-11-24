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

public class DefineCommandTest {
    private final CalculatorData data = new CalculatorData();
    private final Command command = new DefineCommand();

    @Before
    public void setUp() {
        data.stack.clear();
        data.vars.clear();
    }

    @Test
    public void executeTest() {
        try {
            command.execute(data, new ArrayList<String>(List.of("qwe", "123")));
            assertEquals(data.vars.get("qwe"), new Double(123));

            command.execute(data, new ArrayList<String>(List.of("qwe", "456")));
            assertEquals(data.vars.get("qwe"), new Double(456));

            command.execute(data, new ArrayList<String>(List.of("lol", "789")));
            assertEquals(data.vars.get("lol").doubleValue(), 789);
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
            command.execute(data, new ArrayList<String>(List.of("123")));
        });

        assertThrows(WrongArgsNumException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("123", "123", "123")));
        });

        assertThrows(CastException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("123", "123")));
        });

        assertThrows(CastException.class, () -> {
            command.execute(data, new ArrayList<String>(List.of("qwe", "qwe")));
        });
    }
}


