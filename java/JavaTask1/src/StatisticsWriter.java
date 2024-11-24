
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.util.ArrayList;

/**
 * Записывает статистику в CSV файл в формате (Слово; Частота; Частота в %).
 */
public class StatisticsWriter {
    public static void write(String filename, Statistics stats) {
        try (OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(filename))) {
            ArrayList<Pair<String, Integer>> statsList = stats.getSortedArray();
            final int sum = stats.totalNumberOfWords();

            for (Pair<String, Integer> p : statsList) {
                String s = p.first() + "\t" + p.second() + "\t" + Math.ceilDiv(p.second() * 100, sum) + "%\n";
                writer.write(s, 0, s.length());
            }

        } catch (Exception e) {
            System.err.println("Error while reading file: " + e.getLocalizedMessage());
        }
    }
}
