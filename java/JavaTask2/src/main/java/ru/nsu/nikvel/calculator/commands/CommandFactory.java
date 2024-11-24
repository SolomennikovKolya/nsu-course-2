package ru.nsu.nikvel.calculator.commands;

import ru.nsu.nikvel.calculator.exceptions.ConfigException;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;

/**
 * Фабрика для заполнения словаря всех команд калькулятора.
 */
public class CommandFactory {
    /**
     * Получить словарь команд
     *
     * @param commands словарь команд
     * @throws ConfigException при ошибке в работе с файлом config.txt
     */
    public void create(HashMap<String, Command> commands) throws ConfigException {
        InputStream inputStream = CommandFactory.class.getResourceAsStream("/config.txt");
        if (inputStream == null)
        {
            System.out.println("lol");
            throw new ConfigException("Cannot open config.txt");
        }

        try (BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] words = line.split("\\s+");
                if (words.length != 2)
                    throw new ConfigException("Each line of the config file must contain exactly 2 words " +
                            "(the name of the command and the name of the corresponding class)");

                // Получаем объект класса Class по его полному имени
                Class<?> clazz = Class.forName(words[1]);
                // Создаем новый объект этого класса
                Object obj = clazz.newInstance();
                if (!(obj instanceof Command))
                    throw new ConfigException("Wrong command class instance");
                commands.put(words[0], (Command) obj);
            }
        } catch (IOException e) {
            throw new ConfigException("Error while reading config file");
        } catch (ClassNotFoundException e) {
            throw new ConfigException("Class not found exception");
        } catch (InstantiationException e) {
            throw new ConfigException("Instantiation exception");
        } catch (IllegalAccessException e) {
            throw new ConfigException("Illegal access exception");
        } finally {
            try {
                inputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
