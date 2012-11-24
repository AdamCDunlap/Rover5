import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public class ControlPanel extends JPanel
        implements ChangeListener, ActionListener {
    private JSlider yslider;
    private JButton zeroButton;
    
    public ControlPanel() {
        yslider = new JSlider(-255, 255, 0);
        setupSlider(yslider);
        zeroButton = new JButton ("Zero");
        zeroButton.addActionListener(this);
        add(zeroButton);
    }

    public void stateChanged(ChangeEvent e) {
        Object s = e.getSource();
        if (s == yslider) {
            
        } else {
            System.out.println("Don't know where ChangeEvent came from: " + e);
        }
    }

    public void actionPerformed(ActionEvent e) {
        Object s = e.getSource();
        if (s == zeroButton) {
            yslider.setValue(0);
        } else {
            System.out.println("Don't know where ActionEvent came from: " + e);
        }
    }

    private void setupSlider(JSlider s) {
        s.addChangeListener(this);
        s.setMajorTickSpacing(50);
        s.setMinorTickSpacing(10);
        s.setPaintTicks(true);
        add(s);
    }
}
