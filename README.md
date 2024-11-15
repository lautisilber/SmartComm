# SmartComm

This library simplifies inter-device communication. With it you can make your device receive commands with arguments and use callbacks to react to them. The medium through which the messages are received can be changed to whatever you need (be it Serial, SoftwareSerial, Wire/I2C, or whatever interface implemented using the Srteam virtual class).

This library has been designed to use only stack memory and have as small a memory footprint as possible, while still being permormant.

To use SmartComm, you'll define SmartCmd (smart commands) which store a command and a callback. The callback is called whenever the command is received and the arguments passed in the message are be accessible as arguments of the callback. The message structure is customaizable at compile time to fit all needs, without overhead.

A lot of the library's functionality is described at the beginning of the examples so I encourage you to treat them as documentation.
