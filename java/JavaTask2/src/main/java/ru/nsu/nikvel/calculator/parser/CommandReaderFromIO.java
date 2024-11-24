package ru.nsu.nikvel.calculator.parser;

import java.util.ArrayList;
import java.util.Scanner;

/**
 * Поток команд из стандартного потока ввода-вывода.
 */
public class CommandReaderFromIO implements CommandReader {
    private final Scanner scanner; // Сканнер для ввода строк из потока ввода-вывода
    private boolean hasNext = true;

    public CommandReaderFromIO() {
        this.scanner = new Scanner(System.in);
    }

    @Override
    public String nextCommand(ArrayList<String> args) {
        if (!scanner.hasNextLine()) {
            hasNext = false;
            return "#";
        }
        final String line = scanner.nextLine();

        // Сканнер для счёта слов из строки
        Scanner lineScanner = new Scanner(line);
        if (!lineScanner.hasNext()) {
            return "#"; // Пустая строка (нет команды)
        }
        final String command = lineScanner.next();
        while (lineScanner.hasNext()) {
            args.add(lineScanner.next());
        }
        lineScanner.close();
        return command;
    }

    @Override
    public boolean hasNext() {
        return hasNext;
    }
}
