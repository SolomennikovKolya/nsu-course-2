package ru.nsu.nikvel.calculator.parser;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import ru.nsu.nikvel.calculator.Calculator;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import static org.junit.Assert.assertEquals;

public class FileTest {
    private final PrintStream standardOut = System.out;
    private final ByteArrayOutputStream outputStreamCaptor = new ByteArrayOutputStream();

    @Before
    public void redirectingStreams() {
        System.setOut(new PrintStream(outputStreamCaptor));
    }

    @After
    public void redirectingStreamsBack() {
        System.setOut(standardOut);
    }

    @Test
    public void test() {
        Calculator calc = new Calculator();
        calc.start(new String[]{"src/main/resources/testcases/fileTest.txt"});
        assertEquals("10.0", outputStreamCaptor.toString().trim());
        outputStreamCaptor.reset();
    }
}