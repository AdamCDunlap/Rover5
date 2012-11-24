import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;

public class Rover5Viewer extends JFrame {
    public Rover5Viewer() {
        super("Rover5 Viewer");

        ControlPanel controls = new ControlPanel();
        Rover5Information info = new Rover5Information();

        JPanel panel = new JPanel();
        panel.setLayout(new BorderLayout());
        panel.setBorder(new EmptyBorder(0, 5, 0, 5));
        panel.add(info, BorderLayout.CENTER);
        panel.add(controls, BorderLayout.SOUTH);

        Container c = getContentPane();
        c.add(panel, BorderLayout.CENTER);
    }

    public static void main(String[] args) {
        System.out.println("Creating Rover5Viewer");
        Rover5Viewer v = new Rover5Viewer();

        v.setDefaultCloseOperation(EXIT_ON_CLOSE);

        System.out.println("Setting visible");
        v.setVisible(true);
        System.out.println("Set visible");
    }
}
