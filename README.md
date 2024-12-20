GlobalFoundries Remote Monitoring System (RMS) 
Author: Ryan Anderson
Mentor: Michael Bushey

This branch holds the code for RMS, the Arduino C++ code for the Collector, Monitor, and Proxy.
This code can successfully transmit RPM data from a fan connected to the Collector, send to Proxy which sends to Monitor and alerts users using the Python User Interface.

The code for the Collector ticks every time a digital output from the hall effect sensor is a 1, this tick counts the RPM.
The fundemantal problem with the design is the physical constraints between the hall effect sensor and the RPM sensing.
