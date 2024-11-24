
/**
 * Пара константных значений.
 */
public class Pair<T1, T2> {
    final private T1 first;
    final private T2 second;

    Pair(T1 first, T2 second) {
        this.first = first;
        this.second = second;
    }

    public T1 first() {
        return first;
    }

    public T2 second() {
        return second;
    }
}
