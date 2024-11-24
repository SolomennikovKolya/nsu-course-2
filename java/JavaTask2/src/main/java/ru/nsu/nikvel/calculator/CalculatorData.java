package ru.nsu.nikvel.calculator;

import java.util.HashMap;
import java.util.Stack;

/**
 * Класс для хранения изменяемых данных калькулятора (стэк чисел и переменные)
 */
public class CalculatorData {
    public Stack<Double> stack;

    public HashMap<String, Double> vars;

    public CalculatorData() {
        this.stack = new Stack<Double>();
        this.vars = new HashMap<String, Double>();
    }
}
