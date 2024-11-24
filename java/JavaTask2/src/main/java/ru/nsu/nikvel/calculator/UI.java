package ru.nsu.nikvel.calculator;

/**
 * Пользовательский "интерфейс" для работы с калькулятором.
 * Если в качестве аргумента программы передаётся имя файла, то этот файл используется для ввода команд.
 * Если агрументов нет, то калькулятор использует команды из стандартного ввода-вывода.
 */
public class UI {
    public static void main(String[] args) {
        Calculator calc = new Calculator();
        calc.start(args);
    }
}
