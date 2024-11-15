/*
 * This example is very similar to "Simplest", but here we use a different
 * interface for the communication. We can do that by passing the desired
 * interface to the SmarComm class. The only requirement is that the interface
 * is represented by a class that inherits from the Stream virtual class
 */

#include <SoftwareSerial.h>
#include <SmartComm.h>

// we declare the software serial class
SoftwareSerial softSerial(8, 9);


// create the "OK" command which only answers with "OK"
SMART_CMD_CREATE(cmdOk, "OK", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    Serial.println("I'm answering to the SoftwareSerial!");
    stream->println("OK");
});

// create the array of commands (note the & before the class names)
const SmartCmdBase *cmds[] = {
    &cmdOk
};

// create the CmartComm main class with the length of the cmds array
// between angled brackets <>
SmartComm<ARRAY_LENGTH(cmds)> sc(cmds, softSerial);


void setup() {
    // initialize the Serial interface
    Serial.begin(9600);
    softSerial.begin(4800);
}

void loop() {
    // call tick periodically to process the messages
    sc.tick();
}