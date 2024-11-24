
import java.util.Comparator;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Map;

class DataComparator implements Comparator<Pair<String, Integer>> {
    @Override
    public int compare(Pair<String, Integer> a, Pair<String, Integer> b) {
        return b.second() - a.second();
    }
}

/**
 * Статистика для подсчёта колличества слов.
 * По сути это просто обёртка для {@link HashMap}.
 */
public class Statistics {
    private final HashMap<String, Integer> data;
    private int sum;

    Statistics() {
        data = new HashMap<String, Integer>();
        sum = 0;
    }

    /**
     * Увеличивает количество слов с ключём key
     *
     * @param key
     */
    public void add(String key) {
        data.compute(key, (k, v) -> (v == null) ? 1 : v + 1);
        sum++;
    }

    /**
     * Возвращает отсортированный по уменьшению частоты слов {@link ArrayList}, содержащий пары <слово, его количество>
     */
    public ArrayList<Pair<String, Integer>> getSortedArray() {
        ArrayList<Pair<String, Integer>> ans = new ArrayList<Pair<String, Integer>>(data.size());
        for (Map.Entry<String, Integer> entry : data.entrySet()) {
            ans.add(new Pair<String, Integer>(entry.getKey(), entry.getValue()));
        }
        ans.sort(new DataComparator());
        return ans;
    }

    /**
     * Возвращает суммарное количество всех слов
     */
    public int totalNumberOfWords() {
        return sum;
    }
}
