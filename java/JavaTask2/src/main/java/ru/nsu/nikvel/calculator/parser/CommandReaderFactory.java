package ru.nsu.nikvel.calculator.parser;

import ru.nsu.nikvel.calculator.exceptions.FileException;

/**
 * Создаёт парсер команд в зависимости от аргументов программы.
 * Если аргументов нет, то создаётся парсер, работающий со стандартным потоком ввода-вывода,
 * иначе создаёт парсер, который работает с файлом, имя которого берётся из первого агрумента.
 */
public class CommandReaderFactory {
    public CommandReader create(String[] args) throws FileException {
        if (args.length == 0)
            return new CommandReaderFromIO();
        else
            return new CommandReaderFromFile(args[0]);
    }
}
