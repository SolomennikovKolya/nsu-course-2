package ru.nsu.nikvel.tetris.control.console;

import java.util.Scanner;

public class CommandReader extends Thread {

    private final CommandLine commandLine; // Общий ресурс (команда)
    private final Scanner scanner = new Scanner(System.in); // Сканер для считывания команд из стандартного IO

    public CommandReader(String name, CommandLine commandLine) {
        super(name);
        this.commandLine = commandLine;
    }

    @Override
    public void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                if (scanner.hasNextLine()) {
                    commandLine.set(scanner.nextLine());
                }
            }
        } catch (IllegalStateException e) {
            // поток читающий команды прерван, всё ок
            // когда сканер закрывается, hasNextLine() выкидывает исключение, что позволяет выйти из цикла
        }
    }

    public void disable() {
        this.interrupt();
        scanner.close();
    }
}
