package ru.nsu.nikvel.tetris.model;

/**
 * Разные полезные утилиты
 **/
public class Utils {
    /**
     * Обычный математический модуль, который возвращает значения в пределах [0, mod - 1]
     **/
    public static int mod(int val, int mod) {
        if (val < 0)
            return val % mod + mod;
        else
            return val % mod;
    }

    /**
     * Копирование двумерного массива
     **/
    public static int[][] copy2DArr(int[][] source) {
        final int rows = source.length;
        final int cols = source[0].length;
        int[][] dest = new int[rows][cols];

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                dest[i][j] = source[i][j];
            }
        }
        return dest;
    }

    /**
     * Копирование трёхмерного массива
     **/
    public static int[][][] copy3DArr(int[][][] source) {
        final int len1 = source.length;
        final int len2 = source[0].length;
        final int len3 = source[0][0].length;
        int[][][] dest = new int[len1][len2][len3];

        for (int i = 0; i < len1; i++) {
            for (int j = 0; j < len2; j++) {
                for (int k = 0; k < len3; k++) {
                    dest[i][j][k] = source[i][j][k];
                }
            }
        }
        return dest;
    }
}
