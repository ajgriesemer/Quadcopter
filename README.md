# Quadcopter
Closed loop control of a Hubsan X4 quadcopter using Arduino and Pixy.

##Existing work
This project relies heavily on the existing body of knowledge for emulating the Hubsan X4 control protocol. 

1. **[DeviationTX](http://www.rcgroups.com/forums/showthread.php?t=1773853)** by PhracturedBlue - 
A quick analysis of the Hubsan X4 communication protocol that is the basis for all of the other projects. Also includes a [C file](https://bitbucket.org/PhracturedBlue/deviation-universaltx/src/773f8e8e7d2c7796e6bdf97bde483a637036f1f6/src/protocol/hubsan_a7105.c?at=default&fileviewer=file-view-default) with the initialization and handshaking code that is used in future projects.
2. **[Reverse Engineering a Hubsan X4 Quadcopter](http://www.jimhung.co.uk/?p=1349)** by Jim Hung - 
An exhaustive analysis of the Hubsan controller hardware and the SPI communication sent to the A7105.
3. **[Easy Android Controllable Drone](http://www.instructables.com/id/Easy-Android-controllable-PC-Interfaceable-Relati/)** by ShinyShez - 
A port of the DeviationTX code to Arduino to allow control of the quadcopter from a Processing app on an Android tablet.
4. **[Copter Manager](https://github.com/andihit/coptermanager-arduino)** by Andihit - 
An extra layer above the ShinyShez code. This project is designed to provide control of multiple quadcopters from a web based control panel.
