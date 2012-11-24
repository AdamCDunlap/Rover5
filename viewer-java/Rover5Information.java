import java.awt.*;
import javax.swing.*;
import javax.swing.text.*;
import java.io.*;
import java.nio.*;

public class Rover5Information extends JPanel {
    private JTextArea output;
    private static final int packetsize = 58;
    private ByteBuffer serialBuffer = ByteBuffer.allocate(128);

    private FileInputStream serialport;

    public Rover5Information() {
        serialBuffer.order(ByteOrder.LITTLE_ENDIAN);
        try {
            serialport = new FileInputStream("/dev/ttyACM0");
        } catch (FileNotFoundException ex) {
            ex.printStackTrace();
        }

        output = new JTextArea(20, 110);
        output.setEditable(false);
        
        JScrollPane scroll = new JScrollPane(output);
        add(scroll);

        //System.out.println(this.getClass().getName());
        //System.out.println("Thread about to be started");

        new Thread(new Runnable() {
            public void run() {
                //System.out.println("Classname in thread: " + this.getClass().getName());
                try {
                    Thread.sleep(1000);
                    boolean keepgoing = true;
                    while (keepgoing) {
                        try {
                            //while (System.in.available() > 0) {
                            while (serialport.available() > 0) {
                                keepgoing = serialEvent();
                            }
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        Thread.sleep(1);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
        }}).start();
        //System.out.println("Thread started");
    }


    private boolean serialEvent() throws IOException {
        int c = serialport.read();
        if (c == -1) { // End of Stream
            // Probably shouldn't ever get here because available() would
            //  return 0.

            // Kill the thread
            return false;
        }
        serialBuffer.put((byte)c);
        
        //output.append(String.format("0x%x\n", c);
        //output.append(new String(serialBuffer, 0, serialBufCounter) + "\n");

        if (serialBuffer.position() >= packetsize) {
            // Process it
            //output.append("Buffer filed up! Size is " + serialBuffer.position() + " yeah\n");
            //output.append(new String(serialBuffer.array(), 0, serialBuffer.position()) + "\n");

            serialBuffer.flip();

            if ((c = serialBuffer.get()) != '0') serialEventError('0',c);
            //long stmicros = 0xffffffff & ((long)serialBuffer.getInt());
            int stmicros = serialBuffer.getInt();

            if ((c = serialBuffer.get()) != '1') serialEventError('1',c);
            int fld = serialBuffer.getInt();
            if ((c = serialBuffer.get()) != '2') serialEventError('2',c);
            int frd = serialBuffer.getInt();
            if ((c = serialBuffer.get()) != '3') serialEventError('3',c);
            int bld = serialBuffer.getInt();
            if ((c = serialBuffer.get()) != '4') serialEventError('4',c);
            int brd = serialBuffer.getInt();

            if ((c = serialBuffer.get()) != '5') serialEventError('5',c);
            short flp = serialBuffer.getShort();
            if ((c = serialBuffer.get()) != '6') serialEventError('6',c);
            short frp = serialBuffer.getShort();
            if ((c = serialBuffer.get()) != '7') serialEventError('7',c);
            short blp = serialBuffer.getShort();
            if ((c = serialBuffer.get()) != '8') serialEventError('8',c);
            short brp = serialBuffer.getShort();

            if ((c = serialBuffer.get()) != '9') serialEventError('9',c);
            int xpos = serialBuffer.getInt();
            if ((c = serialBuffer.get()) != 'A') serialEventError('A',c);
            int ypos = serialBuffer.getInt();
            if ((c = serialBuffer.get()) != 'B') serialEventError('B',c);
            int ang = serialBuffer.getInt();

            if (serialBuffer.get() != 'C') serialEventError('C',c);
            long endmicros = 0xffffffff & ((long)serialBuffer.getInt());

            if (serialBuffer.get() != 'D') serialEventError('D',c);

            //output.append(String.format(
            //    "stmicros: %d fld: %d frd: %d bld: %d brd: %d" +
            //    "flp: %d frp: %d blp: %d brp: %d xpos: %d" +
            //    "ypos: %d ang: %d endmicros: %d\n", 
            //    stmicros, fld, frd, bld, brd,
            //    flp, frp, blp, brp, xpos,
            //    ypos, ang, endmicros
            //));

            output.append(String.format(
                "stmicros: %x fld: %x frd: %x bld: %x brd: %x" +
                "flp: %x frp: %x blp: %x brp: %x xpos: %x" +
                "ypos: %x ang: %x endmicros: %x\n", 
                stmicros, fld, frd, bld, brd,
                flp, frp, blp, brp, xpos,
                ypos, ang, endmicros
            ));
            serialBuffer.clear();
        }

        return true; // Keep going
    }

    private void serialEventError(int exp, int act) {
        output.append("err ");
        System.out.printf("Error in serialevent: expected `0x%x' but received `0x%x'\n", exp, act);
    }

}
