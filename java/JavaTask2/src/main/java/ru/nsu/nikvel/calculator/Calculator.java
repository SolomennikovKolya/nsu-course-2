package ru.nsu.nikvel.calculator;

import org.apache.logging.log4j.Level;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import ru.nsu.nikvel.calculator.commands.Command;
import ru.nsu.nikvel.calculator.commands.CommandFactory;
import ru.nsu.nikvel.calculator.exceptions.CalculatorException;
import ru.nsu.nikvel.calculator.exceptions.CommandException;
import ru.nsu.nikvel.calculator.parser.CommandReader;
import ru.nsu.nikvel.calculator.parser.CommandReaderFactory;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Стэковый калькулятор)
 */
public class Calculator {
    private final CalculatorData data;

    private final Logger log = LogManager.getLogger(Calculator.class);

    public Calculator() {
        this.data = new CalculatorData();
    }

    /**
     * Запустить калькулятор
     *
     * @param args аргументы программы
     */
    public void start(String[] args) {
        try {
            // Создание парсера команд
            CommandReaderFactory commandReaderFactory = new CommandReaderFactory();
            CommandReader commandReader = commandReaderFactory.create(args);

            // Инициализация всех команд
            CommandFactory commandFactory = new CommandFactory();
            HashMap<String, Command> commands = new HashMap<String, Command>();
            commandFactory.create(commands);

            while (commandReader.hasNext()) {
                // Ввод команды
                ArrayList<String> commandArgs = new ArrayList<String>();
                String commandName = commandReader.nextCommand(commandArgs);

                // Исполнение команды
                try {
                    Command command = commands.get(commandName);
                    if (command == null)
                        throw new CommandException("The \"" + commandName + "\" command does not exist");
                    command.execute(data, commandArgs);

                    StringBuilder lineForLog = new StringBuilder(commandName);
                    for (String commandArg : commandArgs) lineForLog.append(" ").append(commandArg);
                    log.log(Level.INFO, lineForLog.toString());

                } catch (CommandException e) {
                    System.out.println(e.toString());
                    log.log(Level.WARN, e);
                }
            }
        } catch (CalculatorException e) {
            System.out.println(e.toString());
        }
    }

    public void start() {
        start(new String[0]);
    }

}

