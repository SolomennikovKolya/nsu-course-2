package ru.nsu.nikvel.calculator.parser;

import java.util.ArrayList;

/**
 * Интерфейс парсера команд. Используется для чтения команд с файла или из стандартного потока ввода-вывода.
 * Если по какой то причине парсер не может считать следующую команду,
 * возвращается строка "#" (как будто это строка с комментарием)
 */
public interface CommandReader {
    /**
     * Чтение команды
     *
     * @param args список для получения аргументов прочитанной команды
     * @return имя прочитанной команды
     */
    public String nextCommand(ArrayList<String> args);

    /**
     * @return true, если ещё есть команды для чтения, иначе false
     */
    public boolean hasNext();
}
