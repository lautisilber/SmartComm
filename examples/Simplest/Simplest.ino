/*
 * If you run this example as is, you can send messages tou your device through the Serial interface.
 * The messages are ended by the default endChar '\n' which si basically just the enter key. The first
 * word of the message is the command (OK, Hello and One are the available commands in this example) and
 * the following words are the arguments of the command. Words are separated by the default sepChar ' '
 * which is just a space. (You can use the One command to see how arguments are received)
 * 
 * 
 * There are multiple ways in which you can define the SmartCmd classes. I think the easiest one
 * is this one. I provide a macro that hides away some boiler code that is used to improve the
 * memory footprint of the library. Basically, the macro has the following definition:
 * 
 * SMART_CMD_CREATE(className, "command", callback)
 * 
 * where the <className> is the name of the SmartCmd class being defined, <"command"> is the literal
 * string that is the command that will trigger the <callback> function, when it's received. This
 * macro should be treated as if
 * SMART_CMD_CREATE(className, "command", callback) was completely
 * equivalent to
 * SmartCmd className("command", callback)
 * (in some cases it is completely equivalent)
 * 
 * 
 * The SmartCmd classes take in a callback function with the definition of
 * void(*)(Stream*, const SmartCmdArguments*, const char*) which means a function like the followig
 * will work perfectly:
 * 
 * void commandCallback(Stream *stream, const SmartCmdArguments *argrs, const char *cmd)
 * {
 *     ...
 * }
 * 
 * In this example I decided to use lambda functions instead of traditional ones because I think the
 * resulting code is much more compact. However, there is absolutely no problem with using traditional
 * functions. For example, the cmdOk class could be defined in the following way:
 * 
 * void cmdOkCb(Stream *stream, const SmartCmdArguments *argrs, const char *cmd) {
 *     stream->println("OK");
 * }
 * SMART_CMD_CREATE(cmdOk, "OK", cmdOkCb);
 * 
 * which would result in exactly the same functionality.
 * 
 * 
 * The cmds array is necessary for the library to work. I chose this approach because this way it is
 * possible to avoid heap allocation, and ensure the array containing all the SmartCmd is as small as
 * possible. So we need to declare a const fixed-sized array of SmartCmdBase pointers. We use SmartCmdBase
 * instead of SmartCmd because this allows me to define different functionality for when resources are
 * available to reduce RAM memory usage (that functionality is hidden behind the SMART_CMD_CREATE macro,
 * so you don't have to worry).
 * 
 * 
 * Lastly we need to pass the cmds array to the SmartComm main class. Once that is done, there's no more
 * configuration needed. We only have to make sure to call SmartComm's tick() method to process incoming
 * messages. This method has negligible performance impact when no data has to be processed and it only
 * processes the message once it is input, so you can call this function as often as you want without
 * having to worry about it slowing your sketch down.
 * Note the angled brackets <> on the class declaration. Because SmartComm is a templated class, you have
 * to indicate the number of SmartCmd in the array inside the angled brackets. You could do this manually
 * by writing SmartComm<3> sc(cmds);. However, this is a bit prone to mistakes (like forgetting to update
 * this number when you change the number of SmartCmd) so the ARRAY_LENGTH macro allows you to get the length
 * of the coms array at compile time, so you don't have to worry about it.
 * 
 * 
 * In this particular example we're using the Serial interface to communicate. If you'd prefer to use
 * anohter interface, check out the other examples.
 */

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
SmartComm<ARRAY_LENGTH(cmds)> sc(cmds);

void setup() {
    // initialize the Serial interface
    Serial.begin(9600);
}

void loop() {
    // call tick periodically to process the messages
    sc.tick();
}