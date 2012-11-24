import javax.swing.*;
import java.io.*;

/**
 * This class is supposed to start a thread that continuously reads from
 * stdin and then `invokeLater's Rover5Information
 * 
 */
class StdinReader implements Runnable {
    Runnable info;
    StdinReader(Runnable ininfo) {
        info = ininfo;
    }

    int i;
    public void run() {

        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        while (true) {
            try {
                if (System.in.available() > 0) {
                    SwingUtilities.invokeLater(info);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            //try {
                Thread.yield(); //sleep(1);
            //} catch (InterruptedException e) {
            //    e.printStackTrace();
            //}
        }
    }
}
