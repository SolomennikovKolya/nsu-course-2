package ru.nsu.nikvel.calculator.parser;

import ru.nsu.nikvel.calculator.exceptions.FileNotFoundException;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * Поток команд из файла.
 */
public class CommandReaderFromFile implements CommandReader {
    private final BufferedReader reader;
    private boolean hasNext = true;

    public CommandReaderFromFile(String filename) throws FileNotFoundException {
        try {
            this.reader = new BufferedReader(new FileReader(filename));
        } catch (IOException e) {
            e.printStackTrace();
            throw new FileNotFoundException("Cannot create CommandReader");
        }
    }

    @Override
    public String nextCommand(ArrayList<String> args) {
        String command = "";
        try {
            final String line = reader.readLine();
            if (line == null) {
                hasNext = false;
                return "#";
            }

            Scanner lineScanner = new Scanner(line);
            if (!lineScanner.hasNext()) {
                return "#";
            }
            command = lineScanner.next();
            while (lineScanner.hasNext()) {
                args.add(lineScanner.next());
            }
            lineScanner.close();
            return command;

        } catch (IOException e) {
            hasNext = false;
            e.printStackTrace();
        }
        return command;
    }

    @Override
    public boolean hasNext() {
        return hasNext;
    }
}
