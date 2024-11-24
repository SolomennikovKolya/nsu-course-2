package ru.nsu.nikvel.tetris.control.console;

public class CommandLine {
    private String command;

    public CommandLine() {
        this.command = null;
    }

    /**
     * Устанавливает значение для команды, а затем ждёт, пока её прочитает другой поток.
     * Этот метод должен вызывать только CommandReader (поток, который считывает команды)
     **/
    public synchronized void set(String command) {
        try {
            this.command = command;
            wait(); // ждёт пока другой поток сделает get()

        } catch (InterruptedException e) {
            // поток читающий команды прерван
            // throw new RuntimeException(e);
        }

    }

    /**
     * Получение команды. После получения данный ресурс (доступная команды) станет null
     * и CommandReader (счётчик команд) сможет снова считать следующую команду.
     **/
    public synchronized String get() {
        if (command == null)
            return null;
        else {
            final String commandToReturn = command;
            command = null;
            notify();
            return commandToReturn;
        }
    }
}
