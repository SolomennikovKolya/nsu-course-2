package ru.nsu.nikvel.tetris.model;

import java.io.Serializable;

public record Pair<K, V>(K key, V value) implements Serializable {
}