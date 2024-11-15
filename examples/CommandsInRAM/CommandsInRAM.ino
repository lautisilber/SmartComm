/*
 * This examples show you the use of the macro SMART_CMD_CREATE_RAM. Usually, the macro
 * SMART_CMD_CREATE will check if your target architecture (board) is capable of using
 * flash-stored variables, and will use them to store the command string data. This
 * may (or may not in some cases) reduce the RAM usage of the library. However, if the
 * number of commands is not high, and the length of each command is not too great, it
 * might be more memory efficient not to store the commands in flash and keep it all in
 * RAM. For those cases, the macro SMART_CMD_CREATE_RAM can be employed. It works the same
 * as SMART_CMD_CREATE, but forces the command to stay in RAM, which is exactly the same as
 * defining the class yourself. That means that
 * 
 * SMART_CMD_CREATE_RAM(cmdOk, "OK", callback);
 * 
 * and
 * 
 * SmartCmd cmdOk("OK", callback);
 * 
 * Are literally the same. When in doubt I encourage you to try both SMART_CMD_CREATE and
 * SMART_CMD_CREATE_RAM and choose the one that fits your project the best.
 */

#include <SmartComm.h>


// create the "OK" command which only answers with "OK"
SMART_CMD_CREATE_RAM(cmdOk, "OK", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    stream->println("OK");
});
// create the "Hello" command which answers with "Hello, there!"
SMART_CMD_CREATE_RAM(cmdHello, "Hello", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    stream->println("Hello there!");
});
// create the "One" command which answers with the provided arguments
SMART_CMD_CREATE_RAM(cmdOne, "One", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
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
SmartComm<ARRAY_LENGTH(cmds)> sc(cmds);

void setup() {
    // initialize the Serial interface
    Serial.begin(9600);
}

void loop() {
    // call tick periodically to process the messages
    sc.tick();
}