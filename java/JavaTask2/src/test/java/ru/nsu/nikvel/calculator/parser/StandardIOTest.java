package ru.nsu.nikvel.calculator.parser;

import ru.nsu.nikvel.calculator.Calculator;
import org.junit.*;
import static org.junit.Assert.*;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.io.ByteArrayInputStream;
import java.io.InputStream;

public class StandardIOTest {
    private final PrintStream standardOut = System.out;
    private final ByteArrayOutputStream outputStreamCaptor = new ByteArrayOutputStream();
    private final InputStream standardInp = System.in;
    private final ByteArrayInputStream inputStreamCaptor = new ByteArrayInputStream("PUSH 10\nPRINT".getBytes());

    @Before
    public void redirectingStreams() {
        System.setOut(new PrintStream(outputStreamCaptor));
        System.setIn(inputStreamCaptor);
    }

    @After
    public void redirectingStreamsBack() {
        System.setOut(standardOut);
        System.setIn(standardInp);
    }

    @Test
    public void test() {
        Calculator calc = new Calculator();
        calc.start();
        assertEquals("10.0", outputStreamCaptor.toString().trim());
        outputStreamCaptor.reset();
    }
}