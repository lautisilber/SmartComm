/*
 * This example is a copy of Simplest, and it's goal is only to show you you can also
 * use SmartComm with the Wire library which implements I2C communication
 */

#include <Wire.h>
#include <SmartComm.h>


// create the "OK" command which only answers with "OK"
SMART_CMD_CREATE(cmdOk, "OK", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    stream->println("OK");
});
// create the "Hello" command which answers with "Hello, there!"
SMART_CMD_CREATE(cmdHello, "Hello", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    stream->println("Hello there!");
});
// create the "One" command which answers with the provided arguments
SMART_CMD_CREATE(cmdOne, "One", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    stream->print("I received the command: 'ONE' ");
    if (!args->N)
    {
        stream->println("without arguments.");
        return;
    }
    stream->println("With the arguments:");
    for (int i = 0; i < args->N; i++)
    {
        String str;
        args->to(i, &str);

        stream->print("\t ");
        stream->print(i);
        stream->print(": ");
        stream->println(str);
    }
});

// create the array of commands (note the & before the class names)
const SmartCmdBase *cmds[] = {
    &cmdOk, &cmdHello, &cmdOne
};

// create the CmartComm main class with the length of the cmds array
// between angled brackets <>
// note that we use the Wire interface
SmartComm<ARRAY_LENGTH(cmds)> sc(cmds, Wire);

void setup() {
    // initialize the Serial interface
    Serial.begin(9600);
    // initialize I2C interface
    Wire.begin();
}

void loop() {
    // call tick periodically to process the messages
    sc.tick();
}