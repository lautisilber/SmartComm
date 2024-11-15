# Software Serial

This example is very similar to "Simplest", but here we use a different interface for the communication. We can do that by passing the desired interface to the ```SmarComm``` class. The only requirement is that the interface is represented by a class that inherits from the ```Stream``` virtual class, like ```SoftwareSerial```, ```Wire``` and ```Ethernet```