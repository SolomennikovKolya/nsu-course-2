
public class Main {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Bad args for program");
            return;
        }
        Statistics stats = new Statistics();
        StatisticsReader.read(args[0], stats);
        StatisticsWriter.write(args[1], stats);
    }
}
