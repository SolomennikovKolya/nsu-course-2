
import java.lang.StringBuilder;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.lang.Character;

/**
 * Заполнает статистику слов во входном файле.
 */
public class StatisticsReader {
    public static void read(String filename, Statistics stats) {
        InputStreamReader reader = null;

        try {
            reader = new InputStreamReader(new FileInputStream(filename));
            StringBuilder word = new StringBuilder();
            int tmp;
    
            while ((tmp = reader.read()) != -1) {
                final char newChar = (char) tmp;
                if (Character.isLetterOrDigit(newChar)) {
                    word.append(newChar);
                } else if (!word.isEmpty()) {
                    stats.add(word.toString());
                    word.delete(0, word.length());
                }
            }

            if (!word.isEmpty()) {
                stats.add(word.toString());
            }

        } catch (Exception e) {
            System.err.println("Error while reading file: " + e.getLocalizedMessage());

        } finally {
            if (null != reader) {
                try {
                    reader.close();
                } catch (Exception e) {
                    e.printStackTrace(System.err);
                }
            }
        }
    }
}
