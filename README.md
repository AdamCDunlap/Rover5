Rover5
======
All the files needed to run my rover5 robot

In the arduino directory there is all the arduino code --- "interface" gets uploaded onto the interface arduino that is directly wired into the motor controller.

To compile it all, you need several things. First, move the directory entitled "library" to under a directory called "libraries" in your arduino sketchbook, and change its name to "Rover5" Then, if you want to control it with WASDEQ on your computer, upload the "sendToPC" sketch. For the code running on the computer, you need a library called ncurses --- http://www.gnu.org/software/ncurses/ Install this and you should be able to compile it by running make in the viewer-c directory. viewer-java was an attempt to write the same code in java, but it didn't pan out.

The kinematics directory explains some of the math.
